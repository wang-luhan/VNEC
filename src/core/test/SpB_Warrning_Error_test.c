#include "SpB_Error.h"
#include "SpB_Warning.h"
#include "OpenSparseBLAS.h"

void run_SpB_Warning()
{
    SpB_Warning("This is a SpB_Warning test\n", __FILE__, __LINE__);
}
void run_no_SpB_Warning()
{
    SpB_Warning_switch_set_ext(false);
    SpB_Warning("This is a no SpB_Warning test\n", __FILE__, __LINE__);
}

void run_SpB_Error()
{
    SpB_Error("This is a SpB_Error test!\n", __FILE__, __LINE__);
}