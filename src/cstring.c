#include <Python.h>

struct cstring {
    PyObject_VAR_HEAD
    char value[];
};

#define CSTRING_VALUE(self) (((struct cstring *)self)->value)

static PyObject *cstring_new(PyTypeObject *type, PyObject *args, PyObject **kwargs) {
    char *value = NULL;

    if(!PyArg_ParseTuple(args, "s", &value))
        return NULL;

    int size = strlen(value) + 1;

    struct cstring *new = type->tp_alloc(type, size);
    memcpy(new->value, value, size);
    return (PyObject *)new;
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

static PySequenceMethods cstring_as_sequence = {
    .sq_length = cstring_len,
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
