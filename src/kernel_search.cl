__kernel void mult(__global float *a , __global float *b , __global float *result)
{
    int id = get_global_id(0);
    result[id] = a[id] * b[id];
}

__kernel void add(__global float *a , __global float *b , __global float *result)
{
    int id = get_global_id(0);
    result[id] = a[id] + b[id];
}

__kernel void sub(__global float *a , __global float *b , __global float *result)
{
    int id = get_global_id(0);
    result[id] = a[id] - b[id];
}