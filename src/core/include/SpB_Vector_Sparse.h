#pragma once
#include "SpB_Vector.h"
#include <stdint.h>
#include <stdbool.h>
#include "SpB_MACRO.h"

/**
 * @ingroup SpB_Vector
 * @defgroup SpB_Vector_Sparse
 * @brief A sparse vector class
 * @details The sparse vector format uses a list approach to store only all non-zero elements of the vector, wherthe list can be indexed ordered and unordered.
 * @{
 */
typedef struct
{
    struct SpB_Vector_opaque vector_base; /**< The base vector msg*/
    void *values;                         /**< The data array */
    SpB_Index *indices;                   /**< The index array */
    SpB_Index nnz;                        /**< The number of non zero; the size of array valuse and indices*/
    SpB_Index capacity;                   /**< The size of the current array request.
                                           * When adding elements to make nnz larger than the capacity, it needs to be expanded*/
    bool is_sorted;                       /**< A notation for whether the array \link values \endlink and \link indices \endlink is ordered. */
} SpB_Vector_Sparse;

/**
 * @ingroup SpB_Vector_Sparse
 * @brief Create a SpB_Vector_Sparse object
 * @param[out] v A Pointer to the newly created sparse vector
 * @param[in] type Type defines the data type of the vector elements
 * @param[in] n Vector length
 * @return SpB_Info
 */
SpB_Info SpB_Vector_Sparse_new(SpB_Vector *v, SpB_Type type, SpB_Index n);

/**
 * @ingroup SpB_Vector_Sparse
 * @brief Create SpB_Vector_Sparse_setElement_T functions with different data types
 * @param type Macro param: Data Type, e.g., bool, int8_t, int16_t...
 * @param T Macro param: Function Suffix, e.g., BOOL, INT8, INT16...
 * @param[in,out] w The input vector
 * @param[in] x The set value
 * @param[in] row The index of the input vector
 * @return SpB_Info
 */
#define SpB_VECTOR_SPARSE_SET(type, T)                                             \
    SpB_Info SpB_TOKEN_PASTING2(SpB_Vector_Sparse_setElement_, T) /* w(row) = x */ \
        (                                                                          \
            SpB_Vector w, /* vector to modify           */                         \
            type x,       /* scalar to assign to w(row) */                         \
            SpB_Index row /* row index                  */                         \
        )

SpB_VECTOR_SPARSE_SET(bool, BOOL);
SpB_VECTOR_SPARSE_SET(int8_t, INT8);
SpB_VECTOR_SPARSE_SET(int16_t, INT16);
SpB_VECTOR_SPARSE_SET(int32_t, INT32);
SpB_VECTOR_SPARSE_SET(int64_t, INT64);
SpB_VECTOR_SPARSE_SET(uint8_t, UINT8);
SpB_VECTOR_SPARSE_SET(uint16_t, UINT16);
SpB_VECTOR_SPARSE_SET(uint32_t, UINT32);
SpB_VECTOR_SPARSE_SET(uint64_t, UINT64);
SpB_VECTOR_SPARSE_SET(float, FP32);
SpB_VECTOR_SPARSE_SET(double, FP64);
#undef SpB_VECTOR_SPARSE_SET

/**
 * @ingroup SpB_Vector_Sparse
 * @brief Create SpB_Vector_Sparse_extractElement_T functions with different data types
 * @param type Macro param: Data Type, e.g., bool, int8_t, int16_t...
 * @param T Macro param: Function Suffix, e.g., BOOL, INT8, INT16...
 * @param[out] x The pointer to result
 * @param[in] v The input vector
 * @param[in] row The index of the input vector
 * @return SpB_Info
 */
#define SpB_VECTOR_SPARSE_EXTRACT(type, T)                                           \
    SpB_Info SpB_TOKEN_PASTING2(SpB_Vector_Sparse_extractElement_, T) /* x = v(i) */ \
        (                                                                            \
            type * x,           /* scalar to extract           */                    \
            const SpB_Vector v, /* vector to extract a scalar from */                \
            SpB_Index i         /* index                  */                         \
        )

SpB_VECTOR_SPARSE_EXTRACT(bool, BOOL);
SpB_VECTOR_SPARSE_EXTRACT(int8_t, INT8);
SpB_VECTOR_SPARSE_EXTRACT(int16_t, INT16);
SpB_VECTOR_SPARSE_EXTRACT(int32_t, INT32);
SpB_VECTOR_SPARSE_EXTRACT(int64_t, INT64);
SpB_VECTOR_SPARSE_EXTRACT(uint8_t, UINT8);
SpB_VECTOR_SPARSE_EXTRACT(uint16_t, UINT16);
SpB_VECTOR_SPARSE_EXTRACT(uint32_t, UINT32);
SpB_VECTOR_SPARSE_EXTRACT(uint64_t, UINT64);
SpB_VECTOR_SPARSE_EXTRACT(float, FP32);
SpB_VECTOR_SPARSE_EXTRACT(double, FP64);
#undef SpB_VECTOR_SPARSE_EXTRACT

SpB_Info SpB_Vector_Sparse_extractTuples(SpB_Index *indices, int *values, SpB_Index *n, const SpB_Vector v);
/**
 * @ingroup SpB_Vector_Sparse
 * @brief Create SpB_Vector_Sparse_extractTuples_T functions with different data types
 * @param type Macro param: Data Type, e.g., bool, int8_t, int16_t...
 * @param T Macro param: Function Suffix, e.g., BOOL, INT8, INT16...
 * @param[in,out] I Array for returning row indices of tuples
 * @param[out] X array for returning values of tuples
 * @param[in] nvals Pointer to \link I \endlink and \link X \endlink size.
 * @param[in] v Vector to extract tuples
 * @return SpB_Info
 */
#define SpB_VECTOR_SPARSE_EXTRACT_TUPLES(type, T)                                           \
    SpB_Info SpB_TOKEN_PASTING2(SpB_Vector_Sparse_extractTuples_, T) /*[I,~,X] = find (v)*/ \
        (                                                                                   \
            SpB_Index * I,     /*array for returning row indices of tuples*/                \
            type * X,          /*array for returning values of tuples*/                     \
            SpB_Index * nvals, /*I, X size on input; # tuples on output*/                   \
            const SpB_Vector v /*vector to extract tuples*/                                 \
        )

SpB_VECTOR_SPARSE_EXTRACT_TUPLES(bool, BOOL);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(int8_t, INT8);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(int16_t, INT16);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(int32_t, INT32);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(int64_t, INT64);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(uint8_t, UINT8);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(uint16_t, UINT16);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(uint32_t, UINT32);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(uint64_t, UINT64);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(float, FP32);
SpB_VECTOR_SPARSE_EXTRACT_TUPLES(double, FP64);

#undef SpB_VECTOR_SPARSE_EXTRACT_TUPLES

/**
 * @ingroup SpB_Vector_Sparse
 * @brief Frees the space of a sparse vector object
 * @param[in] v A Pointer to the sparse vector needed to be freed
 * @return SpB_Info
 */
SpB_Info SpB_Vector_Sparse_free(SpB_Vector *v);

/**
 * @ingroup SpB_Vector_Sparse
 * @brief Growing the capacity of a sparse vector
 * @param[in] v A Pointer to the sparse vector needed to be freed
 * @param[in] capacity New capacity to be allocated
 * @return SpB_Info
 */
SpB_Info SpB_Vector_Sparse_grow(SpB_Vector_Sparse *v, SpB_Index capacity);

/**
 * @anchor SpB_Vector_Sparse_build
 * @name SpB_Vector_Sparse_build
 * @ingroup SpB_Vector_Sparse
 * @brief Store elements from tuples into a vector
 * @details
 * @param[in,out] w An existing Vector object to store the result.
 * @param [in] indices Pointer to an array of indices.
 * @param [in] values Pointer to an array of scalars of a type that is compatible with the domain of vector w.
 * @param [in] n The number of entries contained in each array (the same for indices and values).
 * @param [in] dup An associative and commutative binary operator to apply when duplicate values for the same location are present in the input arrays. All three domains of dup must be the same. If **dup** is SpB_BINARYOP_NULL, then duplicate locations will result in an error. The current implementation only supports SpB_BINARYOP_NULL.
 * @return SpB_Info
 * @todo Support multiple **dup** operations
 */
///@{
#define SpB_VECTOR_SPARSE_BUILD(type, T)                           \
    SpB_Info SpB_Vector_Sparse_build_##T(SpB_Vector_Sparse *w,     \
                                         const SpB_Index *indices, \
                                         const type *values,       \
                                         SpB_Index n,              \
                                         const SpB_BinaryOp dup)

SpB_VECTOR_SPARSE_BUILD(bool, BOOL);
SpB_VECTOR_SPARSE_BUILD(int8_t, INT8);
SpB_VECTOR_SPARSE_BUILD(int16_t, INT16);
SpB_VECTOR_SPARSE_BUILD(int32_t, INT32);
SpB_VECTOR_SPARSE_BUILD(int64_t, INT64);
SpB_VECTOR_SPARSE_BUILD(uint8_t, UINT8);
SpB_VECTOR_SPARSE_BUILD(uint16_t, UINT16);
SpB_VECTOR_SPARSE_BUILD(uint32_t, UINT32);
SpB_VECTOR_SPARSE_BUILD(uint64_t, UINT64);
SpB_VECTOR_SPARSE_BUILD(float, FP32);
SpB_VECTOR_SPARSE_BUILD(double, FP64);
#undef SpB_VECTOR_SPARSE_BUILD
///@}
/**@}*/ // end for SpB_Vector_Sparse