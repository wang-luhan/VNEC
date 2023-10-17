#include "SpB_Alloc.h"
#include "SpB_Type.h"
#include "SpB_Scalar.h"
#include "SpB_Error.h"

SpB_Info SpB_Scalar_new(
    SpB_Scalar *scalar,
    SpB_Type d)
{
    *scalar = (struct SpB_Scalar_opaque *)
        SpB_aligned_malloc(
            sizeof(struct SpB_Scalar_opaque));
    if (*scalar == NULL)
    {
        return SpB_FAILURE;
    }
    (*scalar)->element_type = d;
    return SpB_SUCCESS;
}

SpB_Info SpB_Scalar_free(SpB_Scalar *scalar)
{
    if (*scalar == NULL)
    {
        return SpB_FAILURE;
    }
    else
    {
        free(*scalar);
        return SpB_SUCCESS;
    }
}

#define CASE_SETZERO(type, T)          \
    case SpB_##T:                      \
        s->value.T##_value = ZERO_##T; \
        break;

#define CASES_SETZERO               \
    CASE_SETZERO(bool, BOOL);       \
    CASE_SETZERO(int8_t, INT8);     \
    CASE_SETZERO(int16_t, INT16);   \
    CASE_SETZERO(int32_t, INT32);   \
    CASE_SETZERO(int64_t, INT64);   \
    CASE_SETZERO(uint8_t, UINT8);   \
    CASE_SETZERO(uint16_t, UINT16); \
    CASE_SETZERO(uint32_t, UINT32); \
    CASE_SETZERO(uint64_t, UINT64); \
    CASE_SETZERO(float, FP32);      \
    CASE_SETZERO(double, FP64);

SpB_Info SpB_Scalar_setZero(SpB_Scalar s)
{
    switch (s->element_type)
    {
        CASES_SETZERO;

    default:
        SpB_Error("Unknown element type \
in the SpB_Scalar_setZero!\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_SUCCESS;
}
#undef CASES_SETZERO
#undef CASE_SETZERO
