/*
 *
 *   Author: Suman Shrestha
 *
 *   gaussianBlur.c
 *
 *   This file includes:
 *
 *   -> void *gaussianBlur(void *ptr)
 *       # Thread function to apply gaussian blur
 *       # Takes a portion of the image, applies the gaussian blur algorithm,
 *         and stores the result in a temporary buffer
 *       # Copies the blurred portion back to the original image
 *
 *   -> int main()
 *          -> lodepng_decode32_file()
 *              # Decodes the input image
 *
 *          -> image row / pthread
 *              # Divides the image into equal portions for each thread
 *
 *          -> pthread_create()
 *              # Creates threads to process each portion of the image
 *
 *          -> pthread_join()
 *              # Waits for all threads to finish
 *
 *          -> lodepng_encode32_file()
 *              # Encodes the resulting image after blurring
 *
 *          -> free()
 *              # Cleans up dynamically allocated memory
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lodepng/lodepng.h"

// Struct for passing data to thread function
typedef struct
{
    unsigned char *image;
    unsigned char *blurredImage;
    int start_h;
    int end_h;
    int h;
    int w;
} imageinfo;

// Thread function to apply gaussian  blur
void *gaussianBlur(void *ptr)
{
    imageinfo *p = (imageinfo *)ptr;

    int h = p->h;
    int w = p->w;
    int start_h = p->start_h;
    int end_h = p->end_h;

    unsigned char *blurredImageThread = (unsigned char *)malloc(h * w * 4 * sizeof(unsigned char));
    // 4 channels (RGBA)
    if (blurredImageThread == NULL)
    {
        printf("Memory allocation failed!\n");
        pthread_exit(NULL);
    }

    // Apply gaussina blur
    for (int i = start_h; i <= end_h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int red = 0, green = 0, blue = 0, alpha = 0, num_of_pixels = 0;

            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    int x_pixel = i + k;
                    int y_pixel = j + l;

                    // ensure that the image is within the bounds
                    if (x_pixel >= 0 && y_pixel >= 0 && x_pixel < h && y_pixel < w)
                    {
                        int pixelIndex = (x_pixel * w + y_pixel) * 4;
                        red += p->image[pixelIndex];
                        green += p->image[pixelIndex + 1];
                        blue += p->image[pixelIndex + 2];
                        alpha += p->image[pixelIndex + 3];
                        num_of_pixels++;
                    }
                }
            }

            int pixelIndex = (i * w + j) * 4;
            blurredImageThread[pixelIndex] = red / num_of_pixels;
            blurredImageThread[pixelIndex + 1] = green / num_of_pixels;
            blurredImageThread[pixelIndex + 2] = blue / num_of_pixels;
            blurredImageThread[pixelIndex + 3] = alpha / num_of_pixels;
        }
    }

    // Copy blurred portion back to original image
    for (int i = start_h; i <= end_h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int pixelIndex = (i * w + j) * 4; // 4 channels (RGBA)
            p->blurredImage[pixelIndex] = blurredImageThread[pixelIndex];
            p->blurredImage[pixelIndex + 1] = blurredImageThread[pixelIndex + 1];
            p->blurredImage[pixelIndex + 2] = blurredImageThread[pixelIndex + 2];
            p->blurredImage[pixelIndex + 3] = blurredImageThread[pixelIndex + 3];
        }
    }

    // Free the temporary allocated memory
    free(blurredImageThread);

    pthread_exit(NULL);
}

int main()
{
    // Decode the image
    unsigned char *image, *blurredImage;
    int error, height, width;
    error = lodepng_decode32_file(&image, &width, &height, "./img/meow.png");

    // Check for decoding errors
    if (error)
    {
        printf("%s\n", lodepng_error_text(error));
        return 1;
    }

    printf("Image decoded successfully!\n");
    printf("Resolution: %d X %d\n", width, height);

    // Allocate memory for the result image
    blurredImage = (unsigned char *)malloc(height * width * 4 * sizeof(unsigned char)); // 4 channels (RGBA)
    if (blurredImage == NULL)
    {
        printf("Memory allocation failed!\n");
        free(image);
        return 1;
    }

    // Allocate memory for array of pthreads and struct array
    pthread_t pArr[8];
    imageinfo hrArr[8];

    // Divide image into equal portions for each thread
    int rowsPerThread = height / 8;
    for (int i = 0; i < 8; i++)
    {
        hrArr[i].start_h = i * rowsPerThread;
        hrArr[i].end_h = (i == 7) ? height - 1 : (i + 1) * rowsPerThread - 1;
        hrArr[i].image = image;
        hrArr[i].blurredImage = blurredImage;
        hrArr[i].h = height;
        hrArr[i].w = width;

        // Create thread
        if (pthread_create(&pArr[i], NULL, gaussianBlur, &hrArr[i]) != 0)
        {
            printf("Error creating thread %d\n", i);
            free(image);
            free(blurredImage);
            return 1;
        }
    }

    printf("Applying blue effect!\n");
    // Wait for all threads to finish
    for (int i = 0; i < 8; i++)
    {
        pthread_join(pArr[i], NULL);
    }

    // Encode the result image
    error = lodepng_encode32_file("./img/OutputMeowBlur.png", blurredImage, width, height);
    if (error)
    {
        printf("%s\n", lodepng_error_text(error));
        free(image);
        free(blurredImage);
        return 1;
    }

    printf("Successfully applied blur effect\nThe image has been encoded as OutputMeowBlur.png");

    // Clean up dynamically allocated memory
    free(image);
    free(blurredImage);

    return 0;
}
