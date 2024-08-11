#include <stdio.h>

#include "log.h"

int main()
{
    LOG_INIT(LOG_MODE_STDOUT, LOG_LEVEL_DEBUG, NULL);
    LOG_DEBUG("main start ...\n");


    return 0;
}
