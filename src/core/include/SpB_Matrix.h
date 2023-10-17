#pragma once
#include "OpenSparseBLAS.h"
#include <stdlib.h>

/**
 * @ingroup SpB_Matrix
 * @brief A structure to represent matirixs
 * @details This class stores basic information about a matrix, which is inherited by other concrete matrix classes of various formats.
 */
struct SpB_Matrix_opaque
{
    SpB_Type element_type; /**< Element data type */
    SpB_Matrix_Format_Value_ext format_type; /**<  Marker of matrix format */
    SpB_Index row; /**< Number of rows*/
    SpB_Index col; /**< Number of rows */
    size_t element_size; /**< byte number of a single element.*/
};
