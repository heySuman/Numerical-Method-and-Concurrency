/*
 *  Author: Suman Shrestha
 *
 *  leibneiz.c
 *
 *  This file includes:
 *
 *
 *  -> Struct to save the range of iteration
 *      #members:
 *          start
 *          end
 *
 *  -> void *calculatePi(void *numberRange)
 *      #find the items of series and add it to the sumOfSeries
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

double sumOfSeries = 0;
pthread_mutex_t L;
/*
    struct to save the range
*/
typedef struct range
{
    int start;
    int end;
} range;

/*
    Leibniz
*/
void *calculatePi(void *numberRange)
{
    range *r = (range *)numberRange;
    int start = r->start;
    int end = r->end;

    double out = 0;
    /*
        calculate the series based on range data
    */
    for (int i = start; i <= end; i++)
    {
        out += (double)pow(-1, i) / (2 * i + 1);
    }

    /*
        Ensure that the writing during parralelism won't cause race condition
    */
    pthread_mutex_lock(&L);
    sumOfSeries += out;
    pthread_mutex_unlock(&L);
}

int main()
{
    long int iteration;
    int threads;
    pthread_mutex_init(&L, NULL);

    time_t begin = clock();

    printf("Enter the number of iteration: ");
    scanf("%ld", &iteration);

    printf("Enter the number of threads: ");
    scanf("%d", &threads);

    // create thread
    // create range array
    pthread_t *threadsArr = (pthread_t *)malloc(threads * sizeof(pthread_t));
    range *rangeArr = (range *)malloc(threads * sizeof(range));

    for (int i = 0; i < threads; i++)
    {
        if (i == iteration - 1)
        {
            rangeArr[i].start = (int)iteration / threads * i;
            rangeArr[i].end = (int)iteration / threads * (i + 1) + iteration % threads;
        }
        else
        {
            rangeArr[i].start = (int)iteration / threads * i;
            rangeArr[i].end = (int)iteration / threads * (i + 1);
        }
    }

    /*
        pthread_create
    */
    for (int i = 0; i < threads; i++)
    {
        pthread_create(&threadsArr[i], NULL, calculatePi, &rangeArr[i]);
    }

    /*
        pthread join
    */
    for (int i = 0; i < threads; i++)
    {
        pthread_join(threadsArr[i], NULL);
    }

    // print the pi value
    printf("Pi value : %.15lf\n", 4 * sumOfSeries);
    pthread_mutex_destroy(&L);

    // free the dynamically allocated memory
    free(threadsArr);
    free(rangeArr);

    // calculate the time it took to execute the program
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Execution time: %lf", time_spent);
    return 0;
}