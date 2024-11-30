/*----------------------------------------------------------------------------------
 * File: Lab03_P1.c
 * Description: This program controls two LEDs (Red and Green) on the MSP430 board based on
 * the state of two input switches (S1 and S2). The program performs the following:
 * - If Switch 1 (S1) is pressed, the Red LED turns off and the Green LED toggles at 7 Hz.
 * - If Switch 2 (S2) is pressed, the Green LED turns on and the Red LED toggles at 9 Hz.
 * - If both switches are pressed, both LEDs toggle alternately at 3 Hz.
 * - If neither switch is pressed, the Red LED remains on and the Green LED remains off.
 * Debouncing is implemented to avoid false triggering due to mechanical switch bounce.
 * Board: 5529
 * Input: S1 (P2.1), S2 (P1.1)
 * Output: Red LED (P1.0), Green LED (P4.7)
 * Author: Gianna Foti
 * Date: September 5, 2024
 *----------------------------------------------------------------------------*/

#include <msp430.h>

// S1 is pressed when P2.1 is low
#define S1_PRESSED (!(P2IN & BIT1))

// S2 is pressed when P1.1 is low
#define S2_PRESSED (!(P1IN & BIT1))

// Mask for P1.0 (Red LED)
#define REDLED BIT0

// Mask for P4.7 (Green LED)
#define GREENLED BIT7

void main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set Red LED (P1.0) as output
    P1DIR |= REDLED;
    // Set Green LED (P4.7) as output
    P4DIR |= GREENLED;

    // Start with Red LED on and Green LED off
    P1OUT |= REDLED;
    P4OUT &= ~GREENLED;

    // Configure S1 (P2.1) as input with pull-up resistor
    P2DIR &= ~BIT1;
    P2REN |= BIT1;
    P2OUT |= BIT1;

    // Configure S2 (P1.1) as input with pull-up resistor
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    //initializing unsigned int to 0
    unsigned int i = 0;

    // Infinite loop
    while (1)
    {
        // S1 pressed, S2 not pressed
        if (S1_PRESSED && !S2_PRESSED)
        {
            // Debounce delay ~20ms
            for (i = 2000; i > 0; i--);
            //if s1 is pressed
            if (S1_PRESSED)
            {
                // 7Hz delay
                __delay_cycles(71429);
                // Turn off Red LED
                P1OUT &= ~REDLED;
                // Toggle Green LED
                P4OUT ^= GREENLED;
            }
        }
        // S2 pressed and S1 not pressed
        else if (S2_PRESSED && !S1_PRESSED)
        {
            // Debounce delay ~20ms
            for (i = 2000; i > 0; i--);
            if (S2_PRESSED)
            {
                // 9Hz delay
                __delay_cycles(55556);
                // Turn on green LED
                P4OUT |= GREENLED;
                // Toggle the red LED
                P1OUT ^= REDLED;
            }
        }
        // both S1 and S2 pressed
        else if (S1_PRESSED && S2_PRESSED)
        {
            // Debounce delay ~20ms
            for (i = 2000; i > 0; i--);
            // 3Hz delay
            __delay_cycles(166667);
            // turn on red LED
            P1OUT |= REDLED;
            // turn off green LED
            P4OUT &= ~GREENLED;
            // 3Hz delay
            __delay_cycles(166667);
            // turn off red LED
            P1OUT &= ~REDLED;
            // turn on green LED
            P4OUT |= GREENLED;
        }
        // neither S1 nor S2 pressed, return to default state
        else
        {
            // red LED on
            P1OUT |= REDLED;
            // green LED off
            P4OUT &= ~GREENLED;
        }
    }
}
