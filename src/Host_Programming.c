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

Managing Device with contexts
-----------------------------

1. In OpenCL , a context identifies a set of devices. Contexts makes it possible to create command queues , the structures that allow hosts to send kernels to 
   devices.
2. The Devices in a context must be provided by the same plaform. We can't create a context containing both AMD and Nvidia devices. We have to create a different 
   context containing both AMD and Nvidia devices.
3. Host application can manage devies using more than one context, and it can create multiple contexts from devices in a single platform.
4. Devices from different platforms can't be places in the same context, but a host application can still utilize multiple contexts during its processing.
5. Host can send kernels and other commands to Nvidia and AMD devices, but the devices can't share the resources associated with contexts.
6. Creating Contexts
    1. represented by cl_context data structures.
    2. clCreateContextFromType and clCreateContext return cl_contexts instead of  error codes. You don't have to allocate and deallocate memory for the structure.
       If the cl_context is declared as a local variable , its memory will be automatically freed when the enclosing function terminates.
    3. External routines, such as those in a third-party library, might need to continue accessing the cl_context after its enclosing function terminates. 
       For this reason, OpenCL keeps track of how many times cl_context structures are accessed. This number is called the retain count or the reference count.
       It’s set to 1 when the structure is created, and when the count equals 0, the structure is deallocated.
    4. clRetainContext increments the count and clReleaseContext decrements the count.
7. Storing Device Code in Programs

    1. A program is represented by a cl_program data strucuture.
    2. Creating Programs
        1. clCreateProgramWithSource and clCreateProgramWithBinary.
        2. Does not accept filenames or file handles.
        3. Read the file's content into a buffer and pass that buffer.

8. Building Programs

    1. clBuildProgram(function) compiles and links a cl_program for devices associated with the platform. It doesn't return a new cl_program.
    2. Modifies the input data structure.

9. Obtaining Program Information

    1. clGetProgramInfo and clGetProgramBuildInfo. Provides information about data structures associated with the program, such as context and target devices.
    2. clGetProgramBuildInfo is a vital function to know. It’s the only way to find out
       what happened during the program’s build process.
    3. To find out why a build failed, you need to invoke clGetProgramBuildInfo with
        the CL_PROGRAM_BUILD_LOG parameter.
    4. After clBuildProgram returns a negative value, the clGetProgramBuildInfo function will place the compiler’s error message in the
       program_log buffer.

10. Packaging functions in kernels

    1. Kernel is represented by cl_kernel data structure.
    2. clCreateKernelsInProgram - construct cl_kernel structures from a cl_program.
    3. If you want to create a single kernel , clCreateKernel Function.
    4. Obtaining Kernel Information
        1. if you want to check about the created cl_kernel (which function it represents) and which program it belongs to.
        2. clGetKernelInfo is the function to use.

11. Collecting kernels in a command queue.

    1. A command is a message sent from the host that tells a device to perform an operation.
    2. Besides kernel execution , many OpenCL command operations involve data transfers, reading data from the device to the hsot
       writing data from the host to the device , copying data between device.
    3. Each device has its own command queue.
    4. Commands in a command queue move in one direction only, from host to device.
    5. The device doesn't send commands to the host.
    6. By default , the command queues process commands in the order in which they're received,
       but you can change this default behavior when you create a command queue.
    7. Creating command queues

        1. Command queues represented by cl_command_queue strucutres.
        2. clCreateCommandQueue functions.
        3. This returns a cl_command_queue whose reference count can be incremented with clRetainCommandQueue and decremented with clReleaseCommandQueue.
    8. Enqueuing kernel execution commands

        1. OpenCL provides many functions that start with clEnqueue, and each of them dispatches a command to a device through a command queue. The simplest of these is
           clEnqueueTask, which sends a kernel execution command to a device through a command queue
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


#define NUM_FILES 2
#define PROGRAM_FILE_1 "good.cl"
#define PROGRAM_FILE_2 "bad.cl"
#define PROGRAM_FILE_3 "kernel_search.cl"
#define KERNEL_FUNC_NAME "mult"
#define ARRAY_SIZE 1024
#define KERNEL_SOURCE "kernel_compute.cl"
//----------------------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------------------
void device_extension_test()
{
    printf("%s\n", "****************************************************");
    cl_platform_id platform;
    cl_device_id *devices;
    cl_uint num_devices , addr_data;
    cl_int i , err;
    char name_data[48], ext_data[4096];

    // Access first Platform
    err = clGetPlatformIDs(1 , &platform , NULL);

    if(err < 0)
    {
        perror("Couldn't find any plaforms");
        exit(1);
    }

    // Determine number of Devices
    err = clGetDeviceIDs(platform , CL_DEVICE_TYPE_ALL , 1 , NULL , &num_devices);
    if(err < 0)
    {
        perror("Couldn't find any devices");
        exit(1);
    }

    //Allocate device memory
    devices = (cl_device_id*)malloc(sizeof(cl_device_id)* num_devices);

    // Populate Device Memory
    clGetDeviceIDs(platform , CL_DEVICE_TYPE_ALL, num_devices, devices , NULL);

    // Get Device Name
    for(i = 0  ; i < num_devices ; i++)
    {
        err = clGetDeviceInfo(devices[i] , CL_DEVICE_NAME , sizeof(name_data) , name_data , NULL);
        if(err < 0)
        {
            perror("Couldn't read extension data");
            exit(1);
        }

        // Get Device address width
        clGetDeviceInfo(devices[i] , CL_DEVICE_ADDRESS_BITS , sizeof(ext_data) , &addr_data , NULL);

        // Get Device extensions
        clGetDeviceInfo(devices[i] , CL_DEVICE_EXTENSIONS, sizeof(ext_data) , ext_data , NULL);

        printf("Name: %s \n Address_width : %u \n Extensions: %s", name_data , addr_data , ext_data);
    }

    free(devices);
}
//----------------------------------------------------------------------------------------------------------------------------------
void context_count()
{
    printf("%s\n", "****************************************************");
    cl_platform_id platform;
    cl_context context;
    cl_uint ref_count;
    cl_device_id devices;
    cl_int i , err;

    // Create Platform
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0) {
        perror("Couldn't find an OpenCL platform");
        exit(1);
    }

    // Get device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &devices, NULL);
    if (err < 0) {
        perror("Couldn't find an OpenCL device");
        exit(1);
    }
    //Create context
    context = clCreateContext(NULL, 1 , &devices, NULL , NULL , &err);

    if(err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    err = clGetContextInfo(context , CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count) , &ref_count , NULL);

    if(err < 0)
    {
        perror("Couldn't read the reference count");
        exit(1);
    }

    printf("Initial Reference count: %u \n", ref_count);

    clRetainContext(context);
    clGetContextInfo(context , CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count) , &ref_count , NULL);

    printf("Reference Count : %u\n", ref_count);

    clReleaseContext(context);
    clGetContextInfo(context , CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count) , &ref_count , NULL);
    printf("Reference count : %u \n", ref_count);
    clReleaseContext(context);
}

//----------------------------------------------------------------------------------------------------------------------------------
void program_build()
{
    printf("%s\n", "****************************************************");
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_program program;

    FILE *program_handle;
    char *program_buffer[NUM_FILES];
    char *program_log;
    const char *file_name[] = {PROGRAM_FILE_1 , PROGRAM_FILE_2};
    const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";
    size_t program_size[NUM_FILES];
    size_t log_size;
    cl_int err , i;

    // Get Platform
    err = clGetPlatformIDs(1 , &platform , NULL);
    if(err < 0)
    {
        perror("Couldn't find an OpenCL Platform");
        exit(1);
    }


    // Get Device
    err = clGetDeviceIDs(platform , CL_DEVICE_TYPE_GPU , 1 , &device , NULL);
    if(err < 0)
    {
        perror("Couldn't find an OpenCL Device");
        exit(1);
    }

    // Create Context
    context = clCreateContext(NULL , 1 , &device , NULL , NULL , &err);
    if(err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    // Load program files into memory
    for(i = 0 ; i < NUM_FILES ; i++)
    {
        program_handle = fopen(file_name[i] , "r");
        if(program_handle == NULL)
        {
            perror("Couldn't find the program file");
            exit(1);
        }

        // find the size of the source file
        fseek(program_handle , 0 , SEEK_END);
        program_size[i] = ftell(program_handle);
        rewind(program_handle);

        // Read the source code into the buffer
        program_buffer[i] = (char*)malloc(program_size[i] + 1);
        program_buffer[i][program_size[i]]= '\0';
        fread(program_buffer[i] , sizeof(char) , program_size[i] , program_handle);
        fclose(program_handle);
    }

    // Create program from source
    program = clCreateProgramWithSource(context , NUM_FILES , (const char**)program_buffer , program_size , &err);
    if(err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }

    // Build Program
    err = clBuildProgram(program , 1 , &device , options , NULL , NULL);
    if(err < 0)
    {
        // Find size of the build log and print it
        clGetProgramBuildInfo(program , device , CL_PROGRAM_BUILD_LOG , 0 , NULL , &log_size);

        // Allocate memory for the log and print it
        program_log  = (char*)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program , device , CL_PROGRAM_BUILD_LOG, log_size + 1, program_log , NULL);
        printf("Build log : /n%s\n", program_log);
        free(program_log);
        exit(1);
    }

    printf("Program built successfully.\n");

    // Free Resources
    for(i = 0 ; i < NUM_FILES ; i++)
    {
        free(program_buffer[i]);
    }

    clReleaseProgram(program);
    clReleaseContext(context);
}

//----------------------------------------------------------------------------------------------------------------------------------
void kernel_search()
{
    printf("%s\n", "****************************************************");
    //Initialize openCL variables
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel *kernels , found_kernel;
    cl_command_queue queue;
    cl_int err;
    cl_uint num_kernels;
    char kernel_name[20];

    // Obtain platform and device
    err = clGetPlatformIDs(1 , & platform , NULL);
    if(err < 0)
    {
        perror("Couldn't find any platforms");
        exit(1);
    }

    err = clGetDeviceIDs(platform , CL_DEVICE_TYPE_GPU, 1 , &device , NULL);
    if(err < 0)
    {
        perror("Couldn't find any devices");
        exit(1);
    }

    // Create a context
    context = clCreateContext(NULL , 1 , &device , NULL , NULL , &err);
    if(err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    // Load kernel source file
    FILE *program_handle = fopen(PROGRAM_FILE_3 , "r");
    if(program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }

    fseek(program_handle , 0 , SEEK_END);
    size_t program_size = ftell(program_handle);
    rewind(program_handle);

    char * program_buffer = (char*) malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer , sizeof(char) , program_size , program_handle);
    fclose(program_handle);

    // create program from source
    program = clCreateProgramWithSource(context , 1 , (const char**)&program_buffer, &program_size, &err);
    free(program_buffer);

    if(err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }

    // Build Program
    err = clBuildProgram(program , 1 , &device , NULL , NULL , NULL);
    if(err < 0)
    {
        size_t log_size;
        clGetProgramBuildInfo(program , device , CL_PROGRAM_BUILD_LOG , 0 , NULL , &log_size);
        char *program_log = (char*)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program , device , CL_PROGRAM_BUILD_LOG , log_size + 1 , program_log , NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    //Find kernels
    err = clCreateKernelsInProgram(program , 0 , NULL , &num_kernels);
    if(err < 0)
    {
        perror("Couldn't find any kernels");
        exit(1);
    }

    printf("Number of kernels : %u\n", num_kernels);

    kernels = (cl_kernel*)malloc(num_kernels * sizeof(cl_kernel));
    clCreateKernelsInProgram(program , num_kernels , kernels , NULL);

    // search for the target kernel
    for(cl_uint i = 0 ; i < num_kernels ; i++)
    {
        clGetKernelInfo(kernels[i] , CL_KERNEL_FUNCTION_NAME , sizeof(kernel_name) , kernel_name , NULL);
        if(strcmp(kernel_name , KERNEL_FUNC_NAME) == 0)
        {
            found_kernel = kernels[i];
            printf("Found the kernel '%s' at index %u.\n", KERNEL_FUNC_NAME , i);
            break;
        }
    }

    // Release Resources
    for(cl_uint i = 0 ; i < num_kernels ; i++)
    {
        clReleaseKernel(kernels[i]);
    }
    free(kernels);
    clReleaseProgram(program);
    clReleaseContext(context);

}

void queue_kernel()
{
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem bufferA , bufferB , bufferC;

    float A[ARRAY_SIZE] , B[ARRAY_SIZE] , C[ARRAY_SIZE];

    // Initialize input arrays with sample data
    for(int i = 0 ; i < ARRAY_SIZE ; i++)
    {
        A[i] =  i * 1.0f;
        B[i] = (ARRAY_SIZE - i) * 1.0f;
    }

    // 1. Set up the OpenCL environment
    /*
        cl_int clGetPlatformIDs(cl_uint num_entries ,
                                cl_platform_id* platforms ,
                                cl_uint* num_platforms)
        
        1. Gives the list of platforms available can be obtained with the function.
        2. num_entries is the number of cl_platform_id entries that can be added to platforms. If platforms is not NULL, num_entries must be greater than zero. 
        3. platforms returns a list of OpenCL platforms found. The cl_platform_id values returned in platforms can be used to identify a specific OpenCL platform. If platforms is NULL, this argument is ignored. The number of OpenCL platforms returned is the minimum of the value specified by num_entries or the number of OpenCL platforms available.
        4. num_platforms returns the number of OpenCL platforms available. If num_platforms is NULL, this argument is ignored.

        returns - CL_SUCCESS if the function is executed and , if the cl_khr_icd extension is supported, there are a non-zero number of platforms available.otherwise
                  returns error.
    */
    err = clGetPlatformIDs(1 , &platform , NULL);

    /*
        cl_int clGetDeviceIDs( cl_platform_id platform,
                               cl_device_type device_type,
                               cl_uint num_entries,
                               cl_device_id* devices,
                               cl_uint* num_devices)

        1. platform refers to the platform ID returned by clGetPlatformIDs or can be NULL. If platform is NULL, the behavior is implementation-defined.
        2. device_type is a bitfield that identifies the type of OpenCL device. The device_type can be used to query specific OpenCL devices or all OpenCL devices
           available. The valid values for device_type are specified in the Device Types table.
        3. num_entries is the number of cl_device_id entries that can be added to devices. If devices is not NULL, the num_entries must be greater than zero.
        4. devices returns a list of OpenCL devices found. The cl_device_id values returned in devices can be used to identify a specific OpenCL device. If devices is NULL, this argument
           is ignored. The number of OpenCL devices returned is the minimum of the value specified by num_entries or the number of OpenCL devices whose type matches
           device_type.
        5. num_devices returns the number of OpenCL devices available that match device_type. If num_devices is NULL, this argument is ignored.

        returns -  CL_SUCCESS if the function is executed successfully. Otherwise, it returns error.
    */
    err |= clGetDeviceIDs(platform , CL_DEVICE_TYPE_GPU , 1 , &device , NULL);

    /*
        cl_context clCreateContext(const cl_context_properties* properties,
                                   cl_uint num_devices,
                                   const cl_device_id* devices,
                                   void (CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info, size_t cb, void* user_data),
                                   void* user_data,
                                   cl_int* errcode_ret);

        1. properties specifies a list of context property names and their corresponding values. Each property name is immediately followed by the corresponding 
           desired value. The list is terminated with 0. The list of supported properties, and their default values if not present in properties, is described in
           the Context Properties table. properties can be NULL, in which case all properties take on their default values.
        2. num_devices is the number of devices specified in the devices argument.
        3. devices is a pointer to a list of unique devices returned by clGetDeviceIDs or sub-devices created by clCreateSubDevices for a platform. [11]
        4. pfn_notify is a callback function that can be registered by the application. This callback function will be used by the OpenCL implementation to report information on errors during context creation as well as errors that occur at runtime in this context. This callback function may be called asynchronously by the OpenCL implementation. It is the application’s responsibility to ensure that the callback function is thread-safe. If pfn_notify is NULL, no callback function is registered.
        5. user_data will be passed as the user_data argument when pfn_notify is called. user_data can be NULL.
        6. errcode_ret will return an appropriate error code. If errcode_ret is NULL, no error code is returned.


        An OpenCL context is a fundamental part of the OpenCL programming model that provides the environment within which OpenCL operations are executed.
        Think of the context as a workspace that binds together the different components needed to execute programs on a device, such as the GPU, CPU, or other 
        accelerators.
    */
    context = clCreateContext(NULL , 1 , &device , NULL , NULL, &err);

    // 2. Create command queue
    cl_command_queue_properties props[] = {CL_QUEUE_PROPERTIES , 0 , 0};
    queue = clCreateCommandQueueWithProperties(context , device , props , &err);

    // 3. Load and Build the kernel program
    FILE *program_handle = fopen(KERNEL_SOURCE , "r");
    if(program_handle == NULL)
    {
        perror("Couldn't find the kernel file");
    }
    fseek(program_handle , 0 , SEEK_END);
    size_t program_size = ftell(program_handle);
    rewind(program_handle);
    char *program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer , sizeof(char) , program_size , program_handle);
    fclose(program_handle);

    program = clCreateProgramWithSource(context , 1 , (const char **)&program_buffer , &program_size , &err);
    free(program_buffer);
    err = clBuildProgram(program , 1 , &device , NULL , NULL , NULL);

    if (err < 0) {
        // Print build log in case of an error
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = (char *)malloc(log_size + 1);
        log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, log, NULL);
        printf("%s\n", log);
        free(log);
        exit(1);
    }
    // 4. Create the kernel
    kernel = clCreateKernel(program , "add_arrays" , &err);

    if (err != CL_SUCCESS) {
        printf("Error creating kernel: %d\n", err);
        exit(1);
    }
    // 5. Create Buffers and transfer data
    bufferA = clCreateBuffer(context , CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR , ARRAY_SIZE * sizeof(float) , A , &err);
    if (err != CL_SUCCESS) {
        printf("Error creating bufferA: %d\n", err);
        exit(1);
    }
    
    bufferB = clCreateBuffer(context , CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR , ARRAY_SIZE * sizeof(float) , B , &err);
    if (err != CL_SUCCESS) {
        printf("Error creating bufferB: %d\n", err);
        exit(1);
    }

    bufferC = clCreateBuffer(context , CL_MEM_WRITE_ONLY  , ARRAY_SIZE * sizeof(float) , NULL , &err);
    if (err != CL_SUCCESS) {
        printf("Error creating bufferC: %d\n", err);
        exit(1);
    }
    // 6. Set kernel arguments
    clSetKernelArg(kernel , 0 , sizeof(cl_mem) , &bufferA);
    clSetKernelArg(kernel , 1 , sizeof(cl_mem) , &bufferB);
    clSetKernelArg(kernel , 2 , sizeof(cl_mem) , &bufferC);

    // 7. Enqueue Kernel Execution
    size_t global_size = ARRAY_SIZE;
    err = clEnqueueNDRangeKernel(queue , kernel , 1 , NULL , &global_size , NULL , 0 , NULL , NULL);

    // 8. Read the result back to host
    err = clEnqueueReadBuffer(queue , bufferC , CL_TRUE , 0 , ARRAY_SIZE * sizeof(float), C , 0 , NULL , NULL);

    if (err != CL_SUCCESS) {
    printf("Error during X step: %d\n", err);  // Replace X with the appropriate step
    exit(1);
    }
    // 9. Verify Results
    int correct = 1;
    for(int i = 0 ; i < ARRAY_SIZE ; i++)
    {
        float expected = A[i] + B[i];
        
        if(C[i] != expected)
        {
            correct = 0;
            printf("Mismatch at index %d : Expected %f but got %f \n", i , expected , C[i]);
            break;
        }
    }

    if(correct){printf("Results are correct! \n");}
    else printf("Results are incorrect. \n");

    // 10 . clean up
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
int main()
{
    platform_extension_test();
    device_extension_test();
    context_count();
    program_build();

    kernel_search();
    queue_kernel();
}