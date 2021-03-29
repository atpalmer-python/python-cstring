#include <Python.h>


/*
 * memrchr not available on some systems, so reimplement.
 */
const char *_memrchr(const char *s, int c, size_t n) {
    for(const char *p = s + n - 1; p >= s; --p) {
        if(*p == c)
            return p;
    }
    return NULL;
}



struct cstring {
    PyObject_VAR_HEAD
    Py_hash_t hash;
    char value[];
};

static PyTypeObject cstring_type;

#define CSTRING_HASH(self)          (((struct cstring *)self)->hash)
#define CSTRING_VALUE(self)         (((struct cstring *)self)->value)
#define CSTRING_VALUE_AT(self, i)   (&CSTRING_VALUE(self)[(i)])

#define CSTRING_ALLOC(tp, len)      ((struct cstring *)(tp)->tp_alloc((tp), (len)))

static PyObject *_cstring_new(PyTypeObject *type, const char *value, Py_ssize_t len) {
    struct cstring *new = CSTRING_ALLOC(type, len + 1);
    new->hash = -1;
    memcpy(new->value, value, len);
    new->value[len] = '\0';
    return (PyObject *)new;
}

static PyObject *cstring_new_empty(void) {
    return _cstring_new(&cstring_type, "", 0);
}

static const char *_obj_as_string_and_size(PyObject *o, Py_ssize_t *s) {
    if(PyUnicode_Check(o))
        return PyUnicode_AsUTF8AndSize(o, s);

    if(PyObject_CheckBuffer(o)) {
        /* handles bytes, bytearrays, arrays, memoryviews, etc. */
        Py_buffer view;
        if(PyObject_GetBuffer(o, &view, PyBUF_SIMPLE) < 0)
            return NULL;
        *s = view.len;
        const char *buffer = view.buf;
        PyBuffer_Release(&view);
        return buffer;
    }

    PyErr_Format(
        PyExc_TypeError,
        "Invalid initialization type: %s.",
        Py_TYPE(o)->tp_name);

    *s = -1;
    return NULL;
}

static PyObject *cstring_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyObject *argobj = NULL;
    if(!PyArg_ParseTuple(args, "O", &argobj))
        return NULL;

    if(PyObject_TypeCheck(argobj, type)) {
        Py_INCREF(argobj);
        return argobj;
    }

    Py_ssize_t len = 0;
    const char *buffer = _obj_as_string_and_size(argobj, &len);
    if(!buffer)
        return NULL;

    return _cstring_new(type, buffer, len);
}

static void cstring_dealloc(PyObject *self) {
    Py_TYPE(self)->tp_free(self);
}

static int _ensure_cstring(PyObject *self) {
    if(PyObject_TypeCheck(self, &cstring_type))
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
        /* Should be unreachable */
        PyErr_Format(PyExc_SystemError, "Invalid compare operation: %d", op);
        return NULL;
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

    struct cstring *new = CSTRING_ALLOC(Py_TYPE(left), size);
    memcpy(new->value, CSTRING_VALUE(left), Py_SIZE(left));
    memcpy(&new->value[cstring_len(left)], CSTRING_VALUE(right), Py_SIZE(right)); 
    return (PyObject *)new;
}

static PyObject *cstring_repeat(PyObject *self, Py_ssize_t count) {
    if(!_ensure_cstring(self))
        return NULL;
    if(count <= 0)
        return cstring_new_empty();

    Py_ssize_t size = (cstring_len(self) * count) + 1;

    struct cstring *new = CSTRING_ALLOC(Py_TYPE(self), size);
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
    if(slicelen < 0) {
        PyErr_Format(PyExc_SystemError, "Internal error: Invalid slicelen: %d", slicelen);
        return NULL;
    }

    struct cstring *new = CSTRING_ALLOC(Py_TYPE(self), slicelen + 1);
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
    if(PyObject_TypeCheck(o, &cstring_type)) {
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

    const char *p = params.start;
    long result = 0;
    while((p = strstr(p, params.substr)) != NULL) {
        ++result;
        p += params.substr_len;
        if(p >= params.end)
            break;
    }

    return PyLong_FromLong(result);
}

static const char *_substr_params_str(const struct _substr_params *params) {
    const char *p = strstr(params->start, params->substr);
    if(!p || p + params->substr_len > params->end)
        return NULL;
    return p;
}

static const char *_substr_params_rstr(const struct _substr_params *params) {
    const char *p = params->end - params->substr_len + 1;
    for(;;) {
        p = _memrchr(params->start, *params->substr, p - params->start);
        if(!p)
            goto done;
        if(memcmp(p, params->substr, params->substr_len) == 0)
            return p;
    }
done:
    return NULL;
}

PyDoc_STRVAR(find__doc__, "");
PyObject *cstring_find(PyObject *self, PyObject *args) {
    struct _substr_params params;

    if(!_parse_substr_args(self, args, &params))
        return NULL;

    const char *p = _substr_params_str(&params);
    if(!p)
        return PyLong_FromLong(-1);

    return PyLong_FromSsize_t(p - CSTRING_VALUE(self));
}

PyDoc_STRVAR(index__doc__, "");
PyObject *cstring_index(PyObject *self, PyObject *args) {
    struct _substr_params params;

    if(!_parse_substr_args(self, args, &params))
        return NULL;

    const char *p = _substr_params_str(&params);
    if(!p) {
        PyErr_SetString(PyExc_ValueError, "substring not found");
        return NULL;
    }

    return PyLong_FromSsize_t(p - CSTRING_VALUE(self));
}

PyDoc_STRVAR(isalnum__doc__, "");
PyObject *cstring_isalnum(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(!isalnum(*p))
            Py_RETURN_FALSE;
        ++p;
    }
    Py_RETURN_TRUE;
}

PyDoc_STRVAR(isalpha__doc__, "");
PyObject *cstring_isalpha(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(!isalpha(*p))
            Py_RETURN_FALSE;
        ++p;
    }
    Py_RETURN_TRUE;
}

PyDoc_STRVAR(isdigit__doc__, "");
PyObject *cstring_isdigit(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(!isdigit(*p))
            Py_RETURN_FALSE;
        ++p;
    }
    Py_RETURN_TRUE;
}

PyDoc_STRVAR(islower__doc__, "");
PyObject *cstring_islower(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(isalpha(*p)) {
            if(!islower(*p))
                Py_RETURN_FALSE;
            ++p;
            while(*p) {
                if(isalpha(*p) && !islower(*p))
                    Py_RETURN_FALSE;
                ++p;
            }
            /* at least one lc alpha and no uc alphas */
            Py_RETURN_TRUE;
        }
        ++p;
    }
    Py_RETURN_FALSE;
}

PyDoc_STRVAR(isprintable__doc__, "");
PyObject *cstring_isprintable(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(!isprint(*p))
            Py_RETURN_FALSE;
        ++p;
    }
    Py_RETURN_TRUE;
}

PyDoc_STRVAR(isspace__doc__, "");
PyObject *cstring_isspace(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(!isspace(*p))
            Py_RETURN_FALSE;
        ++p;
    }
    return PyBool_FromLong(p != CSTRING_VALUE(self));
}

PyDoc_STRVAR(isupper__doc__, "");
PyObject *cstring_isupper(PyObject *self, PyObject *args) {
    const char *p = CSTRING_VALUE(self);
    while(*p) {
        if(isalpha(*p)) {
            if(!isupper(*p))
                Py_RETURN_FALSE;
            ++p;
            while(*p) {
                if(isalpha(*p) && !isupper(*p))
                    Py_RETURN_FALSE;
                ++p;
            }
            /* at least one uc alpha and no lc alphas */
            Py_RETURN_TRUE;
        }
        ++p;
    }
    Py_RETURN_FALSE;
}

PyDoc_STRVAR(rfind__doc__, "");
PyObject *cstring_rfind(PyObject *self, PyObject *args) {
    struct _substr_params params;

    if(!_parse_substr_args(self, args, &params))
        return NULL;

    const char *p = _substr_params_rstr(&params);

    if(!p)
        return PyLong_FromLong(-1);

    return PyLong_FromSsize_t(p - CSTRING_VALUE(self));
}

PyDoc_STRVAR(rindex__doc__, "");
PyObject *cstring_rindex(PyObject *self, PyObject *args) {
    struct _substr_params params;

    if(!_parse_substr_args(self, args, &params))
        return NULL;

    const char *p = _substr_params_rstr(&params);
    if(!p) {
        PyErr_SetString(PyExc_ValueError, "substring not found");
        return NULL;
    }

    return PyLong_FromSsize_t(p - CSTRING_VALUE(self));
}

PyDoc_STRVAR(startswith__doc__, "");
PyObject *cstring_startswith(PyObject *self, PyObject *args) {
    struct _substr_params params;
    if(!_parse_substr_args(self, args, &params))
        return NULL;
    if(params.end - params.start < params.substr_len)
        return PyBool_FromLong(0);
    int cmp = memcmp(params.start, params.substr, params.substr_len);
    return PyBool_FromLong(cmp == 0);
}

PyDoc_STRVAR(endswith__doc__, "");
PyObject *cstring_endswith(PyObject *self, PyObject *args) {
    struct _substr_params params;
    if(!_parse_substr_args(self, args, &params))
        return NULL;
    if(params.end - params.start < params.substr_len)
        return PyBool_FromLong(0);
    int cmp = memcmp(params.end - params.substr_len, params.substr, params.substr_len);
    return PyBool_FromLong(cmp == 0);
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
    /* TODO: capitalize */
    /* TODO: casefold */
    /* TODO: center */
    {"count", cstring_count, METH_VARARGS, count__doc__},
    /* TODO: encode (decode???) */
    {"endswith", cstring_endswith, METH_VARARGS, endswith__doc__},
    /* TODO: expandtabs */
    {"find", cstring_find, METH_VARARGS, find__doc__},
    /* TODO: format */
    /* TODO: format_map */
    {"index", cstring_index, METH_VARARGS, index__doc__},
    {"isalnum", cstring_isalnum, METH_VARARGS, isalnum__doc__},
    {"isalpha", cstring_isalpha, METH_VARARGS, isalpha__doc__},
    /* TODO: isascii */
    /* TODO: isdecimal */
    {"isdigit", cstring_isdigit, METH_VARARGS, isdigit__doc__},
    /* TODO: isidentifier */
    {"islower", cstring_islower, METH_VARARGS, islower__doc__},
    /* TODO: isnumeric */
    {"isprintable", cstring_isprintable, METH_VARARGS, isprintable__doc__},
    {"isspace", cstring_isspace, METH_VARARGS, isspace__doc__},
    /* TODO: istitle */
    {"isupper", cstring_isupper, METH_VARARGS, isupper__doc__},
    /* TODO: join */
    /* TODO: ljust */
    /* TODO: lower */
    /* TODO: lstrip */
    /* TODO: maketrans */
    /* TODO: partition */
    /* TODO: removeprefix */
    /* TODO: replace */
    {"rfind", cstring_rfind, METH_VARARGS, rfind__doc__},
    {"rindex", cstring_rindex, METH_VARARGS, rindex__doc__},
    /* TODO: rjust */
    /* TODO: rpartition */
    /* TODO: rsplit */
    /* TODO: rstrip */
    /* TODO: split */
    /* TODO: splitlines */
    {"startswith", cstring_startswith, METH_VARARGS, startswith__doc__},
    /* TODO: strip */
    /* TODO: swapcase */
    /* TODO: title */
    /* TODO: translate */
    /* TODO: upper */
    /* TODO: zfill */
    {0},
};

static PyTypeObject cstring_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cstring.cstring",
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
