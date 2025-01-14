/*------------------------------------------------------------------------------
File:         Lab10_P3.c
Description:  This program generates waveforms using PWM on the MSP430. The
              default waveform is a sine wave generated at 30 Hz using a
              lookup table. The functionality includes:
              - Generating a sine waveform when no switches are pressed.
              - Generating a sawtooth waveform on the fly when SW1 is held.
              - Tripling the waveform frequency to 90 Hz when SW2 is held.
              The output is routed through the buzzer on P3.4, with visual
              indicators using the RED LED on P1.0 (for sound state) and
              potentially the GREEN LED on P4.7 (not implemented in this version).
Input:        P2.1 - SW1 (Hold for Sawtooth waveform) | P1.1 - SW2 (Hold to
              triple frequency)
Output:       P1.0 - RED LED (indicates sound on) | P4.7 - GREEN LED
              (not used in this implementation) | P3.4 - BUZZER (PWM output)
Author:       Gianna Foti
------------------------------------------------------------------------------*/



#include <msp430.h>             // Include MSP430-specific header
#include "sine_lut_256.h"      // Include the sine lookup table header

#define Switch1 (P2IN & BIT1)  // Define macro for reading Switch 1
#define Switch2 (P1IN & BIT1)  // Define macro for reading Switch 2

unsigned char index = 0;       // Variable to keep track of the sine wave index

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;  // Stop the watchdog timer to prevent resets

    // Timer B Configuration
    TB0CCTL0 = CCIE;            // Enable Timer B interrupt
    TB0CTL = TBSSEL_2 + MC_1;   // Use SMCLK as the timer source, set to up mode
    TB0CCR0 = 135;              // Set CCR0 for the desired frequency (2kHz)

    P3DIR = 0xFF;               // Set all pins on Port 3 as outputs
    P3OUT = 0;                  // Initialize Port 3 output to 0
    P1DIR = BIT0;               // Set P1.0 as output for the RED LED

    buttons();                  // Call the button configuration function

    __bis_SR_register(LPM0_bits + GIE); // Enter low power mode with global interrupts enabled
    return 0;                  // Return from main (never reached in LPM)
}

// Function to configure switches
void buttons()
{
    // Configure Switch 1
    P2DIR &= ~BIT1;             // Set P2.1 as input
    P2REN |= BIT1;              // Enable pull-up/pull-down resistor on P2.1
    P2OUT |= BIT1;              // Set pull-up resistor on P2.1

    // Configure Switch 2
    P1DIR &= ~BIT1;             // Set P1.1 as input
    P1REN |= BIT1;              // Enable pull-up/pull-down resistor on P1.1
    P1OUT |= BIT1;              // Set pull-up resistor on P1.1
}

// Timer B interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void timerISR2(void)
{
    index += 1;                 // Increment the sine wave index

    // Check if Switch 2 is pressed and Switch 1 is not pressed
    if ((Switch2 == 0) && (!(Switch1 == 0)))
    {
        index += 2;             // Increase index by 2 (modify frequency)
        P3OUT = lut256[index];  // Output sine value from lookup table
    }
    // Check if Switch 1 is pressed and Switch 2 is not pressed
    else if ((Switch1 == 0) && (!(Switch2 == 0)))
    {
        P1OUT ^= BIT0;          // Toggle the RED LED
        P3OUT = index;          // Output current index value to Port 3
    }
    else
    {
        P3OUT = lut256[index];  // Default: Output sine value from lookup table
    }
}
