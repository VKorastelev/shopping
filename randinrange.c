#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "randinrange.h"

void rand_init()
{
    struct timeval strtime;
    gettimeofday(&strtime, NULL);
    srand(strtime.tv_usec);
}

int rand_in_range(int const min, int const max)
{
    return rand() % (max - min + 1) + min;
}
