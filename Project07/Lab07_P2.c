/*------------------------------------------------------------------------------
File:         Lab07_P2.c
Description:  Controls a buzzer with PWM using Timer B at 2kHz and toggles the
              sound on and off every second using the Watchdog Timer.
Input:        P2.1 - SW1 (Increase) | P1.1 - SW2 (Decrease)
Output:       P1.0 - RED LED | P4.7 - GREEN LED | P7.4 - BUZZER
Author:       Gianna Foti
* ------------------------------------------------------------------------------*/

#include <msp430.h>

static int i = 0;

void main(void)
{
    // Disable Watchdog Timer to prevent system reset
    WDTCTL = WDTPW | WDTHOLD;

    // Configure Watchdog Timer to generate interrupts every 32ms
    WDTCTL = WDT_MDLY_32;    // Set Watchdog Timer mode to 32ms interval
    SFRIE1 |= WDTIE;         // Enable Watchdog Timer interrupts

    // Configure P7.4 as output for the buzzer
    P7DIR |= BIT4;           // Set P7.4 as output pin
    P7SEL |= BIT4;           // Select peripheral function (Timer B) for P7.4

    // Set up Timer B to generate a 2kHz PWM signal on P7.4
    TB0CCTL2 = OUTMOD_7;     // Set output mode to Reset/Set (PWM mode)
    TB0CTL = TBSSEL_2 | MC_1; // Use SMCLK as the clock source, set Timer B in up mode
    TB0CCR0 = 250 - 1;       // Set Timer B period to achieve 2kHz (assuming SMCLK is 1MHz)
    TB0CCR2 = 125;           // Set duty cycle to 50% for 2kHz signal

    // Enter Low Power Mode 0 (LPM0) with global interrupts enabled
    __bis_SR_register(LPM0_bits + GIE);
}

// Watchdog Timer interrupt service routine (ISR)
#pragma vector = WDT_VECTOR
__interrupt void WATCHDOG_TIMER(void)
{
    i++;                      // Increment counter every 32ms
    if (i == 32)              // After 32 interrupts (~1 second has passed)
    {
        TB0CCTL2 ^= OUTMOD_7; // Toggle the buzzer on/off by modifying output mode
        i = 0;                // Reset counter after toggling
    }
}
