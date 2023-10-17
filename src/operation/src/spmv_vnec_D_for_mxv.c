#include "SpB_mxv.h"
struct VNEC_D_FP32 *Build_VNEC_D_FP32(const SpB_Matrix A)
{
    VNEC_D_FP32 *mat_thd = (VNEC_D_FP32 *)malloc(sizeof(VNEC_D_FP32));
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    int num_merge_items = a->nnz + A->row;
    int items_per_thread = (num_merge_items + NUM_THREADS - 1) / NUM_THREADS;
    int *nz_indices = (int *)malloc((a->nnz) * sizeof(int));
    int *diagonal_start = (int *)malloc((NUM_THREADS) * sizeof(int));
    int *diagonal_end = (int *)malloc((NUM_THREADS) * sizeof(int));
    mat_thd->thread_coord_start = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    mat_thd->thread_coord_end = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    for (SpB_Index i = 0; i < a->nnz; i++)
    {
        nz_indices[i] = i;
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        diagonal_start[tid] = min(items_per_thread * tid, num_merge_items);
        diagonal_end[tid] = min(diagonal_start[tid] + items_per_thread, num_merge_items);
        MergePathDivide(diagonal_start[tid], (a->ptr + 1), nz_indices, A->row, a->nnz, mat_thd->thread_coord_start + tid);
        MergePathDivide(diagonal_end[tid], (a->ptr + 1), nz_indices, A->row, a->nnz, mat_thd->thread_coord_end + tid);
    }
    SpB_Index A_COLS = A->col;
    mat_thd->ecr_indices = (int *)malloc((a->nnz + 10) * sizeof(int));
    memset(mat_thd->ecr_indices, 0, (a->nnz + 10) * sizeof(int));

    mat_thd->NEC_NUM = (int *)malloc((NUM_THREADS + 1) * sizeof(int));
    memset(mat_thd->NEC_NUM, 0, sizeof(int) * (NUM_THREADS + 1));

    mat_thd->use_x_indices = (int **)malloc((NUM_THREADS + 1) * sizeof(int *));
    mat_thd->ecr_xx_val = (float **)malloc((NUM_THREADS + 1) * sizeof(float *));

    for (int tid = 0; tid < NUM_THREADS; tid++) // 每一块
    {
        int *not_null_col_flag = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_MAP = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_OFFSET = (int *)malloc(A_COLS * sizeof(int));
        mat_thd->use_x_indices[tid] = (int *)malloc(A_COLS * sizeof(int));
        memset(mat_thd->use_x_indices[tid], 0, A_COLS * sizeof(int));

        coord thread_coord_start = mat_thd->thread_coord_start[tid];
        coord thread_coord_end = mat_thd->thread_coord_end[tid];

        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = col;
            not_null_col_flag[col] = 1;
            IDX_OFFSET[col] = 1;
        }

        for (int j = thread_coord_start.y; j < thread_coord_end.y; ++j)
        {
            not_null_col_flag[a->indices[j]] = 0;
        }
        IDX_OFFSET[0] = not_null_col_flag[0];
        for (SpB_Index col = 1; col < A_COLS; col++)
        {
            IDX_OFFSET[col] = IDX_OFFSET[col - 1] + not_null_col_flag[col];
        }
        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = IDX_MAP[col] - IDX_OFFSET[col];
        }

#pragma omp critical
        {
            for (int j = thread_coord_start.y; j < thread_coord_end.y; ++j)
            {
                mat_thd->ecr_indices[j] = IDX_MAP[a->indices[j]];
            }
        }
        int p = 0;
        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            if (not_null_col_flag[col] != 1)
            {
                mat_thd->use_x_indices[tid][p] = col;
                p++;
            }
        }
        mat_thd->NEC_NUM[tid] = p;
#ifdef X86_SIMD
        mat_thd->ecr_xx_val[tid] = (float *)(_mm_malloc(p * sizeof(float), 64));
#else
        mat_thd->ecr_xx_val[tid] = (float *)malloc(p * sizeof(float));
#endif

        free(not_null_col_flag);
        free(IDX_MAP);
        free(IDX_OFFSET);
    }
    float *aa_val = (float *)malloc((a->nnz + nLanes_f32) * sizeof(float));
    memset(aa_val, 0.0, (a->nnz + nLanes_f32) * sizeof(float));
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (SpB_Index i = 0; i < a->nnz; i++)
    {
        aa_val[i] = ((float *)a->val)[i];
    }
    mat_thd->col_start = (int *)malloc(a->nnz * sizeof(int));
    mat_thd->v_row_ptr = (int *)malloc((a->ptr_len) * sizeof(int));
#ifdef X86_SIMD
    mat_thd->val_align = (float *)(_mm_malloc(a->nnz * nLanes_f32 * sizeof(float), 64));
#else
    mat_thd->val_align = (float *)malloc(a->nnz * nLanes_f32 * sizeof(float));
#endif
    memset(mat_thd->col_start, 0, a->nnz * sizeof(int));
    memset(mat_thd->v_row_ptr, 0, a->ptr_len * sizeof(int));
    memset(mat_thd->val_align, 0.0, a->nnz * nLanes_f32 * sizeof(float));
    int group_index = 0;
    int val_align_idx = 0;
    mat_thd->v_row_ptr[0] = 0;
    // #pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        coord thread_coord_start = mat_thd->thread_coord_start[tid];
        coord thread_coord_end = mat_thd->thread_coord_end[tid];
        for (int i = thread_coord_start.x; i < thread_coord_end.x; ++i)
        {
            int ptr_start = ((int)(a->ptr[i]) > thread_coord_start.y) ? a->ptr[i] : thread_coord_start.y;
            int n_one_line = a->ptr[i + 1] - ptr_start;
            mat_thd->col_start[group_index] = mat_thd->ecr_indices[ptr_start];
            mat_thd->val_align[val_align_idx] = aa_val[ptr_start];

            for (int j = 1; j < n_one_line; j++)
            {
                int dist = mat_thd->ecr_indices[ptr_start + j] - mat_thd->col_start[group_index];
                if (dist < nLanes_f32)
                {
                    mat_thd->val_align[val_align_idx + dist] = aa_val[ptr_start + j];
                }
                else
                {
                    group_index++;
                    val_align_idx += nLanes_f32;
                    mat_thd->col_start[group_index] = mat_thd->ecr_indices[ptr_start + j];
                    mat_thd->val_align[val_align_idx] = aa_val[ptr_start + j];
                }
            }
            if (n_one_line != 0)
            {
                group_index++;
                val_align_idx += nLanes_f32;
            }
            mat_thd->v_row_ptr[i + 1] = group_index;
        }
    }
    free(diagonal_start);
    free(diagonal_end);
    free(nz_indices);
    free(aa_val);
    return mat_thd;
}

void SpMV_VNEC_D_FP32(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, VNEC_D_FP32 *mat_thd)
{
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    SpB_Vector_Dense *xx = (SpB_Vector_Dense *)(x);
    SpB_Index *row_end_offsets = a->ptr + 1; // Merge list A
    // int nz_indices[a->nnz];           // Merge list B
    int row_carry_out[256];
    float value_carry_out[256];
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        int *local_use_x_indices = mat_thd->use_x_indices[tid];
        float *local_ecr_xx_val = mat_thd->ecr_xx_val[tid];
        int NEC_NUM = mat_thd->NEC_NUM[tid];

        // x-vector preprocessing
        int j = 0;
#ifdef X86_SIMD
        for (; j < NEC_NUM - nLanes_f32; j += nLanes_f32)
        {
            __m512i id_avx = _mm512_loadu_si512(local_use_x_indices + j);
            __m512 val_avx = _mm512_i32gather_ps(id_avx, (float *)xx->values, 4);
            _mm512_storeu_ps(local_ecr_xx_val + j, val_avx);
        }
#else
        for (; j <= NEC_NUM - nLanes_f32; j+=nLanes_f32)
        {
            int32x4_t id_neon = vld1q_s32(local_use_x_indices + j);
            float32x4_t val_neon = {*((float *)xx->values + id_neon[0]), *((float *)xx->values + id_neon[1]),
                                    *((float *)xx->values + id_neon[2]), *((float *)xx->values + id_neon[3])};
            vst1q_f32(local_ecr_xx_val + j, val_neon);
        }
#endif
        for (; j < NEC_NUM; j++)
        {
            local_ecr_xx_val[j] = *((float *)xx->values + local_use_x_indices[j]);
        }
        coord thread_coord_start = mat_thd->thread_coord_start[tid];
        coord thread_coord_end = mat_thd->thread_coord_end[tid];
#ifdef X86_SIMD
        for (int i = thread_coord_start.x; i < thread_coord_end.x; ++i)
        {
            SpB_Index ptr_start = mat_thd->v_row_ptr[i];
            SpB_Index n_one_line = mat_thd->v_row_ptr[i + 1] - ptr_start;
            __m512 v_tmp = _mm512_setzero_ps();
            for (SpB_Index j = 0; j < n_one_line; j++)
            {
                SpB_Index col = ptr_start + j;
                __m512 matrix = _mm512_load_ps(mat_thd->val_align + col * nLanes_f32);
                __m512 vector = _mm512_load_ps(local_ecr_xx_val + mat_thd->col_start[col]);
                v_tmp = _mm512_fmadd_ps(matrix, vector, v_tmp);
            }
            ((float *)yy->values)[i] = _mm512_reduce_add_ps(v_tmp);
        }
#else
        for (int i = thread_coord_start.x; i < thread_coord_end.x; ++i)
        {
            SpB_Index ptr_start = mat_thd->v_row_ptr[i];
            SpB_Index n_one_line = mat_thd->v_row_ptr[i + 1] - ptr_start;
            float32x4_t v_tmp = vdupq_n_f32(0.0);
            for (SpB_Index j = 0; j < n_one_line; j++)
            {
                SpB_Index col = ptr_start + j;
                float32x4_t matrix = vld1q_f32(mat_thd->val_align + col * nLanes_f32);
                float32x4_t vector = vld1q_f32(local_ecr_xx_val + mat_thd->col_start[col]);
                v_tmp = vmlaq_f32(v_tmp, matrix, vector);
            }
            ((float *)yy->values)[i] = vaddvq_f32(v_tmp);
        }
#endif
        float running_total = 0.0;
        thread_coord_start.y = row_end_offsets[thread_coord_end.x - 1];
        for (; thread_coord_start.y < thread_coord_end.y; ++thread_coord_start.y)
        {
            running_total += ((float *)a->val)[thread_coord_start.y] * local_ecr_xx_val[mat_thd->ecr_indices[thread_coord_start.y]];
        }
        row_carry_out[tid] = thread_coord_end.x;
        value_carry_out[tid] = running_total;
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS - 1; ++tid)
    {
        ((float *)yy->values)[row_carry_out[tid]] += value_carry_out[tid];
    }
}
struct VNEC_D_FP64 *Build_VNEC_D_FP64(const SpB_Matrix A)
{
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    SpB_Index nrows = A->row;
    SpB_Index ncols = A->col;
    SpB_Index nvals = a->nnz;

    VNEC_D_FP64 *mat_thd = (VNEC_D_FP64 *)malloc(sizeof(VNEC_D_FP64));
    int num_merge_items = nvals + nrows;
    int items_per_thread = (num_merge_items + NUM_THREADS - 1) / NUM_THREADS;
    int *nz_indices = (int *)malloc((nvals) * sizeof(int));
    int *diagonal_start = (int *)malloc((NUM_THREADS) * sizeof(int));
    int *diagonal_end = (int *)malloc((NUM_THREADS) * sizeof(int));
    mat_thd->thread_coord_start = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    mat_thd->thread_coord_end = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    for (SpB_Index i = 0; i < nvals; i++)
    {
        nz_indices[i] = i;
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        diagonal_start[tid] = min(items_per_thread * tid, num_merge_items);
        diagonal_end[tid] = min(diagonal_start[tid] + items_per_thread, num_merge_items);
        MergePathDivide(diagonal_start[tid], (a->ptr + 1), nz_indices, nrows, nvals, mat_thd->thread_coord_start + tid);
        MergePathDivide(diagonal_end[tid], (a->ptr + 1), nz_indices, nrows, nvals, mat_thd->thread_coord_end + tid);
    }
    // malloc
    SpB_Index A_COLS = ncols;
    mat_thd->ecr_indices = (int *)malloc((nvals + 10) * sizeof(int));
    memset(mat_thd->ecr_indices, 0, (nvals + 10) * sizeof(int));

    mat_thd->NEC_NUM = (int *)malloc((NUM_THREADS + 1) * sizeof(int));
    memset(mat_thd->NEC_NUM, 0, sizeof(int) * (NUM_THREADS + 1));

    mat_thd->use_x_indices = (int **)malloc((NUM_THREADS + 1) * sizeof(int *));
    mat_thd->ecr_xx_val = (double **)malloc((NUM_THREADS + 1) * sizeof(double *));
// mat_thd->ecr_xx_val = static_cast<double*>(_mm_malloc((NUM_THREADS + 1) * sizeof(double), 64));
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++) // 每一块
    {
        int *not_null_col_flag = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_MAP = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_OFFSET = (int *)malloc(A_COLS * sizeof(int));
        mat_thd->use_x_indices[tid] = (int *)malloc(A_COLS * sizeof(int));
        memset(mat_thd->use_x_indices[tid], 0, A_COLS * sizeof(int));

        coord thread_coord_start = mat_thd->thread_coord_start[tid];
        coord thread_coord_end = mat_thd->thread_coord_end[tid];

        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = col;
            not_null_col_flag[col] = 1;
            IDX_OFFSET[col] = 1;
        }

        for (int j = thread_coord_start.y; j < thread_coord_end.y; ++j)
        {
            not_null_col_flag[a->indices[j]] = 0;
        }
        IDX_OFFSET[0] = not_null_col_flag[0];
        for (SpB_Index col = 1; col < A_COLS; col++)
        {
            IDX_OFFSET[col] = IDX_OFFSET[col - 1] + not_null_col_flag[col];
        }
        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = IDX_MAP[col] - IDX_OFFSET[col];
        }

        // #pragma omp critical
        {
            for (int j = thread_coord_start.y; j < thread_coord_end.y; ++j)
            {
                mat_thd->ecr_indices[j] = IDX_MAP[a->indices[j]];
            }
        }
        int p = 0;
        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            if (not_null_col_flag[col] != 1)
            {
                mat_thd->use_x_indices[tid][p] = col;
                p++;
            }
        }
        mat_thd->NEC_NUM[tid] = p;
#ifdef X86_SIMD
        mat_thd->ecr_xx_val[tid] = (double *)(_mm_malloc(p * sizeof(double), 64));
#else
        mat_thd->ecr_xx_val[tid] = (double *)malloc(p * sizeof(double));
#endif
        free(not_null_col_flag);
        free(IDX_MAP);
        free(IDX_OFFSET);
    }
    double *aa_val = (double *)malloc((nvals + nLanes_f64) * sizeof(double));
    memset(aa_val, 0.0, (nvals + nLanes_f64) * sizeof(double));
    for (SpB_Index i = 0; i < nvals + nLanes_f64; i++)
    {
        aa_val[i] = ((double *)a->val)[i];
    }
    mat_thd->col_start = (int *)malloc(nvals * sizeof(int));
    mat_thd->v_row_ptr = (int *)malloc((a->ptr_len) * sizeof(int));
    // mat_thd->val_align = (double *)malloc(nvals * nLanes_f64 * sizeof(double));
#ifdef X86_SIMD
    mat_thd->val_align = (double *)(_mm_malloc(nvals * nLanes_f64 * sizeof(double), 64));
#else
    mat_thd->val_align = (double *)malloc(nvals * nLanes_f64 * sizeof(double));
#endif
    memset(mat_thd->col_start, 0, nvals * sizeof(int));
    memset(mat_thd->v_row_ptr, 0, a->ptr_len * sizeof(int));
    memset(mat_thd->val_align, 0.0, nvals * nLanes_f64 * sizeof(double));
    int group_index = 0;
    int val_align_idx = 0;
    mat_thd->v_row_ptr[0] = 0;
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        coord thread_coord_start = mat_thd->thread_coord_start[tid];
        coord thread_coord_end = mat_thd->thread_coord_end[tid];
        for (int i = thread_coord_start.x; i < thread_coord_end.x; ++i)
        {
            int ptr_start = ((int)(a->ptr[i]) > thread_coord_start.y) ? a->ptr[i] : thread_coord_start.y;
            int n_one_line = a->ptr[i + 1] - ptr_start;
            mat_thd->col_start[group_index] = mat_thd->ecr_indices[ptr_start];
            mat_thd->val_align[val_align_idx] = aa_val[ptr_start];

            for (int j = 1; j < n_one_line; j++)
            {
                int dist = mat_thd->ecr_indices[ptr_start + j] - mat_thd->col_start[group_index];
                if (dist < nLanes_f64)
                {
                    mat_thd->val_align[val_align_idx + dist] = aa_val[ptr_start + j];
                }
                else
                {
                    group_index++;
                    val_align_idx += nLanes_f64;
                    mat_thd->col_start[group_index] = mat_thd->ecr_indices[ptr_start + j];
                    mat_thd->val_align[val_align_idx] = aa_val[ptr_start + j];
                }
            }
            if (n_one_line != 0)
            {
                group_index++;
                val_align_idx += nLanes_f64;
            }
            mat_thd->v_row_ptr[i + 1] = group_index;
        }
    }
    free(diagonal_start);
    free(diagonal_end);
    free(nz_indices);
    free(aa_val);
    return mat_thd;
}

void SpMV_VNEC_D_FP64(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, VNEC_D_FP64 *mat_thd)
{
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    SpB_Vector_Dense *xx = (SpB_Vector_Dense *)(x);
    SpB_Index *row_end_offsets = a->ptr + 1; // Merge list A
    // int nz_indices[a->nnz];           // Merge list B
    int row_carry_out[256];
    double value_carry_out[256];
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        int *local_use_x_indices = mat_thd->use_x_indices[tid];
        double *local_ecr_xx_val = mat_thd->ecr_xx_val[tid];
        int NEC_NUM = mat_thd->NEC_NUM[tid];

        // x-vector preprocessing
        int j = 0;
#ifdef X86_SIMD
        for (; j < NEC_NUM - nLanes_f64; j += nLanes_f64)
        {
            __m256i id_avx = _mm256_loadu_si256((const __m256i *)(local_use_x_indices + j));
            __m512d val_avx = _mm512_i32gather_pd(id_avx, (double *)xx->values, 8);
            _mm512_storeu_pd(local_ecr_xx_val + j, val_avx);
        }
#else
        for (; j <= NEC_NUM - nLanes_f64; j += nLanes_f64)
        {
            int32x2_t id_neon = vld1_s32(local_use_x_indices + j);
            float64x2_t val_neon = {*((double *)xx->values + id_neon[0]), *((double *)xx->values + id_neon[1])};
            vst1q_f64(local_ecr_xx_val + j, val_neon);
        }
#endif
        for (; j < NEC_NUM; j++)
        {
            local_ecr_xx_val[j] = *((double *)xx->values + local_use_x_indices[j]);
        }
        coord thread_coord_start = mat_thd->thread_coord_start[tid];
        coord thread_coord_end = mat_thd->thread_coord_end[tid];
#ifdef X86_SIMD
        for (int i = thread_coord_start.x; i < thread_coord_end.x; ++i)
        {
            SpB_Index ptr_start = mat_thd->v_row_ptr[i];
            SpB_Index n_one_line = mat_thd->v_row_ptr[i + 1] - ptr_start;
            __m512d v_tmp = _mm512_setzero_pd();
            for (SpB_Index j = 0; j < n_one_line; j++)
            {
                SpB_Index col = ptr_start + j;
                __m512d matrix = _mm512_load_pd(mat_thd->val_align + col * nLanes_f64);
                __m512d vector = _mm512_load_pd(local_ecr_xx_val + mat_thd->col_start[col]);
                v_tmp = _mm512_fmadd_pd(matrix, vector, v_tmp);
            }
            ((double *)yy->values)[i] = _mm512_reduce_add_pd(v_tmp);
        }
#else
        for (int i = thread_coord_start.x; i < thread_coord_end.x; ++i)
        {
            SpB_Index ptr_start = mat_thd->v_row_ptr[i];
            SpB_Index n_one_line = mat_thd->v_row_ptr[i + 1] - ptr_start;
            float64x2_t v_tmp = vdupq_n_f64(0.0);
            for (SpB_Index j = 0; j < n_one_line; j++)
            {
                SpB_Index col = ptr_start + j;
                float64x2_t matrix = vld1q_f64(mat_thd->val_align + col * nLanes_f64);
                float64x2_t vector = vld1q_f64(local_ecr_xx_val + mat_thd->col_start[col]);
                v_tmp = vmlaq_f64(v_tmp, matrix, vector);
            }
            ((double *)yy->values)[i] = vaddvq_f64(v_tmp);
        }
#endif
        double running_total = 0.0;
        thread_coord_start.y = row_end_offsets[thread_coord_end.x - 1];
        for (; thread_coord_start.y < thread_coord_end.y; ++thread_coord_start.y)
        {
            running_total += ((double *)a->val)[thread_coord_start.y] * local_ecr_xx_val[mat_thd->ecr_indices[thread_coord_start.y]];
        }
        row_carry_out[tid] = thread_coord_end.x;
        value_carry_out[tid] = running_total;
    }
// update the values in y for rows that span multiple threads
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS - 1; ++tid)
    {
        ((double *)yy->values)[row_carry_out[tid]] += value_carry_out[tid];
    }
}