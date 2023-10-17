#include "SpB_Global.h"
#include "SpB_Alloc.h"
#include <stdlib.h>

SpB_Index SpB_ALL_object = 0;
SpB_Index *SpB_ALL = &SpB_ALL_object;

typedef struct
{
    bool is_csc;
    SpB_Vector_Format_Value_ext vector_format;
} SpB_Global_struct;

extern SpB_Global_struct SpB_Global;

SpB_Global_struct SpB_Global = {
    .is_csc = true,
    .vector_format = SpB_DENSE};

void SpB_Global_is_csc_set(bool is_csc)
{
    SpB_Global.is_csc = is_csc;
}

extern bool SpB_Global_is_csc_get()
{
    return (SpB_Global.is_csc);
}

void SpB_Global_Vector_Format_set(
    SpB_Vector_Format_Value_ext format)
{
    SpB_Global.vector_format = format;
}

extern SpB_Vector_Format_Value_ext SpB_Global_get_Vector_Format()
{
    return (SpB_Global.vector_format);
}

LinkedListMes_t LinkedListMes = {
    .capacity_nnz = 512,
    .capacity_nnz_row = 64,
    .capacity_vector_nnz = 64};

void SpB_LinkedList_malloc()
{
    unsigned int nnz = LinkedListMes.capacity_nnz;
    LinkedListMes.ListNodes = SpB_aligned_malloc(sizeof(listNode) * nnz);

    unsigned int nnz_row = LinkedListMes.capacity_nnz_row;
    LinkedListMes.ListPtr = SpB_aligned_malloc(sizeof(listNode) * nnz_row);
}

void SpB_LinkedList_grow(unsigned int nnz, unsigned int nnz_row)
{
    unsigned int new_capacity = LinkedListMes.capacity_nnz;

    while (nnz > new_capacity)
    {
        new_capacity = ((new_capacity) ? ((new_capacity) << 4) : 1);
    }

    if (new_capacity > LinkedListMes.capacity_nnz)
    {
        LinkedListMes.capacity_nnz = new_capacity;
        LinkedListMes.ListNodes =
            SpB_aligned_realloc(LinkedListMes.ListNodes, sizeof(listNode) * new_capacity);
    }

    new_capacity = LinkedListMes.capacity_nnz_row;
    while (nnz_row > new_capacity)
    {
        new_capacity = ((new_capacity) ? ((new_capacity) << 4) : 1);
    }

    if (new_capacity > LinkedListMes.capacity_nnz_row)
    {
        LinkedListMes.capacity_nnz_row = new_capacity;
        LinkedListMes.ListPtr =
            SpB_aligned_realloc(LinkedListMes.ListPtr, sizeof(listNode *) * new_capacity);
    }
}

void SpB_LinkedList_free()
{
    free(LinkedListMes.ListNodes);
    free(LinkedListMes.ListPtr);
}

void SpB_NNZ_Counting_Mes_malloc()
{
    unsigned int vector_nnz = LinkedListMes.capacity_vector_nnz;
    LinkedListMes.Indices_NZRows = SpB_aligned_malloc(sizeof(unsigned int) * vector_nnz);
    LinkedListMes.LL_Index_Ptr = SpB_aligned_malloc(sizeof(unsigned int) * (vector_nnz + 1));
}

void SpB_NNZ_Counting_Mes_grow(unsigned int nnz)
{
    unsigned int new_capacity = LinkedListMes.capacity_vector_nnz;

    while (nnz > new_capacity)
    {
        // 16 times increase.
        new_capacity = ((new_capacity) ? ((new_capacity) << 4) : 1);
    }

    if (new_capacity > LinkedListMes.capacity_vector_nnz)
    {
        LinkedListMes.capacity_vector_nnz = new_capacity;
        LinkedListMes.Indices_NZRows =
            SpB_aligned_realloc(LinkedListMes.Indices_NZRows, sizeof(unsigned int) * new_capacity);
        LinkedListMes.LL_Index_Ptr =
            SpB_aligned_realloc(LinkedListMes.LL_Index_Ptr, sizeof(unsigned int) * (new_capacity + 1));
    }
}

void SpB_NNZ_Counting_Mes_free()
{
    free(LinkedListMes.Indices_NZRows);
    free(LinkedListMes.LL_Index_Ptr);
}

bool warrning_switch = true;

SpB_Info SpB_Warning_switch_set_ext(bool val)
{
    warrning_switch = val;
    return SpB_SUCCESS;
}

bool SpB_Warning_switch_get()
{
    return warrning_switch;
}