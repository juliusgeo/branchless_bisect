#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(__OPTIMIZE__)

/* LIKELY(), UNLIKELY() definition */
/* Checks taken from
    https://github.com/python/cpython/blob/main/Objects/obmalloc.c */
#if defined(__GNUC__) && (__GNUC__ > 2)
#  define UNLIKELY(value) __builtin_expect((value), 0)
#  define LIKELY(value) __builtin_expect((value), 1)
#else
#  define UNLIKELY(value) (value)
#  define LIKELY(value) (value)
#endif

/* ASSUME() definition */
#ifdef __clang__
#  define ASSUME(value) (void)__builtin_assume(value)
#elif defined(__GNUC__) && (__GNUC__ > 4) && (__GNUC_MINOR__ >= 5)
/* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79469
   `__builtin_object_size(( (void)(value), "" ), 2)` checks whether the expression is constant. */
#  define ASSUME(value) \
    (__builtin_object_size(( (void)(value), "" ), 2) \
        ? ((value) ? (void)0 : __builtin_unreachable()) \
        : (void)0 \
    )
#elif defined(_MSC_VER)
/* We don't have the "just to make sure it's constant" check here. */
#  define ASSUME(value) (void)__assume(value)
#else
#  define ASSUME(value) ((void)0)
#endif

#endif /* __OPTIMIZE__ */

static inline Py_ALWAYS_INLINE PyObject *
bisect(PyObject *list_obj, PyObject *value, int compare)
{
    int res, retres;
    PyObject *resitem;
    Py_ssize_t size, step, begin = 0;

    size = PyList_GET_SIZE(list_obj);

    if (UNLIKELY(size == 0)) {
        return PyLong_FromLong(0L);
    }

#ifdef __GNUC__
    step = 1 << (63 - __builtin_clzll(size));
#elif defined(_MSC_VER)
    {
        unsigned lzcnt;
        assert(_BitScanReverse64(&lzcnt, size));
        step = 1 << (63 - lzcnt);
    }
#else
    {
        step = 1;
        Py_ssize_t length = size;
        while ((length >>= 1)) {
            step <<= 1;
        }
    }
#endif

    if (LIKELY(step != size)) {
        res = PyObject_RichCompareBool(PyList_GET_ITEM(list_obj, begin + step - 1), value, compare);
        if (UNLIKELY(res == -1)) {
            return NULL;
        }
        ASSUME(res == 0 || res == 1);
        begin += res * step;
    }

    resitem = PyList_GET_ITEM(list_obj, begin);
    retres = -1;

    Py_ssize_t next = 0;
    for (step >>= 1; step != 0; step >>= 1) {
        if (LIKELY((next = begin + step) < size)) {
            PyObject *item = PyList_GET_ITEM(list_obj, next);
            res = PyObject_RichCompareBool(item, value, compare);
            if (UNLIKELY(res == -1)) {
                return NULL;
            }
            ASSUME(res == 0 || res == 1);
            if (res) {
                begin = next;
                resitem = item;
                retres = res;
            }
        }
    }

    if (UNLIKELY(retres == -1)) {
        retres = PyObject_RichCompareBool(resitem, value, compare);
        if (UNLIKELY(retres == -1)) {
            return NULL;
        }
        ASSUME(res == 0 || res == 1);
    }
    return PyLong_FromSsize_t(begin + (begin < size && retres));
}

static PyObject *
bl_bisect_left(PyObject *self,
               PyObject *const *args,
               Py_ssize_t nargs)
{
    PyObject *list_obj;
    PyObject *value;

    if (UNLIKELY(nargs != 2)) {
        PyErr_Format(PyExc_TypeError,
                     "bisect_left() expected 2 arguments, got %zd",
                     nargs);
        return NULL;
    }

    list_obj = args[0];
    if (UNLIKELY(!PyList_CheckExact(list_obj))) {
        PyErr_Format(PyExc_TypeError,
                     "bisect_left() expected 'list' (argument 0), got '%.200s'",
                     Py_TYPE(list_obj)->tp_name);
        return NULL;
    }

    value = args[1];

    return bisect(list_obj, value, Py_LT);
}

static PyObject *
bl_bisect_right(PyObject *self,
                PyObject *const *args,
                Py_ssize_t nargs)
{
    PyObject *list_obj;
    PyObject *value;

    if (UNLIKELY(nargs != 2)) {
        PyErr_Format(PyExc_TypeError,
                     "bisect_right() expected 2 arguments, got %zd",
                     nargs);
        return NULL;
    }

    list_obj = args[0];
    if (UNLIKELY(!PyList_CheckExact(list_obj))) {
        PyErr_Format(PyExc_TypeError,
                     "bisect_right() expected 'list' (argument 0), got '%.200s'",
                     Py_TYPE(list_obj)->tp_name);
        return NULL;
    }

    value = args[1];

    return bisect(list_obj, value, Py_LE);
}

static PyMethodDef bl_bl_methods[] = {
    {"bl_bisect_left",  (PyCFunction)(void(*)(void))bl_bisect_left, METH_FASTCALL,
     "Branchless bisect left"},
    {"bl_bisect_right",  (PyCFunction)(void(*)(void))bl_bisect_right, METH_FASTCALL,
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
