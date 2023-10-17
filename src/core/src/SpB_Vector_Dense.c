#include "SpB_Vector_Dense.h"
#include "SpB_Type.h"
#include <stdlib.h>
#include <string.h>
#include "SpB_Alloc.h"
#include "SpB_Error.h"
#include "SpB_MACRO.h"
#define max(x, y) (x > y) ? x : y;
SpB_Info SpB_Vector_Dense_new(
    SpB_Vector *v_in,
    SpB_Type type,
    SpB_Index n)
{
    SpB_Vector_Dense **v =
        (SpB_Vector_Dense **)v_in;

    *v = (SpB_Vector_Dense *)
        SpB_aligned_malloc(
            sizeof(SpB_Vector_Dense));
    (*v)->vector_base.format_type = SpB_DENSE;
    (*v)->vector_base.element_type = type;
    switch (type)
    {
        SpB_TYPE_CASES(v_in);
    }
    size_t size = (*v_in)->element_size;
    if (!(*v))
        return SpB_FAILURE;
    (*v)->values = SpB_aligned_malloc(n * size);
    if (!(*v)->values)
        return SpB_FAILURE;
    // The entire vector is initialized to zero.
    memset((*v)->values, 0, n * size);
    // Set the vector length.
    (*v)->vector_base.n = n;
    return SpB_SUCCESS;
}

SpB_Info SpB_Vector_Dense_dup(
    SpB_Vector *v1,
    SpB_Vector v2)
{
    SpB_Vector_Dense **v = (SpB_Vector_Dense **)v1;
    *v = (SpB_Vector_Dense *)
        SpB_aligned_malloc(
            sizeof(SpB_Vector_Dense));
    (*v)->vector_base.format_type = SpB_DENSE;
    SpB_Type type = v2->element_type;
    (*v)->vector_base.element_type = type;
    switch (type)
    {
        SpB_TYPE_CASES(v1);
    }
    size_t size = (*v1)->element_size;
    SpB_Index n = v2->n;
    (*v1)->n = n;
    (*v1)->element_size = size;

    if (!(*v))
        return SpB_FAILURE;

    (*v)->values = SpB_aligned_malloc(n * size);
    if (!(*v)->values)
        return SpB_FAILURE;

    memcpy((*v)->values,
           ((SpB_Vector_Dense *)v2)->values, n * size);
    return SpB_SUCCESS;
}

//------------------------------------------------------------------------------
// SpB_Vector_setElement: set an entry in a vector, w (row) = x
//------------------------------------------------------------------------------
// Set a single scalar, w(row) = x, typecasting from the type of x to
// the type of w as needed.
#define SpB_VECTOR_DENSE_SET(type, T)                      \
    SpB_Info SpB_TOKEN_PASTING2(                           \
        SpB_Vector_Dense_setElement_, T)(                  \
        SpB_Vector w,                                      \
        type x,                                            \
        SpB_Index row)                                     \
    {                                                      \
        ((type *)(((SpB_Vector_Dense *)w)->values))[row] = \
            x;                                             \
        return SpB_SUCCESS;                                \
    }

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

//------------------------------------------------------------------------------
// SpB_Vector_extractElement: extract a single entry from a vector
//------------------------------------------------------------------------------
// Extract a single entry, x = v(i), typecasting from the type of v to the type
// of x, as needed.

// Returns SpB_SUCCESS if v(i) is present, and sets x to its value.

#define SpB_VECTOR_DENSE_EXTRACT(type, T)                   \
    SpB_Info SpB_TOKEN_PASTING2(                            \
        SpB_Vector_Dense_extractElement_, T)(               \
        type * x,                                           \
        const SpB_Vector v,                                 \
        SpB_Index i)                                        \
    {                                                       \
        if (v->element_type != SpB_##T)                     \
            SpB_Error("The data types of the vector \
elements and the target do not match \
in SpB_Vector_Dense_extractElement_" #T "!\n",              \
                      __FILE__, __LINE__);                  \
        (*x) =                                              \
            ((type *)(((SpB_Vector_Dense *)v)->values))[i]; \
        return SpB_SUCCESS;                                 \
    }

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

#undef SpB_VECTOR_DENSE_SET

//------------------------------------------------------------------------------
// SpB_Vector_extractTuples: extract all tuples from a vector
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2022, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// Extracts all tuples from a column, like [I,~,X] = find (v).  If
// any parameter I and/or X is NULL, then that component is not extracted.  The
// size of the I and X arrays (those that are not NULL) is given by nvals,
// which must be at least as large as SpB_nvals (&nvals, v).  The values in the
// typecasted to the type of X, as needed.

// If any parameter I and/or X is NULL, that component is not extracted.  So to
// extract just the row indices, pass I as non-NULL, and X as NULL.  This is
// like [I,~,~] = find (v).

// If v is iso and X is not NULL, the iso scalar vx [0] is expanded into X.

#define SpB_VECTOR_DENSE_EXTRACT_TUPLES(type, T)                    \
    SpB_Info SpB_TOKEN_PASTING2(                                    \
        SpB_Vector_Dense_extractTuples_, T)(                        \
        SpB_Index * I,                                              \
        type * X,                                                   \
        SpB_Index * p_nvals,                                        \
        const SpB_Vector v)                                         \
    {                                                               \
        if (*p_nvals != v->n)                                       \
        {                                                           \
            return SpB_FAILURE;                                     \
        }                                                           \
        else                                                        \
        {                                                           \
            for (SpB_Index i = 0; i < *p_nvals; i++)                \
            {                                                       \
                I[i] = i;                                           \
                X[i] =                                              \
                    ((type *)(((SpB_Vector_Dense *)v)->values))[i]; \
            }                                                       \
            return SpB_SUCCESS;                                     \
        }                                                           \
    }

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

SpB_Info SpB_Vector_Dense_free(SpB_Vector *v_in)
{
    SpB_Vector_Dense **v =
        (SpB_Vector_Dense **)v_in;
    if ((*v)->values)
    {
        free((*v)->values);
        free(*v);
        return SpB_SUCCESS;
    }
    else
    {
        return SpB_FAILURE;
    }
}

#define SpB_VECTOR_DENSE_BUILD(type, T)           \
    SpB_Info SpB_Vector_Dense_build_##T(          \
        SpB_Vector_Dense *w,                      \
        const SpB_Index *indices,                 \
        const type *values,                       \
        SpB_Index n,                              \
        const SpB_BinaryOp dup)                   \
    {                                             \
        if (dup == SpB_BINARYOP_NULL)             \
        {                                         \
            SpB_Index n_w = w->vector_base.n;     \
            SpB_Index element_size =              \
                w->vector_base.element_size;      \
            memset(w->values, 0,                  \
                   n_w *element_size);            \
                                                  \
            for (SpB_Index i = 0; i < n; ++i)     \
            {                                     \
                ((type *)w->values)[indices[i]] = \
                    values[i];                    \
            }                                     \
        }                                         \
        else                                      \
        {                                         \
            SpB_Error("SpB_BinaryOp that have not yet been implemented \
in SpB_Vector_Dense_build_" #T ". !\n",           \
                      __FILE__, __LINE__);        \
        }                                         \
                                                  \
        return SpB_SUCCESS;                       \
    }

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
