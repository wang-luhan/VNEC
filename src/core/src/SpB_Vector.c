/**
 * @file SpB_Vector.c
 * @xulei (xulei19b@ict.ac.com)
 * @brief Member functions of SpB_Vector
 * @version 0.1
 * @date 2022-08-24
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "SpB_Global.h"
#include "SpB_Scalar.h"
#include "SpB_Vector.h"
#include "SpB_Vector_Dense.h"
#include "SpB_Vector_Sparse.h"
#include <stdlib.h>
#include <assert.h>
#include "SpB_Debug.h"
#include "SpB_Error.h"
#include "SpB_MACRO.h"

struct SpB_Vector_opaque SpB_Vector_NULL_opaque =
    {SpB_NULL_TYPE,
     SpB_VECTOR_NO_FORMAT,
     0,
     0};
SpB_Vector SpB_Vector_NULL = &SpB_Vector_NULL_opaque;

SpB_Info SpB_Vector_nvals(SpB_Index *n, SpB_Vector v)
{
    *n = v->n;
    return SpB_SUCCESS;
}

SpB_Info SpB_Vector_new(
    SpB_Vector *v,
    SpB_Type type,
    SpB_Index n)
{

    SpB_Vector_Format_Value_ext format_type =
        SpB_Global_get_Vector_Format();
    SpB_Info info;

    switch (format_type)
    {
    case (SpB_DENSE):
        info = SpB_Vector_Dense_new(v, type, n);
        return info;
    case (SpB_SPARSE):
        info = SpB_Vector_Sparse_new(v, type, n);
        return info;
    default:
        SpB_Error("Unknown Vector Format \
in the SpB_Vector_new!\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_FAILURE;
}

// Todo : This is a very slow implementation.
SpB_Info SpB_Vector_dup(SpB_Vector *v1, SpB_Vector v2)
{
    SpB_Vector_Format_Value_ext format_type = v2->format_type;
    SpB_Info info;

    switch (format_type)
    {
    case (SpB_DENSE):
        info = SpB_Vector_Dense_dup(v1, v2);
        return info;
    case (SpB_SPARSE):
        SpB_Error("GxB_SPARSE is not supported \
for Vector Format in the SpB_Vector_dup!\n",
                  __FILE__, __LINE__);
        return SpB_FAILURE;
    default:
        SpB_Error("Unknown vector format_type \
in the SpB_Vector_dup!\n",
                  __FILE__, __LINE__);
        return SpB_FAILURE;
    }
}

SpB_Info SpB_Vector_copy_format_ext(
    SpB_Vector *out,
    const SpB_Vector in)
{
    SpB_Info info;
    if (*out != NULL)
    {
        SAFE_CALL(SpB_Vector_free(out));
    }

    const SpB_Type type = in->element_type;
    const SpB_Index n = in->n;
    const SpB_Vector_Format_Value_ext format =
        in->format_type;

    info = SpB_Vector_new_format_ext(
        out, type, n, format);

    return info;
}

SpB_Info SpB_Vector_new_format_ext(
    SpB_Vector *v,
    SpB_Type type,
    SpB_Index n,
    SpB_Vector_Format_Value_ext format)
{
    SpB_Info info;

    switch (format)
    {
    case (SpB_DENSE):
        info = SpB_Vector_Dense_new(v, type, n);
        return info;
        break;
    case (SpB_SPARSE):
        info = SpB_Vector_Sparse_new(v, type, n);
        return info;
        break;
    default:
        SpB_Error("Unknown Vector Format \
in the SpB_Vector_new!\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_FAILURE;
}

SpB_Vector_Format_Value_ext SpB_Vector_format_ext(const SpB_Vector v)
{
    return v->format_type;
}

#ifdef _OPENMP
#define GRB_VECTOR_SET(type, T)                          \
    SpB_Info SpB_Vector_setElement_##T(                  \
        SpB_Vector v,                                    \
        type value,                                      \
        SpB_Index index)                                 \
    {                                                    \
        SpB_Vector_Format_Value_ext format =             \
            v->format_type;                              \
        SpB_Info info;                                   \
        switch (format)                                  \
        {                                                \
        case (SpB_DENSE):                                \
            return SpB_Vector_Dense_setElement_##T(      \
                v, value, index);                        \
        case (SpB_SPARSE):                               \
        {                                                \
            _Pragma("omp critical")                      \
            {                                            \
                info = SpB_Vector_Sparse_setElement_##T( \
                    v, value, index);                    \
            }                                            \
            return info;                                 \
        }                                                \
        default:                                         \
            SpB_Error("Unknown Vector Format \
in the SpB_Vector_setElement_##T !\n",                   \
                      __FILE__, __LINE__);               \
            break;                                       \
        }                                                \
        return SpB_FAILURE;                              \
    }
#else
#define GRB_VECTOR_SET(type, T)                      \
    SpB_Info SpB_Vector_setElement_##T(              \
        SpB_Vector v,                                \
        type value,                                  \
        SpB_Index index)                             \
    {                                                \
        SpB_Vector_Format_Value_ext format =         \
            v->format_type;                          \
        SpB_Info info;                               \
        switch (format)                              \
        {                                            \
        case (SpB_DENSE):                            \
        {                                            \
            info = SpB_Vector_Dense_setElement_##T(  \
                v, value, index);                    \
            return info;                             \
        }                                            \
        case (SpB_SPARSE):                           \
        {                                            \
            info = SpB_Vector_Sparse_setElement_##T( \
                v, value, index);                    \
            return info;                             \
        }                                            \
        default:                                     \
            SpB_error("Unknown Vector Format \
in the SpB_Vector_setElement_##T !\n");              \
            break;                                   \
        }                                            \
        return SpB_FAILURE;                          \
    }
#endif
GRB_VECTOR_SET(bool, BOOL);
GRB_VECTOR_SET(int8_t, INT8);
GRB_VECTOR_SET(int16_t, INT16);
GRB_VECTOR_SET(int32_t, INT32);
GRB_VECTOR_SET(int64_t, INT64);
GRB_VECTOR_SET(uint8_t, UINT8);
GRB_VECTOR_SET(uint16_t, UINT16);
GRB_VECTOR_SET(uint32_t, UINT32);
GRB_VECTOR_SET(uint64_t, UINT64);
GRB_VECTOR_SET(float, FP32);
GRB_VECTOR_SET(double, FP64);
#undef GRB_VECTOR_SET

#define CASE_SWICTH_DATA_TYPE_A_TO_B(                  \
    type_A, T_A, type_B, T_B)                          \
    case SpB_##T_B:                                    \
        info = SpB_Vector_setElement_##T_B(            \
            v, (type_B)(s->value.T_A##_value), index); \
        break;

#define CASES_SWICTH_DATA_TYPE_A_TO_B(type_A, T_A) \
    switch (v->element_type)                       \
    {                                              \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, bool, BOOL);              \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int8_t, INT8);            \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int16_t, INT16);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int32_t, INT32);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int64_t, INT64);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint8_t, UINT8);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint16_t, UINT16);        \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint32_t, UINT32);        \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint64_t, UINT64);        \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, float, FP32);             \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, double, FP64);            \
    default:                                       \
        SpB_Error("unknown element type \
of the scalar !\n",                                \
                  __FILE__, __LINE__);             \
        break;                                     \
    }

#define CASE_SETELEMENT_SCALAR(type_A, T_A) \
    case SpB_##T_A:                         \
        CASES_SWICTH_DATA_TYPE_A_TO_B(      \
            type_A, T_A);                   \
        break;

#define CASES_SETELEMENT_SCALAR           \
    CASE_SETELEMENT_SCALAR(bool, BOOL);       \
    CASE_SETELEMENT_SCALAR(int8_t, INT8);     \
    CASE_SETELEMENT_SCALAR(int16_t, INT16);   \
    CASE_SETELEMENT_SCALAR(int32_t, INT32);   \
    CASE_SETELEMENT_SCALAR(int64_t, INT64);   \
    CASE_SETELEMENT_SCALAR(uint8_t, UINT8);   \
    CASE_SETELEMENT_SCALAR(uint16_t, UINT16); \
    CASE_SETELEMENT_SCALAR(uint32_t, UINT32); \
    CASE_SETELEMENT_SCALAR(uint64_t, UINT64); \
    CASE_SETELEMENT_SCALAR(float, FP32);      \
    CASE_SETELEMENT_SCALAR(double, FP64);

SpB_Info SpB_Vector_setElement_Scalar(
    SpB_Vector v,
    SpB_Scalar s,
    SpB_Index index)
{
    SpB_Info info = SpB_FAILURE;
    switch (s->element_type)
    {
        CASES_SETELEMENT_SCALAR;
    default:
        SpB_Error("unknown element type of scalar \
in the SpB_Vector_setElement_Scalar!\n",
                  __FILE__, __LINE__);
        break;
    }
    return info;
}
#undef CASES_SETELEMENT_SCALAR
#undef CASE_SETELEMENT_SCALAR
#undef CASES_SWICTH_DATA_TYPE_A_TO_B
#undef CASE_SWICTH_DATA_TYPE_A_TO_B

#define GRB_VECTOR_EXTRACT(type, T)                      \
    SpB_Info SpB_Vector_extractElement_##T(              \
        type *value,                                     \
        SpB_Vector v,                                    \
        SpB_Index index)                                 \
    {                                                    \
        SpB_Vector_Format_Value_ext format =             \
            v->format_type;                              \
        switch (format)                                  \
        {                                                \
        case (SpB_DENSE):                                \
            return SpB_Vector_Dense_extractElement_##T(  \
                value, v, index);                        \
        case (SpB_SPARSE):                               \
            return SpB_Vector_Sparse_extractElement_##T( \
                value, v, index);                        \
            break;                                       \
        default:                                         \
            SpB_Error("Unknown Vector Format \
in the SpB_Vector_extractElement_" #T " !\n",            \
                      __FILE__, __LINE__);               \
            break;                                       \
        }                                                \
        return SpB_FAILURE;                              \
    }

GRB_VECTOR_EXTRACT(bool, BOOL);
GRB_VECTOR_EXTRACT(int8_t, INT8);
GRB_VECTOR_EXTRACT(int16_t, INT16);
GRB_VECTOR_EXTRACT(int32_t, INT32);
GRB_VECTOR_EXTRACT(int64_t, INT64);
GRB_VECTOR_EXTRACT(uint8_t, UINT8);
GRB_VECTOR_EXTRACT(uint16_t, UINT16);
GRB_VECTOR_EXTRACT(uint32_t, UINT32);
GRB_VECTOR_EXTRACT(uint64_t, UINT64);
GRB_VECTOR_EXTRACT(float, FP32);
GRB_VECTOR_EXTRACT(double, FP64);
#undef GRB_VECTOR_EXTRACT

#define CASE_SWICTH_DATA_TYPE_A_TO_B(   \
    type_A, T_A, type_B, T_B)           \
    case SpB_##T_B:                     \
        s->value.T_B##_value =          \
            (type_B)(temp.T_A##_value); \
        break;

#define CASES_SWICTH_DATA_TYPE_A_TO_B(type_A, T_A) \
    switch (s->element_type)                       \
    {                                              \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, bool, BOOL);              \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int8_t, INT8);            \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int16_t, INT16);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int32_t, INT32);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, int64_t, INT64);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint8_t, UINT8);          \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint16_t, UINT16);        \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint32_t, UINT32);        \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, uint64_t, UINT64);        \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, float, FP32);             \
        CASE_SWICTH_DATA_TYPE_A_TO_B(              \
            type_A, T_A, double, FP64);            \
    default:                                       \
        SpB_Error("unknown element type \
of the scalar !\n",                                \
                  __FILE__, __LINE__);             \
        break;                                     \
    }

#define CASE_EXTRACTELEMENT_SCALAR(type_A, T_A) \
    case SpB_##T_A:                             \
        info = SpB_Vector_extractElement_##T_A( \
            &(temp.T_A##_value), v, index);     \
        if (info == SpB_SUCCESS)                \
            CASES_SWICTH_DATA_TYPE_A_TO_B(      \
                type_A, T_A);                   \
        break;

#define CASES_EXTRACTELEMENT_SCALAR               \
    CASE_EXTRACTELEMENT_SCALAR(bool, BOOL);       \
    CASE_EXTRACTELEMENT_SCALAR(int8_t, INT8);     \
    CASE_EXTRACTELEMENT_SCALAR(int16_t, INT16);   \
    CASE_EXTRACTELEMENT_SCALAR(int32_t, INT32);   \
    CASE_EXTRACTELEMENT_SCALAR(int64_t, INT64);   \
    CASE_EXTRACTELEMENT_SCALAR(uint8_t, UINT8);   \
    CASE_EXTRACTELEMENT_SCALAR(uint16_t, UINT16); \
    CASE_EXTRACTELEMENT_SCALAR(uint32_t, UINT32); \
    CASE_EXTRACTELEMENT_SCALAR(uint64_t, UINT64); \
    CASE_EXTRACTELEMENT_SCALAR(float, FP32);      \
    CASE_EXTRACTELEMENT_SCALAR(double, FP64);

SpB_Info SpB_Vector_extractElement_Scalar(
    SpB_Scalar s,
    const SpB_Vector v,
    const SpB_Index index)
{
    SpB_Info info;
    SpB_Type v_tpye = v->element_type;
    Scalar_Value temp;
    switch (v_tpye)
    {
        CASES_EXTRACTELEMENT_SCALAR;
    default:
        SpB_Error("Unknown the scalar element type\
of the input vector \
in the SpB_Vector_extractElement_Scalar\n",
                  __FILE__, __LINE__);
        info = SpB_FAILURE;
        break;
    }
    return info;
}
#undef CASES_EXTRACTELEMENT_SCALAR
#undef CASE_EXTRACTELEMENT_SCALAR
#undef CASES_SWICTH_DATA_TYPE_A_TO_B
#undef CASE_SWICTH_DATA_TYPE_A_TO_B

#define SpB_VECTOR_EXTRACT_TUPLES(type, T)              \
    SpB_Info SpB_Vector_extractTuples_##T(              \
        SpB_Index *indices,                             \
        type *values,                                   \
        SpB_Index *n,                                   \
        const SpB_Vector v)                             \
    {                                                   \
        SpB_Vector_Format_Value_ext format =            \
            v->format_type;                             \
        switch (format)                                 \
        {                                               \
        case (SpB_DENSE):                               \
            return SpB_Vector_Dense_extractTuples_##T(  \
                indices, values, n, v);                 \
        case (SpB_SPARSE):                              \
            return SpB_Vector_Sparse_extractTuples_##T( \
                indices, values, n, v);                 \
        default:                                        \
            SpB_Error("Unknown Vector Format \
in SpB_Vector_extractTuples_" #T " !\n",                \
                      __FILE__, __LINE__);              \
            break;                                      \
        }                                               \
        return SpB_FAILURE;                             \
    }
SpB_VECTOR_EXTRACT_TUPLES(bool, BOOL);
SpB_VECTOR_EXTRACT_TUPLES(int8_t, INT8);
SpB_VECTOR_EXTRACT_TUPLES(int16_t, INT16);
SpB_VECTOR_EXTRACT_TUPLES(int32_t, INT32);
SpB_VECTOR_EXTRACT_TUPLES(int64_t, INT64);
SpB_VECTOR_EXTRACT_TUPLES(uint8_t, UINT8);
SpB_VECTOR_EXTRACT_TUPLES(uint16_t, UINT16);
SpB_VECTOR_EXTRACT_TUPLES(uint32_t, UINT32);
SpB_VECTOR_EXTRACT_TUPLES(uint64_t, UINT64);
SpB_VECTOR_EXTRACT_TUPLES(float, FP32);
SpB_VECTOR_EXTRACT_TUPLES(double, FP64);
#undef SpB_VECTOR_EXTRACT_TUPLES

/**
 * @callgraph
 */
SpB_Info SpB_Vector_free(SpB_Vector *v)
{
    SpB_Vector_Format_Value_ext format =
        (*v)->format_type;
    switch (format)
    {
    case (SpB_DENSE):
        return SpB_Vector_Dense_free(v);
    case (SpB_SPARSE):
        return SpB_Vector_Sparse_free(v);
    default:
        SpB_Error("Unknown Vector Format \
in the SpB_Vector_free !\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_FAILURE;
}

#define SpB_VECTOR_BUILD(type, T)               \
    SpB_Info SpB_Vector_build_##T(              \
        SpB_Vector v,                           \
        const SpB_Index *indices,               \
        const type *values,                     \
        SpB_Index n,                            \
        const SpB_BinaryOp dup)                 \
    {                                           \
        SpB_Vector_Format_Value_ext format =    \
            v->format_type;                     \
        switch (format)                         \
        {                                       \
        case (SpB_DENSE):                       \
            return SpB_Vector_Dense_build_##T(  \
                (SpB_Vector_Dense *)v,          \
                indices, values, n, dup);       \
        case (SpB_SPARSE):                      \
            return SpB_Vector_Sparse_build_##T( \
                (SpB_Vector_Sparse *)v,         \
                indices, values, n, dup);       \
        default:                                \
            SpB_Error("Unknown Vector Format \
in the SpB_Vector_build_" #T " !\n",            \
                      __FILE__, __LINE__);      \
            break;                              \
        }                                       \
        return SpB_FAILURE;                     \
    }
SpB_VECTOR_BUILD(bool, BOOL);
SpB_VECTOR_BUILD(int8_t, INT8);
SpB_VECTOR_BUILD(int16_t, INT16);
SpB_VECTOR_BUILD(int32_t, INT32);
SpB_VECTOR_BUILD(int64_t, INT64);
SpB_VECTOR_BUILD(uint8_t, UINT8);
SpB_VECTOR_BUILD(uint16_t, UINT16);
SpB_VECTOR_BUILD(uint32_t, UINT32);
SpB_VECTOR_BUILD(uint64_t, UINT64);
SpB_VECTOR_BUILD(float, FP32);
SpB_VECTOR_BUILD(double, FP64);
#undef SpB_VECTOR_BUILD
