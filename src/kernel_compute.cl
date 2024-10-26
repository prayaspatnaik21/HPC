__kernel void add_arrays(__global const float *A , __global const float *B , __global float *C)
{
    int index = get_global_id(0);
    C[index] = A[index]  + B[index];
}