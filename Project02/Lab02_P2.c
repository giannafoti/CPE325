/*----------------------------------------------------------------------------------
 * File:        Lab02_P2.c
 * Description:  A C program that declares and initializes two integer arrays x and y
 *               of at least 5 elements. It computes a float array z where each element
 *               of z is the average of corresponding elements in x and y.
 * Board:       5529
 * Input:       Two integer arrays x and y of at least 5 elements.
 * Output:      The float array z, with each element being the average of corresponding
 *               elements in x and y.
 * Author:      Gianna Foti, ghf0004@uah.edu
 * Date:        September 3, 2024
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <msp430.h>

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    // Declare and initialize arrays x and y
    int x[] = {1, 2, 3, 4, 5, 6};
    int y[] = {7, 8, 9, 10, 11, 12};


    // Calculate the number of elements in the arrays
    int size = sizeof(x) / sizeof(x[0]);

    // Ensure we have at least 5 elements
    if (size < 5)
    {
        printf("Error: Arrays must have at least 5 elements.\n");
        return 1;
    }

    // Declare array z to store the averages, hardcoded it to 6
    float z_result[6];

    // Compute averages and store in z
    int i;
    for (i = 0; i < size; i++)
    {
        z_result[i] = ((float)x[i] + (float)y[i]) / 2.0f;
    }

    // Print input arrays
    printf("Input Array x: [");
    int j;
    for ( j = 0; j < size; j++)
    {
        printf("%d", x[j]);
        if (j < size - 1)
            printf(" ");
    }
    printf("]\n");

    printf("Input Array y: [");
    int k;
    for (k = 0; k < size; k++)
    {
        printf("%d", y[k]);
        if (k < size - 1)
            printf(" ");
    }
    printf("]\n");

    // Print output array
    printf("Output Array z: [");
    int l;
    for (l = 0; l < size; l++)
    {
        printf("%.2f", z_result[l]);
        if (l < size - 1)
            printf(" ");
    }
    printf("]\n");

    return 0;
}
