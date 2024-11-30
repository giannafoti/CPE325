/*
; ---------------------------------------------------------------------------------------------------------------------
; file:        Lab06_P2.c
; description: c program interfaces with switches 1 & 2, and leds 1 & 2 (red and green).
;               1. both leds are initially off.
;               2. when sw1 is pressed, decrease clock frequency by half (minimum 1 mhz).
;               3. when sw2 is pressed, set clock frequency to 8 mhz.
;               4. if neither switch is pressed, toggle both leds.
; input:       p2.1 - sw1 | p1.1 - sw2
; output:      p1.0 - red led | p4.7 - green led
; author:      gianna foti
;------------------------------------------------------------------------------------------------------------------------
*/



#include <msp430.h>

#define SWITCH1 (!(P2IN & BIT1))            // sw1 is pressed when p2.1 is 0
#define SWITCH2 (!(P1IN & BIT1))            // sw2 is pressed when p1.1 is 0
#define REDLED BIT0                         // red led connected to p1.0
#define GREENLED BIT7                       // green led connected to p4.7

void ClockyClock();              // function prototype
void ClockyClock8MHz();          // function prototype
int ClockyClock1MHz(int);        // function prototype

volatile int clockDivider = 32;  // global variable for clock division

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;               // stop watchdog timer

    // configure leds
    P1DIR |= BIT0;                          // set led1 (red led) as output
    P1OUT = 0x00;                           // clear led1 status (turn off)

    P4DIR |= BIT7;                          // set led2 (green led) as output
    P4OUT = 0x80;                           // set led2 status to 0x80 (turn off)

    // configure switches
    P1DIR &= ~BIT1;                         // set p1.1 as input for sw2
    P1REN |= BIT1;                          // enable pull-up resistor at p1.1
    P1OUT |= BIT1;                          // set pull-up for sw2 to 1

    P2DIR &= ~BIT1;                         // set p2.1 as input for sw1
    P2REN |= BIT1;                          // enable pull-up resistor at p2.1
    P2OUT |= BIT1;                          // set pull-up for sw1 to 1

    // configure interrupts for switches in C style
    P1IE |= BIT1;                           // enable interrupt for p1.1 (sw2)
    P1IES |= BIT1;                          // set interrupt to trigger on high-to-low transition
    P1IFG &= ~BIT1;                         // clear interrupt flag for p1.1

    P2IE |= BIT1;                           // enable interrupt for p2.1 (sw1)
    P2IES |= BIT1;                          // set interrupt to trigger on high-to-low transition
    P2IFG &= ~BIT1;                         // clear interrupt flag for p2.1

    __bis_SR_register(GIE);                 // enable global interrupts

    ClockyClock();  // set initial clock

    while (1)
    {
        __delay_cycles(500000); // delay for toggling leds

        // if neither switch is pressed, toggle both leds
        P1OUT ^= REDLED;     // toggle red led
        P4OUT ^= GREENLED;   // toggle green led
    }
}

void ClockyClock()
{
    UCSCTL3 = SELREF_2;         // set dco fll reference = refo
    UCSCTL4 |= SELA_2;          // set aclk = refo
    UCSCTL0 = 0x0000;           // set lowest possible dcox, modx

    // loop until xt1, xt2 & dco stabilize
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG); // clear xt2, xt1, dco fault flags
        SFRIFG1 &= ~OFIFG;      // clear oscillator fault flag
    }
    while (SFRIFG1 & OFIFG);  // test oscillator fault flag
}

void ClockyClock8MHz()
{
    __bis_SR_register(SCG0);    // disable the fll control loop
    UCSCTL1 = DCORSEL_5;        // select dco range for 8mhz operation
    UCSCTL2 = 249;              // set dco multiplier for 8mhz (249 + 1) * 32768 = 8mhz
    __bic_SR_register(SCG0);    // enable the fll control loop

    __delay_cycles(250000);     // delay for dco to settle
}

int ClockyClock1MHz(int i)
{
    __bis_SR_register(SCG0);    // disable the fll control loop
    UCSCTL1 = DCORSEL_3;        // select dco range for 1mhz operation
    if (i >= 32)
    {
        UCSCTL2 = i / 2;        // set dco multiplier for the current clock
    }
    __bic_SR_register(SCG0);    // enable the fll control loop

    __delay_cycles(33792);      // delay for dco to settle
    return (i >= 32) ? i / 2 : i;  // ensure i does not go below the threshold
}

// interrupt service routine for port 1 (sw2 pressed)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    ClockyClock8MHz();  // set clock to 8 mhz
    clockDivider = 250; // reset clock divider for further frequency changes
    P1IFG &= ~BIT1;     // clear interrupt flag for p1.1
}

// interrupt service routine for port 2 (sw1 pressed)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    clockDivider = ClockyClock1MHz(clockDivider);  // decrease clock frequency by half
    P2IFG &= ~BIT1;     // clear interrupt flag for p2.1
}
