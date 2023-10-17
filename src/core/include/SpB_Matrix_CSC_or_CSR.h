#pragma once
#include "SpB_Matrix.h"

/**
 * @ingroup SpB_Matrix
 * @defgroup SpB_Matrix_CSC_or_CSR
 * @brief A CSC/CSR format matrix class
 * @details Sparse matrix stored in traditional CSC (Compressed sparse column) or CSR (compressed sparse row) format. Since these two formats have symmetry, a class is reused to manage both formats.
 * @{
 */

typedef struct
{
    struct SpB_Matrix_opaque matrix_base;
    void *val;          ///< Value of each non-zero element.
    SpB_Index *indices; ///< Row coordinate for CSC, and col coordinate for CSR.
    SpB_Index *ptr;     ///< Col initial position for CSC, and row initial position for CSR.
    SpB_Index ptr_len;  ///< Length of ptr array.
    SpB_Index nnz;      ///< Total non-zero elements.
    SpB_Index capacity; ///< the capacity of val, indices array.
} SpB_Matrix_CSC_or_CSR;

SpB_Index SpB_Matrix_CSC_or_CSR_nnz(SpB_Matrix_CSC_or_CSR *m);

SpB_Info SpB_Matrix_CSC_or_CSR_new(SpB_Matrix *m_in,
                                   SpB_Type type,
                                   SpB_Index row,
                                   SpB_Index col,
                                   SpB_Matrix_Format_Value_ext format_type);

/**
 * @anchor SpB_Matrix_CSC_or_CSR_build
 * @name SpB_Matrix_CSC_or_CSR_build
 * @brief Store elements from tuples into a CSC/CSR matrix.
 * @param[in,out] m_in An existing CSC/CSR Matrix object to store the result.
 * @param[in] row_indices Pointer to an array of row indices.
 * @param[in] col_indices Pointer to an array of column indices.
 * @param[in] values Pointer to an array of scalars of a type that is compatible with the domain of matrix, m_in.
 * @param[in] n The number of entries contained in each array (the same for row_indices, col_indices, and values).
 * @param[in] dup An associative and commutative binary operator to apply when duplicate values for the same location are present in the input arrays. All three domains of dup must be the same. If **dup** is SpB_BINARYOP_NULL, then duplicate locations will result in an error.
 * @return SpB_Info
 * @todo (1) Support multiple **dup** operations; the current implementation only supports SpB_BINARYOP_NULL.
 * @todo (2) Sort the input arrays : **row_indices**, **col_indices**, **values**.
 */
///@{
#define GRB_MATRIX_CSC_OR_CSR_FAST_BUILD_EXT(type, T)    \
    SpB_Info SpB_Matrix_CSC_or_CSR_fast_build_##T##_ext( \
        SpB_Matrix m_in,                                 \
        const SpB_Index *row_indices,                    \
        const SpB_Index *col_indices,                    \
        const type *values,                              \
        const SpB_Index n,                               \
        const SpB_BinaryOp dup);
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

#define SpB_MATRIX_CSC_OR_CSR_BUILD(type, T)  \
    SpB_Info SpB_Matrix_CSC_or_CSR_build_##T( \
        SpB_Matrix m_in,                          \
        const SpB_Index *row_indices,             \
        const SpB_Index *col_indices,             \
        const type *values,                       \
        const SpB_Index n,                        \
        const SpB_BinaryOp dup);
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

///@}

/**
 * @anchor SpB_Matrix_CSC_or_CSR_setElement
 * @name SpB_Matrix_CSC_or_CSR_setElement
 * @brief Set a single element in a CSC/CSR matrix
 * @param[in,out] m_in An existing CSC/CSR matrix for which an element is to be assigned
 * @param[in] value Scalar to assign. Its domain (type) must be compatible with the domain of m_in.
 * @param[in] row_index Row index of element to be assigned.
 * @param[in] col_index Column index of element to be assigned.
 * @return SpB_Info
 */
///@{
#define SpB_MATRIX_CSC_OR_CSR_SETELEMENT(type, T)                      \
    SpB_Info SpB_Matrix_CSC_or_CSR_setElement_##T(SpB_Matrix m_in,     \
                                                  type value,          \
                                                  SpB_Index row_index, \
                                                  SpB_Index col_index)
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
///@}

/**
 * @anchor SpB_Matrix_CSC_or_CSR_extractElement
 * @name SpB_Matrix_CSC_or_CSR_extractElement
 * @brief Extract a single element from a CSC/CSR matrix
 * @param[in,out] val An existing scalar whose domain is compatible with the domain of matrix **m_in**. On successful return, this scalar holds the result of the extract. Any previous value stored in **val** is overwritten.
 * @param[in] m_in The CSC/CSR matrix from which an element is extracted.
 * @param[in] row_index The row index of location in **m_in** to extract.
 * @param[in] col_index The row index of location in **m_in** to extract.
 * @return SpB_Info
 */
///@{
#define SpB_MATRIX_CSC_OR_CSR_EXTRACTELEMENT(type, T)                        \
    SpB_Info SpB_Matrix_CSC_or_CSR_extractElement_##T(type *val,             \
                                                      const SpB_Matrix m_in, \
                                                      SpB_Index row_index,   \
                                                      SpB_Index col_index)

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
///@}

SpB_Info SpB_Matrix_CSC_or_CSR_free(SpB_Matrix *m_in);

/**@}*/
