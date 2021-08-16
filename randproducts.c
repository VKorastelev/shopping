#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "randproducts.h"

/*
int main()
{
    int min_num = 900;
    int max_num = 1100;
    int rand_num;

    rand_init();

    for (int i = 0; i < 1000; i++)
    {
        rand_num = rand_products(min_num, max_num);
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
    srand(time(NULL));
}

int rand_products(int const min, int const max)
{
    return rand() % (max - min + 1) + min;
}
