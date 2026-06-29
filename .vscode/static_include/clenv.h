#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <CL/cl.h>

int check_error(cl_int err, const char *msg)
{
    if (err != CL_SUCCESS)
    {
        PyErr_Format(PyExc_RuntimeError, "Error: %s (Code %d)\n", msg, err);
    }
    return err != CL_SUCCESS;
}