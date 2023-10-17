#include "SpB_Global.h"
#include "SpB_Scalar.h"
#include "SpB_Matrix.h"
#include "SpB_Matrix_CSC_or_CSR.h"
#include "SpB_Error.h"
#include <stdbool.h>

struct SpB_Matrix_opaque SpB_Matrix_NULL_opaque;
SpB_Matrix SpB_Matrix_NULL = &SpB_Matrix_NULL_opaque;

SpB_Info SpB_Matrix_new(
    SpB_Matrix *m,
    SpB_Type type,
    SpB_Index row,
    SpB_Index col)
{
    SpB_Matrix_Format_Value_ext format_type;
    if (SpB_Global_is_csc_get())
        format_type = SpB_CSC;
    else
        format_type = SpB_CSR;
    switch (format_type)
    {
    case (SpB_CSR):
        return SpB_Matrix_CSC_or_CSR_new(
            m, type, row, col, format_type);
        break;
    case (SpB_CSC):
        return SpB_Matrix_CSC_or_CSR_new(
            m, type, row, col, format_type);
        break;
    default:
        SpB_Error("Unknown Matrix Format \
in the SpB_Matrix_new!\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_FAILURE;
}

SpB_Info SpB_Matrix_new_format_ext(
    SpB_Matrix *m, SpB_Type type,
    SpB_Index row, SpB_Index col,
    SpB_Matrix_Format_Value_ext format)
{
    SpB_Info info;

    switch (format)
    {
    case (SpB_CSR):
        info = SpB_Matrix_CSC_or_CSR_new(
            m, type, row, col, format);
        return info;
        break;
    case (SpB_CSC):
        info = SpB_Matrix_CSC_or_CSR_new(
            m, type, row, col, format);
        return info;
        break;
    default:
        SpB_Error("Unknown Matrix Format \
in the SpB_Matrix_new!\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_FAILURE;
}

SpB_Matrix_Format_Value_ext SpB_Matrix_format_ext(
    const SpB_Matrix m)
{
    return m->format_type;
}

SpB_Info SpB_Matrix_type(
    SpB_Type *type, SpB_Matrix m)
{
    *type = m->element_type;
    return SpB_SUCCESS;
}

SpB_Info SpB_Matrix_nrows(
    SpB_Index *n, SpB_Matrix m)
{
    *n = m->row;
    return SpB_SUCCESS;
}
SpB_Info SpB_Matrix_ncols(
    SpB_Index *n, SpB_Matrix m)
{
    *n = m->col;
    return SpB_SUCCESS;
}

#define SpB_MATRIX_FAST_BUILD_EXT(type, T)                     \
    SpB_Info SpB_Matrix_fast_build_##T##_ext(                  \
        SpB_Matrix m_in,                                       \
        const SpB_Index *row_indices,                          \
        const SpB_Index *col_indices,                          \
        const type *values,                                    \
        const SpB_Index n,                                     \
        const SpB_BinaryOp dup)                                \
    {                                                          \
        SpB_Matrix_Format_Value_ext format =                   \
            m_in->format_type;                                 \
        switch (format)                                        \
        {                                                      \
        case (SpB_CSR):                                        \
            return SpB_Matrix_CSC_or_CSR_fast_build_##T##_ext( \
                m_in,                                          \
                row_indices,                                   \
                col_indices,                                   \
                values,                                        \
                n, dup);                                       \
            break;                                             \
        case (SpB_CSC):                                        \
            return SpB_Matrix_CSC_or_CSR_fast_build_##T##_ext( \
                m_in,                                          \
                row_indices,                                   \
                col_indices,                                   \
                values,                                        \
                n, dup);                                       \
            break;                                             \
        default:                                               \
            SpB_Error("Unknown Matrix Format \
in SpB_Matrix_build!\n",                                       \
                      __FILE__, __LINE__);                     \
            break;                                             \
        }                                                      \
        return SpB_FAILURE;                                    \
    }

SpB_MATRIX_FAST_BUILD_EXT(bool, BOOL);
SpB_MATRIX_FAST_BUILD_EXT(int8_t, INT8);
SpB_MATRIX_FAST_BUILD_EXT(int16_t, INT16);
SpB_MATRIX_FAST_BUILD_EXT(int32_t, INT32);
SpB_MATRIX_FAST_BUILD_EXT(int64_t, INT64);
SpB_MATRIX_FAST_BUILD_EXT(uint8_t, UINT8);
SpB_MATRIX_FAST_BUILD_EXT(uint16_t, UINT16);
SpB_MATRIX_FAST_BUILD_EXT(uint32_t, UINT32);
SpB_MATRIX_FAST_BUILD_EXT(uint64_t, UINT64);
SpB_MATRIX_FAST_BUILD_EXT(float, FP32);
SpB_MATRIX_FAST_BUILD_EXT(double, FP64);
#undef SpB_MATRIX_BUILD_EXT

#define SpB_MATRIX_BUILD(type, T)                   \
    SpB_Info SpB_Matrix_build_##T(                  \
        SpB_Matrix m_in,                            \
        const SpB_Index *row_indices,               \
        const SpB_Index *col_indices,               \
        const type *values,                         \
        const SpB_Index n,                          \
        const SpB_BinaryOp dup)                     \
    {                                               \
        SpB_Matrix_Format_Value_ext format =        \
            m_in->format_type;                      \
        switch (format)                             \
        {                                           \
        case (SpB_CSR):                             \
            return SpB_Matrix_CSC_or_CSR_build_##T( \
                m_in,                               \
                row_indices,                        \
                col_indices,                        \
                values,                             \
                n, dup);                            \
            break;                                  \
        case (SpB_CSC):                             \
            return SpB_Matrix_CSC_or_CSR_build_##T( \
                m_in,                               \
                row_indices,                        \
                col_indices,                        \
                values,                             \
                n, dup);                            \
            break;                                  \
        default:                                    \
            SpB_Error("Unknown Matrix Format \
in SpB_Matrix_build!\n",                            \
                      __FILE__, __LINE__);          \
            break;                                  \
        }                                           \
        return SpB_FAILURE;                         \
    }

SpB_MATRIX_BUILD(bool, BOOL);
SpB_MATRIX_BUILD(int8_t, INT8);
SpB_MATRIX_BUILD(int16_t, INT16);
SpB_MATRIX_BUILD(int32_t, INT32);
SpB_MATRIX_BUILD(int64_t, INT64);
SpB_MATRIX_BUILD(uint8_t, UINT8);
SpB_MATRIX_BUILD(uint16_t, UINT16);
SpB_MATRIX_BUILD(uint32_t, UINT32);
SpB_MATRIX_BUILD(uint64_t, UINT64);
SpB_MATRIX_BUILD(float, FP32);
SpB_MATRIX_BUILD(double, FP64);
#undef SpB_MATRIX_BUILD

#define SpB_MATRIX_SETELEMENT(type, T)                   \
    SpB_Info SpB_Matrix_setElement_##T(                  \
        SpB_Matrix m,                                    \
        type value,                                      \
        SpB_Index row,                                   \
        SpB_Index col)                                   \
    {                                                    \
        SpB_Matrix_Format_Value_ext format =             \
            m->format_type;                              \
        switch (format)                                  \
        {                                                \
        case (SpB_CSR):                                  \
            return SpB_Matrix_CSC_or_CSR_setElement_##T( \
                m, value, row, col);                     \
            break;                                       \
        case (SpB_CSC):                                  \
            return SpB_Matrix_CSC_or_CSR_setElement_##T( \
                m, value, row, col);                     \
            break;                                       \
        default:                                         \
            SpB_Error("Unknown Matrix Format \
in the SpB_Matrix_setElement!\n",                        \
                      __FILE__, __LINE__);               \
            break;                                       \
        }                                                \
        return SpB_FAILURE;                              \
    }

SpB_MATRIX_SETELEMENT(bool, BOOL);
SpB_MATRIX_SETELEMENT(int8_t, INT8);
SpB_MATRIX_SETELEMENT(int16_t, INT16);
SpB_MATRIX_SETELEMENT(int32_t, INT32);
SpB_MATRIX_SETELEMENT(int64_t, INT64);
SpB_MATRIX_SETELEMENT(uint8_t, UINT8);
SpB_MATRIX_SETELEMENT(uint16_t, UINT16);
SpB_MATRIX_SETELEMENT(uint32_t, UINT32);
SpB_MATRIX_SETELEMENT(uint64_t, UINT64);
SpB_MATRIX_SETELEMENT(float, FP32);
SpB_MATRIX_SETELEMENT(double, FP64);
#undef SpB_MATRIX_SETELEMENT

#define SpB_MATRIX_EXTRACTELEMENT(type, T)                   \
    SpB_Info SpB_Matrix_extractElement_##T(                  \
        type *p,                                             \
        SpB_Matrix m,                                        \
        SpB_Index row,                                       \
        SpB_Index col)                                       \
    {                                                        \
        SpB_Matrix_Format_Value_ext format =                 \
            m->format_type;                                  \
        switch (format)                                      \
        {                                                    \
        case (SpB_CSR):                                      \
            return SpB_Matrix_CSC_or_CSR_extractElement_##T( \
                p, m, row, col);                             \
            break;                                           \
        case (SpB_CSC):                                      \
            return SpB_Matrix_CSC_or_CSR_extractElement_##T( \
                p, m, row, col);                             \
            break;                                           \
        default:                                             \
            SpB_Error("Unknown Matrix Format \
in the SpB_Matrix_extractElement!\n",                        \
                      __FILE__, __LINE__);                   \
            break;                                           \
        }                                                    \
        return SpB_FAILURE;                                  \
    }
SpB_MATRIX_EXTRACTELEMENT(bool, BOOL);
SpB_MATRIX_EXTRACTELEMENT(int8_t, INT8);
SpB_MATRIX_EXTRACTELEMENT(int16_t, INT16);
SpB_MATRIX_EXTRACTELEMENT(int32_t, INT32);
SpB_MATRIX_EXTRACTELEMENT(int64_t, INT64);
SpB_MATRIX_EXTRACTELEMENT(uint8_t, UINT8);
SpB_MATRIX_EXTRACTELEMENT(uint16_t, UINT16);
SpB_MATRIX_EXTRACTELEMENT(uint32_t, UINT32);
SpB_MATRIX_EXTRACTELEMENT(uint64_t, UINT64);
SpB_MATRIX_EXTRACTELEMENT(float, FP32);
SpB_MATRIX_EXTRACTELEMENT(double, FP64);
#undef SpB_MATRIX_EXTRACTELEMENT

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

#define CASE_EXTRACTELEMENT_SCALAR(type_A, T_A)         \
    case SpB_##T_A:                                     \
        info = SpB_Matrix_extractElement_##T_A(         \
            &(temp.T_A##_value), m, row, col);          \
        if (info == SpB_SUCCESS)                        \
            CASES_SWICTH_DATA_TYPE_A_TO_B(type_A, T_A); \
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

SpB_Info SpB_Matrix_extractElement_Scalar(
    SpB_Scalar s,
    const SpB_Matrix m,
    const SpB_Index row,
    const SpB_Index col)
{
    SpB_Info info;
    SpB_Type m_tpye = m->element_type;
    Scalar_Value temp;
    switch (m_tpye)
    {
        CASES_EXTRACTELEMENT_SCALAR;
    default:
        SpB_Error("Unknown the scalar element type\
of the input Matrix \
in the SpB_Matrix_extractElement_Scalar\n",
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

SpB_Info SpB_Matrix_free(SpB_Matrix *m)
{
    SpB_Matrix_Format_Value_ext format =
        (*m)->format_type;
    switch (format)
    {
    case (SpB_CSR):
        return SpB_Matrix_CSC_or_CSR_free(m);
        break;
    case (SpB_CSC):
        return SpB_Matrix_CSC_or_CSR_free(m);
        break;
    default:
        SpB_Error("Unknown Matrix Format in \
SpB_Matrix_free!\n",
                  __FILE__, __LINE__);
        break;
    }
    return SpB_FAILURE;
}