#include "OpenSparseBLAS.h"
#include "SpB_Global.h"
#include "SpB_Error.h"

SpB_Info SpB_Global_Option_set_ext // set a global default option
    (
        SpB_Option_Field_ext field,  // option to change
        SpB_Object_Format_ext format // value to change it to
    )
{
    switch (field)
    {
    case SpB_MATRIX_FORMAT:
    {
        if (!(format.matrix_format == SpB_CSC ||
              format.matrix_format == SpB_CSR))
        {
            return SpB_FAILURE;
        }
        SpB_Global_is_csc_set(format.matrix_format != SpB_CSR);
        break;
    }
    case SpB_VECTOR_FORMAT:
    {
        SpB_Global_Vector_Format_set(format.vector_format);
        break;
    }
    default:
        SpB_Error("Unknown global option!\n \
(Maybe it has not been implemented!)\n",
                  __FILE__, __LINE__);
        break;
    }

    return SpB_SUCCESS;
}