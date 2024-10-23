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
4. The first step in every host application is to access cl_platform_id.

Accessing Installed Devices
---------------------------

1. Once we can access vendor's platform , we can access every connected device provided by the vendor.
2. In OpenCL application , devices receive tasks and data from the host.
3. In code , cl_device_id structures represent devices.

4. cl_device_id represents device.
5. The clGetDeviceIDs funtion makes this possible. It populates a cl_device_id array with structures corresponding to OpenCL devices.
6. OpenCL Device Types
    CL_DEVICE_TYPE_ALL                  Identifies all devices associated with the platform
    CL_DEVICE_TYPE_DEFAULT              Identifies devices associated with the platform’s default type
    CL_DEVICE_TYPE_CPU                  Identifies the host processor
    CL_DEVICE_TYPE_GPU                  Identifies a device containing a graphics processor unit (GPU)
    CL_DEVICE_TYPE_ACCELERATOR          Identifies an external device used to accelerate computation
7.clGetDeviceInfo provides information about the corresponding device.
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
    size_t ext_size;
    const char icd_ext[] = "cl_khr_icd";

    // Find number of Platforms
    err = clGetPlatformIDs(1 , NULL , &num_platforms);
    if(err < 0)
    {
        perror("Couldn't find any platforms");
        exit(1);
    }

    // Allocate platform array
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);

    // Initialize platform array
    clGetPlatformIDs(num_platforms, platforms, NULL);

    // Find size of Extension Data
    for(i = 0  ; i < num_platforms ; i++)
    {
        err = clGetPlatformInfo(platforms[i] , CL_PLATFORM_EXTENSIONS , 0 , NULL , &ext_size);
        if(err < 0)
        {
            perror("Couldn't read extension data");
            exit(1);
        }

        // Read Extension data
        ext_data = (char*)malloc(ext_size);
        clGetPlatformInfo(platforms[i] , CL_PLATFORM_EXTENSIONS, ext_size , ext_data , NULL);
        printf("Platform %d supports extensions: %s\n", i , ext_data);

        if(strstr(ext_data , icd_ext) != NULL)
        {
            free(ext_data);
            platform_index = i;
            break;
        }
        
        free(ext_data);
    }

    if(platform_index > -1)
    {
        printf("Platform %d supports the %s extension. \n", platform_index , icd_ext);
    }
    else
    {
        printf("No Platforms support the %s extension. \n", icd_ext);
    }

    free(platforms);
}
int main()
{
    platform_extension_test();
    device_extension_test();
}