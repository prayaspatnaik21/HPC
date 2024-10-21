/*

1. OpenCL (Open Computing Language)
2. OpenCL routines can be executed on GPUs and CPUs from major manufacturers like AMD , Nvidia , Intel.
3. Non Proprietary- based on public standard.
4. OpenCL standard defines a set of data types , data structures and functions that augment C and C++.
5. OpenCL - Portable , Standardized Vector Processing
    5.1 Computational Vector - A data structure that contains multiple elements of the same data type. During a vector operation, 
                               each element(called a component) is operated upon in the same clock cycle.
    5.2 vector instructions are usually vendor-specific. Intel processors use SSE extensions, Nvidia devices require PTX instructions,
        and IBM devices rely on AltiVec instructions to process vectors. These instruction sets have nothing in common.
        But with OpenCL, you can code your vector routines once and run them on any compliant processor. When you compile your application
        Nvidia’s OpenCL compiler will produce PTX instructions. An IBM compiler for OpenCL will produce AltiVec
        instructions. Clearly, if you intend to make your high-performance application available on multiple platforms, 
        coding with OpenCL will save you a great deal of time.
6. An OpenCL application can configure different devices to perform different tasks,and each task can operate on different data.
   In other words, OpenCL provides full task-parallelism. This is an important advantage over many other parallel-programming
   toolsets, which only enable data-parallelism.
7. Portability, vector processing, and parallel programming make OpenCL more powerful than regular C and C++,

8. Most of the code is devoted to creating OpenCL's data structure.
9. The creation of the cl_program and the cl_kernel structure changes from application to application.

10. The OpenCL Working Group doesn’t provide any frameworks of its own. Instead,
    vendors who produce OpenCL-compliant devices release frameworks as part of their
    software development kits (SDKs). The two most popular OpenCL SDKs are released
    by Nvidia and AMD.
*/

/*
    1. Set up file paths and function names for the OpenCL kernel file and the kernel function to be executed on the device (GPU)
*/
#define PROGRAM_FILE "mat_vec.cl"
#define KERNEL_FUNC "mat_vec_mult"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/*
    1. This block handles platform-specific differences between macOS and other systems.
       macOS uses a different path(<OpenCL/cl.h>) to include the OpenCL header , while linux
       or windows uses <CL/cl.h>.
    2. The CL_TARGET_OPENCL_VERSION 300 ensures the program uses OpenCL 3.0 API functions and structures.
*/
#ifdef MAC
#include <OpenCL/Cl.h>
#else
#define CL_TARGET_OPENCL_VERSION 300  // For OpenCL 2.0, or use 300 for OpenCL 3.0
#include <CL/cl.h>
#endif

int main() {

    /*

        1. Declares OpenCL Platform, device , context , command queue and error tracking.
        2. cl_platform_id : Identifies the platform (vendor -> intel , AMD , Nvidia)
        3. cl_device_id : Identifies the specific device (eg. GPU or CPU)
        4. cl_context : Represents the context in which the OpenCL Program executes (memory , devices)
        5. cl_command_queue : A queue for executing commands (kernel executions , memory transfers).
        6. cl_int err : Used to check for errors during OpenCL function calls.
    */

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_int i , err;

    /*
        1. cl_program : Holds the OpenCL program created from a source file(the kernel).
        2. FILE *program_handle : Standard file pointer used to read the kernel code.
        3. char *program_buffer : Stores the kernel code read from the file.
        4. cl_kernel : Represents the compiled OpenCL Kernel function that will be executed on the device.
        5. size_t : Unsigned integer type used for buffer sizes and work unit calculations.
    */

    cl_program program;
    FILE *program_handle;
    char *program_buffer , *program_log;
    size_t program_size , log_size;
    cl_kernel kernel;
    size_t work_units_per_kernel;

    /*
        1. mat : 4*4(16 elements) for matrix - vector multiplication.
        2. vec : 4 element vector.
        3. result : stores the result of matrix vector multiplication.
        4. correct : Used for verification to check if the computation is correct.
        5. cl_mem : Handles for OpenCL memory buffers that will store the matrix , vector , and result data on the device.


    */
    float mat[16] , vec[4] , result[4];
    float correct[4] = {0.0f , 0.0f , 0.0f , 0.0f};
    cl_mem mat_buff , vec_buff , res_buff;

    //Initialize Data
    for(i = 0 ; i < 16 ; i++)
    {
        mat[i] = i * 2.0f;
    }

    for(i = 0 ; i < 4 ; i++)
    {
        vec[i] = i * 3.0f;
        correct[0] += mat[i] * vec[i];
        correct[1] += mat[i+4] * vec[i];
        correct[2] += mat[i+8] * vec[i];
        correct[3] += mat[i+12] * vec[i];
    }

    // Set Platform/Device Context
    /*
        1. clGetPlatformIDs : Gets the OpenCL platform ID
        2. clGetDeviceIDs : Retrives the device ID of a GPU from the platform.
        3. clCreateContext : Creates an OpenCL context, where the kernel will be executed. This 
           context allows the host (CPU) to interact with the device (GPU) and manage memory and tasks.
        4. err: Tracks any errors in the context creation.

    */
    clGetPlatformIDs(1 , &platform , NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU , 1, & device , NULL);
    context = clCreateContext(NULL , 1 , &device , NULL , NULL , &err);

    // Read Program File
    /*
        1. Reads the OpenCL kernel source code from the file PROGRAM_FILE
        2. fopen opens the file, fseek moves to the end , and ftell gets the file size.
        3. rewind moves back to the start and fread reads the file into the program_buffer.
    */
    program_handle = fopen(PROGRAM_FILE, "r");
    fseek(program_handle, 0 , SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char) , program_size, program_handle);
    fclose(program_handle);

    // Compile Program
    /*
        1. clCreateProgramWithSource : Creates an OpenCL program from the kernel source code.
        2. clBuildProgram : Compiles the OpenCL program for the specific device(GPU)
    */
    program = clCreateProgramWithSource(context , 1 , (const char**)&program_buffer, &program_size , &err);
    free(program_buffer);
    clBuildProgram(program , 0 , NULL , NULL , NULL , NULL);

    // Create Kernel / Queue
    /*
        1. clCreateKernel : Creates an OpenCL kernel object from the compiled program.
        2. clCreateCommandQueueWithProperties : Creates a queue to send commands (like kernel execution and memory transfer) to the device.
    */

    kernel = clCreateKernel(program , KERNEL_FUNC , &err);
    queue = clCreateCommandQueueWithProperties (context , device , 0 , &err);

    /*
        1. clCreateBuffer : Creates OpenCL memory buffers for the matrix,vector and result.
        2. CL_MEM_READ_ONLY : specifies that the mat and vec buffers are read only for the device.
        3. CL_MEM_COPY_HOST_PTR : copies data from the host (CPU) to the device (GPU).
        4. CL_MEM_WRITE_ONLY : Used for the result buffer, as it will only be written by the GPU.
    */

    mat_buff = clCreateBuffer(context , CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*16, mat , &err);
    vec_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*4, vec , &err);
    res_buff = clCreateBuffer(context, CL_MEM_WRITE_ONLY , sizeof(float)*4, NULL , &err);

    // Set Kernel Arguments
    /*
        1. clSetKernelArg : Sets the arguments(matrix , vector and result buffers) for the openCL kernel.
                            These tell the kernel which memory buffers to use.
            
    */
    clSetKernelArg(kernel , 0  , sizeof(cl_mem), &mat_buff);
    clSetKernelArg(kernel , 1 , sizeof(cl_mem) , &vec_buff);
    clSetKernelArg(kernel , 2 , sizeof(cl_mem) , &res_buff);

    //Execute Kernel
    /*
        1. clEnqueueNDRangeKernel : Enqueues the kernel for execution on the device (GPU). This executes the kernel over work_units_per_kernel.
    */
    work_units_per_kernel = 4;
    clEnqueueNDRangeKernel(queue,kernel,1, NULL , &work_units_per_kernel, NULL , 0 , NULL , NULL);

    clEnqueueReadBuffer(queue, res_buff , CL_TRUE, 0 , sizeof(float)*4, result , 0 , NULL , NULL);

    if(result[0] == correct[0] &&
       result[1] == correct[1] &&
       result[2] == correct[2] &&
       result[3] == correct[3])
       {
        printf("Matrix - Vector Multiplication successful. \n");
       }
    else
    {
        printf("Matrix - Vector Multiplication unsuccessful. \n");
    }

    clReleaseMemObject(mat_buff);
    clReleaseMemObject(vec_buff);
    clReleaseMemObject(res_buff);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    return 0;

}
