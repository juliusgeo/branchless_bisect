from sortedcontainers import SortedList
from bisect import bisect_left as bisect_left_c
from bl_bl import bl_bisect_left
bisect_left = lambda arr, value: arr.bisect_left(value)
def bit_floor(x):
    return 2 ** (x.bit_length() - 1) if x !=0 else 0
def bit_ceil(x):
    return 1 if x<=0 else 1 << (bits) if x != (1 << ((bits := x.bit_length())- 1)) else x

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

from timeit import timeit
import matplotlib.pyplot as plt
sizes = [2**i for i in range(1, 29)]
times = []
times_me = []
times_c = []
times_bl_c = []

num_trials = 20
for size in sizes:
    test_arr = list(range(size))
    value=size//30
    times_me.append(timeit(lambda:branchless_bisect_left(test_arr, value), number=num_trials))
    test_arr2 = SortedList(test_arr)
    times.append(timeit(lambda:bisect_left(test_arr2, value), number=num_trials))
    times_c.append(timeit(lambda:bisect_left_c(test_arr, value), number=num_trials))
    times_bl_c.append(timeit(lambda:bl_bisect_left(test_arr, value), number=num_trials))
    assert len(set([i(test_arr2 if i==bisect_left else test_arr, value) for i in [bisect_left, branchless_bisect_left, bisect_left_c, bl_bisect_left]])) ==1

plt.plot(sizes, times, label="bisect_left")
plt.plot(sizes, times_c, label="bisect_left_c")
plt.plot(sizes, times_me, label="branchless_bisect_left")
plt.plot(sizes, times_bl_c, label="branchless_bisect_left_c")
plt.xlabel('Input Size')
plt.ylabel('Execution Time (s)')
plt.title('Performance vs Input Size')
plt.legend()

plt.show()