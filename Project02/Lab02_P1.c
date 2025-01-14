/*----------------------------------------------------------------------------------
 * File:        Lab02_P1.c
 * Description: This program prints the sizes and ranges of common data types char, short
 * int, int, long int, long long int, unsigned char, unsigned short int, unsigned int, unsigned
 * long int, unsigned long long int, float, and double
 * Board:       5529
 * Input:       none
 * Output:      shows the date type, size in bytes, as well as the minimum and maximum
 * Author:      Gianna Foti, ghf0004@uah.edu
 * Date:        September 3, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

int main()
{

// Stop WatchDogTimer
    WDTCTL = WDTPW + WDTHOLD;
//
    int zero = 0;
    printf("-------------------------------------------------------------------------------------------------\n");
    printf("| Data Type              |    Size (in bytes)    | Minimum             | Maximum                |\n");
    printf("-------------------------------------------------------------------------------------------------\n");
    printf("| char                   | %-20zu | %-21d | %-21d |\n", sizeof(char), SCHAR_MIN, SCHAR_MAX);
    printf("| short int              | %-20zu | %-21d | %-21d |\n", sizeof(short int), SHRT_MIN, SHRT_MAX);
    printf("| int                    | %-20zu | %-21d | %-21d |\n", sizeof(int), INT_MIN, INT_MAX);
    printf("| long int               | %-20zu | %-21ld | %-21ld |\n", sizeof(long int), LONG_MIN, LONG_MAX);
    printf("| long long int          | %-20zu | %-21lld | %-21lld |\n", sizeof(long long int), LLONG_MIN, LLONG_MAX);
    printf("| unsigned char          | %-20zu | %-21u | %-21u |\n", sizeof(unsigned char), zero, UCHAR_MAX);
    printf("| unsigned short int     | %-20zu | %-21u | %-21u |\n", sizeof(unsigned short int), zero, USHRT_MAX);
    printf("| unsigned int           | %-20zu | %-21u | %-21u |\n", sizeof(unsigned int), zero, UINT_MAX);
    printf("| unsigned long int      | %-20zu | %-21u | %-21lu |\n", sizeof(unsigned long int), zero, ULONG_MAX);
    printf("| unsigned long long int | %-20zu | %-21u | %-21llu |\n", sizeof(unsigned long long int), zero, ULLONG_MAX);
    printf("| float                  | %-20zu | %-21e | %-21e |\n", sizeof(float), FLT_MIN, FLT_MAX);
    printf("| double                 | %-20zu | %-21e | %-21e |\n", sizeof(double), DBL_MIN, DBL_MAX);
    printf("-------------------------------------------------------------------------------------------------\n");
    
    return 0;
}
