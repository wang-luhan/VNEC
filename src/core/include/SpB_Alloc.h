#pragma once

#define SpB_MALLOC_ALIGN 64

#define OK_ALLOC(info) \
    if (!(info))       \
        return SpB_FAILURE;

#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif
void *SpB_aligned_malloc(size_t size);

void *SpB_aligned_calloc(size_t size);

void *SpB_aligned_realloc(void *ptr, size_t size);
#ifdef __cplusplus
}
#endif
