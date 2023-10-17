// #include "GraphBLAS.h"
#include <gtest/gtest.h>
#include "OpenSparseBLAS.h"
#include "stdio.h"
#include "SpB_Debug.h"
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void generate_a_random_index(std::vector<SpB_Index> &a, SpB_Index n)
{
    for (SpB_Index i = 0; i < n; ++i)
    {
        a.push_back((SpB_Index)i);
    }
    std::random_shuffle(a.begin(), a.end());
}

#define TEST_VECTOR_SET_EXTRACT_CPP_TEST(type, T, size)      \
    TEST(Vector_Set_Extract_Test, CPP_##T)                   \
    {                                                        \
        int val_space_size = size;                           \
        std::vector<SpB_Index> random_a;                     \
        generate_a_random_index(random_a, 1000);             \
        type value_array[1000];                              \
        SpB_init();                                          \
        SpB_Vector v;                                        \
        SpB_Vector_new(&v, SpB_##T, 1000);                   \
        for (int i = 0; i < 1000; i++)                       \
        {                                                    \
            type value = mrand48() % val_space_size;         \
            value_array[i] = value;                          \
            SpB_Vector_setElement(v, value, random_a[i]);    \
        }                                                    \
        for (int i = 0; i < 1000; i++)                       \
        {                                                    \
            type value;                                      \
            if (SpB_Vector_extractElement(                   \
                    &value, v, random_a[i]) == SpB_NO_VALUE) \
                value = false;                               \
            ASSERT_EQ(value_array[i], value);                \
        }                                                    \
        SpB_Vector_free(&v);                                 \
        SpB_finalize();                                      \
    }

TEST_VECTOR_SET_EXTRACT_CPP_TEST(bool, BOOL, 2)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(int8_t, INT8, 128)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(int16_t, INT16, 32768)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(int32_t, INT32, 2147483648)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(int64_t, INT64, 2147483648)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(uint8_t, UINT8, 128)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(uint16_t, UINT16, 32768)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(uint32_t, UINT32, 2147483648)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(uint64_t, UINT64, 2147483648)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(float, FP32, 2147483648)
TEST_VECTOR_SET_EXTRACT_CPP_TEST(double, FP64, 2147483648)
#undef TEST_VECTOR_SET_EXTRACT_CPP_TEST

#define TEST_VECTOR_SET_EXTRACT_C_TEST(type, T, size)        \
    TEST(Vector_Set_Extract_Test, C_##T)                     \
    {                                                        \
        int val_space_size = size;                           \
        std::vector<SpB_Index> random_a;                     \
        generate_a_random_index(random_a, 1000);             \
        type value_array[1000];                              \
        SpB_init();                                          \
        SpB_Vector v;                                        \
        SpB_Vector_new(&v, SpB_##T, 1000);                   \
        for (int i = 0; i < 1000; i++)                       \
        {                                                    \
            type value = mrand48() % val_space_size;         \
            value_array[i] = value;                          \
            SpB_Vector_setElement_##T(                       \
                v, value, random_a[i]);                      \
        }                                                    \
        for (int i = 0; i < 1000; i++)                       \
        {                                                    \
            type value;                                      \
            if (SpB_Vector_extractElement_##T(               \
                    &value, v, random_a[i]) == SpB_NO_VALUE) \
                value = false;                               \
            ASSERT_EQ(value_array[i], value);                \
        }                                                    \
        SpB_Vector_free(&v);                                 \
        SpB_finalize();                                      \
    }

TEST_VECTOR_SET_EXTRACT_C_TEST(bool, BOOL, 2)
TEST_VECTOR_SET_EXTRACT_C_TEST(int8_t, INT8, 128)
TEST_VECTOR_SET_EXTRACT_C_TEST(int16_t, INT16, 32768)
TEST_VECTOR_SET_EXTRACT_C_TEST(int32_t, INT32, 2147483648)
TEST_VECTOR_SET_EXTRACT_C_TEST(int64_t, INT64, 2147483648)
TEST_VECTOR_SET_EXTRACT_C_TEST(uint8_t, UINT8, 128)
TEST_VECTOR_SET_EXTRACT_C_TEST(uint16_t, UINT16, 32768)
TEST_VECTOR_SET_EXTRACT_C_TEST(uint32_t, UINT32, 2147483648)
TEST_VECTOR_SET_EXTRACT_C_TEST(uint64_t, UINT64, 2147483648)
TEST_VECTOR_SET_EXTRACT_C_TEST(float, FP32, 2147483648)
TEST_VECTOR_SET_EXTRACT_C_TEST(double, FP64, 2147483648)
#undef TEST_VECTOR_SET_EXTRACT_C_TEST

#define TEST_MATRIX_SET_EXTRACT_C_TEST(type, T, size)     \
    TEST(Matrix_Set_Extract_Test, C_##T)                  \
    {                                                     \
        int val_space_size = size;                        \
        std::vector<SpB_Index> random_a;                  \
        std::vector<SpB_Index> random_b;                  \
        generate_a_random_index(random_a, 1000);          \
        generate_a_random_index(random_b, 1000);          \
        type validation_m[1000][1000];                    \
        memset(validation_m, 0,                           \
               sizeof(type) * 1000 * 1000);               \
        SpB_init();                                       \
        SpB_Matrix m;                                     \
        SpB_Matrix_new(&m, SpB_##T, 1000, 1000);          \
        for (int i = 0; i < 1000; i++)                    \
        {                                                 \
            type value = mrand48() % val_space_size;      \
            SpB_Matrix_setElement_##T(                    \
                m, value, random_a[i], random_b[i]);      \
            validation_m[random_a[i]][random_b[i]] =      \
                value;                                    \
        }                                                 \
        for (int i = 0; i < 1000; i++)                    \
        {                                                 \
            for (int j = 0; j < 1000; j++)                \
            {                                             \
                type value;                               \
                if (SpB_Matrix_extractElement_##T(        \
                        &value, m, i, j) == SpB_NO_VALUE) \
                {                                         \
                    value = (type)0;                      \
                }                                         \
                ASSERT_EQ(validation_m[i][j], value);     \
            }                                             \
        }                                                 \
        SpB_Matrix_free(&m);                              \
        SpB_finalize();                                   \
    }
TEST_MATRIX_SET_EXTRACT_C_TEST(bool, BOOL, 2)
TEST_MATRIX_SET_EXTRACT_C_TEST(int8_t, INT8, 128)
TEST_MATRIX_SET_EXTRACT_C_TEST(int16_t, INT16, 32768)
TEST_MATRIX_SET_EXTRACT_C_TEST(int32_t, INT32, 2147483648)
TEST_MATRIX_SET_EXTRACT_C_TEST(int64_t, INT64, 2147483648)
TEST_MATRIX_SET_EXTRACT_C_TEST(uint8_t, UINT8, 128)
TEST_MATRIX_SET_EXTRACT_C_TEST(uint16_t, UINT16, 32768)
TEST_MATRIX_SET_EXTRACT_C_TEST(uint32_t, UINT32, 2147483648)
TEST_MATRIX_SET_EXTRACT_C_TEST(uint64_t, UINT64, 2147483648)
TEST_MATRIX_SET_EXTRACT_C_TEST(float, FP32, 2147483648)
TEST_MATRIX_SET_EXTRACT_C_TEST(double, FP64, 2147483648)
#undef TEST_MATRIX_SET_EXTRACT_C_TEST