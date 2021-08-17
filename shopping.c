#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include "randinrange.h"

#define NUM_SHOPS 5
#define MIN_NUM_PRODUCTS 900
#define MAX_NUM_PRODUCTS 1100

#define NUM_SUPPLIERS 1
#define NUM_BUYERS 3
#define MAX_NUM_PURCHASED_PRODUCT 10000

static void *thf_buyer(void *args);
static void *thf_supplier(void *args);

/* Магазин с одной полкой
   shop_num - номер магазина
   num_of_product - количество продуката на полке 
   mutex_shop_state - mutex, идет продажа c полки или выгрузка товара на полку
*/
typedef struct shop_info
{
    int shop_num;
    int num_of_product;
    pthread_mutex_t mutex_shop_state;
} shop_info;

/* Поставщик товара
   supplier_num - номер поставщика
   tid_supplier - id потока поставщика
*/
typedef struct supplier_info
{
    int supplier_num;
    pthread_t tid_supplier;
} supplier_info;

/* Покуптель
   buyer_num - номер покупателя
   buyer_tid - id потока покупателя
   num_purchased_product - количество купленного товара
*/
typedef struct buyer_info
{
    int buyer_num;
    pthread_t tid_buyer;
    int num_purchased_product;
} buyer_info;

// Массивы
struct shop_info shops[NUM_SHOPS];
struct supplier_info suppliers[NUM_SUPPLIERS];
struct buyer_info buyers[NUM_BUYERS];

int main(void)
{
    int i = 0;
    int ret = 0;
    void *res = NULL;

    rand_init();

    for (i = 0; i < NUM_SHOPS; i++)
    {
        shops[i].shop_num = i + 1;
        shops[i].num_of_product = rand_in_range(MIN_NUM_PRODUCTS, MAX_NUM_PRODUCTS);
        //shops[i].mutex_shop_state = PTHREAD_MUTEX_INITIALIZER;

        ret = pthread_mutex_init(&shops[i].mutex_shop_state, NULL);
        if (0 != ret)
        {
            errno = ret;
            perror("Error pthread_mutex_init(...) for shop");
            exit(EXIT_FAILURE);
        }

        printf("Num product = %d\n", shops[i].num_of_product);
    }

    for (i = 0; i < NUM_SUPPLIERS; i++)
    {
        suppliers[i].supplier_num = i + 1;
        ret = pthread_create(&suppliers[i].tid_supplier, NULL, &thf_supplier,
                &suppliers[i]); 
        if (0 != ret) 
        {
            errno = ret; 
            perror("Error pthread_create(...) for supplier"); 
            exit(EXIT_FAILURE); 
        }
    }

    for (i = 0; i < NUM_BUYERS; i++)
    {
        buyers[i].buyer_num = i + 1;
        ret = pthread_create(&buyers[i].tid_buyer, NULL, &thf_buyer, &buyers[i]); 
        if (0 != ret) 
        {
            errno = ret; 
            perror("Error pthread_create(...) for buyer"); 
            exit(EXIT_FAILURE); 
        }
    }

    for (i = 0; i < NUM_BUYERS; i++)
    {
        ret = pthread_join(buyers[i].tid_buyer, NULL); 
        if (0 != ret) 
        {
            errno = ret; 
            perror("Error pthread_join(...) for buyer"); 
            exit(EXIT_FAILURE); 
        }
        printf("Buyer N %d joined. Num of purchased product items = %d\n",
                buyers[i].buyer_num,
                buyers[i]. num_purchased_product);
    }

    for (i = 0; i < NUM_SUPPLIERS; i++)
    {
        ret = pthread_cancel(suppliers[i].tid_supplier);
        if (0 != ret)
        {
            errno = ret;
            perror("Error pthread_cansel(...) for supplier");
            exit(EXIT_FAILURE);
        }

        ret = pthread_join(suppliers[i].tid_supplier, &res);
        if (0 != ret)
        {
            errno = ret;
            perror("Error pthread_join(...) for supplier");
            exit(EXIT_FAILURE);
        }

        if (PTHREAD_CANCELED == res)
        {
            printf("OK! Supplier N %d was canselled\n", suppliers[i].supplier_num);
        }
        else
        {
            printf("WARNING! Supplier N %d was not canselled\n",
                    suppliers[i].supplier_num);
        }
    }

    //printf("PTHREAD_CANCELED = %p\n", PTHREAD_CANCELED);

    for (i = 0; i < NUM_SHOPS; i++)
    {
        ret = pthread_mutex_destroy(&shops[i].mutex_shop_state);
        if (0 != ret)
        {
            errno = ret;
            perror("Error pthread_mutex_destroy(...) for shop");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}


static void *thf_supplier(void *args)
{
    int i;
    struct supplier_info *sinfo = args;

    while (1)
    {
        i = rand_in_range(0, NUM_SHOPS - 1);
        
        if (0 == pthread_mutex_trylock(&shops[i].mutex_shop_state))
        {
            shops[i].num_of_product += 200;

            printf("\n\033[4m***Supplier N %d added 200 items to the shop N %d, num"
                    " of product items = %d\n\033[0m",
                    sinfo->supplier_num,
                    shops[i].shop_num,
                    shops[i].num_of_product);
                
            pthread_mutex_unlock(&shops[i].mutex_shop_state);
                
            sleep(1);
        }
        pthread_testcancel();
    }

    return NULL;
}


static void *thf_buyer(void *args)
{
    int i;
    struct buyer_info *binfo = args;
    bool purch_ok = false;

    while(1)
    {
        i = rand_in_range(0, NUM_SHOPS - 1);
        
        if (0 == pthread_mutex_trylock(&shops[i].mutex_shop_state))
        {
            if (shops[i].num_of_product > 0)
            {
                binfo->num_purchased_product += shops[i].num_of_product;
                purch_ok = true;

                printf("Buyer N %d purchased %d items in the shop N %d, num of"
                        " purchased product = %d\n",
                        binfo->buyer_num, 
                        shops[i].num_of_product,
                        shops[i].shop_num,
                        binfo->num_purchased_product);
                shops[i].num_of_product = 0;
            }
            else
            {
                purch_ok = false;
            }

            pthread_mutex_unlock(&shops[i].mutex_shop_state);

            if (true == purch_ok)
            {
                if (binfo->num_purchased_product < MAX_NUM_PURCHASED_PRODUCT)
                {
                    sleep(2);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return NULL;
}
