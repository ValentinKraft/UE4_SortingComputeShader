**Parallel Bitonic Sorting on the GPU by a Compute Shader**

This is a plugin for the Unreal Engine that realises parallel sorting on the GPU via compute shaders. It sorts three-dimensional vectors according to their distance to the current camera position. Currently only textures with maximum size of 1024\*1024 are supported, that is, 1 Million points can be sorted in total at maximum.

The texture/problem size has to be set manually in "ComputeShaderUsageExample.h" and "BitonicSortingKernelComputeShader.usf":

```CPP
#define BITONIC_BLOCK_SIZE 1024
#define TRANSPOSE_BLOCK_SIZE 16
```
```CPP
const UINT NUM_ELEMENTS = 1024 * 1024;
const UINT BITONIC_BLOCK_SIZE = 1024;
```

A tutorial might follow as well.
Tested in Unreal 4.18.

[![Demo](https://img.youtube.com/vi/jVZaz-0Y4Ek/0.jpg)](https://www.youtube.com/watch?v=jVZaz-0Y4Ek)
