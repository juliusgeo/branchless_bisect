## Beating Bisect With Branchless Binary Search

I recently read this article: https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/, and I was inspired.
First I implemented the same algorithm in pure Python:

https://github.com/juliusgeo/branchless_bisect/blob/a98c576dd57be271bd4a2d60d4fee2da77645223/main.py#L10-L25

And then I compared it against `sortedcontainers`'s implementation of bisect_left across a large range of array sizes:

![image](https://user-images.githubusercontent.com/2801178/235347234-e6dc9e41-5a87-471f-b1c2-445699d651cd.png)

Pretty handily beats it! However, most people using Python would probably be using `bisect_left` from the `bisect` library in the stdlib.
To try to beat that implementation, though, I will have to descend into C-land. Here's my implementation as a Python C-extension:

https://github.com/juliusgeo/branchless_bisect/blob/a98c576dd57be271bd4a2d60d4fee2da77645223/submodule/main.c#L15-L45

![image](https://user-images.githubusercontent.com/2801178/235353568-fa0ed3d0-9b8d-47bc-980c-339fb3bd42c9.png)

That beats it as well! Admittedly this is only for arrays of size up to `2**29`, but still pretty cool.
I also checked whether it successfully compiled with a `CMOVE` instruction:

https://github.com/juliusgeo/branchless_bisect/blob/a98c576dd57be271bd4a2d60d4fee2da77645223/submodule/objdump_output.txt#L67-L71

It does! You can see the full compiled dump in `objdump_output.txt`.
Now, here are all of them combined (what you will get if you run `main.py`):

![image](https://user-images.githubusercontent.com/2801178/235348480-d843b4ed-23b9-4d17-b055-1d65ea45f5a7.png)

This repo contains the submodule and benchmarking code I used to obtain these results.
