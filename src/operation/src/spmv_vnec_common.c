#include "SpB_mxv.h"

 void MergePathDivide(
    int diagonal,
    SpB_Index *a,
    int *b,
    int a_len,
    int b_len,
    coord *path_coordinate)
{
    // Diagonal search range (in x coordinate space)
    int x_min = max(diagonal - b_len, 0);
    int x_max = min(diagonal, a_len);
    // printf("\n x_min = %d and x_max = %d \n", x_min, x_max);

    // 2D binary-search along the diagonal search range
    while (x_min < x_max)
    {
        int pivot = (x_min + x_max) >> 1;
        if ((int)a[pivot] <= b[diagonal - pivot - 1])
        {
            // Keep top-right half of diagonal range
            x_min = pivot + 1;
        }
        else
        {
            // Keep bottom-left half of diagonal range
            x_max = pivot;
        }
    }
    path_coordinate->x = min(x_min, a_len);
    path_coordinate->y = diagonal - x_min;
}

void *align_malloc(size_t size, size_t alignment)
{
    void *ptr = NULL;
    int ret = posix_memalign(&ptr, alignment, size);
    if (ret != 0)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    return ptr;
}


int comp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}