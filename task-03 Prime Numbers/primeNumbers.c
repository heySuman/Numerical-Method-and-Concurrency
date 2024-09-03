/*
 *
 *   Author: Suman Shrestha
 *
 *   primeNumbers.c
 *
 *   this file includes:
 *
 *   -> void concatFiles(char *filename, FILE *fp, int *countOfNumbers)
 *       # to find the total numbers of data present in the given datasets
 *       # to make a new combined file
 *
 *   -> void intoArray(char *filename, int *arr)
 *       # to convert the file into the array of numbers
 *
 *   -> void *primeNumber(void *nr)
 *       # to find the prime numbers present in the files
 *       # to save the found prime numbers in a seperate file
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

// global variables
int *arr, countOfPrime = 0;
pthread_mutex_t mutex;
FILE *prime;

// struct to save the range data
typedef struct numberRange
{
    int start;
    int end;
} numberRange;

/*
    make a combined file that includes all files and
    find the number of the data present in all files combined
 */

void concatFiles(char *filename, FILE *fp, int *countOfNumbers)
{
    int localCount = 0;
    int num;
    FILE *read = fopen(filename, "r");
    if (fp == read)
    {
        printf("Error encountered during the opening of the file\n");
    }

    while (fscanf(read, "%d\n", &num) == 1)
    {
        fprintf(fp, "%d\n", num);
        localCount += 1;
    }
    *countOfNumbers += localCount;
    fclose(read);
}

/*
    convert the combined files into the array of numbers
*/
void intoArray(char *filename, int *arr)
{
    FILE *read = fopen(filename, "r");
    int n;
    int i = 0;
    while (fscanf(read, "%d\n", &n) == 1)
    {
        // add the numbers into the array
        *(arr + i) = n;
        i++;
    }
}

/*
    find the prime numbers
    save it in the new text file
*/
void *primeNumber(void *nr)
{
    int localPrimeCount = 0;
    pthread_mutex_lock(&mutex);
    /*
        destruct the struct
        save it in different variables
    */
    numberRange *temp = (numberRange *)nr;
    int start = temp->start;
    int end = temp->end;

    for (int i = start; i <= end; i++)
    {
        int flag = 1;
        /*
            if the number to be checked is not prime:
                the number will have a factor within the square root of the number
            else
                the number is prime
        */
        for (int j = 2; j <= (int)sqrt(arr[i]); j++)
        {
            if (arr[i] % j == 0)
            {
                flag = 0;
                break;
            }
        }

        // if the number is less than 1 continue
        if (arr[i] <= 1)
            continue;

        if (flag)
        {
            /*
                print the number in the file
            */
            fprintf(prime, "%d\n", arr[i]);
            /*
                increase the count of prime
            */
            localPrimeCount += 1;
            countOfPrime++;
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main()
{
    clock_t begin = clock();

    pthread_mutex_init(&mutex, NULL);
    prime = fopen("primeNumbers.txt", "w");
    int countOfNumbers = 0;
    int numberofThreads;
    int dataPerThread = 0;

    //************************************* read the files *********************************//
    FILE *fp = fopen("allNumbers.txt", "w");
    if (fp == NULL)
    {
        printf("Error encountered during the opening of the file\n");
        return 1;
    }
    else
    {
        printf("File opened successfully!\n");
    }

    // list of files
    char files[3][50] = {
        "./Task 3 files/PrimeData1.txt",
        "./Task 3 files/PrimeData2.txt",
        "./Task 3 files/PrimeData3.txt"};

    for (int i = 0; i < 3; i++)
    {
        concatFiles(files[i], fp, &countOfNumbers);
    }

    fclose(fp);

    //***************************** Convert the files Into Array ****************************//

    arr = (int *)malloc(countOfNumbers * sizeof(int));
    intoArray("allNumbers.txt", arr);

    //********************************** Multithreading *************************************//

    /*
        Ask the user for the number of threads to be used
    */
    printf("The total number of data present in the files is %d\n", countOfNumbers);
    printf("Enter the number of threads: ");
    scanf("%d", &numberofThreads);

    // array of struct to save the number ranges
    numberRange *arrOfNumberRange = (numberRange *)malloc(numberofThreads * sizeof(numberRange));

    dataPerThread = countOfNumbers / numberofThreads;
    int remainder = countOfNumbers % numberofThreads;

    //*************************************** create threads ***********************************//
    for (int i = 0; i < numberofThreads; i++)
    {
        if (i == numberofThreads - 1)
        {
            // assign the range of numbers to the thread
            arrOfNumberRange[i].start = dataPerThread * i;
            // assign remaining numbers to the last thread
            arrOfNumberRange[i].end = dataPerThread * (i + 1) + remainder;
        }
        else
        {
            // assign the range of numbers to the thread
            arrOfNumberRange[i].start = dataPerThread * i;
            // assign remaining numbers to the last thread
            arrOfNumberRange[i].end = dataPerThread * (i + 1);
        }
    }

    /*
        allocate memory for the array of threads id
    */
    pthread_t *PID = (pthread_t *)malloc(numberofThreads * sizeof(pthread_t));

    // pthread create
    for (int i = 0; i < numberofThreads; i++)
    {
        pthread_create(&PID[i], NULL, primeNumber, &arrOfNumberRange[i]);
    }

    // pthread join
    for (int i = 0; i < numberofThreads; i++)
    {
        pthread_join(PID[i], NULL);
    }

    printf("The total number of prime present in the given files is %d.\n", countOfPrime);

    /*
        close and free the allocated memory
    */
    fclose(prime);
    free(PID);
    free(arr);
    free(arrOfNumberRange);
    pthread_mutex_destroy(&mutex);

    // calculation of time comsumed during the execution of the program
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The execution time: %lf seconds\n\n", time_spent);
    return 0;
}