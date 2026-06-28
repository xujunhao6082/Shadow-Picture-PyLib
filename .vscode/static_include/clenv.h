#include <CL/cl.h>
#ifdef DEBUG
#include <stdio.h>
#endif
void check_error(cl_int err, const char *msg)
{
    if (err != CL_SUCCESS)
    {
        #ifdef DEBUG
        fprintf(stderr, "Error: %s (Code %d)\n", msg, err);
        #endif
        exit(EXIT_FAILURE);
    }
}