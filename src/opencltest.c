#include "clenv.h"
#include "clcode.h"

/*
Example Progarm:
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem buf_R, buf_W;
    size_t global_size = 0; // VEC_SIZE

    err = clGetPlatformIDs(1, &platform, NULL);
    check_error(err, "Platform Failure");

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err == CL_DEVICE_NOT_FOUND)
    {
        printf("Turning to CPU\n");
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    }
    check_error(err, "Device Failure");

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    check_error(err, "Context Failure");

    queue = clCreateCommandQueue(context, device, 0, &err);
    check_error(err, "Command Queue Failure");

    buf_R = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                           VEC_SIZE * sizeof(float), h_R, &err);
    check_error(err, "Buffer R Failure");
    buf_W = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                           VEC_SIZE * sizeof(float), NULL, &err);
    check_error(err, "Buffer W Failure");

    program = clCreateProgramWithSource(context, 1, &cl_source, NULL, &err);
    check_error(err, "Create Progarm Failure");
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = (char *)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        fprintf(stderr, "Build Failure:\n%s\n", log);
        free(log);
        exit(EXIT_FAILURE);
    }

    kernel = clCreateKernel(program, "...", &err);
    check_error(err, "Create Kernel Failure");
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buf_R);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buf_W);
    check_error(err, "Argument Failure");

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    check_error(err, "Execute Failure");

    err = clEnqueueReadBuffer(queue, buf_W, CL_TRUE, 0,
                              VEC_SIZE * sizeof(float), h_c, 0, NULL, NULL);
    check_error(err, "Read Failure");

    clReleaseMemObject(buf_R);
    clReleaseMemObject(buf_W);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(h_R);
    free(h_W);
*/

int main()
{
    return 0;
}