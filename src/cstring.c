#include <Python.h>

struct cstring {
    PyObject_VAR_HEAD
    Py_hash_t hash;
    char value[];
};

#define CSTRING_HASH(self)          (((struct cstring *)self)->hash)
#define CSTRING_VALUE(self)         (((struct cstring *)self)->value)
#define CSTRING_VALUE_AT(self, i)   (&CSTRING_VALUE(self)[(i)])


static PyObject *_cstring_new(PyTypeObject *type, const char *value, size_t len) {
    struct cstring *new = type->tp_alloc(type, len + 1);
    new->hash = -1;
    memcpy(new->value, value, len);
    new->value[len] = '\0';
    return (PyObject *)new;
}

#define CSTRING_NEW_EMPTY(tp)   (_cstring_new(tp, "", 0))

static PyObject *cstring_new(PyTypeObject *type, PyObject *args, PyObject **kwargs) {
    char *value = NULL;
    if(!PyArg_ParseTuple(args, "s", &value))
        return NULL;
    size_t len = strlen(value);
    return _cstring_new(type, value, len);
}

static void cstring_dealloc(PyObject *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyTypeObject cstring_type;

static int _ensure_cstring(PyObject *self) {
    if(Py_TYPE(self) == &cstring_type)
        return 1;
    PyErr_Format(
        PyExc_TypeError,
        "Object must have type cstring, not %s.",
        Py_TYPE(self)->tp_name);
    return 0;
}

static PyObject *cstring_str(PyObject *self) {
    return PyUnicode_FromString(CSTRING_VALUE(self));
}

static PyObject *cstring_repr(PyObject *self) {
    PyObject *tmp = cstring_str(self);
    PyObject *repr = PyObject_Repr(tmp);
    Py_DECREF(tmp);
    return repr;
}

static Py_hash_t cstring_hash(PyObject *self) {
    if(CSTRING_HASH(self) == -1)
        CSTRING_HASH(self) = _Py_HashBytes(CSTRING_VALUE(self), Py_SIZE(self));
    return CSTRING_HASH(self);
}

static PyObject *cstring_richcompare(PyObject *self, PyObject *other, int op) {
    if(!_ensure_cstring(other))
        return NULL;

    const char *left = CSTRING_VALUE(self);
    const char *right = CSTRING_VALUE(other);

    for(;*left && *right && *left == *right; ++left, ++right)
        ;

    switch (op) {
    case Py_EQ:
        return PyBool_FromLong(*left == *right);
    case Py_NE:
        return PyBool_FromLong(*left != *right);
    case Py_LT:
        return PyBool_FromLong(*left < *right);
    case Py_GT:
        return PyBool_FromLong(*left > *right);
    case Py_LE:
        return PyBool_FromLong(*left <= *right);
    case Py_GE:
        return PyBool_FromLong(*left >= *right);
    default:
        assert(0);
    }
}

static Py_ssize_t cstring_len(PyObject *self) {
    return Py_SIZE(self) - 1;
}

static PyObject *cstring_concat(PyObject *left, PyObject *right) {
    if(!_ensure_cstring(left))
        return NULL;
    if(!_ensure_cstring(right))
        return NULL;

    Py_ssize_t size = cstring_len(left) + cstring_len(right) + 1;

    struct cstring *new = Py_TYPE(left)->tp_alloc(Py_TYPE(left), size);
    memcpy(new->value, CSTRING_VALUE(left), Py_SIZE(left));
    memcpy(&new->value[cstring_len(left)], CSTRING_VALUE(right), Py_SIZE(right)); 
    return (PyObject *)new;
}

static PyObject *cstring_repeat(PyObject *self, Py_ssize_t count) {
    if(!_ensure_cstring(self))
        return NULL;
    if(count <= 0)
        return CSTRING_NEW_EMPTY(Py_TYPE(self));

    Py_ssize_t size = (cstring_len(self) * count) + 1;

    struct cstring *new = Py_TYPE(self)->tp_alloc(Py_TYPE(self), size);
    for(Py_ssize_t i = 0; i < size - 1; i += cstring_len(self)) {
        memcpy(&new->value[i], CSTRING_VALUE(self), Py_SIZE(self));
    }
    return (PyObject *)new;
}

static Py_ssize_t _ensure_valid_index(PyObject *self, Py_ssize_t i) {
    if(i >= 0 && i < cstring_len(self))
        return i;
    PyErr_SetString(PyExc_IndexError, "Index is out of bounds");
    return -1;
}

static PyObject *cstring_item(PyObject *self, Py_ssize_t i) {
    if(_ensure_valid_index(self, i) < 0)
        return NULL;
    return _cstring_new(Py_TYPE(self), CSTRING_VALUE_AT(self, i), 1);
}

static int cstring_contains(PyObject *self, PyObject *arg) {
    if(!_ensure_cstring(arg))
        return -1;
    if(strstr(CSTRING_VALUE(self), CSTRING_VALUE(arg)))
        return 1;
    return 0;
}

static PyObject *_cstring_subscript_index(PyObject *self, PyObject *index) {
    Py_ssize_t i = PyNumber_AsSsize_t(index, PyExc_IndexError);
    if(PyErr_Occurred())
        return NULL;
    if(i < 0)
        i += cstring_len(self);
    return cstring_item(self, i);
}

static PyObject *_cstring_subscript_slice(PyObject *self, PyObject *slice) {
    Py_ssize_t start, stop, step;
    if(PySlice_Unpack(slice, &start, &stop, &step) < 0)
        return NULL;
    Py_ssize_t slicelen = PySlice_AdjustIndices(cstring_len(self), &start, &stop, step);
    assert(slicelen >= 0);

    struct cstring *new = Py_TYPE(self)->tp_alloc(Py_TYPE(self), slicelen + 1);
    char *src = CSTRING_VALUE_AT(self, start);
    for(Py_ssize_t i = 0; i < slicelen; ++i) {
        new->value[i] = *src;
        src += step;
    }
    new->value[slicelen] = '\0';
    return (PyObject *)new;
}

static PyObject *cstring_subscript(PyObject *self, PyObject *key) {
    if(PyIndex_Check(key))
        return _cstring_subscript_index(self, key);
    if(PySlice_Check(key))
        return _cstring_subscript_slice(self, key);

    PyErr_SetString(PyExc_TypeError, "Subscript must be int or slice.");
    return NULL;
}

static const char *_obj_to_utf8(PyObject *o, Py_ssize_t *len_p) {
    if(PyUnicode_Check(o))
        return PyUnicode_AsUTF8AndSize(o, len_p);
    if(Py_TYPE(o) == &cstring_type) {
        *len_p = cstring_len(o);
        return CSTRING_VALUE(o);
    }
    PyErr_Format(
        PyExc_TypeError, "Object cannot be type %s.", Py_TYPE(o)->tp_name);
    return NULL;
}

static Py_ssize_t _fix_index(Py_ssize_t i, Py_ssize_t len) {
    Py_ssize_t result = i;
    if(result < 0)
        result += len;
    if(result < 0)
        result = 0;
    if(result > len)
        result = len;
    return result;
}

struct _substr_params {
    const char *start;
    const char *end;
    const char *substr;
    Py_ssize_t substr_len;
};

static struct _substr_params *_parse_substr_args(PyObject *self, PyObject *args, struct _substr_params *params) {
    PyObject *substr_obj;
    Py_ssize_t start = 0;
    Py_ssize_t end = PY_SSIZE_T_MAX;

    if(!PyArg_ParseTuple(args, "O|nn", &substr_obj, &start, &end))
        return NULL;

    Py_ssize_t substr_len;
    const char *substr = _obj_to_utf8(substr_obj, &substr_len);
    if(!substr)
        return NULL;

    start = _fix_index(start, cstring_len(self));
    end = _fix_index(end, cstring_len(self));

    params->start = CSTRING_VALUE_AT(self, start);
    params->end = CSTRING_VALUE_AT(self, end);
    params->substr = substr;
    params->substr_len = substr_len;

    return params;
}

PyDoc_STRVAR(count__doc__, "");
static PyObject *cstring_count(PyObject *self, PyObject *args) {
    struct _substr_params params;

    if(!_parse_substr_args(self, args, &params))
        return NULL;

    char *p = params.start;
    long result = 0;
    while((p = strstr(p, params.substr)) != NULL) {
        ++result;
        p += params.substr_len;
        if(p >= params.end)
            break;
    }

    return PyLong_FromLong(result);
}

PyDoc_STRVAR(find__doc__, "");
PyObject *cstring_find(PyObject *self, PyObject *args) {
    struct _substr_params params;

    if(!_parse_substr_args(self, args, &params))
        return NULL;

    char *p = strstr(params.start, params.substr);
    if(!p)
        return PyLong_FromLong(-1);
    if(p + params.substr_len > params.end)
        return PyLong_FromLong(-1);

    return PyLong_FromSsize_t(p - CSTRING_VALUE(self));
}

static PySequenceMethods cstring_as_sequence = {
    .sq_length = cstring_len,
    .sq_concat = cstring_concat,
    .sq_repeat = cstring_repeat,
    .sq_item = cstring_item,
    .sq_contains = cstring_contains,
};

static PyMappingMethods cstring_as_mapping = {
    .mp_length = cstring_len,
    .mp_subscript = cstring_subscript,
};

static PyMethodDef cstring_methods[] = {
    {"count", cstring_count, METH_VARARGS, count__doc__},
    {"find", cstring_find, METH_VARARGS, find__doc__},
    {0},
};

static PyTypeObject cstring_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cstring",
    .tp_doc = "",
    .tp_basicsize = sizeof(struct cstring),
    .tp_itemsize = sizeof(char),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = cstring_new,
    .tp_dealloc = cstring_dealloc,
    .tp_richcompare = cstring_richcompare,
    .tp_str = cstring_str,
    .tp_repr = cstring_repr,
    .tp_hash = cstring_hash,
    .tp_as_sequence = &cstring_as_sequence,
    .tp_as_mapping = &cstring_as_mapping,
    .tp_methods = cstring_methods,
};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cstring",
    .m_doc = "",
    .m_size = 0,
    .m_methods = NULL,
};

PyMODINIT_FUNC PyInit_cstring(void) {
    if(PyType_Ready(&cstring_type) < 0)
        return NULL;
    Py_INCREF(&cstring_type);
    PyObject *m = PyModule_Create(&module);
    PyModule_AddObject(m, "cstring", (PyObject *)&cstring_type);
    return m;
}
