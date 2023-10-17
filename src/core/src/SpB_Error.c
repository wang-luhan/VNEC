#include "SpB_Error.h"
#include <stdio.h>
#include <stdlib.h>

void SpB_Error(char *errorMsg, char * filename, const int line)
{
    fprintf(stderr, "\033[31m[ FATAL ERROR ] \033[0m%s at %s:%d\n", errorMsg, filename, line);
    exit(-1);
}