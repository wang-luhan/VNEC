#pragma once

#include "OpenSparseBLAS.h"
#include <stdlib.h>

#define SpB_TYPE_CASE(type, object, SIZE) \
    case (type):                          \
        (*object)->element_size = SIZE;           \
        break;

#define SpB_TYPE_CASES(object)            \
    SpB_TYPE_CASE(SpB_BOOL, object, 1);   \
    SpB_TYPE_CASE(SpB_INT8, object, 1);   \
    SpB_TYPE_CASE(SpB_INT16, object, 2);  \
    SpB_TYPE_CASE(SpB_INT32, object, 4);  \
    SpB_TYPE_CASE(SpB_INT64, object, 8);  \
    SpB_TYPE_CASE(SpB_UINT8, object, 1);  \
    SpB_TYPE_CASE(SpB_UINT16, object, 2); \
    SpB_TYPE_CASE(SpB_UINT32, object, 4); \
    SpB_TYPE_CASE(SpB_UINT64, object, 8); \
    SpB_TYPE_CASE(SpB_FP32, object, 4);   \
    SpB_TYPE_CASE(SpB_FP64, object, 8);   \
    default:                              \
        return SpB_FAILURE;

#define ZERO_BOOL false
#define ZERO_INT8 0
#define ZERO_INT16 0
#define ZERO_INT32 0
#define ZERO_INT64 0
#define ZERO_UINT8 0
#define ZERO_UINT16 0
#define ZERO_UINT32 0
#define ZERO_UINT64 0
#define ZERO_FP32 0.0f
#define ZERO_FP64 0.0f

#define ZERO_bool false
#define ZERO_int8_t 0
#define ZERO_int16_t 0
#define ZERO_int32_t 0
#define ZERO_int64_t 0
#define ZERO_uint8_t 0
#define ZERO_uint16_t 0
#define ZERO_uint32_t 0
#define ZERO_uint64_t 0
#define ZERO_float 0.0f
#define ZERO_double 0.0f