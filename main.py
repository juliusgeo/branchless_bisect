def branchless_bisect_left(arr, value):
    begin, end=0,len(arr)
    length = end - begin
    if length == 0:
        return end
    step = 1 << (length.bit_length() - 1)
    if step != length and arr[step]<value:
        length -= step + 1
        if length == 0:
            return end
        begin = end - step
    step >>= 1
    for s in (step:=step >> 1 for _ in range(step.bit_length())):
        begin += s*(arr[s+begin]<value)
    return begin+int(arr[begin]<value)

def branchless_bisect_right(arr, value):
    begin, end=0,len(arr)
    length = end - begin
    if length == 0:
        return end
    step = 1 << (length.bit_length() - 1)
    if step != length and arr[step]<=value:
        length -= step + 1
        if length == 0:
            return end
        begin = end - step
    step >>= 1
    for s in (step:=step >> 1 for _ in range(step.bit_length())):
        begin += s*(arr[s+begin]<=value)
    return begin+int(arr[begin]<=value)
