## Beating Bisect With Branchless Binary Search

I recently read this article: https://probablydance.com/2023/04/27/beautiful-branchless-binary-search/, and I was inspired.
First I implemented the same algorithm in pure Python:

https://github.com/juliusgeo/branchless_bisect/blob/f3dd6e38182b6489700c1d0456e78027aba50efb/main.py#L10-L25
And then I compared it against `sortedcontainers`'s implementation of bisect_left across a large range of array sizes:

![image](https://user-images.githubusercontent.com/2801178/235347234-e6dc9e41-5a87-471f-b1c2-445699d651cd.png)

Pretty handily beats it! However, most people using Python would probably be using `bisect_left` from the `bisect` library in the stdlib.
To try to beat that implementation, though, I will have to descend into C-land. Here's my implementation as a Python C-extension:

https://github.com/juliusgeo/branchless_bisect/blob/f3dd6e38182b6489700c1d0456e78027aba50efb/submodule/main.c#L15-L48

![image](https://user-images.githubusercontent.com/2801178/235347364-f06bd1ad-f362-4254-ba93-b261cce1c9bc.png)

That beats it as well! Admittedly this is only for arrays of size up to `2**29`, but still pretty cool.
I also checked whether it successfully compiled with a `CMOVE` instruction:
```

  ba:	48 89 c7             	mov    %rax,%rdi
  bd:	31 d2                	xor    %edx,%edx
  bf:	e8 00 00 00 00       	call   c4 <_bl_bisect_left+0xa4>
  c4:	83 f8 01             	cmp    $0x1,%eax
  c7:	4c 0f 44 fb          	cmove  %rbx,%r15
  cb:	48 83 fb 02          	cmp    $0x2,%rbx
  cf:	73 35                	jae    106 <_bl_bisect_left+0xe6>
  d1:	4d 39 f7             	cmp    %r14,%r15
  d4:	7d 5e                	jge    134 <_bl_bisect_left+0x114>
```
It does! You can see the full compiled dump in `objdump_output.txt`.
Now, here are all of them combined (what you will get if you run `main.py`):

![image](https://user-images.githubusercontent.com/2801178/235348480-d843b4ed-23b9-4d17-b055-1d65ea45f5a7.png)

This repo contains the submodule and benchmarking code I used to obtain these results.
