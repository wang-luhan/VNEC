#include "OpenSparseBLAS.h"

#define SpB_VECTOR_EXTRACT_CPP(type, T)                                    \
    SpB_Info SpB_Vector_extractElement(type *x, const SpB_Vector v, SpB_Index i) \
    {                                                                      \
        return SpB_Vector_extractElement_##T(x, v, i);                     \
    }

SpB_VECTOR_EXTRACT_CPP(bool, BOOL);
SpB_VECTOR_EXTRACT_CPP(int8_t, INT8);
SpB_VECTOR_EXTRACT_CPP(int16_t, INT16);
SpB_VECTOR_EXTRACT_CPP(int32_t, INT32);
SpB_VECTOR_EXTRACT_CPP(int64_t, INT64);
SpB_VECTOR_EXTRACT_CPP(uint8_t, UINT8);
SpB_VECTOR_EXTRACT_CPP(uint16_t, UINT16);
SpB_VECTOR_EXTRACT_CPP(uint32_t, UINT32);
SpB_VECTOR_EXTRACT_CPP(uint64_t, UINT64);
SpB_VECTOR_EXTRACT_CPP(float, FP32);
SpB_VECTOR_EXTRACT_CPP(double, FP64);
#undef SpB_VECTOR_EXTRACT_CPP

#define SpB_VECTOR_EXTRACT_TUPLES_CPP(type, T)                                    \
    SpB_Info SpB_Vector_extractTuples(SpB_Index *I, type * X, SpB_Index *p_nvals, const SpB_Vector v) \
    {                                                                      \
        return SpB_Vector_extractTuples_##T(I, X, p_nvals, v);                     \
    }

SpB_VECTOR_EXTRACT_TUPLES_CPP(bool, BOOL);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int8_t, INT8);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int16_t, INT16);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int32_t, INT32);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int64_t, INT64);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint8_t, UINT8);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint16_t, UINT16);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint32_t, UINT32);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint64_t, UINT64);
SpB_VECTOR_EXTRACT_TUPLES_CPP(float, FP32);
SpB_VECTOR_EXTRACT_TUPLES_CPP(double, FP64);
#undef SpB_VECTOR_EXTRACT_TUPLES_CPP

#define SpB_VECTOR_SET_CPP(type, T)                                   \
    SpB_Info SpB_Vector_setElement(SpB_Vector v, type x, SpB_Index i) \
    {                                                                 \
        return SpB_Vector_setElement_##T(v, x, i);                    \
    }

SpB_VECTOR_SET_CPP(bool, BOOL);
SpB_VECTOR_SET_CPP(int8_t, INT8);
SpB_VECTOR_SET_CPP(int16_t, INT16);
SpB_VECTOR_SET_CPP(int32_t, INT32);
SpB_VECTOR_SET_CPP(int64_t, INT64);
SpB_VECTOR_SET_CPP(uint8_t, UINT8);
SpB_VECTOR_SET_CPP(uint16_t, UINT16);
SpB_VECTOR_SET_CPP(uint32_t, UINT32);
SpB_VECTOR_SET_CPP(uint64_t, UINT64);
SpB_VECTOR_SET_CPP(float, FP32);
SpB_VECTOR_SET_CPP(double, FP64);
#undef SpB_VECTOR_SET_CPP