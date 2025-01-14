/*-------------------------------------------------------------------------
* File: Lab01_P1.c
* Function: Implements a Caesar cipher to encrypt a string
* Description:
* Input: any string of characters
* Output: the shifted ASCII value of alphabetical characters right by 3
* Author(s): Gianna Foti
* Date: 29 August 2024
*-----------------------------------------------------------------------*/

//include statements
#include <msp430.h> 
#include <stdio.h>
#include <string.h>

//main function
int main()
{
    //this ends the Watchdog Timer
    WDTCTL = WDTPW + WDTHOLD;

    //hard coded string
    char msg[]="Hello all, welcome to CPE325 Fall 2024!";

    //setting the index to 0
    int index = 0;

    //the size of the message
    int size = strlen(msg);

    //while loop
    while (index < size)
    {
        //element in the array
        char x = msg[index];
        //we use 90, 65, 97, and 122 due to the ASCII characters
        //90 is z, 65 is A, 97 is a, 122 is z
        //checking to see if the character is upper or lower case
        if ((x<=90 && x>= 65) || (x>=97 && x<=122))
        {
            //shifting to the right by 3
            msg[index] = x + 3;
        }
            //incrementing the index after processing
            index++;
    }

    //this line was given to us to print a single line
    printf("%s\n", msg);

    return 0;
}
