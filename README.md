## Beating Bisect With Branchless Binary Search

I recently read this article: https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/, and I was inspired.
First I implemented the same algorithm in pure Python:

https://github.com/juliusgeo/branchless_bisect/blob/a98c576dd57be271bd4a2d60d4fee2da77645223/main.py#L10-L25

And then I compared it against `sortedcontainers`'s implementation of bisect_left across a large range of array sizes:

![image](/images/1.png "Figure 1")

Pretty handily beats it! However, most people using Python would probably be using `bisect_left` from the `bisect` library in the stdlib.
To try to beat that implementation, though, I will have to descend into C-land. Here's my implementation as a Python C-extension:

https://github.com/juliusgeo/branchless_bisect/blob/a98c576dd57be271bd4a2d60d4fee2da77645223/submodule/main.c#L15-L45

![image](/images/2.png "Figure 2")

That beats it as well! Admittedly this is only for arrays of size up to `2**29`, but still pretty cool.
I also checked whether it successfully compiled with a `CMOVE` instruction:

https://github.com/juliusgeo/branchless_bisect/blob/a98c576dd57be271bd4a2d60d4fee2da77645223/submodule/objdump_output.txt#L67-L71

It does! You can see the full compiled dump in `objdump_output.txt`.
Now, here are all of them combined (what you will get if you run `main.py`):

![image](/images/3.png "Figure 3")

This repo contains the submodule and benchmarking code I used to obtain these results.
