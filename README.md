## Beating Bisect With Branchless Binary Search

I recently read this article: https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/, and I was inspired.
First I implemented the same algorithm in pure Python:
```
def branchless_bisect_left(arr, value):
    begin, end=0,len(arr)
    length = end - begin
    if length == 0:
        return end
    step = bit_floor(length)
    if step != length and arr[step]<value:
        length -= step + 1
        if length == 0:
            return end
        step = bit_ceil(length)
        begin = end - step
    step >>= 1
    while step:=step >> 1:
        begin += step if arr[step+begin]<value else 0
    return begin+int(arr[begin]<value)
```
And then I compared it against `sortedcontainers`'s implementation of bisect_left across a large range of array sizes:
![image](https://user-images.githubusercontent.com/2801178/235347234-e6dc9e41-5a87-471f-b1c2-445699d651cd.png)

Pretty handily beats it! However, most people using Python would probably be using `bisect_left` from the `bisect` library in the stdlib.
To try to beat that implementation, though, I will have to descend into C-land. Here's my implementation as a Python C-extension:
```
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
```
![image](https://user-images.githubusercontent.com/2801178/235347364-f06bd1ad-f362-4254-ba93-b261cce1c9bc.png)

That beats it as well! Admittedly this is only for arrays of size up to `2**29`, but still pretty cool.

This repo contains the submodule and benchmarking code I used to obtain these results.