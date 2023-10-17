#include "OpenSparseBLAS.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void print_Vector_BOOL(SpB_Vector v);
    void print_Vector_INT32(SpB_Vector v);
    void print_Vector_FP32(SpB_Vector v);
    void print_Matrix_INT32(SpB_Matrix m);
    void print_Matrix_FP32(SpB_Matrix m);
    void print_Matrix_BOOL(SpB_Matrix m);
#ifdef __cplusplus
}
#endif

#include <stdlib.h>
#define SAFE_CALL(method)    \
    info = method;           \
    if (info != SpB_SUCCESS) \
    {                        \
        exit(9);             \
    }
