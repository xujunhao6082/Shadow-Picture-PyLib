#include "clenv.h"
#include "clcode.h"

#include "metainfo.h"

typedef struct
{
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
} OpenCLObj; // OpenCL计算对象

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
} RGBAOrigin;

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} RGBOrigin;

// 主功能函数
static PyObject *py_mix(PyObject *self, PyObject *args)
{
    PyObject *a, *b, *pres, *data_bytes = NULL;
    RGBAOrigin *res;
    Py_buffer bp0, bp1;
    OpenCLObj *state;
    float k = 0;
    if (!PyArg_ParseTuple(args, "OO|f", &a, &b, &k))
        return NULL;

    if (PyObject_GetBuffer(a, &bp0, PyBUF_READ) < 0)
    {
        return NULL;
    }
    if (PyObject_GetBuffer(b, &bp1, PyBUF_READ) < 0)
    {
        PyBuffer_Release(&bp0);
        return NULL;
    }
    if (bp0.len != bp1.len)
    {
        PyErr_Format(PyExc_ValueError, "Arg 0/1 has ineq length.");
        goto preerr;
    }
    if (bp0.len % (sizeof(RGBOrigin)))
    {
        PyErr_Format(PyExc_ValueError, "Arg 0/1 can't be resolved as RGB Buffer.");
        goto preerr;
    }
    if(!bp0.len){
        data_bytes = PyBytes_FromStringAndSize(NULL, 0);
        goto preerr;
    }

    // Open CL Stuff

    if (!(state = PyModule_GetState(self)))
    {
        PyErr_Format(PyExc_RuntimeError, "Module didn't be initialized correctly.");
    preerr:
        PyBuffer_Release(&bp0);
        PyBuffer_Release(&bp1);
        return data_bytes;
    }

    cl_int err;

    // 分配工作集
    cl_mem buf_P0, buf_P1, buf_PR;
    buf_P0 = clCreateBuffer(state->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            bp0.len, bp0.buf, &err);
    PyBuffer_Release(&bp0);
    if (check_error(err, "Buffer 0 Failure"))
    {

        PyBuffer_Release(&bp1);
        return NULL;
    }
    buf_P1 = clCreateBuffer(state->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            bp1.len, bp1.buf, &err);
    PyBuffer_Release(&bp1);
    if (check_error(err, "Buffer 1 Failure"))
    {
        clReleaseMemObject(buf_P0);
        return NULL;
    }
    size_t i = bp0.len / sizeof(RGBOrigin);
    buf_PR = clCreateBuffer(state->context, CL_MEM_WRITE_ONLY,
                            i * sizeof(RGBAOrigin), NULL, &err);
    if (check_error(err, "Buffer Out Failure"))
        goto memobjrel_PR;

    // 执行
    err = clSetKernelArg(state->kernel, 0, sizeof(cl_mem), &buf_P0);
    err |= clSetKernelArg(state->kernel, 1, sizeof(cl_mem), &buf_P1);
    err |= clSetKernelArg(state->kernel, 2, sizeof(cl_mem), &buf_PR);
    err |= clSetKernelArg(state->kernel, 3, sizeof(float), &k);
    if (check_error(err, "Argument Failure"))
        goto memobjrel;

    err = clEnqueueNDRangeKernel(state->queue, state->kernel, 1, NULL, &i, NULL, 0, NULL, NULL);
    if (check_error(err, "Execute Failure"))
        goto memobjrel;

    if (!(res = malloc(i * sizeof(RGBAOrigin))))
    {
        PyErr_Format(PyExc_RuntimeError, "Error: Memory Failure");
        goto memobjrel;
    }

    err = clEnqueueReadBuffer(state->queue, buf_PR, CL_TRUE, 0,
                              i * sizeof(RGBAOrigin), res, 0, NULL, NULL);

    if (check_error(err, "Read Failure"))
    {
        goto releaseall;
    }
    clFinish(state->queue);
    data_bytes = PyBytes_FromStringAndSize((const char *)res, i * sizeof(RGBAOrigin));
releaseall:
    free(res);
memobjrel:
    clReleaseMemObject(buf_PR);
memobjrel_PR:
    clReleaseMemObject(buf_P0);
    clReleaseMemObject(buf_P1);
    return data_bytes;
}

static PyMethodDef method_table[] = {
    {"mix", py_mix, METH_VARARGS, "Mix two array"},
    {NULL, NULL, 0, NULL}};

static void free_resource(PyObject *module)
{
    if (!module)
        return;
    OpenCLObj *state = PyModule_GetState(module);
    if (!state)
        return;
    if (state->kernel)
    {
        clReleaseKernel(state->kernel);
        state->kernel = NULL;
    }
    if (state->program)
    {
        clReleaseProgram(state->program);
        state->program = NULL;
    }
    if (state->queue)
    {
        clReleaseCommandQueue(state->queue);
        state->queue = NULL;
    }
    if (state->context)
    {
        clReleaseContext(state->context);
        state->context = NULL;
    }
}

static struct PyModuleDef moduledef = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = MODULE_NAME,
    .m_doc = MODULE_DOC,
    .m_size = sizeof(OpenCLObj),
    .m_methods = method_table,
    .m_free = free_resource};

// 初始化
PyMODINIT_FUNC
PyInit_shadowimage(void)
{
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    // Python初始化
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (m == NULL)
        return NULL;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (check_error(err, "Platform Failure"))
        return NULL;

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err == CL_DEVICE_NOT_FOUND)
    {
        printf("GPU is not avilable, turning to CPU.\n");
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    }
    if (check_error(err, "Device Failure"))
        return NULL;

    OpenCLObj *state = PyModule_GetState(m);

    state->context = NULL;
    state->kernel = NULL;
    state->program = NULL;
    state->queue = NULL;

    // 进入资源初始化
    state->context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (check_error(err, "Context Failure"))
        goto error;

    state->queue = clCreateCommandQueue(state->context, device, 0, &err);
    if (check_error(err, "Command Queue Failure"))
        goto error;

    state->program = clCreateProgramWithSource(state->context, 1, &cl_source, NULL, &err);
    if (check_error(err, "Create Progarm Failure"))
        goto error;

    err = clBuildProgram(state->program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t log_size;
        clGetProgramBuildInfo(state->program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = (char *)malloc(log_size);
        if (log)
        {
            clGetProgramBuildInfo(state->program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
            PyErr_Format(PyExc_RuntimeError, "Shader Build Failure:\n%s\n", log);
            free(log);
        }
        else
        {
            PyErr_Format(PyExc_RuntimeError, "Shader Build Failure: (unable to retrieve log)");
        }
        goto error;
    }

    state->kernel = clCreateKernel(state->program, "arrmix", &err);
    if (check_error(err, "Create Kernel Failure"))
        goto error;

    return m;
error:
    free_resource(m);
    return NULL;
}