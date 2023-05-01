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
static PyObject* bisect(PyObject *list_obj, PyObject *value, PyObject *compare){
    long size = PyList_Size(list_obj);

    long begin = 0, end = size;
    long length = end - begin;

    if (length == 0) {
        return PyLong_FromLong(end);
    }

    long step = bit_floor_shifted(length);

    if (step != length && PyObject_RichCompareBool(PyList_GetItem(list_obj, begin + step - 1), value, compare)) {
        begin += step;
        length -= step;
    }

    long next = 0;
    for (step >>= 1; step != 0; step >>=1) {
        if ((next = begin + step) < size) {
            begin += PyObject_RichCompareBool(PyList_GetItem(list_obj, next), value, compare) * step;
        }
    }
    return PyLong_FromLong(begin + (begin < size && PyObject_RichCompareBool(PyList_GetItem(list_obj, begin), value, compare)));
}
static PyObject* bl_bisect_left(PyObject *self, PyObject *args) {
    PyObject *list_obj;
    PyObject *value;
    if (!PyArg_ParseTuple(args, "OO", &list_obj, &value))
        return NULL;
    return bisect(list_obj, value, Py_LT);
}

static PyObject* bl_bisect_right(PyObject *self, PyObject *args) {
    PyObject *list_obj;
    PyObject *value;
    if (!PyArg_ParseTuple(args, "OO", &list_obj, &value))
        return NULL;
    return bisect(list_obj, value, Py_LE);
}

static PyMethodDef bl_bl_methods[] = {
    {"bl_bisect_left",  bl_bisect_left, METH_VARARGS,
     "Branchless bisect left"},
    {"bl_bisect_right",  bl_bisect_right, METH_VARARGS,
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
