#pragma once

#include "OpenSparseBLAS.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <cstring>
#include <tuple>
#include <algorithm>
#include <vector>
#include <assert.h>
#include <typeinfo>

typedef char MM_type_info[4];

#define MM_MAX_LINE_LENGTH 1025
#define MM_MAX_TOKEN_LENGTH 64

#define MatrixMarketBanner "%%MatrixMarket"
#define MM_MTX_STR "matrix"

#define MM_SPARSE_STR "coordinate"
#define MM_DENSE_STR "array"

#define MM_REAL_STR "real"
#define MM_COMPLEX_STR "complex"
#define MM_PATTERN_STR "pattern"
#define MM_INT_STR "integer"

#define MM_GENERAL_STR "general"
#define MM_SYMM_STR "symmetric"
#define MM_HERM_STR "hermitian"
#define MM_SKEW_STR "skew-symmetric"
/********************* Matrix Market error codes ***************************/

#define MM_COULD_NOT_READ_FILE 11
#define MM_PREMATURE_EOF 12
#define MM_NOT_MTX 13
#define MM_NO_HEADER 14
#define MM_UNSUPPORTED_TYPE 15
#define MM_LINE_TOO_LONG 16
#define MM_COULD_NOT_WRITE_FILE 17

#define mm_set_matrix(typecode) ((*typecode)[0] = 'M')

#define mm_set_coordinate(typecode) ((*typecode)[1] = 'C')
#define mm_set_array(typecode) ((*typecode)[1] = 'A')
#define mm_set_dense(typecode) mm_set_array(typecode)
#define mm_set_sparse(typecode) mm_set_coordinate(typecode)

#define mm_set_complex(typecode) ((*typecode)[2] = 'C')
#define mm_set_real(typecode) ((*typecode)[2] = 'R')
#define mm_set_pattern(typecode) ((*typecode)[2] = 'P')
#define mm_set_integer(typecode) ((*typecode)[2] = 'I')

#define mm_set_symmetric(typecode) ((*typecode)[3] = 'S')
#define mm_set_general(typecode) ((*typecode)[3] = 'G')
#define mm_set_skew(typecode) ((*typecode)[3] = 'K')
#define mm_set_hermitian(typecode) ((*typecode)[3] = 'H')

#define mm_clear_typecode(typecode) ((*typecode)[0] = (*typecode)[1] = \
                                         (*typecode)[2] = ' ',         \
                                     (*typecode)[3] = 'G')

/********************* MM_typecode query fucntions ***************************/

#define mm_is_matrix(typecode) ((typecode)[0] == 'M')

#define mm_is_sparse(typecode) ((typecode)[1] == 'C')
#define mm_is_coordinate(typecode) ((typecode)[1] == 'C')
#define mm_is_dense(typecode) ((typecode)[1] == 'A')
#define mm_is_array(typecode) ((typecode)[1] == 'A')

#define mm_is_real(typecode) ((typecode)[2] == 'R')
#define mm_is_pattern(typecode) ((typecode)[2] == 'P')
#define mm_is_integer(typecode) ((typecode)[2] == 'I')

#define mm_is_symmetric(typecode) ((typecode)[3] == 'S')
#define mm_is_general(typecode) ((typecode)[3] == 'G')
#define mm_is_skew(typecode) ((typecode)[3] == 'K')
#define mm_is_hermitian(typecode) ((typecode)[3] == 'H')

int mm_read_banner(FILE *f, MM_type_info *matcode);

int mm_read_mtx_crd_size(FILE *f, SpB_Index *M, SpB_Index *N, SpB_Index *nz);

template <typename T>
void readTuples(std::vector<SpB_Index> *row_indices,
                std::vector<SpB_Index> *col_indices,
                std::vector<T> *values,
                SpB_Index nvals,
                FILE *f)
{
    SpB_Index row_ind, col_ind;
    T value = (T)1;

    // Currently checks if there are fewer rows than promised
    // Could add check for edges in diagonal of adjacency matrix
    char OneLine[100];
    SpB_Index i = 0;
    while (fgets(OneLine, 100, f) != NULL && i < nvals)
    {
        i++;
        char *p = strtok(OneLine, " ");
        sscanf(p, "%lu", &row_ind);
        if (p == NULL)
        {
            printf("Error: Not enough rows in mtx file!\n");
            return;
        }
        else
        {
            p = strtok(NULL, " ");
            sscanf(p, "%lu", &col_ind);
            // int u = fscanf(f, "%lu", &col_ind);
            if (!p)
            {
                printf("Error: Not read the col_ind file!\n");
                exit(1);
            }

            // Convert 1-based indexing MTX to 0-based indexing C++
            row_indices->push_back(row_ind - 1);
            col_indices->push_back(col_ind - 1);
            values->push_back(value);
        }
    }
}

template <typename T, typename mtxT>
void readTuples(std::vector<SpB_Index> *row_indices,
                std::vector<SpB_Index> *col_indices,
                std::vector<T> *values,
                SpB_Index nvals,
                FILE *f)
{
    SpB_Index row_ind, col_ind;
    T value;
    mtxT raw_value;
    char type_str[3];
    type_str[0] = '%';
    if (typeid(mtxT) == typeid(int))
        type_str[1] = 'd';
    else if (typeid(mtxT) == typeid(float))
        type_str[1] = 'f';

    // Currently checks if there are fewer rows than promised
    // Could add check for edges in diagonal of adjacency matrix
    for (SpB_Index i = 0; i < nvals; i++)
    {
        if (fscanf(f, "%lu", &row_ind) == EOF)
        {
            printf("Error: Not enough rows in mtx file!\n");
            return;
        }
        else
        {
            int u = fscanf(f, "%lu", &col_ind);
            if (u != 1)
            {
                printf("Error: Not read the col_ind file!\n");
                exit(1);
            }
            // assert(u == 1);

            // Convert 1-based indexing MTX to 0-based indexing C++
            row_indices->push_back(row_ind - 1);
            col_indices->push_back(col_ind - 1);

            u = fscanf(f, type_str, &raw_value);
            if (u != 1)
            {
                printf("Error: Not read the col_ind file!\n");
                exit(1);
            }
            value = static_cast<T>(raw_value);

            values->push_back(value);
        }
    }
}

template <typename T>
bool compare_row_priority(const std::tuple<SpB_Index,
                              SpB_Index,
                              T,
                              SpB_Index> &lhs,
             const std::tuple<SpB_Index,
                              SpB_Index,
                              T,
                              SpB_Index> &rhs)
{
    SpB_Index a = std::get<0>(lhs);
    SpB_Index b = std::get<0>(rhs);
    SpB_Index c = std::get<1>(lhs);
    SpB_Index d = std::get<1>(rhs);
    if (a == b)
        return c < d;
    else
        return a < b;
}

template <typename T>
bool compare_col_priority(const std::tuple<SpB_Index,
                              SpB_Index,
                              T,
                              SpB_Index> &lhs,
             const std::tuple<SpB_Index,
                              SpB_Index,
                              T,
                              SpB_Index> &rhs)
{
    SpB_Index a = std::get<0>(lhs);
    SpB_Index b = std::get<0>(rhs);
    SpB_Index c = std::get<1>(lhs);
    SpB_Index d = std::get<1>(rhs);
    if (c == d)
        return a < b;
    else
        return c < d;
}

template <typename T>
void customSort(std::vector<SpB_Index> *row_indices,
                std::vector<SpB_Index> *col_indices,
                std::vector<T> *values, bool isCSC)
{
    SpB_Index nvals = row_indices->size();
    std::vector<std::tuple<SpB_Index,
                           SpB_Index,
                           T,
                           SpB_Index>>
        my_tuple;

    for (SpB_Index i = 0; i < nvals; ++i)
        my_tuple.push_back(std::make_tuple((*row_indices)[i], (*col_indices)[i],
                                           (*values)[i], i));

    if(isCSC)
        std::sort(my_tuple.begin(), my_tuple.end(), compare_col_priority<T>);
    else
        std::sort(my_tuple.begin(), my_tuple.end(), compare_row_priority<T>);

    std::vector<SpB_Index> v1 = *row_indices;
    std::vector<SpB_Index> v2 = *col_indices;
    std::vector<T> v3 = *values;

    for (SpB_Index i = 0; i < nvals; ++i)
    {
        SpB_Index index = std::get<3>(my_tuple[i]);
        (*row_indices)[i] = v1[index];
        (*col_indices)[i] = v2[index];
        (*values)[i] = v3[index];
    }
}

template <typename T>
inline T getEnv(const char *key, T default_val)
{
    const char *val = std::getenv(key);
    if (val == NULL)
        return default_val;
    else
        return static_cast<T>(atoi(val));
}

/*!
 * Remove self-loops, duplicates and make graph undirected if option is set
 */
template <typename T>
void removeSelfloop(std::vector<SpB_Index> *row_indices,
                    std::vector<SpB_Index> *col_indices,
                    std::vector<T> *values,
                    SpB_Index *nvals,
                    bool undirected, bool isCSC)
{
    bool remove_self_loops = getEnv("GRB_UTIL_REMOVE_SELFLOOP", true);

    if (undirected)
    {
        for (SpB_Index i = 0; i < *nvals; i++)
        {
            if ((*col_indices)[i] != (*row_indices)[i])
            {
                row_indices->push_back((*col_indices)[i]);
                col_indices->push_back((*row_indices)[i]);
                values->push_back((*values)[i]);
            }
        }
    }

    *nvals = row_indices->size();

    // Sort
    customSort<T>(row_indices, col_indices, values, isCSC);

    SpB_Index curr = (*col_indices)[0];
    SpB_Index last;
    SpB_Index curr_row = (*row_indices)[0];
    SpB_Index last_row;

    // Detect self-loops and duplicates
    for (SpB_Index i = 0; i < *nvals; i++)
    {
        last = curr;
        last_row = curr_row;
        curr = (*col_indices)[i];
        curr_row = (*row_indices)[i];

        // Self-loops
        if (remove_self_loops && curr_row == curr)
            (*col_indices)[i] = SpB_INDEX_MAX;

        // Duplicates
        if (i > 0 && curr == last && curr_row == last_row)
            (*col_indices)[i] = SpB_INDEX_MAX;
    }

    SpB_Index shift = 0;

    // Remove self-loops and duplicates marked -1.
    SpB_Index back = 0;
    for (SpB_Index i = 0; i + shift < *nvals; i++)
    {
        if ((*col_indices)[i] == SpB_INDEX_MAX)
        {
            for (; back <= *nvals; shift++)
            {
                back = i + shift;
                if ((*col_indices)[back] != SpB_INDEX_MAX)
                {
                    (*col_indices)[i] = (*col_indices)[back];
                    (*row_indices)[i] = (*row_indices)[back];
                    (*col_indices)[back] = SpB_INDEX_MAX;
                    break;
                }
            }
        }
    }

    *nvals = *nvals - shift;
    row_indices->resize(*nvals);
    col_indices->resize(*nvals);
    values->resize(*nvals);
}
//TODO: 自环有问题：只把index给去了，value没有去掉
template <typename T>
int readMtx(const char *fname,
            std::vector<SpB_Index> *row_indices,
            std::vector<SpB_Index> *col_indices,
            std::vector<T> *values,
            SpB_Index *nrows,
            SpB_Index *ncols,
            SpB_Index *nvals,
            int directed,
            int isPattern,
            bool isCSC,
            bool selfloop)
{
    int ret_code;

    MM_type_info mat_type_info;

    FILE *f;

    if ((f = fopen(fname, "r")) == NULL)
    {
        printf("Can't find the file %s\n", fname);
        throw(1);
    }

    // Read MTX banner
    if (mm_read_banner(f, &mat_type_info) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    // Read MTX Size
    if ((ret_code = mm_read_mtx_crd_size(f, nrows, ncols, nvals)) != 0)
        exit(1);

    printf("The mat_type_info : %c%c%c%c\n", mat_type_info[0], mat_type_info[1], mat_type_info[2], mat_type_info[3]);
    printf("Undirected due to mtx: %d\n", mm_is_symmetric(mat_type_info));
    printf("Undirected due to cmd: %d\n", directed == 2);
    bool is_undirected = mm_is_symmetric(mat_type_info) || directed == 2;
    is_undirected = (directed == 1) ? false : is_undirected;
    printf("Undirected: %d\n", is_undirected);

    if (mm_is_integer(mat_type_info) && !isPattern)
        readTuples<T, int>(row_indices, col_indices, values, *nvals, f);
    else if (mm_is_real(mat_type_info) && !isPattern)
        readTuples<T, float>(row_indices, col_indices, values, *nvals, f);
    // else if (mm_is_pattern(mat_type_info))
    else
        readTuples<T>(row_indices, col_indices, values, *nvals, f);

    if(!selfloop)
        removeSelfloop<T>(row_indices, col_indices, values, nvals, is_undirected, isCSC);

    customSort<T>(row_indices, col_indices, values, isCSC);

    return ret_code;
}

int writeVec(const char *fname, int *v, SpB_Index n);