#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "randinrange.h"

/*
int main()
{
    int min_num = 0;
    int max_num = 5;
    int rand_num;

    rand_init();

    for (int i = 0; i < 1000000; i++)
    {
        rand_num = rand_in_range(min_num, max_num);
        if (rand_num < min_num || rand_num > max_num)
        {
            printf("Вышли за границу интервала случайных чисел [%d, %d]\n", min_num,
                    max_num);
            exit(EXIT_FAILURE);
        }

        if (min_num == rand_num || max_num == rand_num)
        {
            printf("Граница интервала [%d, %d], rand_num = %d\n", min_num, max_num,
                    rand_num);
        }
        else
        {
            printf("rand_num = %d\n", rand_num);
        }
    }

    return 0;
}
*/

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
