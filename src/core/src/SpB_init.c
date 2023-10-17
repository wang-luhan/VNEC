#include "SpB_Global.h"

void SpB_init()
{
    SpB_Warning_switch_set_ext(true);
    SpB_NNZ_Counting_Mes_malloc();
    // SpB_LinkedList_malloc();
    return;
}