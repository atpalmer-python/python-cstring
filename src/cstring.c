#include <Python.h>

struct cstring {
    PyObject_HEAD
};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cstring",
    .m_doc = "",
    .m_size = 0,
    .m_methods = NULL,
};

PyMODINIT_FUNC PyInit_cstring(void) {
    return PyModule_Create(&module);
}
