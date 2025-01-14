/*------------------------------------------------------------------------------
 * File: Lab10_P1_P2.c
 * Desciption:
 * This program interfaces the MSP430 microcontroller with the ADXL335 accelerometer
 * to capture and process acceleration data along the x, y, and z axes. Real-time
 * acceleration values (in g) are sent via UART to the UAH serial app for display.
 *
 * Part 1:
 * - Samples acceleration values at a rate of 10 Hz.
 * - Converts ADC values to gravitational acceleration (g) for each axis (x, y, z).
 * - Sends the converted values to the UAH serial app, displayed as separate x, y, and z readings.
 *
 * Part 2:
 * - Implements crash detection by calculating the total acceleration magnitude from all axes.
 * - If the calculated magnitude reaches or exceeds 2g, the RED LED is turned on to indicate
 *   a potential airbag deployment.
 * - This part demonstrates threshold-based signaling for safety applications.
 *
 * Input: ACCELEROMETER | ANALOG SIGNAL
 * Output: Xg, Yg, Zg TO UAH SERIAL APP & RED LED ON WHEN MAGNITUDE REACHES 2g | DIGITAL SIGNAL
 * Author: Gianna Foti
 *------------------------------------------------------------------------------*/

#include <msp430.h>
#include <math.h>

volatile long int ADCXval, ADCYval, ADCZval;
volatile float Xper, Yper, Zper;
volatile float magyMagnitude;

void TimerA_setup(void)
{
    TA0CCTL0 = CCIE;                     // Enabled interrupt
    TA0CCR0 = 3276;                      // 3277 / 32768 = .1s for ACLK
    TA0CTL = TASSEL_1 + MC_1;            // ACLK, up mode
    P1DIR |= BIT0;                        //blink bro
}



void ADC_setup(void) {
    int i =0;

    P6DIR &= ~(BIT3 + BIT4 + BIT5);             // Configure P6.3, P6.4 as input pins
    P6SEL |= BIT3 + BIT4 + BIT5;               // Configure P6.3, P6.4 as analog pins
    // configure ADC converter
    ADC12CTL0 = ADC12ON + ADC12SHT0_6 + ADC12MSC;
    ADC12CTL1 = ADC12SHP + ADC12CONSEQ_3;         // Use sample timer, single sequence
    ADC12MCTL0 = ADC12INCH_3;                // ADC A3 pin -X-axis
    ADC12MCTL1 = ADC12INCH_4;           // ADC A4 pin - Y-axis
    ADC12MCTL2 = ADC12INCH_5 + ADC12EOS;

                                        // EOS - End of Sequence for Conversions
    ADC12IE = ADC12IE0;                    // Enable ADC12IFG.1
    for (i = 0; i < 0x3600; i++);       // Delay for reference start-up
    ADC12CTL0 |= ADC12ENC;                   // Enable conversions
}



void UART_putCharacter(char c) {
    while (!(UCA0IFG&UCTXIFG));    // Wait for previous character to transmit
    UCA0TXBUF = c;                  // Put character into tx buffer
}

void UART_setup(void) {
    P3SEL |= BIT3 + BIT4;               // Set up Rx and Tx bits
    UCA0CTL0 = 0;                       // Set up default RS-232 protocol
    UCA0CTL1 |= BIT0 + UCSSEL_2;        // Disable device, set clock
    UCA0BR0 = 27;                       // 1048576 Hz / 38400
    UCA0BR1 = 0;
    UCA0MCTL = 0x94;
    UCA0CTL1 &= ~BIT0;                  // Start UART device
}

#define ADC_TO_G(n) ((3.0 * n / 4095 - 1.5) / 0.3) //convert to g
void sendData(void)
{
    int i;

    // Part 1 - get samples from ADC
    Xper = (ADC_TO_G(ADCXval));    // Calculate percentage outputs
    Yper = (ADC_TO_G(ADCYval));    // Calculate percentage outputs
    Zper = (ADC_TO_G(ADCZval));    // Calculate percentage outputs

    // Use character pointers to send one byte at a time
    char *xpointer=(char *)&Xper;
    char *ypointer=(char *)&Yper;
    char *zpointer=(char *)&Zper;

    UART_putCharacter(0x55);            // Send header
    for(i = 0; i < 4; i++)
    {            // Send x percentage - one byte at a time
        UART_putCharacter(xpointer[i]);
    }
    for(i = 0; i < 4; i++)
    {            // Send y percentage - one byte at a time
        UART_putCharacter(ypointer[i]);
    }
    for(i = 0; i < 4; i++)
    {            // Send z percentage - one byte at a time
        UART_putCharacter(zpointer[i]);
    }
}



void main(void) {
    WDTCTL = WDTPW +WDTHOLD;            // Stop WDT

    TimerA_setup();                     // Setup timer to send ADC data
    ADC_setup();                        // Setup ADC
    UART_setup();                       // Setup UART for RS-232
    _EINT();

    while (1){
        ADC12CTL0 |= ADC12SC;               // Start conversions
        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0
    }
}


#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void) {
    ADC12IFG = 0x00;
    ADCXval = ADC12MEM0;                  // Move results, IFG is cleared
    ADCYval = ADC12MEM1;
    ADCZval = ADC12MEM2;
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerA_isr() {
    magyMagnitude = sqrt((Xper * Xper) + (Yper * Yper) + (Zper * Zper));
    if (magyMagnitude > 2) // When the magnitude reaches the critical value of 2G, activate LED1; otherwise, keep LED1 turned off.
    {
        P1OUT |= BIT0;
    }
    else
    {
        P1OUT &= ~BIT0;
    }
    sendData();                           // Send data to serial app
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}
