#include "utilities/command_line.h"
#include "utilities/graph.hpp"
#include "utilities/builder.hpp"

template <typename T>
inline void CHECK_NULLPTR(T *ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
    }
}

/**
 * @brief Read the matrix data from the.sg/.wsg file and store it in three arrays in COO format. The elements are arranged in row-first order (order in CSR format) .
 *
 * @tparam T The datatype of values of Matrix.
 * @param[in] full_path The full path of the file to be read.
 * @param[out] values The values array of COO.
 * @param[out] row_indices The row_indices array of COO.
 * @param[out] col_indices The col_indices array of COO.
 * @param[out] nrows The number of rows.
 * @param[out] ncols The number of cols.
 * @param[out] nvals The number of non-zero values.
 */
template <typename T>
void Read_CSR_Matrix(
    const std::string full_path,
    T *&values,
    SpB_Index *&row_indices,
    SpB_Index *&col_indices,
    SpB_Index &nrows,
    SpB_Index &ncols,
    SpB_Index &nvals)
{
    // Read input graph from a .sg/.wsg file.
    Builder<SpB_Index, SpB_Index> builder(full_path);
    CSRGraph<SpB_Index, SpB_Index> g = builder.MakeGraph();

    nrows = static_cast<SpB_Index>(g.num_nodes());
    ncols = static_cast<SpB_Index>(g.num_nodes());
    nvals = static_cast<SpB_Index>(g.num_edges());

    CHECK_NULLPTR(values);
    CHECK_NULLPTR(row_indices);
    CHECK_NULLPTR(col_indices);

    values = new T[nvals];
    col_indices = new SpB_Index[nvals];
    memcpy(col_indices, g.out_destValues(), nvals * sizeof(SpB_Index));
    row_indices = new SpB_Index[nvals];
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
            values[index] = true;
            row_indices[index++] = i;
        }
    }
}


/**
 * @brief Read the matrix data from the.sg/.wsg file and store it in three arrays in COO format. The elements are arranged in col-first order (order in CSC format) .
 *
 * @tparam T The datatype of values of Matrix.
 * @param[in] full_path The full path of the file to be read.
 * @param[out] values The values array of COO.
 * @param[out] row_indices The row_indices array of COO.
 * @param[out] col_indices The col_indices array of COO.
 * @param[out] nrows The number of rows.
 * @param[out] ncols The number of cols.
 * @param[out] nvals The number of non-zero values.
 */
template <typename T>
void Read_CSC_Matrix(
    const std::string full_path,
    T *&values,
    SpB_Index *&row_indices,
    SpB_Index *&col_indices,
    SpB_Index &nrows,
    SpB_Index &ncols,
    SpB_Index &nvals)
{
    // Read input graph from a .sg/.wsg file.
    Builder<SpB_Index, SpB_Index> builder(full_path);
    CSRGraph<SpB_Index, SpB_Index> g = builder.MakeGraph();

    nrows = static_cast<SpB_Index>(g.num_nodes());
    ncols = static_cast<SpB_Index>(g.num_nodes());
    nvals = static_cast<SpB_Index>(g.num_edges());

    CHECK_NULLPTR(values);
    CHECK_NULLPTR(row_indices);
    CHECK_NULLPTR(col_indices);

    values = new T[nvals];
    col_indices = new SpB_Index[nvals];
    memcpy(col_indices, g.in_destValues(), nvals * sizeof(SpB_Index));
    row_indices = new SpB_Index[nvals];
    SpB_Index *offsets = g.in_offsets();
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
            values[index] = true;
            row_indices[index++] = i;
        }
    }
}