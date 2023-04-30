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
