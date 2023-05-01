from sortedcontainers import SortedList
from bisect import bisect_left as bisect_left_c
from bl_bl import bl_bisect_left
bisect_left = lambda arr, value: arr.bisect_left(value)
from main import branchless_bisect_left
from timeit import timeit
import matplotlib.pyplot as plt
sizes = [i for i in range(0, 2**15)]
times = []
times_me = []
times_c = []
times_bl_c = []

num_trials = 20
for size in sizes:
    test_arr = list(range(size))
    value=size//30
    #times_me.append(timeit(lambda:branchless_bisect_left(test_arr, value), number=num_trials))
    test_arr2 = SortedList(test_arr)
    #times.append(timeit(lambda:bisect_left(test_arr2, value), number=num_trials))
    times_c.append(timeit(lambda:bisect_left_c(test_arr, value), number=num_trials))
    times_bl_c.append(timeit(lambda:bl_bisect_left(test_arr, value), number=num_trials))
    assert len(set([i(test_arr2 if i==bisect_left else test_arr, value) for i in [bisect_left, branchless_bisect_left, bisect_left_c, bl_bisect_left]])) ==1

#plt.plot(sizes, times, label="bisect_left")
plt.plot(sizes, times_c, label="bisect_left_c")
#plt.plot(sizes, times_me, label="branchless_bisect_left")
plt.plot(sizes, times_bl_c, label="branchless_bisect_left_c")
plt.xlabel('Input Size')
plt.ylabel('Execution Time (s)')
plt.title('Performance vs Input Size')
plt.legend()

plt.show()