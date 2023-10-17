#include "OpenSparseBLAS.h"
#include "SpB_Global.h"

void SpB_finalize(){
    SpB_NNZ_Counting_Mes_free();
    SpB_LinkedList_free();
    return;
}