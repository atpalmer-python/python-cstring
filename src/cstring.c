#include <Python.h>

struct cstring {
    PyObject_VAR_HEAD
    char value[];
};

#define CSTRING_VALUE(self) (((struct cstring *)self)->value)

static PyObject *_cstring_new(PyTypeObject *type, const char *value, size_t len) {
    struct cstring *new = type->tp_alloc(type, len + 1);
    memcpy(new->value, value, len);
    new->value[len] = '\0';
    return (PyObject *)new;
}

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

static PyObject *cstring_str(PyObject *self) {
    return PyUnicode_FromString(CSTRING_VALUE(self));
}

static Py_ssize_t cstring_len(PyObject *self) {
    return Py_SIZE(self) - 1;
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
        return _cstring_new(Py_TYPE(self), "", 0);

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
    return _cstring_new(Py_TYPE(self), &CSTRING_VALUE(self)[i], 1);
}

static PySequenceMethods cstring_as_sequence = {
    .sq_length = cstring_len,
    .sq_concat = cstring_concat,
    .sq_repeat = cstring_repeat,
    .sq_item = cstring_item,
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
    .tp_str = cstring_str,
    .tp_as_sequence = &cstring_as_sequence,
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
