/*

1. Host Programming: Fundamental Data structures
------------------------------------------------

Accessing Platforms
-------------------

1. cl_platform_id - Data structure which tells the OpenCL devices available.
2. Represents a different OpenCl implementation called a platoform installed on the host.
3. clGetPlatformIDs initialize these data structures

    * cl_int clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms)

4. The function places cl_platform_id structures in the memory referenced by platforms.(Provides an array of cl_platform_id structures).
5. It places the number of available platforms in the memory referenced by num_platforms.
6. It returns an integer , 0 for success and negative for failure.
7. num_entries identifies the maximum number of platforms you are interested in detecting.(If it is zero , the function return error)
8. This will be maximum number of cl_platform_id structures that will be placed in the platforms array.
9. num_platforms is the number of platforms detected on the host.
10. either platforms or num_platform can be set to NULL. if you want to create a cl_platform_id structure for every
    platform on your system, you have to know in advance how many platforms are installed.


Obtaining Platform Information
------------------------------

1. clGetPlatformInfo provides the opencl version a platform supports or which vendor created it.
2. char pform_vendor[40];
   clGetPlatformInfo(platforms[0], CL_PLATFORM_VENDOR, sizeof(pform_vendor), &pform_vendor, NULL);
    This code allocates the char array first and calls clGetPlatformInfo second. This poses no problem, because a vendor’s name is unlikely to exceed 40 characters. 
3. If you don't know the size of the extensions a platform supports , call clGetPlatformInfo twice.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MAC
#include <opencl-c-base.h>/cl.h>
#else
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#endif

void platform_extension_test()
{
    cl_platform_id* platforms;
    cl_uint num_platforms;
    cl_int i , err , platform_index = -1;

    char* ext_data;
}
int main()
{
    platform_extension_test();
}