/*----------------------------------------------------------------------------------
 * File:        Lab02_BONUS.c
 * Description: This program performs the matrix multiplication on two 8x8
 * matrices with all values 1 for matrix A and 2 for matrix B
 * Board:       5529
 * Input:       matrix a and matrix b
 * Output:      the result of the multiplication of a and b
 * Author:      Gianna Foti, ghf0004@uah.edu
 * Date:        September 3, 2024
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <msp430.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>


int main(void)
{
    // stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    int i;
    int k;
    int p;

    // declared a with all values set to 1 as prompted
    int A[8][8] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1}
    };

// Calculating size of A
    //row a
    const int sizeRowA = sizeof(A) / sizeof(A[0]);
    //column a
    const int sizeColumnA = sizeof(A[0]) / sizeof(A[0][0]);

    //Printing matrix A
    printf("Matrix A:\n");
    for (i = 0; i < sizeRowA; i++)
    {
        for (k= 0; k< sizeColumnA; k++)
            {
                printf("%d ", A[i][k]);
            }
            printf("\n");
    }
    printf("\n");

    //PRINT_ARRAY(sizeRowA, sizeColumnA, A); // FUNCTION CALL FOR PRINT_ARRAY

    int B[8][8] = // DECLARE ARRAY B
    {
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2}
    };

// Calculating size of B
    //row b
    const int sizeRowB = sizeof(B) / sizeof(B[0]);
    const int sizeColumnB = sizeof(B[0]) / sizeof(B[0][0]);

    printf("Matrix B:\n");

    //Printing matrix B
    for (i = 0; i < sizeRowB; i++)
    {
        for (k= 0; k< sizeColumnB; k++)
            {
                printf("%d ", B[i][k]);
            }
            printf("\n");
    }
    printf("\n");

    // declaring the z result matrix
    int zresult[8][8] = {0};

    // matrix multiplication and stored in z
    for(i = 0; i < sizeRowA; i++)
    {
        for (k= 0; k< sizeColumnB; k++)
        {
            zresult[i][k] = 0;
            for(p = 0; p < sizeColumnA; p++)
            {
                zresult[i][k] += A[i][p] * B[p][k];
            }
        }
    }


// Size calculation for zS
    //rows of z
    const int sizeRowZ = sizeof(zresult) / sizeof(zresult[0]);
    //columns of z
    const int sizeColumnZ = sizeof(zresult[0]) / sizeof(zresult[0][0]);
    printf("Matrix C:\n");

    //printing C array
    for (i = 0; i < sizeRowZ; i++)
    {
        for (k= 0; k < sizeColumnZ; k++)
            {
                printf("%d ", zresult[i][k]);
            }
            printf("\n");
    }
    printf("\n");

    return 0;
}


