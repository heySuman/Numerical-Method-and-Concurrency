/*
 *
 *   Author: Suman Shrestha
 *
 *   linearRegression.c
 *
 *   this file includes:
 *
 *   -> void findTotalNumberOfData(char *filename, FILE *fp, int *totalNumOfData)
 *       # to find the total data of (x,y) from the given datasets
 *
 *   -> convertDatasetIntoArray(FILE *data, int *xCoordinates, int *yCoordinates, int totalNumOfData)
 *       # to convert the file into the array of x and y coordinates
 *
 *   -> findGradientIntercept(int *xCoordinates, int *yCoordinates, int totalNumOfData)
 *       # to calculate the gradient and intercept from the resultant arrays of (x,y)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float gradient, intercept;

// to convert the data set into array
void convertDatasetIntoArray(FILE *data, int *xCoordinates, int *yCoordinates, int totalNumOfData)
{
    int x, y;
    for (int i = 0; i < totalNumOfData; i++)
    {
        fscanf(data, "%d,%d\n", &x, &y);
        *(xCoordinates + i) = x;
        *(yCoordinates + i) = y;
    }
}

// to find the gradient and intercept of the given dataset
void findGradientIntercept(int *xCoordinates, int *yCoordinates, int totalNumOfData)
{
    float sumX = 0, sumY = 0, sqSumX = 0, sumXY = 0;

    for (int i = 0; i < totalNumOfData; i++)
    {
        sumX += xCoordinates[i];
        sumY += yCoordinates[i];
        sqSumX += xCoordinates[i] * xCoordinates[i];
        sumXY += xCoordinates[i] * yCoordinates[i];
    }

    // print the summation of X,Y,X square, XY
    printf("summation of x: %.2f\n", sumX);
    printf("summation of y: %.2f\n", sumY);
    printf("summation of x squared: %.2f\n", sqSumX);
    printf("summation of xy: %.2f\n", sumXY);

    // using LR formulae to find the gradient and y-intercept
    gradient = (totalNumOfData * sumXY - sumX * sumY) / (totalNumOfData * sqSumX - sumX * sumX);
    intercept = (sumY * sqSumX - sumX * sumXY) / (totalNumOfData * sqSumX - sumX * sumX);
}

// to count the total number of data present in datasets
void findTotalNumberOfData(char *filename, FILE *fp, int *totalNumOfData)
{
    int x, y;
    FILE *r = fopen(filename, "r");
    // if there is error while opening file
    if (r == NULL)
    {
        printf("Error opening the file!\n");
    }

    // count the numbers of dataset
    while (fscanf(r, "%d,%d\n", &x, &y) == 2)
    {
        fprintf(fp, "%d,%d\n", x, y);
        *totalNumOfData += 1;
    }

    fclose(r);
}

int main()
{
    FILE *fp = fopen("allfiles.txt", "a");
    int totalNumOfData = 0;

    // if the file fails to open
    if (fp == NULL)
    {
        printf("error opening the file!\n");
        return 1;
    }

    // all given files
    char files[4][50] = {
        "./Task 1 files/datasetLR1.txt",
        "./Task 1 files/datasetLR2.txt",
        "./Task 1 files/datasetLR3.txt",
        "./Task 1 files/datasetLR4.txt"};

    // call the function to find the total number of data and concat all data in a single file
    for (int i = 0; i < 4; i++)
    {
        findTotalNumberOfData(files[i], fp, &totalNumOfData);
    }

    fclose(fp);
    printf("\nThe total number of the data present in all files combined is: %d\n", totalNumOfData);

    // open the file "allfiles.txt"
    FILE *data = fopen("allfiles.txt", "r");

    // allocate the memory for the the arrays dynamically
    int *xCoordinates = (int *)malloc(totalNumOfData * sizeof(int));
    int *yCoordinates = (int *)malloc(totalNumOfData * sizeof(int));

    if (xCoordinates == NULL || yCoordinates == NULL)
    {
        printf("Error occured while allocating the memory!\n");
        return 1;
    }

    // convert the data into array
    convertDatasetIntoArray(data, xCoordinates, yCoordinates, totalNumOfData);

    // find the gradient
    findGradientIntercept(xCoordinates, yCoordinates, totalNumOfData);

    // print the equation
    printf("Function :\ny = %.2fx + %.2f\n", gradient, intercept);
    fclose(data);

    // free the dynamically allocated memory
    free(xCoordinates);
    free(yCoordinates);

    //******************* Find the output of user input based on found gradient and y-intercept ****************//

    int x;
    printf("Enter a number(x): ");
    scanf("%d", &x);

    printf("y = %.2f\n", gradient * x + intercept);
    return 0;
}