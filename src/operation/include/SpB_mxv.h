#pragma once
#include "OpenSparseBLAS.h"
#include "SpB_Vector.h"
#include "SpB_Matrix.h"
#include "SpB_Matrix_CSC_or_CSR.h"
#include "SpB_Vector_Dense.h"
#include <assert.h>
#include "SpB_Error.h"
#include "SpB_Debug.h"
#include "SpB_Vector_Sparse.h"
#include <string.h>
#include "SpB_Alloc.h"
#include "SpB_Global.h"
#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#define always_inline __inline__ __attribute__((always_inline))
#ifdef ARM_NEON
#include <arm_neon.h>
#define NUM_THREADS 48
#define nLanes_f32 4
#define nLanes_f64 2
#define IRD_thr_fp32 0.543
#define IRD_thr_fp64 0.76
#endif
#ifdef X86_SIMD
#include <immintrin.h>
#define NUM_THREADS 36
#define nLanes_f32 16
#define nLanes_f64 8
#define IRD_thr_fp32 0.437
#define IRD_thr_fp64 0.477
#endif

#define max(x, y) (x > y) ? x : y;
#define min(x, y) (x < y) ? x : y;



#ifdef X86_SIMD
always_inline void AVX512_spvv_cross_row_fp32(const int *rows, int *row_begin, int *row_end, int *col, float *nnz, float *x, SpB_Vector y)
{
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    __m512i rs = _mm512_loadu_si512((const __m512i *)rows);
    __m512 acc = _mm512_setzero_ps();

    int rowlen = *row_end - *row_begin;
    int base = *row_begin;
    {
        int idx0 = rows[0];
        int idx1 = rows[1];
        int idx2 = rows[2];
        int idx3 = rows[3];
        int idx4 = rows[4];
        int idx5 = rows[5];
        int idx6 = rows[6];
        int idx7 = rows[7];

        int idx8 = rows[8];
        int idx9 = rows[9];
        int idx10 = rows[10];
        int idx11 = rows[11];
        int idx12 = rows[12];
        int idx13 = rows[13];
        int idx14 = rows[14];
        int idx15 = rows[15];

        _mm_prefetch((float *)(((float *)yy->values) + idx0), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx1), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx2), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx3), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx4), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx5), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx6), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx7), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx8), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx9), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx10), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx11), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx12), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx13), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx14), _MM_HINT_T0);
        _mm_prefetch((float *)(((float *)yy->values) + idx15), _MM_HINT_T0);

        // __builtin_prefetch(((float *)yy->values) + idx0, 0, 3);
        // __builtin_prefetch(((float *)yy->values) + idx1, 0, 3);
        // __builtin_prefetch(((float *)yy->values) + idx2, 0, 3);
        // __builtin_prefetch(((float *)yy->values) + idx3, 0, 3);
    }
    for (int c = 0; c < rowlen; c++)
    {
        int offset = base + c * nLanes_f32;
        __m512i cc = _mm512_loadu_si512((const __m512i *)(col + offset));
        __m512 nz = _mm512_loadu_ps(nnz + offset);
        __m512 xx = _mm512_i32gather_ps(cc, x, 4);
        acc = _mm512_fmadd_ps(nz, xx, acc);
    }
    _mm512_i32scatter_ps(((float *)yy->values), rs, acc, 4);
}

always_inline float AVX512_spvv_in_row_fp32(int *col, float *nnz, int rowlen, float *x)
{
    int limit = rowlen - 15;
    int *col_p;
    float *nnz_p;
    float sum = 0.0f;
    __m512i c1;
    __m512 v1, v2, s;
    s = _mm512_setzero_ps();
    int i = 0;
    for (i = 0; i < limit; i += nLanes_f32)
    {
        col_p = col + i;
        nnz_p = nnz + i;
        c1 = _mm512_loadu_si512((const __m512i *)col_p);
        v2 = _mm512_i32gather_ps(c1, x, 4);
        v1 = _mm512_loadu_ps(nnz_p);
        s = _mm512_fmadd_ps(v1, v2, s);
    }
    sum += _mm512_reduce_add_ps(s);
    for (; i < rowlen; i++)
    {
        sum += nnz[i] * x[col[i]];
    }
    return sum;
}

always_inline double AVX512_spvv_in_row_fp64(int *col, double *nnz, int rowlen, double *x)
{
    int limit = rowlen - 7;
    int *col_p;
    double *nnz_p;
    double sum = 0.0f;
    __m256i c1;
    __m512d v1, v2, s;
    s = _mm512_setzero_pd();
    int i = 0;
    for (i = 0; i < limit; i += nLanes_f64)
    {
        col_p = col + i;
        nnz_p = nnz + i;
        c1 = _mm256_loadu_si256((const __m256i *)col_p);
        v2 = _mm512_i32gather_pd(c1, x, 8);
        v1 = _mm512_loadu_pd(nnz_p);
        s = _mm512_fmadd_pd(v1, v2, s);
    }
    sum += _mm512_reduce_add_pd(s);
    for (; i < rowlen; i++)
    {
        sum += nnz[i] * x[col[i]];
    }
    return sum;
}

always_inline void AVX512_spvv_cross_row_fp64(const int *rows, int *row_begin, int *row_end, int *col, double *nnz, double *x, SpB_Vector y)
{
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    __m256i rs = _mm256_loadu_si256((const __m256i *)rows);
    __m512d acc = _mm512_setzero_pd();

    int rowlen = *row_end - *row_begin;
    int base = *row_begin;
    {
        int idx0 = rows[0];
        int idx1 = rows[1];
        int idx2 = rows[2];
        int idx3 = rows[3];
        int idx4 = rows[4];
        int idx5 = rows[5];
        int idx6 = rows[6];
        int idx7 = rows[7];

        _mm_prefetch((double *)(((double *)yy->values) + idx0), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx1), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx2), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx3), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx4), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx5), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx6), _MM_HINT_T0);
        _mm_prefetch((double *)(((double *)yy->values) + idx7), _MM_HINT_T0);
    }
    for (int c = 0; c < rowlen; c++)
    {
        int offset = base + c * nLanes_f64;
        __m256i cc = _mm256_loadu_si256((const __m256i *)(col + offset));
        __m512d nz = _mm512_loadu_pd(nnz + offset);
        __m512d xx = _mm512_i32gather_pd(cc, x, 8);
        acc = _mm512_fmadd_pd(nz, xx, acc);
    }
    _mm512_i32scatter_pd(((double *)yy->values), rs, acc, 8);
}
#endif

#ifdef ARM_NEON
always_inline float NEON_spvv_in_row_fp32(int *col, float *nnz, int rowlen, float *x)
{
    int limit = rowlen - 3;
    int *col_p;
    float *nnz_p;
    float sum = 0.0f;
    int32x4_t c1;
    float32x4_t v1, v2, s;
    s = vdupq_n_f32(0.0f);
    v2 = vdupq_n_f32(0.0f);
    int i;

    for (i = 0; i < limit; i += 4)
    {
        col_p = col + i;
        nnz_p = nnz + i;
        c1 = vld1q_s32(col_p);
        v2[0] = x[c1[0]];
        v2[1] = x[c1[1]];
        v2[2] = x[c1[2]];
        v2[3] = x[c1[3]];
        v1 = vld1q_f32(nnz_p);
        s = vfmaq_f32(s, v1, v2);
    }
    sum += vaddvq_f32(s);

    for (; i < rowlen; i++)
    {
        sum += nnz[i] * x[col[i]];
    }
    return sum;
}

always_inline void NEON_spvv_cross_row_fp32(const int *rows, int *row_begin, int *row_end, int *col, float *nnz, float *x, SpB_Vector y)
{
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    int32x4_t rs = vld1q_s32(rows);
    float32x4_t acc = vdupq_n_f32(0.0f);

    int rowlen = *row_end - *row_begin;
    int base = *row_begin;
    {
        int idx0 = rows[0];
        int idx1 = rows[1];
        int idx2 = rows[2];
        int idx3 = rows[3];

        __builtin_prefetch(((float *)yy->values) + idx0, 0, 3);
        __builtin_prefetch(((float *)yy->values) + idx1, 0, 3);
        __builtin_prefetch(((float *)yy->values) + idx2, 0, 3);
        __builtin_prefetch(((float *)yy->values) + idx3, 0, 3);
    }
    for (int c = 0; c < rowlen; c++)
    {
        int offset = base + c * 4;
        int32x4_t cc = vld1q_s32(col + offset);
        float32x4_t nz = vld1q_f32(nnz + offset);
        float32x4_t x_x;
        x_x[0] = x[cc[0]];
        x_x[1] = x[cc[1]];
        x_x[2] = x[cc[2]];
        x_x[3] = x[cc[3]];
        acc = vfmaq_f32(acc, nz, x_x);
    }

    ((float *)yy->values)[rs[0]] = acc[0];
    ((float *)yy->values)[rs[1]] = acc[1];
    ((float *)yy->values)[rs[2]] = acc[2];
    ((float *)yy->values)[rs[3]] = acc[3];
}

always_inline double NEON_spvv_in_row_fp64(int *col, double *nnz, int rowlen, double *x)
{
    int limit = rowlen - 1;
    int *col_p;
    double *nnz_p;
    double sum = 0.0f;
    int32x2_t c1;
    float64x2_t v1, v2, s;
    s = vdupq_n_f64(0.0f);
    v2 = vdupq_n_f64(0.0f);
    int i;

    for (i = 0; i < limit; i += 2)
    {
        col_p = col + i;
        nnz_p = nnz + i;
        c1 = vld1_s32(col_p);
        v2[0] = x[c1[0]];
        v2[1] = x[c1[1]];
        v1 = vld1q_f64(nnz_p);
        s = vfmaq_f64(s, v1, v2);
    }
    sum += vaddvq_f64(s);

    for (; i < rowlen; i++)
    {
        sum += nnz[i] * x[col[i]];
    }
    return sum;
}

always_inline void NEON_spvv_cross_row_fp64(const int *rows, int *row_begin, int *row_end, int *col, double *nnz, double *x, SpB_Vector y)
{
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    int32x2_t rs = vld1_s32(rows);
    float64x2_t acc = vdupq_n_f64(0.0f);

    int rowlen = *row_end - *row_begin;
    int base = *row_begin;
    {
        int idx0 = rows[0];
        int idx1 = rows[1];

        __builtin_prefetch(((double *)yy->values) + idx0, 0, 3);
        __builtin_prefetch(((double *)yy->values) + idx1, 0, 3);
    }
    for (int c = 0; c < rowlen; c++)
    {
        int offset = base + c * 2;
        int32x2_t cc = vld1_s32(col + offset);
        float64x2_t nz = vld1q_f64(nnz + offset);
        float64x2_t x_x;
        x_x[0] = x[cc[0]];
        x_x[1] = x[cc[1]];
        acc = vfmaq_f64(acc, nz, x_x);
    }

    ((double *)yy->values)[rs[0]] = acc[0];
    ((double *)yy->values)[rs[1]] = acc[1];
}

#endif
void MergePathDivide(
    int diagonal,
    SpB_Index *a,
    int *b,
    int a_len,
    int b_len,
    coord *path_coordinate);

void *align_malloc(size_t size, size_t alignment);

int comp(const void *a, const void *b);