/*----------------------------------------------------------------------------------------------
 * File: FINALPROJECT.c
 * Description: Uses a 115,200 baud connection to create an interactive chatbot program
 * Input: User input
 * Output: Chat output, Buzzer sound, LED blinking
 * Author: Gianna Foti
 *----------------------------------------------------------------------------------------------*/

#include <msp430.h>          // Include the MSP430 library for microcontroller functions
#include <string.h>          // Include string library for string manipulation functions

// Define the Buzzer Pin
#define BUZZER_PIN BIT4      // Buzzer is connected to P7.4

// Define Colors for UART Output (Optional)
#define colorReset   "\x1b[0m"  // Reset terminal color formatting
#define colorBot     "\x1b[31m" // Bot messages displayed in red
#define colorUser    "\x1b[35m" // User messages displayed in purple

// Morse Code Definitions
#define DOT_DURATION 1000000    // Duration of a dot in Morse code (adjusted for clock speed)
#define DASH_DURATION 3000000   // Duration of a dash in Morse code (adjusted for clock speed)
#define SYMBOL_SPACE  1000000   // Space between Morse code symbols
#define LETTER_SPACE  3000000   // Space between Morse code letters

// Function Prototypes
void UART_SETUP();              // Function to set up UART communication
void UART_sendString(char* str); // Function to send a string via UART
void UART_getLine(char* buf, int limit); // Function to receive a line of input via UART
void ConfigureTimerForBuzzer(); // Function to configure the buzzer's timer
void StartBuzzer();             // Function to start the buzzer
void StopBuzzer();              // Function to stop the buzzer
void buttonyButSetup();         // Function to configure the button (SW1)
void watchyWatchSetup();        // Function to configure the Watchdog Timer
void ConfigureTimerForLED();    // Function to configure the LED's timer
void FlashMorseCode(char* message); // Function to flash a message in Morse code

// Global Variables
volatile unsigned int beepyBeep = 0; // Counter for buzzer operation duration
volatile unsigned int debounceCounter = 0; // Counter for button debounce logic
volatile unsigned int useyUse = 1;    // Example flag variable
volatile unsigned int switchTriggered = 0; // Flag to indicate switch press

// Add other necessary global variables here
// Morse code for "HELP"
const char* morseHelp = "HELP"; // Morse code message to flash when HELP is needed

// Function Implementations
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop the watchdog timer to prevent unwanted resets
    UART_SETUP();              // Initialize UART for communication
    watchyWatchSetup();        // Set up the Watchdog Timer for periodic tasks
    buttonyButSetup();         // Configure button (SW1) with interrupts
    lightyLightSetup();        // Configure LED operation
    __enable_interrupt();      // Enable global interrupts

    char input[50];            // Buffer to store user input
    char message[100];         // Buffer to store bot response messages

    while (1) {                // Main infinite loop
        // Bot greets the user
        UART_sendString(colorBot "[Help Bot]: " colorReset); // Send bot prompt
        UART_sendString("\r\nHello! I am a Help Bot. Type 'help' if you need assistance and a buzzer will sound indicating you are in danger. \r\n");
        UART_sendString("\r\nIf you are in danger and cannot risk the buzzer, press switch 1 and help will blink in morse code. \r\n");

        // User's turn to type a command
        UART_sendString(colorUser "[User]: " colorReset); // Prompt user for input
        UART_getLine(input, sizeof(input));               // Receive input from the user

        // Check for "help" keyword
        if (strcmp(input, "help") == 0) {                // Compare input with "help"
            // Bot acknowledges and activates the buzzer
            UART_sendString(colorBot "[Help Bot]: " colorReset);
            UART_sendString("Activating buzzer for assistance...\r\n");

            beepyBeep = 32;                              // Set buzzer duration
            __delay_cycles(1000000);                    // Delay for ~1 second (assuming 1MHz SMCLK)

            // Bot confirms buzzer action
            UART_sendString(colorBot "[Help Bot]: " colorReset);
            UART_sendString("Buzzer activated. Let me know if you need further help.\r\n");
        } else {
            // Bot informs the user of invalid input
            snprintf(message, sizeof(message),          // Format the invalid input message
                     "I didn't recognize '%s'. Please type 'help' if you need assistance.\r\n",
                     input);
            UART_sendString(colorBot "[Help Bot]: " colorReset);
            UART_sendString(message);                  // Send the formatted message
        }
    }
}

void buttonyButSetup() {
    // Configure SW1 (P2.1) as input with pull-up resistor and enable interrupt
    P2DIR &= ~BIT1;             // Set P2.1 as input
    P2REN |= BIT1;              // Enable pull-up resistor on P2.1
    P2OUT |= BIT1;              // Set pull-up resistor to default high state
    P2IE |= BIT1;               // Enable interrupt on P2.1
    P2IES |= BIT1;              // Trigger interrupt on high-to-low transition
    P2IFG &= ~BIT1;             // Clear interrupt flag for P2.1
}

void UART_SETUP() {
    P3SEL |= BIT3 + BIT4;       // Set P3.3 (TXD) and P3.4 (RXD) for UART functionality
    UCA0CTL1 |= UCSWRST;        // Enable software reset to configure UART
    UCA0CTL1 |= UCSSEL_2;       // Select SMCLK as the clock source
    UCA0BR0 = 9;                // Configure baud rate to 115200 (assuming 1MHz clock)
    UCA0BR1 = 0;                // Upper byte of baud rate is 0
    UCA0MCTL = 2;               // Set modulation pattern for baud rate
    UCA0CTL1 &= ~UCSWRST;       // Release software reset, enabling UART
}

// Rest of the functions and ISRs follow the same commenting style


// Send a string via UART
void UART_sendString(char* str) {
    while (*str) {
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = *str++;
    }
}
// Receive a line of input via UART
void UART_getLine(char* buf, int limit) {
    int i = 0;
      char ch;
      while (1) {
          while (!(UCA0IFG & UCRXIFG));
          ch = UCA0RXBUF;
          if (ch == '\r' || ch == '\n') {
              buf[i] = '\0';  // Null-terminate
              UART_sendString("\r\n");
              break;
          } else if (ch == '\b' || ch == 127) {
              if (i > 0) {
                  i--;
                  UART_sendString("\b \b");
              }
          } else if (i < limit - 1) {
              buf[i++] = ch;
              while (!(UCA0IFG & UCTXIFG));
              UCA0TXBUF = ch;  // Echo
          }
      }
}
//// Configure Timer for Buzzer
void watchyWatchSetup()
{
    WDTCTL = WDT_MDLY_32;       // Interval timer mode with ~32ms
    SFRIE1 |= WDTIE;          // Enable Watchdog Timer interrupts
    P7DIR |= BUZZER_PIN;        // Set P7.4 as output
    P7SEL |= BUZZER_PIN;        // Select peripheral function (TimerB output)
    TB0CCTL2 = OUTMOD_0;        // Initialize with output off
    TB0CTL = TBSSEL_2 | MC_1;   // SMCLK, up mode
    TB0CCR0 = 500 - 1;          // 2 kHz frequency
    TB0CCR2 = 250;              // 50% duty cycle
}

void lightyLightSetup()
{
    // Assuming LED1 is connected to P1.0
    P1DIR |= BIT0;               // Set P1.0 as output
    P1OUT &= ~BIT0;              // Ensure LED is off
}
void FlashMorseCode(char* message)
{
    int i; // Declare 'i' at the beginning for C89 compatibility
    while (*message) {
        if (*message == 'H') {
            // H: ....
            for(i = 0; i < 4; i++) {
                P1OUT |= BIT0;  // LED on
                __delay_cycles(DOT_DURATION); // Delay for dot
                P1OUT &= ~BIT0; // LED off
                __delay_cycles(SYMBOL_SPACE); // Space between symbols
            }
            __delay_cycles(LETTER_SPACE); // Space between letters
        }
        else if (*message == 'E') {
            // E: .
            P1OUT |= BIT0;
            __delay_cycles(DOT_DURATION);
            P1OUT &= ~BIT0;
            __delay_cycles(SYMBOL_SPACE);
            __delay_cycles(LETTER_SPACE);
        }
        else if (*message == 'L') {
            // L: .-..
            // .
            P1OUT |= BIT0;
            __delay_cycles(DOT_DURATION);
            P1OUT &= ~BIT0;
            __delay_cycles(SYMBOL_SPACE);
            // -
            P1OUT |= BIT0;
            __delay_cycles(DASH_DURATION);
            P1OUT &= ~BIT0;
            __delay_cycles(SYMBOL_SPACE);
            // ..
            for(i = 0; i < 2; i++) {
                P1OUT |= BIT0;
                __delay_cycles(DOT_DURATION);
                P1OUT &= ~BIT0;
                __delay_cycles(SYMBOL_SPACE);
            }
            __delay_cycles(LETTER_SPACE);
        }
        else if (*message == 'P') {
            // P: .--.
            // .
            P1OUT |= BIT0;
            __delay_cycles(DOT_DURATION);
            P1OUT &= ~BIT0;
            __delay_cycles(SYMBOL_SPACE);
            // --
            for(i = 0; i < 2; i++) {
                P1OUT |= BIT0;
                __delay_cycles(DASH_DURATION);
                P1OUT &= ~BIT0;
                __delay_cycles(SYMBOL_SPACE);
            }
            // .
            P1OUT |= BIT0;
            __delay_cycles(DOT_DURATION);
            P1OUT &= ~BIT0;
            __delay_cycles(LETTER_SPACE);
        }
        message++;
    }
}
// UART RX ISR to handle incoming data if needed
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    // You can handle UART RX interrupts here if you prefer
    // Currently, UART_getLine() is polling, so this ISR is optional
}

// ISR for SW1 ("No" response)
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if (P2IFG & BIT1)
    {
        volatile unsigned long k; // Declare 'k' for debounce
        for(k = 25000; k > 0; k--); // Debounce delay
        useyUse = 0;           // Set "no" response (ensure 'useyUse' is declared)
        P2IFG &= ~BIT1;             // Clear interrupt flag

        // Display message over UART
        UART_sendString(colorBot "[Help Bot]: " colorReset);
        UART_sendString("Switch triggered, blinking HELP in Morse code.\r\n");

        // Flash Morse code for "HELP"
        FlashMorseCode("HELP");

        // Return to Help Bot prompt
        UART_sendString(colorBot "\r\n[Help Bot]: " colorReset);
        UART_sendString("\r\nHello! I am a Help Bot. Type 'help' if you need assistance and a buzzer will sound indicating you are in danger.\r\n");
        UART_sendString(colorUser "[User]: " colorReset);
    }
}


// Watchdog Timer ISR
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    if (beepyBeep > 0) {
        beepyBeep--;
        TB0CCTL2 = OUTMOD_7; // Start PWM (buzzer on)
        if (beepyBeep == 0) {
            TB0CCTL2 = OUTMOD_0; // Stop PWM (buzzer off)
        }
    } else {
        TB0CCTL2 = OUTMOD_0; // Ensure buzzer remains off
    }
}



