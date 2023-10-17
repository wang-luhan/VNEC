#include "SpB_Vector_Sparse.h"
#include "SpB_Type.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "SpB_Alloc.h"
#include "SpB_Error.h"
#include "SpB_Debug.h"

#define Vector_compute_next_grow(size) \
    ((size) ? ((size) << 1) : 1)

SpB_Info SpB_Vector_Sparse_grow(
    SpB_Vector_Sparse *v,
    SpB_Index capacity)
{
    if (capacity > v->capacity)
    {
        size_t size =
            v->vector_base.element_size;
        if (v->values)
            v->values =
                SpB_aligned_realloc(
                    v->values, size * capacity);
        else
            v->values =
                SpB_aligned_malloc(
                    size * capacity);
        assert(v->values);
        if (v->indices)
            v->indices = (SpB_Index *)
                SpB_aligned_realloc(
                    v->indices,
                    sizeof(SpB_Index) * capacity);
        else
            v->indices = (SpB_Index *)
                SpB_aligned_malloc(
                    sizeof(SpB_Index) * capacity);
        assert(v->indices);
        v->capacity = capacity;
    }
    return SpB_SUCCESS;
}

SpB_Info SpB_Vector_Sparse_new(
    SpB_Vector *v_in,
    SpB_Type type,
    SpB_Index n)
{
    SpB_Vector_Sparse **v =
        (SpB_Vector_Sparse **)v_in;

    *v = (SpB_Vector_Sparse *)
        SpB_aligned_malloc(
            sizeof(SpB_Vector_Sparse));
    (*v)->vector_base.format_type =
        SpB_SPARSE;
    (*v)->vector_base.element_type =
        type;
    (*v)->vector_base.n = n;
    (*v)->values = NULL;
    (*v)->indices = NULL;
    (*v)->nnz = 0;
    (*v)->capacity = 0;
    (*v)->is_sorted = true;
    switch (type)
    {
        SpB_TYPE_CASES(v_in);
    }
    return SpB_SUCCESS;
}

static SpB_Index SpB_Vector_Sparse_capacity(
    SpB_Vector_Sparse *v)
{
    return v->capacity;
}

static SpB_Index SpB_Vector_Sparse_nnz(
    SpB_Vector_Sparse *v)
{
    return v->nnz;
}

static bool SpB_Vector_Sparse_findElement(
    SpB_Vector_Sparse *v,
    SpB_Index target,
    SpB_Index *ans)
{
    if (v->nnz)
    {
        SpB_Index left = 1;
        SpB_Index right = v->nnz;

        while (left <= right)
        {
            SpB_Index mid = ((right - left) >> 1) + left;
            SpB_Index val = v->indices[mid - 1];
            if (target == val)
            {
                *ans = mid - 1;
                return true;
            }
            else if (target < val)
            {
                *ans = mid - 1;
                right = mid - 1;
            }
            else
            {
                *ans = mid;
                left = mid + 1;
            }
        }
    }
    return false;
}

// TODO : 完成未排序稀疏格式的插入。
//  SpB_Info SpB_Vector_Sparse_set_unsorted_bool
//  (
//      SpB_Vector w,
//      bool x,
//      SpB_Index i
//  )
//  {
//      SpB_Vector_Sparse *v = (SpB_Vector_Sparse *)w;

//     //if v is sorted, this function should not be called !
//     if(v->is_sorted)
//     {
//         return SpB_FAILURE;
//     }

//     for(int i =0 ; i < v->nnz; ++i){

//     }

// };

//------------------------------------------------------------------------------
// SpB_Vector_setElement: set an entry in a vector, w (row) = x
//------------------------------------------------------------------------------
// Set a single scalar, w(row) = x, typecasting from the type of x to
// the type of w as needed.
#define SpB_VECTOR_SPARSE_SET(type, T)                         \
    SpB_Info SpB_Vector_Sparse_setElement_##T /* w(i) = x */   \
        (                                                      \
            SpB_Vector w, /* vector to modify           */     \
            type x,       /* scalar to assign to w(i) */       \
            SpB_Index i   /* index                  */         \
        )                                                      \
    {                                                          \
        SpB_Vector_Sparse *v = (SpB_Vector_Sparse *)w;         \
        size_t size = w->element_size;                         \
                                                               \
        assert(i < w->n);                                      \
        SpB_Index index = 0;                                   \
        if (SpB_Vector_Sparse_findElement(v, i, &index))       \
        {                                                      \
            if (x)                                             \
                ((type *)v->values)[index] = x;                \
            else                                               \
            {                                                  \
                v->nnz--;                                      \
                memmove(v->values + index * size,              \
                        v->values + (index + 1) * size,        \
                        size * (v->nnz - index));              \
                memmove(v->indices + index,                    \
                        v->indices + index + 1,                \
                        sizeof(SpB_Index) * (v->nnz - index)); \
            }                                                  \
        }                                                      \
        else if (x)                                            \
        {                                                      \
            SpB_Index v_capacity =                             \
                SpB_Vector_Sparse_capacity(v);                 \
            SpB_Index v_size = SpB_Vector_Sparse_nnz(v);       \
            if (v_capacity <= v_size)                          \
            {                                                  \
                SpB_Vector_Sparse_grow(                        \
                    v,                                         \
                    Vector_compute_next_grow((v_capacity)));   \
            }                                                  \
            if (index < v_size)                                \
            {                                                  \
                memmove(v->values + (index + 1) * size,        \
                        v->values + index * size,              \
                        size * (v_size - index));              \
                memmove(v->indices + index + 1,                \
                        v->indices + index,                    \
                        sizeof(SpB_Index) * (v_size - index)); \
            }                                                  \
            ((type *)v->values)[index] = x;                    \
            v->indices[index] = i;                             \
            v->nnz++;                                          \
        }                                                      \
        return SpB_SUCCESS;                                    \
    }
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

//------------------------------------------------------------------------------
// SpB_Vector_extractElement: extract a single entry from a vector
//------------------------------------------------------------------------------
// Extract a single entry, x = v(i), typecasting from the type of v to the type
// of x, as needed.

// Returns SpB_SUCCESS if v(i) is present, and sets x to its value.

#define SpB_VECTOR_SPARSE_EXTRACT(type, T)         \
    SpB_Info SpB_Vector_Sparse_extractElement_##T( \
        type *x,                                   \
        const SpB_Vector v_in,                     \
        SpB_Index i)                               \
    {                                              \
        if (v_in->element_type != SpB_##T)         \
            SpB_Error("The data types of the vector elements \
and the target do not match in \
SpB_Vector_Sparse_extractElement!\n",              \
                      __FILE__, __LINE__);         \
                                                   \
        SpB_Vector_Sparse *v =                     \
            (SpB_Vector_Sparse *)v_in;             \
                                                   \
        SpB_Index index = 0;                       \
        if (SpB_Vector_Sparse_findElement(         \
                v, i, &index))                     \
        {                                          \
            *x = ((type *)v->values)[index];       \
            return SpB_SUCCESS;                    \
        }                                          \
        else                                       \
        {                                          \
            return SpB_NO_VALUE;                   \
        }                                          \
    }
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

#undef SpB_VECTOR_SPARSE_SET

//------------------------------------------------------------------------------
// SpB_Vector_extractTuples: extract all tuples from a vector
//------------------------------------------------------------------------------

#define SpB_VECTOR_SPARSE_EXTRACT_TUPLES(type, T)     \
    SpB_Info SpB_TOKEN_PASTING2(                      \
        SpB_Vector_Sparse_extractTuples_, T)(         \
        SpB_Index * I,                                \
        type * X,                                     \
        SpB_Index * p_nvals,                          \
        const SpB_Vector v)                           \
    {                                                 \
        if (*p_nvals != v->n)                         \
        {                                             \
            return SpB_FAILURE;                       \
        }                                             \
        else                                          \
        {                                             \
            for (SpB_Index i = 0; i < *p_nvals; i++)  \
            {                                         \
                I[i] = i;                             \
                type value = (type)0;                 \
                SpB_Vector_Sparse_extractElement_##T( \
                    &value, v, i);                    \
                X[i] = value;                         \
            }                                         \
            return SpB_SUCCESS;                       \
        }                                             \
    }

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

SpB_Info SpB_Vector_Sparse_free(SpB_Vector *v_in)
{
    SpB_Vector_Sparse **v = (SpB_Vector_Sparse **)v_in;
    if ((*v)->values)
    {
        free((*v)->values);
    }
    else
    {
        return SpB_FAILURE;
    }
    if ((*v)->indices)
    {
        free((*v)->indices);
    }
    else
    {
        return SpB_FAILURE;
    }
    if ((*v))
    {
        free(*v);
    }
    else
    {
        return SpB_FAILURE;
    }

    return SpB_SUCCESS;
}

#define SpB_VECTOR_SPARSE_BUILD(type, T)               \
    SpB_Info SpB_Vector_Sparse_build_##T(              \
        SpB_Vector_Sparse *v,                          \
        const SpB_Index *indices,                      \
        const type *values,                            \
        SpB_Index n,                                   \
        const SpB_BinaryOp dup)                        \
    {                                                  \
        if (dup == SpB_BINARYOP_NULL)                  \
        {                                              \
            SpB_Index v_capacity = v->capacity;        \
            SpB_Info info;                             \
            if (v_capacity <= n)                       \
            {                                          \
                SAFE_CALL(                             \
                    SpB_Vector_Sparse_grow(v, n + 1)); \
                v->capacity = n + 1;                   \
            }                                          \
            memcpy(v->values,                          \
                   values, n * sizeof(type));          \
            memcpy(v->indices,                         \
                   indices, n * sizeof(SpB_Index));    \
            v->nnz = n;                                \
        }                                              \
        else                                           \
        {                                              \
            SpB_Error("SpB_BinaryOp that have not yet been implemented \
in SpB_Vector_Sparse_build_" #T ". !\n",               \
                      __FILE__, __LINE__);             \
        }                                              \
        return SpB_SUCCESS;                            \
    }
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