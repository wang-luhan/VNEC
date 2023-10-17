#include "OpenSparseBLAS.h"
#include "utilities/util.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <stdio.h>
#include <string>
#include "SpB_Debug.h"

inline void test_method_of_Matrix_build(const char *mtx_path, bool iscsc)
{

    std::vector<SpB_Index> row_indices;
    std::vector<SpB_Index> col_indices;

    std::vector<float> values;
    SpB_Index nrows, ncols, nvals;

    SpB_Object_Format_ext format;
    if (iscsc)
        format.matrix_format = SpB_CSC;
    else
        format.matrix_format = SpB_CSR;

    SpB_Global_Option_set_ext(SpB_MATRIX_FORMAT, format);

    readMtx<float>(mtx_path, &row_indices, &col_indices, &values, &nrows, &ncols, &nvals, 2, 1, iscsc, 1);

    SpB_init();
    SpB_Matrix G = NULL;
    SpB_Info info;
    SAFE_CALL(SpB_Matrix_new(&G, SpB_FP32, nrows, ncols));

    SpB_Matrix_build_FP32(G, &row_indices[0], &col_indices[0], (float *)&values[0], nvals, SpB_BINARYOP_NULL);

    float temp_val;
    bool flag_debug = false;
    for (SpB_Index i = 0; i < nvals; i++)
    {
        if (SpB_Matrix_extractElement_FP32(&temp_val, G, row_indices[i], col_indices[i]) == SpB_NO_VALUE)
        {
            temp_val = 0.0;
        }
        if (values[i] != temp_val)
            flag_debug = true;
        if (flag_debug)
            exit(1);
        ASSERT_EQ(values[i], temp_val);
    }

    SpB_Matrix_free(&G);
    SpB_finalize();
}

inline void test_method_of_Matrix_build_ext(const char *mtx_path, bool iscsc)
{

    std::vector<SpB_Index> row_indices;
    std::vector<SpB_Index> col_indices;

    std::vector<char> values;
    SpB_Index nrows, ncols, nvals;

    SpB_Object_Format_ext format;
    if (iscsc)
        format.matrix_format = SpB_CSC;
    else
        format.matrix_format = SpB_CSR;

    SpB_Global_Option_set_ext(SpB_MATRIX_FORMAT, format);

    readMtx<char>(mtx_path, &row_indices, &col_indices, &values, &nrows, &ncols, &nvals, 2, 1, iscsc, 1);

    SpB_init();
    SpB_Matrix G = NULL;
    SpB_Info info;
    SAFE_CALL(SpB_Matrix_new(&G, SpB_BOOL, nrows, ncols));
    SpB_Matrix_fast_build_BOOL_ext(G, &row_indices[0], &col_indices[0], (bool *)&values[0], nvals, SpB_BINARYOP_NULL);

    bool temp_val;
    bool flag_debug = false;
    for (SpB_Index i = 0; i < nvals; i++)
    {
        if (SpB_Matrix_extractElement_BOOL(&temp_val, G, row_indices[i], col_indices[i]) == SpB_NO_VALUE)
        {
            temp_val = 0;
        }
        if (values[i] != temp_val)
            flag_debug = true;
        if (flag_debug)
            exit(1);
        ASSERT_EQ(values[i], temp_val);
    }

    SpB_Matrix_free(&G);
    SpB_finalize();
}

#define DATA_PATH "../data/correctness/common/"
#define DATA_FULL_PATH(file) DATA_PATH file "/" file ".mtx"

// CSC tests
TEST(MATRIX_BUILD, ak2010_CSC)
{
    std::string input_file(DATA_FULL_PATH("ak2010"));
    test_method_of_Matrix_build(input_file.c_str(), true);
}

TEST(MATRIX_BUILD, coAuthorsDBLP_CSC)
{
    std::string input_file(DATA_FULL_PATH("coAuthorsDBLP"));
    test_method_of_Matrix_build(input_file.c_str(), true);
}

TEST(MATRIX_BUILD, amazon0302_CSC)
{
    std::string input_file(DATA_FULL_PATH("delaunay_n13"));
    test_method_of_Matrix_build(input_file.c_str(), true);
}

TEST(MATRIX_BUILD, ak2010_CSC_fast_build)
{
    std::string input_file(DATA_FULL_PATH("ak2010"));
    test_method_of_Matrix_build_ext(input_file.c_str(), true);
}

TEST(MATRIX_BUILD, coAuthorsDBLP_CSC_fast_build)
{
    std::string input_file(DATA_FULL_PATH("coAuthorsDBLP"));
    test_method_of_Matrix_build_ext(input_file.c_str(), true);
}

TEST(MATRIX_BUILD, amazon0302_CSC_fast_build)
{
    std::string input_file(DATA_FULL_PATH("delaunay_n13"));
    test_method_of_Matrix_build_ext(input_file.c_str(), true);
}

// CSR tests
TEST(MATRIX_BUILD, ak2010_CSR)
{
    std::string input_file(DATA_FULL_PATH("ak2010"));
    test_method_of_Matrix_build(input_file.c_str(), false);
}

TEST(MATRIX_BUILD, coAuthorsDBLP_CSR)
{
    std::string input_file(DATA_FULL_PATH("coAuthorsDBLP"));
    test_method_of_Matrix_build(input_file.c_str(), false);
}

TEST(MATRIX_BUILD, delaunay_n13_CSR)
{
   std::string input_file(DATA_FULL_PATH("delaunay_n13"));
    test_method_of_Matrix_build(input_file.c_str(), false);
}

TEST(MATRIX_BUILD, amazon0302_CSR_fast_build)
{
    std::string input_file(DATA_FULL_PATH("ak2010"));
    test_method_of_Matrix_build_ext(input_file.c_str(), false);
}

TEST(MATRIX_BUILD, coAuthorsDBLP_CSR_fast_build)
{
    std::string input_file(DATA_FULL_PATH("coAuthorsDBLP"));
    test_method_of_Matrix_build_ext(input_file.c_str(), false);
}

TEST(MATRIX_BUILD, delaunay_n13_CSR_fast_build)
{
   std::string input_file(DATA_FULL_PATH("delaunay_n13"));
    test_method_of_Matrix_build_ext(input_file.c_str(), false);
}
