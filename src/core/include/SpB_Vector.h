#pragma once
#include "OpenSparseBLAS.h"
#include <stdlib.h>

/**
 * @ingroup SpB_Vector
 * @brief A structure to represent vectors. 
 * @details This class stores basic information about a vector, which is inherited by other concrete vector classes of various formats.
 */
struct SpB_Vector_opaque
{
    SpB_Type element_type;               /**< Element data type */
    SpB_Vector_Format_Value_ext format_type; /**<  Marker of vector format */
    SpB_Index n;                         /**< Vector length.*/
    size_t element_size; /**< byte number of a single element.*/
};