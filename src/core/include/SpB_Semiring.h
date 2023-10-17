#include "OpenSparseBLAS.h"

struct GB_Semiring_opaque   // content of SpB_Semiring
{
    SpB_Monoid add ;        // add operator of the semiring
    SpB_BinaryOp multiply ; // multiply operator of the semiring
} ;