#include "test_spmv_vnec.hpp"
#define ntest 40
#define nwarm 1
inline void test_spmv_vnec_fp32(const std::string &input_path, const std::string &result_path, bool iscsc)
{
    Builder<SpB_Index, SpB_Index> builder(input_path);
    CSRGraph<SpB_Index, SpB_Index> g = builder.MakeGraph();

    SpB_Index nrows, ncols, nvals;

    nrows = static_cast<SpB_Index>(g.num_nodes());
    ncols = static_cast<SpB_Index>(g.num_nodes());
    nvals = static_cast<SpB_Index>(g.num_edges());

    SpB_Index *row_indices;
    SpB_Index *col_indices;
    float *values = new float[nvals];

    row_indices = new SpB_Index[nvals];
    col_indices = g.out_destValues();
    SpB_Index *offsets = g.out_offsets();
    SpB_Index index = 0;
    for (SpB_Index i = 0; i < nrows; i++)
    {

        for (SpB_Index j = 0; j < offsets[i + 1] - offsets[i]; j++)
        {
            if (index >= nvals)
            {
                std::cerr << "Error index >= nvals" << index << " " << nvals << std::endl;
                exit(-1);
            }
            values[index] = i;
            row_indices[index++] = static_cast<SpB_Index>(i);
        }
    }
    SpB_init();
    SpB_Object_Format_ext format;
    format.matrix_format = SpB_CSR;
    SpB_Global_Option_set_ext(SpB_MATRIX_FORMAT, format);
    SpB_Matrix G = NULL;
    SpB_Info info;
    SAFE_CALL(SpB_Matrix_new(&G, SpB_FP32, nrows, ncols));
    SpB_Matrix_build_FP32(G, row_indices, col_indices, values, nvals, SpB_BINARYOP_NULL);
    SpB_Vector x;
    SAFE_CALL(SpB_Vector_new(&x, SpB_FP32, ncols));
    for (SpB_Index j = 0; j < ncols; j++)
    {
        SpB_Vector_setElement_FP32(x, j, j);
    }
    SpB_Vector result_benchmark;
    SAFE_CALL(SpB_Vector_new(&result_benchmark, SpB_FP32, nrows));
    SpB_Vector result_opt;
    SAFE_CALL(SpB_Vector_new(&result_opt, SpB_FP32, nrows));
    struct timeval opt_start, opt_end;
    for (SpB_Index i = 0; i < 1; i++)
    {
        SpB_mxv_FP32_serial(result_benchmark, G, x);
    }
    //---------------------------------------------------------------------------------------------------------------------------
    if(nrows < 3000000)
    {
        SpB_VNEC_type vnec_t = IRD_VNEC(G, SpB_FP32);
        if(vnec_t == SpB_VNEC_D)
        {
            printf("-D is selected\n");
            VNEC_D_FP32 *mat_thd = Build_VNEC_D_FP32(G);
            //warm up
            for (SpB_Index i = 0; i < nwarm; i++)
            {
                SpMV_VNEC_D_FP32(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_start, NULL);
            for (SpB_Index i = 0; i < ntest; i++)
            {
                SpMV_VNEC_D_FP32(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_end, NULL);
            VNEC_D_free_fp32(mat_thd);
        }
        else
        {
            printf("-S is selected\n");
            VNEC_S_FP32 *mat_thd = Build_VNEC_S_FP32(G);
            //warm up
            for (SpB_Index i = 0; i < nwarm; i++)
            {
                SpMV_VNEC_S_FP32(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_start, NULL);
            for (SpB_Index i = 0; i < ntest; i++)
            {
                SpMV_VNEC_S_FP32(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_end, NULL);
            VNEC_S_free_fp32(mat_thd);
        }
    }
    else
    {
        printf("-L is selected\n");
        VNEC_L *mat_thd = Build_VNEC_L(G);
        //warm up
        for (SpB_Index i = 0; i < nwarm; i++)
        {
            SpMV_VNEC_L_FP32(result_opt, G, x, mat_thd);
        }
        gettimeofday(&opt_start, NULL);
        for (SpB_Index i = 0; i < ntest; i++)
        {
            SpMV_VNEC_L_FP32(result_opt, G, x, mat_thd);
        }
        gettimeofday(&opt_end, NULL);
        free(mat_thd->diagonal_start);
        free(mat_thd->diagonal_end);
        free(mat_thd->nz_indices);
        free(mat_thd->thread_coord_start);
        free(mat_thd->thread_coord_end);
        free(mat_thd);
    }
    long opt_timeuse = 1000000 * (opt_end.tv_sec - opt_start.tv_sec) + opt_end.tv_usec - opt_start.tv_usec;
    printf("\n ------------------------------------------------------------------------------");
    printf("\n VNEC mxv overall time elapsed: %f ms\n", opt_timeuse / 1000.0 / ntest);
    printf(" ------------------------------------------------------------------------------ \n");
    float diff = 0.0;
    for (SpB_Index i = 0; i < nrows; ++i)
    {
        float value_benchmark, value_opt;
        SpB_Vector_extractElement_FP32(&value_benchmark, result_benchmark, i);
        SpB_Vector_extractElement_FP32(&value_opt, result_opt, i);
        diff = fabs(value_benchmark - value_opt);
        ASSERT_LE(diff, value_benchmark * 1e-4);
    }

    SpB_Matrix_free(&G);
    SpB_Vector_free(&x);
    SpB_Vector_free(&result_opt);
    SpB_Vector_free(&result_benchmark);
}
inline void test_spmv_vnec_fp64(const std::string &input_path, const std::string &result_path, bool iscsc)
{
    Builder<SpB_Index, SpB_Index> builder(input_path);
    CSRGraph<SpB_Index, SpB_Index> g = builder.MakeGraph();

    SpB_Index nrows, ncols, nvals;

    nrows = static_cast<SpB_Index>(g.num_nodes());
    ncols = static_cast<SpB_Index>(g.num_nodes());
    nvals = static_cast<SpB_Index>(g.num_edges());

    SpB_Index *row_indices;
    SpB_Index *col_indices;
    double *values = new double[nvals];

    row_indices = new SpB_Index[nvals];
    col_indices = g.out_destValues();
    SpB_Index *offsets = g.out_offsets();
    SpB_Index index = 0;
    for (SpB_Index i = 0; i < nrows; i++)
    {

        for (SpB_Index j = 0; j < offsets[i + 1] - offsets[i]; j++)
        {
            if (index >= nvals)
            {
                std::cerr << "Error index >= nvals" << index << " " << nvals << std::endl;
                exit(-1);
            }
            values[index] = i;
            row_indices[index++] = static_cast<SpB_Index>(i);
        }
    }
    SpB_init();
    SpB_Object_Format_ext format;
    format.matrix_format = SpB_CSR;
    SpB_Global_Option_set_ext(SpB_MATRIX_FORMAT, format);
    SpB_Matrix G = NULL;
    SpB_Info info;
    SAFE_CALL(SpB_Matrix_new(&G, SpB_FP64, nrows, ncols));
    SpB_Matrix_build_FP64(G, row_indices, col_indices, values, nvals, SpB_BINARYOP_NULL);
    SpB_Vector x;
    SAFE_CALL(SpB_Vector_new(&x, SpB_FP64, ncols));
    for (SpB_Index j = 0; j < ncols; j++)
    {
        SpB_Vector_setElement_FP64(x, j, j);
    }
    SpB_Vector result_benchmark;
    SAFE_CALL(SpB_Vector_new(&result_benchmark, SpB_FP64, nrows));
    SpB_Vector result_opt;
    SAFE_CALL(SpB_Vector_new(&result_opt, SpB_FP64, nrows));
    struct timeval opt_start, opt_end;
    for (SpB_Index i = 0; i < 1; i++)
    {
        SpB_mxv_FP64_serial(result_benchmark, G, x);
    }
    //---------------------------------------------------------------------------------------------------------------------------
    if(nrows < 3000000)
    {
        SpB_VNEC_type vnec_t = IRD_VNEC(G, SpB_FP64);
        if(vnec_t == SpB_VNEC_D)
        {
            printf("-D is selected\n");
            VNEC_D_FP64 *mat_thd = Build_VNEC_D_FP64(G);
            //warm up
            for (SpB_Index i = 0; i < nwarm; i++)
            {
                SpMV_VNEC_D_FP64(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_start, NULL);
            for (SpB_Index i = 0; i < ntest; i++)
            {
                SpMV_VNEC_D_FP64(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_end, NULL);
            VNEC_D_free_fp64(mat_thd);
        }
        else
        {
            printf("-S is selected\n");
            VNEC_S_FP64 *mat_thd = Build_VNEC_S_FP64(G);
            //warm up
            for (SpB_Index i = 0; i < nwarm; i++)
            {
                SpMV_VNEC_S_FP64(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_start, NULL);
            for (SpB_Index i = 0; i < ntest; i++)
            {
                SpMV_VNEC_S_FP64(result_opt, G, x, mat_thd);
            }
            gettimeofday(&opt_end, NULL);
            VNEC_S_free_fp64(mat_thd);
        }
    }
    else
    {
        printf("-L is selected\n");
        VNEC_L *mat_thd = Build_VNEC_L(G);
        //warm up
        for (SpB_Index i = 0; i < nwarm; i++)
        {
            SpMV_VNEC_L_FP64(result_opt, G, x, mat_thd);
        }
        gettimeofday(&opt_start, NULL);
        for (SpB_Index i = 0; i < ntest; i++)
        {
            SpMV_VNEC_L_FP64(result_opt, G, x, mat_thd);
        }
        gettimeofday(&opt_end, NULL);
        free(mat_thd->diagonal_start);
        free(mat_thd->diagonal_end);
        free(mat_thd->nz_indices);
        free(mat_thd->thread_coord_start);
        free(mat_thd->thread_coord_end);
        free(mat_thd);
    }
    long opt_timeuse = 1000000 * (opt_end.tv_sec - opt_start.tv_sec) + opt_end.tv_usec - opt_start.tv_usec;
    printf("\n ------------------------------------------------------------------------------");
    printf("\n VNEC_FP64 mxv overall time elapsed: %f ms\n", opt_timeuse / 1000.0 / ntest);
    printf(" ------------------------------------------------------------------------------ \n");
    double diff = 0.0;
    for (SpB_Index i = 0; i < nrows; ++i)
    {
        double value_benchmark, value_opt;
        SpB_Vector_extractElement_FP64(&value_benchmark, result_benchmark, i);
        SpB_Vector_extractElement_FP64(&value_opt, result_opt, i);
        diff = fabs(value_benchmark - value_opt);
        ASSERT_LE(diff, value_benchmark * 1e-8);
    }

    SpB_Matrix_free(&G);
    SpB_Vector_free(&x);
    SpB_Vector_free(&result_opt);
    SpB_Vector_free(&result_benchmark);
}


TEST(mxv_test_VNEC_FP32, roadNet_CA)
{
    std::string input_file(MATRIX_SG_FULL_PATH(roadNet-CA));
    std::string output_file(RESULT_FULL_PATH(roadNet-CA));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, amazon0312)
{
    std::string input_file(MATRIX_SG_FULL_PATH(amazon0312));
    std::string output_file(RESULT_FULL_PATH(amazon0312));
    test_spmv_vnec_fp32(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP32, com_Youtube)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-Youtube));
    std::string output_file(RESULT_FULL_PATH(com-Youtube));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, web_Stanford)
{
    std::string input_file(MATRIX_SG_FULL_PATH(web-Stanford));
    std::string output_file(RESULT_FULL_PATH(web-Stanford));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, as_Skitter)
{
    std::string input_file(MATRIX_SG_FULL_PATH(as-Skitter));
    std::string output_file(RESULT_FULL_PATH(as-Skitter));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, road_usa)
{
    std::string input_file(MATRIX_SG_FULL_PATH(road_usa));
    std::string output_file(RESULT_FULL_PATH(road_usa));
    test_spmv_vnec_fp32(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP32, coAuthorsCiteseer)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coAuthorsCiteseer));
    std::string output_file(RESULT_FULL_PATH(coAuthorsCiteseer));
    test_spmv_vnec_fp32(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP32, coAuthorsDBLP)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coAuthorsDBLP));
    std::string output_file(RESULT_FULL_PATH(coAuthorsDBLP));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, web_Google)
{
    std::string input_file(MATRIX_SG_FULL_PATH(web-Google));
    std::string output_file(RESULT_FULL_PATH(web-Google));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, com_Orkut)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-Orkut));
    std::string output_file(RESULT_FULL_PATH(com-Orkut));
    test_spmv_vnec_fp32(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP32, higgs_twitter)
{
    std::string input_file(MATRIX_SG_FULL_PATH(higgs-twitter));
    std::string output_file(RESULT_FULL_PATH(higgs-twitter));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, wiki_topcats)
{
    std::string input_file(MATRIX_SG_FULL_PATH(wiki-topcats));
    std::string output_file(RESULT_FULL_PATH(wiki-topcats));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, web_BerkStan)
{
    std::string input_file(MATRIX_SG_FULL_PATH(web-BerkStan));
    std::string output_file(RESULT_FULL_PATH(web-BerkStan));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, com_Amazon)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-Amazon));
    std::string output_file(RESULT_FULL_PATH(wcom-Amazon));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, com_DBLP)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-DBLP));
    std::string output_file(RESULT_FULL_PATH(com-DBLP));
    test_spmv_vnec_fp32(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP32, TSOPF_RS_b2383)
{
    std::string input_file(MATRIX_SG_FULL_PATH(TSOPF_RS_b2383));
    std::string output_file(RESULT_FULL_PATH(TSOPF_RS_b2383));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, coPapersCiteseer)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coPapersCiteseer));
    std::string output_file(RESULT_FULL_PATH(coPapersCiteseer));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, eu_2005)
{
    std::string input_file(MATRIX_SG_FULL_PATH(eu-2005));
    std::string output_file(RESULT_FULL_PATH(eu-2005));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, crankseg_2)
{
    std::string input_file(MATRIX_SG_FULL_PATH(crankseg_2));
    std::string output_file(RESULT_FULL_PATH(crankseg_2));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, nd12k)
{
    std::string input_file(MATRIX_SG_FULL_PATH(nd12k));
    std::string output_file(RESULT_FULL_PATH(nd12k));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, coPapersDBLP)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coPapersDBLP));
    std::string output_file(RESULT_FULL_PATH(coPapersDBLP));
    test_spmv_vnec_fp32(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP32, cnr_2000)
{
    std::string input_file(MATRIX_SG_FULL_PATH(cnr-2000));
    std::string output_file(RESULT_FULL_PATH(cnr-2000));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, msdoor)
{
    std::string input_file(MATRIX_SG_FULL_PATH(msdoor));
    std::string output_file(RESULT_FULL_PATH(msdoor));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, bundle_adj)
{
    std::string input_file(MATRIX_SG_FULL_PATH(bundle_adj));
    std::string output_file(RESULT_FULL_PATH(bundle_adj));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, F1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(F1));
    std::string output_file(RESULT_FULL_PATH(F1));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, inline_1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(inline_1));
    std::string output_file(RESULT_FULL_PATH(inline_1));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, audikw_1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(audikw_1));
    std::string output_file(RESULT_FULL_PATH(audikw_1));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, ML_Laplace)
{
    std::string input_file(MATRIX_SG_FULL_PATH(ML_Laplace));
    std::string output_file(RESULT_FULL_PATH(ML_Laplace));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, dielFilterV3real)
{
    std::string input_file(MATRIX_SG_FULL_PATH(dielFilterV3real));
    std::string output_file(RESULT_FULL_PATH(dielFilterV3real));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, Flan_1565)
{
    std::string input_file(MATRIX_SG_FULL_PATH(Flan_1565));
    std::string output_file(RESULT_FULL_PATH(Flan_1565));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, Long_Coup_dt0)
{
    std::string input_file(MATRIX_SG_FULL_PATH(Long_Coup_dt0));
    std::string output_file(RESULT_FULL_PATH(Long_Coup_dt0));
    test_spmv_vnec_fp32(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP32, mip1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(mip1));
    std::string output_file(RESULT_FULL_PATH(mip1));
    test_spmv_vnec_fp32(input_file, output_file, false);
}



TEST(mxv_test_VNEC_FP64, roadNet_CA)
{
    std::string input_file(MATRIX_SG_FULL_PATH(roadNet-CA));
    std::string output_file(RESULT_FULL_PATH(roadNet-CA));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, amazon0312)
{
    std::string input_file(MATRIX_SG_FULL_PATH(amazon0312));
    std::string output_file(RESULT_FULL_PATH(amazon0312));
    test_spmv_vnec_fp64(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP64, com_Youtube)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-Youtube));
    std::string output_file(RESULT_FULL_PATH(com-Youtube));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, web_Stanford)
{
    std::string input_file(MATRIX_SG_FULL_PATH(web-Stanford));
    std::string output_file(RESULT_FULL_PATH(web-Stanford));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, as_Skitter)
{
    std::string input_file(MATRIX_SG_FULL_PATH(as-Skitter));
    std::string output_file(RESULT_FULL_PATH(as-Skitter));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, road_usa)
{
    std::string input_file(MATRIX_SG_FULL_PATH(road_usa));
    std::string output_file(RESULT_FULL_PATH(road_usa));
    test_spmv_vnec_fp64(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP64, coAuthorsCiteseer)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coAuthorsCiteseer));
    std::string output_file(RESULT_FULL_PATH(coAuthorsCiteseer));
    test_spmv_vnec_fp64(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP64, coAuthorsDBLP)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coAuthorsDBLP));
    std::string output_file(RESULT_FULL_PATH(coAuthorsDBLP));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, web_Google)
{
    std::string input_file(MATRIX_SG_FULL_PATH(web-Google));
    std::string output_file(RESULT_FULL_PATH(web-Google));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, com_Orkut)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-Orkut));
    std::string output_file(RESULT_FULL_PATH(com-Orkut));
    test_spmv_vnec_fp64(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP64, higgs_twitter)
{
    std::string input_file(MATRIX_SG_FULL_PATH(higgs-twitter));
    std::string output_file(RESULT_FULL_PATH(higgs-twitter));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, wiki_topcats)
{
    std::string input_file(MATRIX_SG_FULL_PATH(wiki-topcats));
    std::string output_file(RESULT_FULL_PATH(wiki-topcats));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, web_BerkStan)
{
    std::string input_file(MATRIX_SG_FULL_PATH(web-BerkStan));
    std::string output_file(RESULT_FULL_PATH(web-BerkStan));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, com_Amazon)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-Amazon));
    std::string output_file(RESULT_FULL_PATH(wcom-Amazon));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, com_DBLP)
{
    std::string input_file(MATRIX_SG_FULL_PATH(com-DBLP));
    std::string output_file(RESULT_FULL_PATH(com-DBLP));
    test_spmv_vnec_fp64(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP64, TSOPF_RS_b2383)
{
    std::string input_file(MATRIX_SG_FULL_PATH(TSOPF_RS_b2383));
    std::string output_file(RESULT_FULL_PATH(TSOPF_RS_b2383));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, coPapersCiteseer)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coPapersCiteseer));
    std::string output_file(RESULT_FULL_PATH(coPapersCiteseer));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, eu_2005)
{
    std::string input_file(MATRIX_SG_FULL_PATH(eu-2005));
    std::string output_file(RESULT_FULL_PATH(eu-2005));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, crankseg_2)
{
    std::string input_file(MATRIX_SG_FULL_PATH(crankseg_2));
    std::string output_file(RESULT_FULL_PATH(crankseg_2));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, nd12k)
{
    std::string input_file(MATRIX_SG_FULL_PATH(nd12k));
    std::string output_file(RESULT_FULL_PATH(nd12k));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, coPapersDBLP)
{
    std::string input_file(MATRIX_SG_FULL_PATH(coPapersDBLP));
    std::string output_file(RESULT_FULL_PATH(coPapersDBLP));
    test_spmv_vnec_fp64(input_file, output_file, false);
}

TEST(mxv_test_VNEC_FP64, cnr_2000)
{
    std::string input_file(MATRIX_SG_FULL_PATH(cnr-2000));
    std::string output_file(RESULT_FULL_PATH(cnr-2000));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, msdoor)
{
    std::string input_file(MATRIX_SG_FULL_PATH(msdoor));
    std::string output_file(RESULT_FULL_PATH(msdoor));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, bundle_adj)
{
    std::string input_file(MATRIX_SG_FULL_PATH(bundle_adj));
    std::string output_file(RESULT_FULL_PATH(bundle_adj));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, F1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(F1));
    std::string output_file(RESULT_FULL_PATH(F1));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, inline_1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(inline_1));
    std::string output_file(RESULT_FULL_PATH(inline_1));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, audikw_1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(audikw_1));
    std::string output_file(RESULT_FULL_PATH(audikw_1));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, ML_Laplace)
{
    std::string input_file(MATRIX_SG_FULL_PATH(ML_Laplace));
    std::string output_file(RESULT_FULL_PATH(ML_Laplace));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, dielFilterV3real)
{
    std::string input_file(MATRIX_SG_FULL_PATH(dielFilterV3real));
    std::string output_file(RESULT_FULL_PATH(dielFilterV3real));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, Flan_1565)
{
    std::string input_file(MATRIX_SG_FULL_PATH(Flan_1565));
    std::string output_file(RESULT_FULL_PATH(Flan_1565));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, Long_Coup_dt0)
{
    std::string input_file(MATRIX_SG_FULL_PATH(Long_Coup_dt0));
    std::string output_file(RESULT_FULL_PATH(Long_Coup_dt0));
    test_spmv_vnec_fp64(input_file, output_file, false);
}
TEST(mxv_test_VNEC_FP64, mip1)
{
    std::string input_file(MATRIX_SG_FULL_PATH(mip1));
    std::string output_file(RESULT_FULL_PATH(mip1));
    test_spmv_vnec_fp64(input_file, output_file, false);
}