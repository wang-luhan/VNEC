#include "SpB_Matrix_CSC_or_CSR.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "SpB_Alloc.h"
#include "SpB_Type.h"
#include "SpB_Error.h"

#define Matrix_compute_next_grow(size) \
    ((size) ? ((size) << 1) : 1)

static SpB_Info SpB_Matrix_CSC_or_CSR_grow(
    SpB_Matrix_CSC_or_CSR *m,
    SpB_Index capacity)
{
    size_t size = m->matrix_base.element_size;
    if (m->val)
        m->val = SpB_aligned_realloc(
            m->val, size * capacity);
    else
        m->val = SpB_aligned_malloc(
            size * capacity);
    assert(m->val);
    if (m->indices)
        m->indices =
            (SpB_Index *)SpB_aligned_realloc(
                m->indices, sizeof(SpB_Index) * capacity);
    else
        m->indices = (SpB_Index *)SpB_aligned_malloc(
            sizeof(SpB_Index) * capacity);
    assert(m->indices);
    m->capacity = capacity;
    return SpB_SUCCESS;
}

/**
 * @brief Create a matrix of CSC/CSR format.
 * @param[out] m_in the pointer to a matrix
 */
SpB_Info SpB_Matrix_CSC_or_CSR_new(
    SpB_Matrix *m_in,
    SpB_Type type,
    SpB_Index row,
    SpB_Index col,
    SpB_Matrix_Format_Value_ext format_type)
{
    assert(format_type == SpB_CSC ||
           format_type == SpB_CSR);
    SpB_Matrix_CSC_or_CSR **m =
        (SpB_Matrix_CSC_or_CSR **)m_in;
    OK_ALLOC(*m = (SpB_Matrix_CSC_or_CSR *)SpB_aligned_malloc(
                 sizeof(SpB_Matrix_CSC_or_CSR)));
    (*m_in)->element_type = type;
    (*m_in)->format_type = format_type;
    (*m)->matrix_base.row = row;
    (*m)->matrix_base.col = col;
    (*m)->ptr_len =
        format_type == SpB_CSC
            ? (col + 1)
            : (row + 1);
    (*m)->nnz = 0;
    switch (type)
    {
        SpB_TYPE_CASES(m_in);
    }
    (*m)->capacity = 0;
    (*m)->val = NULL;
    (*m)->indices = NULL;
    SpB_Index p_len =
        (*m)->matrix_base.format_type == SpB_CSC
            ? (row + 1)
            : (col + 1);
    OK_ALLOC((*m)->ptr = (SpB_Index *)SpB_aligned_calloc(
                 p_len * sizeof(SpB_Index)));

    return SpB_SUCCESS;
}

#define GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(type, T)    \
    SpB_Info SpB_Matrix_CSC_or_CSR_fast_build_##T##_ext( \
        SpB_Matrix m_in,                                 \
        const SpB_Index *row_indices,                    \
        const SpB_Index *col_indices,                    \
        const type *values,                              \
        const SpB_Index n,                               \
        const SpB_BinaryOp dup)                          \
    {                                                    \
        if (dup == SpB_BINARYOP_NULL)                    \
        {                                                \
            SpB_Matrix_CSC_or_CSR *m =                   \
                (SpB_Matrix_CSC_or_CSR *)m_in;           \
            SpB_Matrix_CSC_or_CSR_grow(m, n);            \
                                                         \
            m->nnz = n;                                  \
                                                         \
            m->ptr[0] = 0;                               \
            SpB_Index ptr_index = 0;                     \
                                                         \
            const SpB_Index *p1_indices =                \
                m_in->format_type == SpB_CSC             \
                    ? row_indices                        \
                    : col_indices;                       \
            const SpB_Index *p2_indices =                \
                m_in->format_type == SpB_CSC             \
                    ? col_indices                        \
                    : row_indices;                       \
            for (SpB_Index i = 0; i < n; ++i)            \
            {                                            \
                m->indices[i] = p1_indices[i];           \
                ((type *)(m->val))[i] = values[i];       \
                while (ptr_index != p2_indices[i])       \
                {                                        \
                    m->ptr[++ptr_index] = i;             \
                }                                        \
            }                                            \
            m->ptr[m->ptr_len - 1] = n;                  \
            return SpB_SUCCESS;                          \
        }                                                \
        else                                             \
        {                                                \
            return SpB_FAILURE;                          \
        }                                                \
    }
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(bool, BOOL);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(int8_t, INT8);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(int16_t, INT16);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(int32_t, INT32);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(int64_t, INT64);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(uint8_t, UINT8);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(uint16_t, UINT16);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(uint32_t, UINT32);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(uint64_t, UINT64);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(float, FP32);
GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(double, FP64);
#undef GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT

typedef union
{
    bool VAL_BOOL;
    int8_t VAL_INT8;
    int16_t VAL_INT16;
    int32_t VAL_INT32;
    int64_t VAL_INT64;
    uint8_t VAL_UINT8;
    uint16_t VAL_UINT16;
    uint32_t VAL_UINT32;
    uint64_t VAL_UINT64;
    float VAL_FP32;
    double VAL_FP64;
} SpB_Build_Object_Format_ext;

typedef struct triple
{
    SpB_Index row;
    SpB_Index col;
    SpB_Build_Object_Format_ext val;
} triple;

int tri_cmp_csr(const void *a, const void *b)
{
    triple *a1 = (triple *)a;
    triple *a2 = (triple *)b;
    if ((*a1).row > (*a2).row)
        return 1;
    else if ((*a1).row < (*a2).row)
        return -1;
    else if ((*a1).row == (*a2).row && (*a1).col > (*a2).col)
        return 1;
    else if ((*a1).row == (*a2).row && (*a1).col < (*a2).col)
        return -1;
    return 0;
}
int tri_cmp_csc(const void *a, const void *b)
{
    triple *a1 = (triple *)a;
    triple *a2 = (triple *)b;
    if ((*a1).col > (*a2).col)
        return 1;
    else if ((*a1).col < (*a2).col)
        return -1;
    else if ((*a1).col == (*a2).col && (*a1).row > (*a2).row)
        return 1;
    else if ((*a1).col == (*a2).col && (*a1).row < (*a2).row)
        return -1;
    return 0;
}

#define SpB_MATRIX_CSC_OR_CSR_BUILD(type, T)                              \
    SpB_Info SpB_Matrix_CSC_or_CSR_build_##T(                             \
        SpB_Matrix m_in,                                                  \
        const SpB_Index *row_indices,                                     \
        const SpB_Index *col_indices,                                     \
        const type *values,                                               \
        const SpB_Index n,                                                \
        const SpB_BinaryOp dup)                                           \
    {                                                                     \
        if (dup == SpB_BINARYOP_NULL)                                     \
        {                                                                 \
            SpB_Matrix_CSC_or_CSR *m = (SpB_Matrix_CSC_or_CSR *)m_in;     \
            SpB_Matrix_CSC_or_CSR_grow(m, n);                             \
                                                                          \
            m->nnz = n;                                                   \
            triple *NNZ_TRIPLE = (triple *)malloc(n * sizeof(triple));    \
            memset(NNZ_TRIPLE, 0, sizeof(triple) * n);                    \
            for (SpB_Index i = 0; i < n; i++)                             \
            {                                                             \
                NNZ_TRIPLE[i].row = row_indices[i];                       \
                NNZ_TRIPLE[i].col = col_indices[i];                       \
                NNZ_TRIPLE[i].val.VAL_##T = values[i];                    \
            }                                                             \
            if (m_in->format_type == SpB_CSC)                             \
            {                                                             \
                qsort(NNZ_TRIPLE, n, sizeof(NNZ_TRIPLE[0]), tri_cmp_csc); \
            }                                                             \
            else                                                          \
            {                                                             \
                qsort(NNZ_TRIPLE, n, sizeof(NNZ_TRIPLE[0]), tri_cmp_csr); \
            }                                                             \
                                                                          \
            m->ptr[0] = 0;                                                \
            SpB_Index ptr_index = 0;                                      \
                                                                          \
            if (m_in->format_type == SpB_CSC)                             \
            {                                                             \
                for (SpB_Index i = 0; i < n; ++i)                         \
                {                                                         \
                    m->indices[i] = NNZ_TRIPLE[i].row;                    \
                    ((type *)(m->val))[i] = NNZ_TRIPLE[i].val.VAL_##T;    \
                    while (ptr_index != NNZ_TRIPLE[i].col)                \
                    {                                                     \
                        m->ptr[++ptr_index] = i;                          \
                    }                                                     \
                }                                                         \
                m->ptr[m->ptr_len - 1] = n;                               \
                return SpB_SUCCESS;                                       \
            }                                                             \
            else                                                          \
            {                                                             \
                for (SpB_Index i = 0; i < n; ++i)                         \
                {                                                         \
                    m->indices[i] = NNZ_TRIPLE[i].col;                    \
                    ((type *)(m->val))[i] = NNZ_TRIPLE[i].val.VAL_##T;    \
                    while (ptr_index != NNZ_TRIPLE[i].row)                \
                    {                                                     \
                        m->ptr[++ptr_index] = i;                          \
                    }                                                     \
                }                                                         \
                m->ptr[m->ptr_len - 1] = n;                               \
                return SpB_SUCCESS;                                       \
            }                                                             \
        }                                                                 \
        else                                                              \
        {                                                                 \
            return SpB_FAILURE;                                           \
        }                                                                 \
    }
SpB_MATRIX_CSC_OR_CSR_BUILD(bool, BOOL);
SpB_MATRIX_CSC_OR_CSR_BUILD(int8_t, INT8);
SpB_MATRIX_CSC_OR_CSR_BUILD(int16_t, INT16);
SpB_MATRIX_CSC_OR_CSR_BUILD(int32_t, INT32);
SpB_MATRIX_CSC_OR_CSR_BUILD(int64_t, INT64);
SpB_MATRIX_CSC_OR_CSR_BUILD(uint8_t, UINT8);
SpB_MATRIX_CSC_OR_CSR_BUILD(uint16_t, UINT16);
SpB_MATRIX_CSC_OR_CSR_BUILD(uint32_t, UINT32);
SpB_MATRIX_CSC_OR_CSR_BUILD(uint64_t, UINT64);
SpB_MATRIX_CSC_OR_CSR_BUILD(float, FP32);
SpB_MATRIX_CSC_OR_CSR_BUILD(double, FP64);
#undef SpB_MATRIX_CSC_OR_CSR_BUILD

SpB_Index SpB_Matrix_CSC_or_CSR_capacity(SpB_Matrix_CSC_or_CSR *m)
{
    return m->capacity;
}

// This function is created by xulei!
// It is designed for the SpB_Matrix_setElement.
// It includes many meanings and needs to be decomposed.
// It will check if the element(row, col) is stored in the CSC matrix(m).
// If true, it provides the address of the element through the pointer p.
// If false, it provides the index of the element to be added to the CSC matrix soon, and returns the SpB_NO_VALUE.
static SpB_Info SpB_Matrix_CSC_or_CSR_findElement(
    void **p,
    SpB_Matrix_CSC_or_CSR *m,
    SpB_Index row,
    SpB_Index col,
    SpB_Index *index)
{
    SpB_Index index1 =
        m->matrix_base.format_type == SpB_CSC ? col : row;
    SpB_Index index2 =
        m->matrix_base.format_type == SpB_CSC ? row : col;

    SpB_Index ptr_start = m->ptr[index1];
    *index = ptr_start;
    SpB_Index n_one_line = m->ptr[index1 + 1] - ptr_start;
    size_t size = m->matrix_base.element_size;
    for (SpB_Index i = 0; i < n_one_line; ++i)
    {
        SpB_Index iter = i + ptr_start;
        SpB_Index index2_temp = m->indices[iter];
        if (index2 == index2_temp)
        {
            *p = m->val + size * iter;
            return SpB_SUCCESS;
        }
        else if (index2 < index2_temp)
        {
            *index = iter;
            break;
        }
        else
        {
            *index = iter + 1;
        }
    }
    return SpB_NO_VALUE;
}

// This function is a binarySearch version of a SpB_Matrix_findElement.
// TODO The binarySearch for the Index of val/row array in a SpB_Matrix_Contents with a column number(target), where the start and end locate a specific line in a CSC sparse matrix and the number of elements in this row is greater than 1.
// SpB_Index binarySearch(SpB_Index *list, SpB_Index start, SpB_Index end, SpB_Index target)
// {
//     end = end - 1;
//     assert(list[start] <= list[end]);
//     while (start <= end)
//     {
//         SpB_Index middle = (start + end) / 2;
//     }
//     return SpB_SUCCESS;
// }

#define SpB_MATRIX_CSC_OR_CSR_SETELEMENT(type, T)                  \
    SpB_Info SpB_Matrix_CSC_or_CSR_setElement_##T(                 \
        SpB_Matrix m_in,                                           \
        type value,                                                \
        SpB_Index row,                                             \
        SpB_Index col)                                             \
    {                                                              \
        SpB_Matrix_CSC_or_CSR *m =                                 \
            (SpB_Matrix_CSC_or_CSR *)m_in;                         \
        size_t size = m_in->element_size;                          \
        SpB_Index index1 =                                         \
            m->matrix_base.format_type == SpB_CSC                  \
                ? col                                              \
                : row;                                             \
        SpB_Index index2 =                                         \
            m->matrix_base.format_type == SpB_CSC                  \
                ? row                                              \
                : col;                                             \
        if (value)                                                 \
        {                                                          \
            void *p = 0;                                           \
            assert(row <= m->matrix_base.row ||                    \
                   col <= m->matrix_base.col);                     \
                                                                   \
            SpB_Index index;                                       \
                                                                   \
            if (SpB_Matrix_CSC_or_CSR_findElement(                 \
                    &p, m, row, col, &index))                      \
            {                                                      \
                SpB_Index m_capacity =                             \
                    SpB_Matrix_CSC_or_CSR_capacity(m);             \
                SpB_Index m_size =                                 \
                    SpB_Matrix_CSC_or_CSR_nnz(m);                  \
                if (m_capacity <= m_size)                          \
                {                                                  \
                    SpB_Matrix_CSC_or_CSR_grow(                    \
                        m,                                         \
                        Matrix_compute_next_grow((m_capacity)));   \
                }                                                  \
                if (index < m_size)                                \
                {                                                  \
                    memmove(m->val + (index + 1) * size,           \
                            m->val + index * size,                 \
                            size * (m_size - index));              \
                                                                   \
                    memmove(m->indices + index + 1,                \
                            m->indices + index,                    \
                            sizeof(SpB_Index) * (m_size - index)); \
                }                                                  \
                ((type *)m->val)[index] = value;                   \
                m->indices[index] = index2;                        \
                for (                                              \
                    SpB_Index i = index1 + 1;                      \
                    i < m->ptr_len;                                \
                    i++)                                           \
                {                                                  \
                    m->ptr[i]++;                                   \
                }                                                  \
                m->nnz++;                                          \
            }                                                      \
            else                                                   \
            {                                                      \
                *((type *)p) = value;                              \
            }                                                      \
            return SpB_SUCCESS;                                    \
        }                                                          \
        else                                                       \
        {                                                          \
            return SpB_FAILURE;                                    \
        }                                                          \
    }

SpB_MATRIX_CSC_OR_CSR_SETELEMENT(bool, BOOL);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(int8_t, INT8);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(int16_t, INT16);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(int32_t, INT32);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(int64_t, INT64);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(uint8_t, UINT8);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(uint16_t, UINT16);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(uint32_t, UINT32);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(uint64_t, UINT64);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(float, FP32);
SpB_MATRIX_CSC_OR_CSR_SETELEMENT(double, FP64);
#undef SpB_MATRIX_CSC_OR_CSR_SETELEMENT

#define SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(type, T)          \
    SpB_Info SpB_Matrix_CSC_or_CSR_extractElement_##T(         \
        type *p,                                               \
        SpB_Matrix m_in,                                       \
        SpB_Index row,                                         \
        SpB_Index col)                                         \
    {                                                          \
        if (m_in->element_type != SpB_##T)                     \
            SpB_Error("The data types of the matrix elements and the target do not match \
in SpB_Matrix_CSC_or_CSR_extractElement!\n",                   \
                      __FILE__, __LINE__);                     \
        SpB_Matrix_CSC_or_CSR *m =                             \
            (SpB_Matrix_CSC_or_CSR *)(m_in);                   \
                                                               \
        SpB_Index index1 =                                     \
            m_in->format_type == SpB_CSC ? col : row;          \
        SpB_Index index2 =                                     \
            m_in->format_type == SpB_CSC ? row : col;          \
                                                               \
        SpB_Index ptr_start = m->ptr[index1];                  \
        SpB_Index n_one_line =                                 \
            m->ptr[index1 + 1] - ptr_start;                    \
                                                               \
        for (SpB_Index i = 0; i < n_one_line; ++i)             \
        {                                                      \
            SpB_Index index2_temp = m->indices[i + ptr_start]; \
            if (index2 == index2_temp)                         \
            {                                                  \
                *p = ((type *)m->val)[i + ptr_start];          \
                return SpB_SUCCESS;                            \
            }                                                  \
            else if (index2 < index2_temp)                     \
                break;                                         \
        }                                                      \
        return SpB_NO_VALUE;                                   \
    }

SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(bool, BOOL);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(int8_t, INT8);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(int16_t, INT16);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(int32_t, INT32);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(int64_t, INT64);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(uint8_t, UINT8);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(uint16_t, UINT16);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(uint32_t, UINT32);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(uint64_t, UINT64);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(float, FP32);
SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(double, FP64);
#undef SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT

SpB_Index SpB_Matrix_CSC_or_CSR_nnz(SpB_Matrix_CSC_or_CSR *m)
{
    return m->nnz;
}

SpB_Info SpB_Matrix_CSC_or_CSR_free(SpB_Matrix *m_in)
{
    SpB_Matrix_CSC_or_CSR **m = (SpB_Matrix_CSC_or_CSR **)m_in;
    if ((*m)->val)
    {
        free((*m)->val);
        free((*m)->indices);
        free((*m)->ptr);

        free(*m);
        return SpB_SUCCESS;
    }
    else
    {
        return SpB_FAILURE;
    }
}