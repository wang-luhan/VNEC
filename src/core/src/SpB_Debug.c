#include "OpenSparseBLAS.h"
#include "SpB_Matrix.h"
#include <stdio.h>

void print_Vector_BOOL(SpB_Vector v)
{
    printf("================================\n");
    SpB_Index n;
    SpB_Vector_nvals(&n, v);
    bool value;
    printf("The vector size is %lu\n", n);
    printf("The vector elements : \n");
    for (SpB_Index i = 0; i < n; ++i)
    {
        value = false;
        SpB_Vector_extractElement_BOOL(&value, v, i);
        printf("%d ", value);
    }
    printf("\n");
    printf("================================\n");
}

void print_Vector_INT32(SpB_Vector v)
{
    printf("================================\n");
    SpB_Index n;
    SpB_Vector_nvals(&n, v);
    int value;
    printf("The vector size is %lu\n", n);
    printf("The vector elements : \n");
    for (SpB_Index i = 0; i < n; ++i)
    {
        SpB_Vector_extractElement_INT32(&value, v, i);
        printf("%d ", value);
    }
    printf("\n");
    printf("================================\n");
}

void print_Vector_FP32(SpB_Vector v)
{
    printf("================================\n");
    SpB_Index n;
    SpB_Vector_nvals(&n, v);
    float value;
    printf("The vector size is %lu\n", n);
    printf("The vector elements : \n");
    for (SpB_Index i = 0; i < n; ++i)
    {
        SpB_Vector_extractElement_FP32(&value, v, i);
        printf("%f ", value);
    }
    printf("\n");
    printf("================================\n");
}

void print_Matrix_INT32(SpB_Matrix m)
{
    SpB_Index n_row, n_col;
    SpB_Matrix_nrows(&n_row, m);
    SpB_Matrix_ncols(&n_col, m);
    printf("================================\n");
    printf("The Matrix size is: %lux%lu\n", n_row, n_col);
    printf("The matrix elements:\n");

    for (SpB_Index i = 0; i < n_row; ++i)
    {
        for (SpB_Index j = 0; j < n_col; ++j)
        {
            int32_t value = 0;
            SpB_Info info = SpB_Matrix_extractElement_INT32(&value, m, i, j);
            if (info == SpB_NO_VALUE)
                value = 0;
            printf("%d ", value);
        }
        printf("\n");
    }
    printf("================================\n");
}

void print_Matrix_FP32(SpB_Matrix m)
{
    SpB_Index n_row, n_col;
    SpB_Matrix_nrows(&n_row, m);
    SpB_Matrix_ncols(&n_col, m);
    printf("================================\n");
    printf("The Matrix size is: %lux%lu\n", n_row, n_col);
    printf("The matrix elements:\n");

    for (SpB_Index i = 0; i < n_row; ++i)
    {
        for (SpB_Index j = 0; j < n_col; ++j)
        {
            float value = 0;
            SpB_Info info = SpB_Matrix_extractElement_FP32(&value, m, i, j);
            if (info == SpB_NO_VALUE)
                value = 0;
            printf("%f ", value);
        }
        printf("\n");
    }
    printf("================================\n");
}

void print_Matrix_BOOL(SpB_Matrix m)
{
    SpB_Index n_row, n_col;
    SpB_Matrix_nrows(&n_row, m);
    SpB_Matrix_ncols(&n_col, m);
    printf("================================\n");
    printf("The Matrix size is: %lux%lu\n", n_row, n_col);
    printf("The matrix elements:\n");

    for (SpB_Index i = 0; i < n_row; ++i)
    {
        for (SpB_Index j = 0; j < n_col; ++j)
        {
            bool value = 0;
            SpB_Info info = SpB_Matrix_extractElement_BOOL(&value, m, i, j);
            if (info == SpB_NO_VALUE)
                value = false;
            printf("%d ", value==0? 0 : 1);
        }
        printf("\n");
    }
    printf("================================\n");
}

// void print_Matrix_CSR(SpB_Matrix_CSR *m, int n)
// {
//     printf("================================\n");
//     printf("val :");
//     for (SpB_Index i = 0; i < n; i++)
//     {
//         printf("%c ", m->val[i]);
//     }
//     printf("\n");

//     printf("col :");
//     for (SpB_Index i = 0; i < n; i++)
//     {
//         printf("%lu ", m->col[i]);
//     }
//     printf("\n");

//     printf("ptr :");
//     for (SpB_Index i = 0; i <= n; i++)
//     {
//         printf("%lu ", m->ptr[i]);
//     }
//     printf("\n");

//     printf("================================\n");
// }