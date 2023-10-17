#include "OpenSparseBLAS.h"
#include <gtest/gtest.h>
#include "SpB_Debug.h"
#include <sys/time.h>

#include "utilities/util.hpp"
#include "utilities/graph.hpp"
#include "utilities/builder.hpp"
#include "SpB_Vector_Dense.h"
#include "SpB_Vector.h"
#include "SpB_Matrix_CSC_or_CSR.h"
#include "SpB_Debug.h"
#include <gtest/gtest.h>
#include <math.h>
#include <sys/time.h>
#include <algorithm>
#include <omp.h>
#ifdef ARM_NEON
#include <arm_neon.h>
#else
#include <immintrin.h>
#endif

#include <SpB_mxv.h>

// #define DATA_PATH "/data/wangluhan/open-graph-blas/data/performance/PageRank/"
#define DATA_PATH "../../../../../disk/feature-SpMV/open-graph-blas/data/performance/PageRank/"
#define MATRIX_FULL_PATH(file) DATA_PATH #file "/" #file ".mtx"
#define RESULT_FULL_PATH(file) DATA_PATH #file "/" #file "_result.mtx"

#define MATRIX_SG_FULL_PATH(file) DATA_PATH #file "/" #file ".sg"
#define always_inline __inline__ __attribute__((always_inline))

void SpB_mxv_FP32_serial(SpB_Vector w, const SpB_Matrix A, const SpB_Vector u)
{
    SpB_Vector_Dense *ww = (SpB_Vector_Dense *)(w);
    SpB_Vector_Dense *uu = (SpB_Vector_Dense *)(u);

    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    float t1, t2, t3;
    for (SpB_Index i = 0; i < w->n; i++)
    {
        t1 = 0.0f;
        SpB_Index ptr_start = a->ptr[i];
        SpB_Index n_one_line = a->ptr[i + 1] - ptr_start;
        for (SpB_Index j = 0; j < n_one_line; j++)
        {
            t2 = ((float *)a->val)[j + ptr_start];
            t3 = 0.0f;
            SpB_Index v_idx = a->indices[j + ptr_start];
            t3 = ((float *)uu->values)[v_idx];
            t1 += t2 * t3;
        }
        ((float *)ww->values)[i] = t1;
    }
}
void SpB_mxv_FP64_serial(SpB_Vector w, const SpB_Matrix A, const SpB_Vector u)
{
    SpB_Vector_Dense *ww = (SpB_Vector_Dense *)(w);
    SpB_Vector_Dense *uu = (SpB_Vector_Dense *)(u);

    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    double t1, t2, t3;
    for (SpB_Index i = 0; i < w->n; i++)
    {
        t1 = 0.0f;
        SpB_Index ptr_start = a->ptr[i];
        SpB_Index n_one_line = a->ptr[i + 1] - ptr_start;
        for (SpB_Index j = 0; j < n_one_line; j++)
        {
            t2 = ((double *)a->val)[j + ptr_start];
            t3 = 0.0f;
            SpB_Index v_idx = a->indices[j + ptr_start];
            t3 = ((double *)uu->values)[v_idx];
            t1 += t2 * t3;
        }
        ((double *)ww->values)[i] = t1;
    }
}

void VNEC_D_free_fp32(VNEC_D_FP32 *mat_thd)
{
    free(mat_thd->thread_coord_start);
    free(mat_thd->thread_coord_end);
    free(mat_thd->ecr_indices);
    free(mat_thd->NEC_NUM);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        free(mat_thd->use_x_indices[i]);
#ifdef X86_SIMD
        _mm_free(mat_thd->ecr_xx_val[i]);
#else
        free(mat_thd->ecr_xx_val[i]);
#endif
    }
    free(mat_thd->use_x_indices);
    free(mat_thd->ecr_xx_val);
    free(mat_thd->col_start);
    free(mat_thd->v_row_ptr);
#ifdef X86_SIMD
    _mm_free(mat_thd->val_align);
#else
    free(mat_thd->val_align);
#endif
    free(mat_thd);
}
void VNEC_S_free_fp32(VNEC_S_FP32 *mat_thd)
{
    free(mat_thd->NEC_NUM);
    free(mat_thd->spvv16_len);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        free(mat_thd->use_x_indices[i]);
        free(mat_thd->ecr_xx_val[i]);
        free(mat_thd->tasks[i]);
    }
    free(mat_thd->use_x_indices);
    free(mat_thd->ecr_xx_val);
    free(mat_thd->tasks);
    free(mat_thd->reorder_mat.nnz);
    free(mat_thd->reorder_mat.col);
    free(mat_thd->reorder_mat.row_begin);
    free(mat_thd->reorder_mat.row_end);
    free(mat_thd->reorder_mat.task_start);
    free(mat_thd->reorder_mat.task_end);
    free(mat_thd);
}


void VNEC_D_free_fp64(VNEC_D_FP64 *mat_thd)
{
    free(mat_thd->thread_coord_start);
    free(mat_thd->thread_coord_end);
    free(mat_thd->ecr_indices);
    free(mat_thd->NEC_NUM);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        free(mat_thd->use_x_indices[i]);
#ifdef X86_SIMD
        _mm_free(mat_thd->ecr_xx_val[i]);
#else
        free(mat_thd->ecr_xx_val[i]);
#endif
    }
    free(mat_thd->use_x_indices);
    free(mat_thd->ecr_xx_val);
    free(mat_thd->col_start);
    free(mat_thd->v_row_ptr);
#ifdef X86_SIMD
    _mm_free(mat_thd->val_align);
#else
    free(mat_thd->val_align);
#endif
    free(mat_thd);
}
void VNEC_S_free_fp64(VNEC_S_FP64 *mat_thd)
{
    free(mat_thd->NEC_NUM);
    free(mat_thd->spvv8_len);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        free(mat_thd->use_x_indices[i]);
        free(mat_thd->ecr_xx_val[i]);
        free(mat_thd->tasks[i]);
    }
    free(mat_thd->use_x_indices);
    free(mat_thd->ecr_xx_val);
    free(mat_thd->tasks);
    free(mat_thd->reorder_mat.nnz);
    free(mat_thd->reorder_mat.col);
    free(mat_thd->reorder_mat.row_begin);
    free(mat_thd->reorder_mat.row_end);
    free(mat_thd->reorder_mat.task_start);
    free(mat_thd->reorder_mat.task_end);
    free(mat_thd);
}