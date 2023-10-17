#pragma once
#include "SpB_Vector.h"
#include <stdint.h>
#include <stdbool.h>
#include "SpB_MACRO.h"

/**
 * @ingroup SpB_Vector
 * @defgroup SpB_Vector_Dense
 * @brief A Dense vector class
 * @details The dense vector format uses a array to represent all elements(including zero elements) in a vector.
 * @{
 */

typedef struct
{
    struct SpB_Vector_opaque vector_base;
    void *values;
} SpB_Vector_Dense;

SpB_Info SpB_Vector_Dense_new(SpB_Vector *v, SpB_Type type, SpB_Index n);

SpB_Info SpB_Vector_Dense_dup(SpB_Vector *v1, SpB_Vector v2);

SpB_Info SpB_Vector_Dense_eWiseAdd(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, SpB_BinaryOp op, const SpB_Vector u, const SpB_Vector v, const SpB_Descriptor desc);

#define SpB_VECTOR_DENSE_SET(type, T)                                             \
    SpB_Info SpB_TOKEN_PASTING2(SpB_Vector_Dense_setElement_, T) /* w(row) = x */ \
        (                                                                         \
            SpB_Vector w, /* vector to modify           */                        \
            type x,       /* scalar to assign to w(row) */                        \
            SpB_Index row /* row index                  */                        \
        )

SpB_VECTOR_DENSE_SET(bool, BOOL);
SpB_VECTOR_DENSE_SET(int8_t, INT8);
SpB_VECTOR_DENSE_SET(int16_t, INT16);
SpB_VECTOR_DENSE_SET(int32_t, INT32);
SpB_VECTOR_DENSE_SET(int64_t, INT64);
SpB_VECTOR_DENSE_SET(uint8_t, UINT8);
SpB_VECTOR_DENSE_SET(uint16_t, UINT16);
SpB_VECTOR_DENSE_SET(uint32_t, UINT32);
SpB_VECTOR_DENSE_SET(uint64_t, UINT64);
SpB_VECTOR_DENSE_SET(float, FP32);
SpB_VECTOR_DENSE_SET(double, FP64);
#undef SpB_VECTOR_DENSE_SET

#define SpB_VECTOR_DENSE_EXTRACT(type, T)                                           \
    SpB_Info SpB_TOKEN_PASTING2(SpB_Vector_Dense_extractElement_, T) /* x = v(i) */ \
        (                                                                           \
            type * x,           /* scalar to extract           */                   \
            const SpB_Vector v, /* vector to extract a scalar from */               \
            SpB_Index i         /* index                  */                        \
        )

SpB_VECTOR_DENSE_EXTRACT(bool, BOOL);
SpB_VECTOR_DENSE_EXTRACT(int8_t, INT8);
SpB_VECTOR_DENSE_EXTRACT(int16_t, INT16);
SpB_VECTOR_DENSE_EXTRACT(int32_t, INT32);
SpB_VECTOR_DENSE_EXTRACT(int64_t, INT64);
SpB_VECTOR_DENSE_EXTRACT(uint8_t, UINT8);
SpB_VECTOR_DENSE_EXTRACT(uint16_t, UINT16);
SpB_VECTOR_DENSE_EXTRACT(uint32_t, UINT32);
SpB_VECTOR_DENSE_EXTRACT(uint64_t, UINT64);
SpB_VECTOR_DENSE_EXTRACT(float, FP32);
SpB_VECTOR_DENSE_EXTRACT(double, FP64);
#undef SpB_VECTOR_DENSE_EXTRACT

#define SpB_VECTOR_DENSE_EXTRACT_TUPLES(type, T)                                           \
    SpB_Info SpB_TOKEN_PASTING2(SpB_Vector_Dense_extractTuples_, T) /*[I,~,X] = find (v)*/ \
        (                                                                                  \
            SpB_Index * I,     /*array for returning row indices of tuples*/               \
            type * X,          /*array for returning values of tuples*/                    \
            SpB_Index * nvals, /*I, X size on input; # tuples on output*/                  \
            const SpB_Vector v /*vector to extract tuples*/                                \
        )

SpB_VECTOR_DENSE_EXTRACT_TUPLES(bool, BOOL);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(int8_t, INT8);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(int16_t, INT16);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(int32_t, INT32);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(int64_t, INT64);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(uint8_t, UINT8);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(uint16_t, UINT16);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(uint32_t, UINT32);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(uint64_t, UINT64);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(float, FP32);
SpB_VECTOR_DENSE_EXTRACT_TUPLES(double, FP64);

#undef SpB_VECTOR_DENSE_EXTRACT_TUPLES

SpB_Info SpB_Vector_Dense_free(SpB_Vector *v);

    /**
     * @anchor SpB_Vector_Dense_build
     * @name SpB_Vector_Dense_build
     * @brief Store elements from tuples into a dense vetctor
     * @details
     *
     * @param[in,out] w An existing dense Vector object to store the result.
     * @param[in] indices Pointer to an array of indices.
     * @param[in] values Pointer to an array of scalars of a type that is compatible with the domain of vector **w**.
     * @param[in] n The number of entries contained in each array (the same for indices and values).
     * @param[in] dup An associative and commutative binary operator to apply when duplicate values for the same location are present in the input arrays. All three domains of **dup** must be the same. If dup is SpB_BINARYOP_NULL, then duplicate locations will result in an error.
     * @return SpB_Info
     */
    ///@{

#define SpB_VECTOR_DENSE_BUILD(type, T)                           \
    SpB_Info SpB_Vector_Dense_build_##T(SpB_Vector_Dense *w,      \
                                        const SpB_Index *indices, \
                                        const type *values,       \
                                        SpB_Index n,              \
                                        const SpB_BinaryOp dup);
SpB_VECTOR_DENSE_BUILD(bool, BOOL);
SpB_VECTOR_DENSE_BUILD(int8_t, INT8);
SpB_VECTOR_DENSE_BUILD(int16_t, INT16);
SpB_VECTOR_DENSE_BUILD(int32_t, INT32);
SpB_VECTOR_DENSE_BUILD(int64_t, INT64);
SpB_VECTOR_DENSE_BUILD(uint8_t, UINT8);
SpB_VECTOR_DENSE_BUILD(uint16_t, UINT16);
SpB_VECTOR_DENSE_BUILD(uint32_t, UINT32);
SpB_VECTOR_DENSE_BUILD(uint64_t, UINT64);
SpB_VECTOR_DENSE_BUILD(float, FP32);
SpB_VECTOR_DENSE_BUILD(double, FP64);
#undef SpB_VECTOR_DENSE_BUILD
///@}
/**@}*/ // end for SpB_Vector_Dense