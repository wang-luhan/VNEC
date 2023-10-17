#include "SpB_mxv.h"
SpB_VNEC_type IRD_VNEC(const SpB_Matrix A, SpB_Type type)
{
    int nLanes = (type == SpB_FP32) ? nLanes_f32 : nLanes_f64;
    float IRD_thr = (type == SpB_FP32) ? IRD_thr_fp32 : IRD_thr_fp64;
    SpB_Matrix_CSC_or_CSR *a = (SpB_Matrix_CSC_or_CSR *)(A);
    int num_merge_items = a->nnz + A->row;
    int items_per_thread = (num_merge_items + NUM_THREADS - 1) / NUM_THREADS;
    int *nz_indices = (int *)malloc((a->nnz) * sizeof(int));
    int *diagonal_start = (int *)malloc((NUM_THREADS) * sizeof(int));
    int *diagonal_end = (int *)malloc((NUM_THREADS) * sizeof(int));
    coord *thread_coord_start = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    coord *thread_coord_end = (coord *)malloc((NUM_THREADS) * sizeof(coord));
    for (SpB_Index i = 0; i < a->nnz; i++)
    {
        nz_indices[i] = i;
    }
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        diagonal_start[tid] = min(items_per_thread * tid, num_merge_items);
        diagonal_end[tid] = min(diagonal_start[tid] + items_per_thread, num_merge_items);
        MergePathDivide(diagonal_start[tid], (a->ptr + 1), nz_indices, A->row, a->nnz, thread_coord_start + tid);
        MergePathDivide(diagonal_end[tid], (a->ptr + 1), nz_indices, A->row, a->nnz, thread_coord_end + tid);
    }
    SpB_Index A_COLS = A->col;
    int *ecr_indices = (int *)malloc((a->nnz + 10) * sizeof(int));
    memset(ecr_indices, 0, (a->nnz + 10) * sizeof(int));
    for (int tid = 0; tid < NUM_THREADS; tid++) // 每一块
    {
        int *not_null_col_flag = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_MAP = (int *)malloc(A_COLS * sizeof(int));
        int *IDX_OFFSET = (int *)malloc(A_COLS * sizeof(int));
        coord thread_coord_start_ = thread_coord_start[tid];
        coord thread_coord_end_ = thread_coord_end[tid];
        for (SpB_Index col = 0; col < A_COLS; col++)
        {
            IDX_MAP[col] = col;
            not_null_col_flag[col] = 1;
            IDX_OFFSET[col] = 1;
        }

        for (int j = thread_coord_start_.y; j < thread_coord_end_.y; ++j)
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
            for (int j = thread_coord_start_.y; j < thread_coord_end_.y; ++j)
            {
                ecr_indices[j] = IDX_MAP[a->indices[j]];
            }
        }
        free(not_null_col_flag);
        free(IDX_MAP);
        free(IDX_OFFSET);
    }
    int *col_start = (int *)malloc(a->nnz * sizeof(int));
    int *v_row_ptr = (int *)malloc((a->ptr_len) * sizeof(int));

    memset(col_start, 0, a->nnz * sizeof(int));
    memset(v_row_ptr, 0, a->ptr_len * sizeof(int));

    int group_index = 0;
    v_row_ptr[0] = 0;
    // #pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
    for (int tid = 0; tid < NUM_THREADS; tid++)
    {
        coord thread_coord_start_ = thread_coord_start[tid];
        coord thread_coord_end_ = thread_coord_end[tid];
        for (int i = thread_coord_start_.x; i < thread_coord_end_.x; ++i)
        {
            int ptr_start = ((int)(a->ptr[i]) > thread_coord_start_.y) ? a->ptr[i] : thread_coord_start_.y;
            int n_one_line = a->ptr[i + 1] - ptr_start;
            col_start[group_index] = ecr_indices[ptr_start];
            for (int j = 1; j < n_one_line; j++)
            {
                int dist = ecr_indices[ptr_start + j] - col_start[group_index];
                if (dist < nLanes)
                {
                }
                else
                {
                    group_index++;
                    col_start[group_index] = ecr_indices[ptr_start + j];
                }
            }
            if (n_one_line != 0)
            {
                group_index++;
            }
            v_row_ptr[i + 1] = group_index;
        }
    }
    int _nnz_ = v_row_ptr[A->row] * nLanes;
    float IRD_mat = (float)a->nnz / (float)_nnz_;
    printf("\n IRD_mat = %f \n", IRD_mat);

    free(diagonal_start);
    free(diagonal_end);
    free(nz_indices);
    free(thread_coord_start);
    free(thread_coord_end);
    free(ecr_indices);
    free(col_start);
    free(v_row_ptr);
    if (IRD_mat >= IRD_thr)
    {
        return SpB_VNEC_D;
    }
    else
    {
        return SpB_VNEC_S;
    }
}