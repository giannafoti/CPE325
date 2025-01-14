/*----------------------------------------------------------------------------------
 * File:        Lab01_S2.c
 * Description: This program finds the prime factorization of a hard coded value
 * Board:       5529
 * Input:       Hardcoded short int number
 * Output:      Factors of input value printed to console (ordered low to high)
 * Author:      Gianna Foti, ghf0004@uah.edu
 * Date:        May 13, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h> 
#include <stdio.h>

#define     true    1
#define     false   0


// This function finds all of the factors in `value`
// .. Factors of `value` are output as elements of `factors`
// .. Function return value is number of factors found (0xFFFF for error)
int get_prime_factors(int *factors, int value)
{
    //Had to uncomment this line out
    int output_factor_num = 0;
    int factor;
    char factor_found;
    
    if (value < 0)
        return 0xFFFF;
    
    // Loop while remaining value is not prime
    // changed to value < 1, the while it was not true was not working
    while (value > 1)
    {
        factor_found = false;
        factor = 2;
        
        // Get lowest remaining factor of `value`
        while (factor <= value)
        {
            //changed from 1 to 0
            if (value % factor == 0)
            {  // Is `factor` a factor of `value`?
                // Factor found
                factor_found = true;
                break;
            }
            
            // Factor not found
            factor++;
        }
        
        if (!factor_found)
            break;
        
        // Add factor to array, and divide out of working value
        factors[output_factor_num] = factor;
        output_factor_num++;
        value /= factor;
    }
    
    return output_factor_num;
}


int main(void)
{
	// stop watchdog timer
    //this ends the Watchdog Timer, had to add
    WDTCTL = WDTPW + WDTHOLD;
    
    // Input value for factorization
    const int INPUT_VALUE = 126;
    
    // Output array
    // Array must have at least as many elements as factors of INPUT_VALUE
    int prime_factors[10] = {};
    
    int num_factors;
    int i;
    
    // Calculate prime factors, and check for function error
    num_factors = get_prime_factors(prime_factors, INPUT_VALUE);
    if (num_factors == 0xFFFF)
        return 1;
    
    // Print input value & output prime factors separated by spaces
    printf("Input value: \n%d\n", INPUT_VALUE);
    printf("Output factors: \n");
    for (i = 0; i < num_factors; i++)
    {
        printf("%d ", prime_factors[i]);
    }

    printf("\n");
    fflush(stdout);
    
	return 0;

}
