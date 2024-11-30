/*------------------------------------------------------------------------------
File:         Lab07_P1.c
Description:  changes the brightness of led1 using timer a as the pwm signal source.
              initial brightness at 50% can be increased or decreased by buttons.
Input:        P2.1 - SW1 (Increase) | P1.1 - SW2 (Decrease)
Output:       P1.0 - RED LED | P4.7 - GREEN LED
Author:       Gianna Foti
* ------------------------------------------------------------------------------*/

#include <msp430.h>

#define BUTTON_INCREASE (P2IN & BIT1)  // STATUS OF SW1 (P2.1)
#define BUTTON_DECREASE (P1IN & BIT1)  // STATUS OF SW2 (P1.1)

// PWM BRIGHTNESS LEVELS: 0%, 12.5%, 25%, 37.5%, 50%, 62.5%, 75%, 87.5%, 100%
const unsigned int brightyBrightLevelyLevel[] = {0, 125, 250, 375, 500, 625, 750, 875, 1000};
static unsigned int currentBrightnessLevel = 4; // INITIAL BRIGHTNESS AT 50%
unsigned int debounceDelay = 0;

// determines da blinking state ya feel
static int isBlinking = 0;
static int blinkCounter = 0;
static int ledState = 1;

//lil function protypes

void updateyDateBrightyBright();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // STOP WATCHDOG TIMER

    // CONFIGURE PWM OUTPUT
    P1DIR |= BIT3;  // Set P1.2 as output
    P1SEL |= BIT3;  // Select TA0.1 for PWM output


// confingure buttons yuh
    // CONFIGURE BUTTON SW1 (Increase)
    P2DIR &= ~BIT1; // Set P2.1 as input
    P2REN |= BIT1;  // Enable pull-up/pull-down resistor
    P2OUT |= BIT1;  // Set as pull-up
    P2IE |= BIT1;   // Enable interrupt for SW1
    P2IES |= BIT1;  // Set interrupt on falling edge
    P2IFG &= ~BIT1; // Clear interrupt flag

    // CONFIGURE BUTTON SW2 (Decrease)
    P1DIR &= ~BIT1; // Set P1.1 as input
    P1REN |= BIT1;  // Enable pull-up/pull-down resistor
    P1OUT |= BIT1;  // Set as pull-up
    P1IE |= BIT1;   // Enable interrupt for SW2
    P1IES |= BIT1;  // Set interrupt on falling edge
    P1IFG &= ~BIT1; // Clear interrupt flag



    WDTCTL = WDT_MDLY_32; // Configure watchdog timer for 32 ms
    SFRIE1 |= WDTIE; // Enable watchdog timer interrupt

    // CONFIGURE TIMER A FOR PWM
   TA0CCR0 = 1000 - 1; // Set period for 1 kHz PWM frequency
   TA0CCTL2 = OUTMOD_7; // Set/reset mode
   TA0CCR2 = brightyBrightLevelyLevel[currentBrightnessLevel]; // Initialize at 50% brightness
   TA0CTL = TASSEL_2 | MC_1; // Use SMCLK, up mode


    _EINT(); // Enable global interrupts

    // Enter low power mode
    __bis_SR_register(LPM0_bits + GIE);
}


void updateyDateBrightyBright()
{
   // Update PWM duty cycle for brightness
    TA0CCR2 = brightyBrightLevelyLevel[currentBrightnessLevel];
}

// Interrupt Service Routine for SW2 (Decrease Brightness)
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    if (P1IFG & BIT1) // Check if SW2 was pressed
    {
        for(debounceDelay = 25000; debounceDelay > 0; debounceDelay--); // Debounce

        if (currentBrightnessLevel > 0) // Check if level can be decreased
        {
            currentBrightnessLevel--; // Decrease brightness level
            updateyDateBrightyBright(); // Update PWM
        }
    }
    P1IFG &= ~BIT1; // Clear interrupt flag
}

// Interrupt Service Routine for SW1 (Increase Brightness)
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if (P2IFG & BIT1) // Check if SW1 was pressed
    {
        for(debounceDelay = 25000; debounceDelay > 0; debounceDelay--); // Debounce

        if (currentBrightnessLevel < 8) // Check if level can be increased
        {
            currentBrightnessLevel++; // Increase brightness level
            updateyDateBrightyBright(); // Update PWM
        }
    }
    P2IFG &= ~BIT1; // Clear interrupt flag
}

// Watchdog Timer Interrupt Service Routine
#pragma vector = WDT_VECTOR
__interrupt void WATCHDOG_TIMER(void)
{
    if ((BUTTON_INCREASE == 0) && (BUTTON_DECREASE == 0)) // Both buttons pressed
    {
        if(isBlinking == 0) // If not already in blinking mode
        {
            isBlinking = 1; // Enable blinking
            blinkCounter = 0; // Reset counter
            ledState = 1; // Set LED state to on
        }
        blinkCounter++; // Increment blink counter

        if (blinkCounter >= 94) // Approximately every 3 seconds
        {
            ledState = !ledState; // Toggle LED state
            if (!ledState) // If LED is off
            {
                TA0CCR2 = brightyBrightLevelyLevel[currentBrightnessLevel]; // Set to current brightness
            }
            else // If LED is on
            {
                TA0CCR2 = 0; // Turn off LED
            }
            blinkCounter = 0; // Reset blink counter
        }
    }
    else // If buttons are not pressed
    {
        if(isBlinking == 1) // If in blinking mode
        {
            isBlinking = 0; // Disable blinking
            blinkCounter = 0; // Reset counter
            ledState = 1; // Turn LED back on
            TA0CCR2 = brightyBrightLevelyLevel[currentBrightnessLevel]; // Restore brightness
        }
    }
}
