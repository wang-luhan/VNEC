#include "SpB_mxv.h"

struct VNEC_S_FP32 *Build_VNEC_S_FP32(const SpB_Matrix A)
{
    struct timeval merge_start, merge_end;
    struct timeval ecr_start, ecr_end;
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    VNEC_S_FP32 *mat_thd = (VNEC_S_FP32 *)malloc(sizeof(VNEC_S_FP32));
    SpB_Index nrows = A->row;
    SpB_Index ncols = A->col;
    SpB_Index nvals = a->nnz;

    int num_merge_items = nvals + nrows;
    int items_per_thread = (num_merge_items + NUM_THREADS - 1) / NUM_THREADS;
    int *diagonal_start = (int *)malloc((NUM_THREADS) * sizeof(int));
    int *diagonal_end = (int *)malloc((NUM_THREADS) * sizeof(int));
    coord *thread_coord_start = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    coord *thread_coord_end = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    int *nz_indices = (int *)malloc((nvals) * sizeof(int));

    for (SpB_Index i = 0; i < nvals; i++)
    {
        nz_indices[i] = i;
    }
    gettimeofday(&merge_start, NULL);
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        diagonal_start[tid] = min(items_per_thread * tid, num_merge_items);
        diagonal_end[tid] = min(diagonal_start[tid] + items_per_thread, num_merge_items);
        MergePathDivide(diagonal_start[tid], (a->ptr + 1), nz_indices, nrows, nvals, thread_coord_start + tid);
        MergePathDivide(diagonal_end[tid], (a->ptr + 1), nz_indices, nrows, nvals, thread_coord_end + tid);
        thread_coord_end[tid].x = (thread_coord_end[tid].x) + 1;
        thread_coord_start[tid].x = (thread_coord_start[tid].x) + 1;
        if (tid == 0)
            thread_coord_start[tid].x--;
        if (tid == NUM_THREADS - 1)
            thread_coord_end[tid].x--;
        thread_coord_start[tid].y = a->ptr[thread_coord_start[tid].x];
        thread_coord_end[tid].y = a->ptr[thread_coord_end[tid].x];
    }
    gettimeofday(&merge_end, NULL);
    // malloc
    SpB_Index A_COLS = ncols;
    int *ecr_indices = (int *)malloc((nvals + 10) * sizeof(int));
    memset(ecr_indices, 0, (nvals + 10) * sizeof(int));

    mat_thd->NEC_NUM = (int *)malloc((NUM_THREADS + 1) * sizeof(int));
    memset(mat_thd->NEC_NUM, 0, sizeof(int) * (NUM_THREADS + 1));

    mat_thd->use_x_indices = (int **)malloc((NUM_THREADS + 1) * sizeof(int *));
    mat_thd->ecr_xx_val = (float **)malloc((NUM_THREADS + 1) * sizeof(float *));
    float ec_rate_sum = 0.0;

    mat_thd->spvv16_len = (int *)malloc(NUM_THREADS * sizeof(int));
    mat_thd->tasks = (int **)malloc(NUM_THREADS * sizeof(int *));

    // int **tasks = (int **)malloc(NUM_THREADS * sizeof(int *));
    gettimeofday(&ecr_start, NULL);
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        int *not_null_col_flag = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_MAP = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_OFFSET = (int *)malloc(A_COLS * sizeof(int));
        mat_thd->use_x_indices[tid] = (int *)malloc(A_COLS * sizeof(int));
        memset(mat_thd->use_x_indices[tid], 0, A_COLS * sizeof(int));

        coord thread_coord_start_t = thread_coord_start[tid];
        coord thread_coord_end_t = thread_coord_end[tid];

        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = col;
            not_null_col_flag[col] = 1;
            IDX_OFFSET[col] = 1;
        }
        for (int j = thread_coord_start_t.y; j < thread_coord_end_t.y; ++j)
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
        {
            for (int j = thread_coord_start_t.y; j < thread_coord_end_t.y; ++j)
            {
                ecr_indices[j] = IDX_MAP[a->indices[j]];
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
        mat_thd->ecr_xx_val[tid] = (float *)malloc(p * sizeof(float));
        float ec_rate = ((float)A_COLS - (float)mat_thd->NEC_NUM[tid]) / ((float)A_COLS);
        ec_rate_sum += ec_rate;
        free(not_null_col_flag);
        free(IDX_MAP);
        free(IDX_OFFSET);
    }
    gettimeofday(&ecr_end, NULL);
    // long ecr_timeuse = 1000000 * (ecr_end.tv_sec - ecr_start.tv_sec) + ecr_end.tv_usec - ecr_start.tv_usec;
    // long merge_timeuse = 1000000 * (merge_end.tv_sec - merge_start.tv_sec) + merge_end.tv_usec - merge_start.tv_usec;
    // printf(" pre-processing overall time elapsed: %f ms\n", (merge_timeuse + ecr_timeuse) / 1000.0 / 1);

#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++) // 每一块
    {
        coord thread_coord_start_t = thread_coord_start[tid];
        coord thread_coord_end_t = thread_coord_end[tid];
        int num_rows = thread_coord_end_t.x - thread_coord_start_t.x;
        int max_rowlen = 0;
        for (int r = thread_coord_start_t.x; r < thread_coord_end_t.x; ++r) // 遍历块中的每一行
        {
            int rowlen = a->ptr[r + 1] - a->ptr[r];
            if (rowlen > max_rowlen)
            {
                max_rowlen = rowlen;
            }
        }
        int **buckets = (int **)malloc((max_rowlen + 1) * sizeof(int *));
        for (int i = 0; i <= max_rowlen; i++)
        {
            buckets[i] = (int *)malloc(num_rows * sizeof(int));
            buckets[i][0] = 0;
        }
        for (int r = thread_coord_start_t.x; r < thread_coord_end_t.x; ++r)
        {
            int rowlen = a->ptr[r + 1] - a->ptr[r];
            int pos = ++buckets[rowlen][0];
            buckets[rowlen][pos] = r; // buckets[rowlen][0] rowlen长度的行有几个 分别是哪些存在值里
        }
        int *keys = (int *)malloc((max_rowlen + 1) * sizeof(int)); // 长度 0 - 54
        int num_keys = 0;
        for (int rowlen = 0; rowlen <= max_rowlen; rowlen++)
        {
            if (buckets[rowlen][0] > 0) // 长度为rowlen且非空的，rowlen插入
            {
                keys[num_keys++] = rowlen;
            }
        }
        qsort(keys, num_keys, sizeof(int), comp);
        int *order = (int *)malloc(num_rows * sizeof(int));
        int *remain = (int *)malloc(num_rows * sizeof(int));
        int order_pos = 0;
        int remain_pos = 0;
        for (int k = 0; k < num_keys; k++)
        {
            int key = keys[k];
            int **samelen_task = &buckets[key];
            int samelen_task_size = (*samelen_task)[0];
            int left = samelen_task_size % nLanes_f32;
            if (key > 32)
            {
                left = samelen_task_size;
            }
            int bulk = samelen_task_size - left;
            for (int i = 1; i <= bulk; i++)
            {
                order[order_pos++] = (*samelen_task)[i];
            }
            for (int i = bulk + 1; i <= samelen_task_size; i++)
            {
                remain[remain_pos++] = (*samelen_task)[i];
            }
        }
        mat_thd->spvv16_len[tid] = order_pos;
        int *task_data = (int *)malloc((order_pos + remain_pos) * sizeof(int));
        for (int i = 0; i < order_pos; i++)
        {
            task_data[i] = order[i];
        }
        for (int i = 0; i < remain_pos; i++)
        {
            task_data[order_pos + i] = remain[i];
        }
        mat_thd->tasks[tid] = task_data;

        // Free memory
        free(keys);
        free(order);
        free(remain);
        for (int i = 0; i <= max_rowlen; i++)
        {
            free(buckets[i]);
        }
        free(buckets);
    }

    mat_thd->reorder_mat.nnz = (float *)align_malloc(nvals * sizeof(float), 64);
    mat_thd->reorder_mat.col = (int *)align_malloc(nvals * sizeof(int), 64);
    mat_thd->reorder_mat.row_begin = (int *)align_malloc(nrows * sizeof(int), 64);
    mat_thd->reorder_mat.row_end = (int *)align_malloc(nrows * sizeof(int), 64);
    mat_thd->reorder_mat.task_start = (int *)align_malloc(NUM_THREADS * sizeof(int), 64);
    mat_thd->reorder_mat.task_end = (int *)align_malloc(NUM_THREADS * sizeof(int), 64);

    int npos_ = 0, pos_ = 0;
    int start = 0, t = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        coord thread_coord_start_t = thread_coord_start[i];
        coord thread_coord_end_t = thread_coord_end[i];
        int num_rows = thread_coord_end_t.x - thread_coord_start_t.x;
        mat_thd->reorder_mat.task_start[t] = start;
        mat_thd->reorder_mat.task_end[t++] = start + num_rows;
        start += num_rows;
        for (int j = 0; j < num_rows; j++)
        {
            int row = mat_thd->tasks[i][j];
            int b = a->ptr[row];
            int e = a->ptr[row + 1];
            mat_thd->reorder_mat.row_begin[pos_] = npos_;
            mat_thd->reorder_mat.row_end[pos_++] = npos_ + e - b;

            for (int k = b; k < e; k++)
            {
                mat_thd->reorder_mat.nnz[npos_] = ((float *)a->val)[k];
                mat_thd->reorder_mat.col[npos_++] = ecr_indices[k];
            }
        }
    }

    for (int t = 0; t < NUM_THREADS; t++)
    {
        int start = mat_thd->reorder_mat.task_start[t];
        int pm_len = mat_thd->spvv16_len[t];
        int p = 0, c = 0;
        for (p = start; c < pm_len; c += nLanes_f32, p += nLanes_f32)
        {
            int rowlen = mat_thd->reorder_mat.row_end[p] - mat_thd->reorder_mat.row_begin[p];
            int base = mat_thd->reorder_mat.row_begin[p];
            float *nnz = (float *)malloc(rowlen * nLanes_f32 * sizeof(float));
            int *col = (int *)malloc(rowlen * nLanes_f32 * sizeof(int));

            memcpy(nnz, mat_thd->reorder_mat.nnz + base, rowlen * nLanes_f32 * sizeof(float));
            memcpy(col, mat_thd->reorder_mat.col + base, rowlen * nLanes_f32 * sizeof(int));

            for (int l = 0; l < rowlen; l++)
            {
                for (int r = 0; r < nLanes_f32; r++)
                {
                    mat_thd->reorder_mat.nnz[base + l * nLanes_f32 + r] = nnz[r * rowlen + l];
                    mat_thd->reorder_mat.col[base + l * nLanes_f32 + r] = col[r * rowlen + l];
                }
            }
            free(nnz);
            free(col);
        }
    }
    return mat_thd;
}
void SpMV_VNEC_S_FP32(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, VNEC_S_FP32 *mat_thd)
{
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    SpB_Vector_Dense *xx = (SpB_Vector_Dense *)(x);

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
        for (; j < NEC_NUM - nLanes_f32; j += nLanes_f32)
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
        /*-------------------------------------------------------------------------------*/
        int *rows = mat_thd->tasks[tid];
        struct csr_f32 *mat = &(mat_thd->reorder_mat);
        int T_start = mat->task_start[tid];
        int T_end = mat->task_end[tid];
        int limit = mat_thd->spvv16_len[tid];
        int p, c;
#ifdef X86_SIMD
        for (p = T_start, c = 0; c < limit; p += nLanes_f32, c += nLanes_f32)
        {
            AVX512_spvv_cross_row_fp32(rows + c, mat->row_begin + p, mat->row_end + p, mat->col, mat->nnz, local_ecr_xx_val, y);
        }
        for (; p < T_end; p++)
        {
            int r = rows[p - T_start];
            int r_begin = mat->row_begin[p];
            int rowlen = mat->row_end[p] - r_begin;
            ((float *)yy->values)[r] = AVX512_spvv_in_row_fp32(mat->col + r_begin, mat->nnz + r_begin, rowlen, local_ecr_xx_val);
        }
#else
        for (p = T_start, c = 0; c < limit; p += nLanes_f32, c += nLanes_f32)
        {
            NEON_spvv_cross_row_fp32(rows + c, mat->row_begin + p, mat->row_end + p, mat->col, mat->nnz, local_ecr_xx_val, y);
        }
        for (; p < T_end; p++)
        {
            int r = rows[p - T_start];
            int r_begin = mat->row_begin[p];
            int rowlen = mat->row_end[p] - r_begin;
           ((float *)yy->values)[r] = NEON_spvv_in_row_fp32(mat->col + r_begin, mat->nnz + r_begin, rowlen, local_ecr_xx_val);
        }

#endif
    }
}

struct VNEC_S_FP64 *Build_VNEC_S_FP64(const SpB_Matrix A)
{
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    VNEC_S_FP64 *mat_thd = (VNEC_S_FP64 *)malloc(sizeof(VNEC_S_FP64));
    SpB_Index nrows = A->row;
    SpB_Index ncols = A->col;
    SpB_Index nvals = a->nnz;

    int num_merge_items = nvals + nrows;
    int items_per_thread = (num_merge_items + NUM_THREADS - 1) / NUM_THREADS;
    int *diagonal_start = (int *)malloc((NUM_THREADS) * sizeof(int));
    int *diagonal_end = (int *)malloc((NUM_THREADS) * sizeof(int));
    coord *thread_coord_start = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    coord *thread_coord_end = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    int *nz_indices = (int *)malloc((nvals) * sizeof(int));
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (SpB_Index i = 0; i < nvals; i++)
    {
        nz_indices[i] = i;
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        diagonal_start[tid] = min(items_per_thread * tid, num_merge_items);
        diagonal_end[tid] = min(diagonal_start[tid] + items_per_thread, num_merge_items);
        MergePathDivide(diagonal_start[tid], (a->ptr + 1), nz_indices, nrows, nvals, thread_coord_start + tid);
        MergePathDivide(diagonal_end[tid], (a->ptr + 1), nz_indices, nrows, nvals, thread_coord_end + tid);
        thread_coord_end[tid].x = (thread_coord_end[tid].x) + 1;
        thread_coord_start[tid].x = (thread_coord_start[tid].x) + 1;
        if (tid == 0)
            thread_coord_start[tid].x--;
        if (tid == NUM_THREADS - 1)
            thread_coord_end[tid].x--;
        thread_coord_start[tid].y = a->ptr[thread_coord_start[tid].x];
        thread_coord_end[tid].y = a->ptr[thread_coord_end[tid].x];
    }
    // malloc
    SpB_Index A_COLS = ncols;
    int *ecr_indices = (int *)malloc((nvals + 10) * sizeof(int));
    memset(ecr_indices, 0, (nvals + 10) * sizeof(int));

    mat_thd->NEC_NUM = (int *)malloc((NUM_THREADS + 1) * sizeof(int));
    memset(mat_thd->NEC_NUM, 0, sizeof(int) * (NUM_THREADS + 1));

    mat_thd->use_x_indices = (int **)malloc((NUM_THREADS + 1) * sizeof(int *));
    mat_thd->ecr_xx_val = (double **)malloc((NUM_THREADS + 1) * sizeof(double *));
    double ec_rate_sum = 0.0;

    mat_thd->spvv8_len = (int *)malloc(NUM_THREADS * sizeof(int));
    mat_thd->tasks = (int **)malloc(NUM_THREADS * sizeof(int *));

// int **tasks = (int **)malloc(NUM_THREADS * sizeof(int *));
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++) // 每一块
    {
        int *not_null_col_flag = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_MAP = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_OFFSET = (int *)malloc(A_COLS * sizeof(int));
        mat_thd->use_x_indices[tid] = (int *)malloc(A_COLS * sizeof(int));
        memset(mat_thd->use_x_indices[tid], 0, A_COLS * sizeof(int));

        coord thread_coord_start_t = thread_coord_start[tid];
        coord thread_coord_end_t = thread_coord_end[tid];

        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = col;
            not_null_col_flag[col] = 1;
            IDX_OFFSET[col] = 1;
        }
        for (int j = thread_coord_start_t.y; j < thread_coord_end_t.y; ++j)
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
        {
            for (int j = thread_coord_start_t.y; j < thread_coord_end_t.y; ++j)
            {
                ecr_indices[j] = IDX_MAP[a->indices[j]];
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
        mat_thd->ecr_xx_val[tid] = (double *)malloc(p * sizeof(double));
        double ec_rate = ((double)A_COLS - (double)mat_thd->NEC_NUM[tid]) / ((double)A_COLS);
        ec_rate_sum += ec_rate;
        free(not_null_col_flag);
        free(IDX_MAP);
        free(IDX_OFFSET);
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++) // 每一块
    {
        coord thread_coord_start_t = thread_coord_start[tid];
        coord thread_coord_end_t = thread_coord_end[tid];
        int num_rows = thread_coord_end_t.x - thread_coord_start_t.x;
        int max_rowlen = 0;
        for (int r = thread_coord_start_t.x; r < thread_coord_end_t.x; ++r)
        {
            int rowlen = a->ptr[r + 1] - a->ptr[r];
            if (rowlen > max_rowlen)
            {
                max_rowlen = rowlen;
            }
        }
        int **buckets = (int **)malloc((max_rowlen + 1) * sizeof(int *));
        for (int i = 0; i <= max_rowlen; i++)
        {
            buckets[i] = (int *)malloc(num_rows * sizeof(int));
            buckets[i][0] = 0;
        }
        for (int r = thread_coord_start_t.x; r < thread_coord_end_t.x; ++r)
        {
            int rowlen = a->ptr[r + 1] - a->ptr[r];
            int pos = ++buckets[rowlen][0];
            buckets[rowlen][pos] = r;
        }
        int *keys = (int *)malloc((max_rowlen + 1) * sizeof(int)); // 长度 0 - 54
        int num_keys = 0;
        for (int rowlen = 0; rowlen <= max_rowlen; rowlen++)
        {
            if (buckets[rowlen][0] > 0)
            {
                keys[num_keys++] = rowlen;
            }
        }
        qsort(keys, num_keys, sizeof(int), comp);
        int *order = (int *)malloc(num_rows * sizeof(int));
        int *remain = (int *)malloc(num_rows * sizeof(int));
        int order_pos = 0;
        int remain_pos = 0;
        for (int k = 0; k < num_keys; k++)
        {
            int key = keys[k];
            int **samelen_task = &buckets[key];
            int samelen_task_size = (*samelen_task)[0];
            int left = samelen_task_size % nLanes_f64;
            if (key > 32)
            {
                left = samelen_task_size;
            }
            int bulk = samelen_task_size - left;
            for (int i = 1; i <= bulk; i++)
            {
                order[order_pos++] = (*samelen_task)[i];
            }
            for (int i = bulk + 1; i <= samelen_task_size; i++)
            {
                remain[remain_pos++] = (*samelen_task)[i];
            }
        }
        mat_thd->spvv8_len[tid] = order_pos;
        int *task_data = (int *)malloc((order_pos + remain_pos) * sizeof(int));
        for (int i = 0; i < order_pos; i++)
        {
            task_data[i] = order[i];
        }
        for (int i = 0; i < remain_pos; i++)
        {
            task_data[order_pos + i] = remain[i];
        }
        mat_thd->tasks[tid] = task_data;

        // Free memory
        free(keys);
        free(order);
        free(remain);
        for (int i = 0; i <= max_rowlen; i++)
        {
            free(buckets[i]);
        }
        free(buckets);
    }

    mat_thd->reorder_mat.nnz = (double *)align_malloc(nvals * sizeof(double), 64);
    mat_thd->reorder_mat.col = (int *)align_malloc(nvals * sizeof(int), 64);
    mat_thd->reorder_mat.row_begin = (int *)align_malloc(nrows * sizeof(int), 64);
    mat_thd->reorder_mat.row_end = (int *)align_malloc(nrows * sizeof(int), 64);
    mat_thd->reorder_mat.task_start = (int *)align_malloc(NUM_THREADS * sizeof(int), 64);
    mat_thd->reorder_mat.task_end = (int *)align_malloc(NUM_THREADS * sizeof(int), 64);
    int npos_ = 0, pos_ = 0;
    int start = 0, t = 0;
    // #pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int i = 0; i < NUM_THREADS; i++)
    {
        coord thread_coord_start_t = thread_coord_start[i];
        coord thread_coord_end_t = thread_coord_end[i];
        int num_rows = thread_coord_end_t.x - thread_coord_start_t.x;
        mat_thd->reorder_mat.task_start[t] = start;
        mat_thd->reorder_mat.task_end[t++] = start + num_rows;
        start += num_rows;
        for (int j = 0; j < num_rows; j++)
        {
            int row = mat_thd->tasks[i][j];
            int b = a->ptr[row];
            int e = a->ptr[row + 1];
            mat_thd->reorder_mat.row_begin[pos_] = npos_;
            mat_thd->reorder_mat.row_end[pos_++] = npos_ + e - b;

            for (int k = b; k < e; k++)
            {
                mat_thd->reorder_mat.nnz[npos_] = ((double *)a->val)[k];
                mat_thd->reorder_mat.col[npos_++] = ecr_indices[k];
            }
        }
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int t = 0; t < NUM_THREADS; t++)
    {
        int start = mat_thd->reorder_mat.task_start[t];
        int pm_len = mat_thd->spvv8_len[t];
        int p = 0, c = 0;
        for (p = start; c < pm_len; c += nLanes_f64, p += nLanes_f64)
        {
            int rowlen = mat_thd->reorder_mat.row_end[p] - mat_thd->reorder_mat.row_begin[p];
            int base = mat_thd->reorder_mat.row_begin[p];
            double *nnz = (double *)malloc(rowlen * nLanes_f64 * sizeof(double));
            int *col = (int *)malloc(rowlen * nLanes_f64 * sizeof(int));

            memcpy(nnz, mat_thd->reorder_mat.nnz + base, rowlen * nLanes_f64 * sizeof(double));
            memcpy(col, mat_thd->reorder_mat.col + base, rowlen * nLanes_f64 * sizeof(int));

            for (int l = 0; l < rowlen; l++)
            {
                for (int r = 0; r < nLanes_f64; r++)
                {
                    mat_thd->reorder_mat.nnz[base + l * nLanes_f64 + r] = nnz[r * rowlen + l];
                    mat_thd->reorder_mat.col[base + l * nLanes_f64 + r] = col[r * rowlen + l];
                }
            }
            free(nnz);
            free(col);
        }
    }

    return mat_thd;
}
void SpMV_VNEC_S_FP64(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, VNEC_S_FP64 *mat_thd)
{
    SpB_Vector_Dense *yy = (SpB_Vector_Dense *)(y);
    SpB_Vector_Dense *xx = (SpB_Vector_Dense *)(x);

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
        for (; j < NEC_NUM - nLanes_f64; j += nLanes_f64)
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
        // thread_coord_start.x 行 - thread_coord_end.x行 向量化处理
        /*-------------------------------------------------------------------------------*/
        int *rows = mat_thd->tasks[tid];
        struct csr_f64 *mat = &(mat_thd->reorder_mat);
        int T_start = mat->task_start[tid];
        int T_end = mat->task_end[tid];
        int limit = mat_thd->spvv8_len[tid];
        int p, c;
#ifdef X86_SIMD
        for (p = T_start, c = 0; c < limit; p += nLanes_f64, c += nLanes_f64)
        {
            AVX512_spvv_cross_row_fp64(rows + c, mat->row_begin + p, mat->row_end + p, mat->col, mat->nnz, local_ecr_xx_val, y);
        }

        for (; p < T_end; p++)
        {
            int r = rows[p - T_start];
            int r_begin = mat->row_begin[p];
            int rowlen = mat->row_end[p] - r_begin;
            // _mm_prefetch(((double *)yy->values) + r, _MM_HINT_ET1);
            ((double *)yy->values)[r] = AVX512_spvv_in_row_fp64(mat->col + r_begin, mat->nnz + r_begin, rowlen, local_ecr_xx_val); // fragment
        }
#else
        for (p = T_start, c = 0; c < limit; p += nLanes_f64, c += nLanes_f64) // 非 fragment
        {
            NEON_spvv_cross_row_fp64(rows + c, mat->row_begin + p, mat->row_end + p, mat->col, mat->nnz, local_ecr_xx_val, y);
        }
        for (; p < T_end; p++)
        {
            int r = rows[p - T_start];
            int r_begin = mat->row_begin[p];
            int rowlen = mat->row_end[p] - r_begin;
           ((double *)yy->values)[r] = NEON_spvv_in_row_fp64(mat->col + r_begin, mat->nnz + r_begin, rowlen, local_ecr_xx_val); // fragment
        }
#endif


    }
}