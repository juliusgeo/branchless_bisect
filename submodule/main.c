#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <stdio.h>
#include <stdlib.h>

static long bit_floor_shifted(long x) {
    long res = 1;
    while (x > 1) {
        x >>= 1;
        res <<= 1;
    }
    return res;
}

static PyObject* bl_bisect_left(PyObject *self, PyObject *args) {
    PyObject *list_obj;
    PyObject *value;

    if (!PyArg_ParseTuple(args, "OO", &list_obj, &value))
        return NULL;

    long size = PyList_Size(list_obj);

    long begin = 0, end = size;
    long length = end - begin;

    if (length == 0) {
        return PyLong_FromLong(end);
    }

    long step = bit_floor_shifted(length);

    if (step != length && PyObject_RichCompareBool(PyList_GetItem(list_obj, begin + step - 1), value, Py_LT) == 1) {
        begin += step;
        length -= step;
    }

    long i = begin;
    long next = 0;
    for (step >>= 1; step != 0; step >>=1) {
        next = i + step;
        if (next < size) {
            i += PyObject_RichCompareBool(PyList_GetItem(list_obj, next), value, Py_LT) * step;
        }
    }
    long result = i + (i < size && PyObject_RichCompareBool(PyList_GetItem(list_obj, i), value, Py_LT));
    return PyLong_FromLong(result);
}



static PyMethodDef bl_bl_methods[] = {
    {"bl_bisect_left",  bl_bisect_left, METH_VARARGS,
     "Branchless bisect left"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef bl_bl_module = {
    PyModuleDef_HEAD_INIT,
    "bl_bl",   /* name of module */
    NULL, /* module documentation, may be NULL */
    0,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    bl_bl_methods,
};

PyMODINIT_FUNC PyInit_bl_bl(void) {
    return PyModule_Create(&bl_bl_module);
}
