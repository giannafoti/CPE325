/*----------------------------------------------------------------------------------------------
 * File: Lab09_Master.c
 * Description: This program uses UART and SPI communication on the MSP430 to control an LED
 *              based on user input for frequency and cycle count. It provides an interactive
 *              prompt at 115,200 baud, allowing users to set the LED blink frequency (1-10 Hz)
 *              and the number of cycles. The program also uses SPI to communicate with a slave
 *              device for data verification, providing feedback through LEDs.
 * Input: User-defined frequency and cycle count via UART
 * Output: LED blink pattern, data verification feedback through LED status
 * Author: Gianna Foti
 *----------------------------------------------------------------------------------------------*/

#include <msp430.h>

unsigned char MST_Data, SLV_Data; // Variables for master and slave data in SPI
unsigned char temp; // Temporary variable

void UART_setup(); // UART setup function declaration
void UARTA1_putchar(char c); // Function to send a character via UART
char UART_getChar(); // Function to receive a character via UART
void PrintString(char * msg); // Function to print a string via UART
void UART_getLine(char* buf, int limit); // Function to get user input line via UART
void Prompt(); // Function to prompt user for frequency and cycle count
void SPI_Master_UCB0_Setup(); // SPI setup function declaration

char Frequency[10]; // Buffer for frequency input from user
char Cycle[10]; // Buffer for cycle count input from user

void SPI_Master_UCB0_Setup(void) {
    // Configure P3.0, P3.1, P3.2 as SPI pins (SIMO, SOMI, CLK)
    P3SEL |= BIT0 + BIT1 + BIT2;
    UCB0CTL1 |= UCSWRST; // Put state machine in reset during setup
    UCB0CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB; // Configure as 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2; // Select SMCLK as clock source
    UCB0BR0 = 18; // Set baud rate divisor
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST; // Initialize USCI state machine
}

int main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    P1OUT = 0; // Initialize output
    P1DIR |= BIT0; // Set P1.0 as output for LED1
    P4DIR |= BIT7; // Set P4.7 as output for LED2
    P4OUT &= ~BIT7; // Turn off LED2
    P1IN &= ~BIT2; // Initialize P1.2 input

    UART_setup(); // Set up UART communication
    SPI_Master_UCB0_Setup(); // Set up SPI communication

    // Wait for slave device to be ready
    while (!(P1IN & BIT2)); // Wait until P1.2 goes high (slave ready signal)
    P1OUT |= BIT0; // Light LED1 when slave is ready
    __delay_cycles(1000000); // Delay for visual indication
    P1OUT &= ~BIT0; // Turn off LED1

    MST_Data = 0x01; // Initialize master data to 1
    SLV_Data = 0x00; // Initialize expected slave data to 0

    for (;;) {
        Prompt(); // Continuously prompt user for input
    }
}

void Prompt() {
    PrintString("\r\nYou Can Set the Frequency of LED-2 from 1 - 10 Hz by entering the value\r\n");
    PrintString("What is your intended frequency\r\n");
    UART_getLine(Frequency, 10); // Get frequency input from user

    // Ensure frequency is within 1-10 Hz range
    while (atoi(Frequency) < 1 || atoi(Frequency) > 10) {
        PrintString("Error: You can set the Frequency of LED-2 from 1 - 10 Hz by entering the value\r\n");
        UART_getLine(Frequency, 10); // Re-prompt user for valid frequency
    }

    PrintString("What is your intended cycle\r\n");
    UART_getLine(Cycle, 10); // Get cycle count input from user

    int NumFrequency = atoi(Frequency); // Convert frequency input to integer
    int NumCycle = atoi(Cycle); // Convert cycle count input to integer
    double g = ((1.0 / NumFrequency) / 2.0) * 10000; // Calculate delay period based on frequency

    int h = 0; // Cycle counter
    int f = 0; // Inner loop counter

    // Loop to blink LED and perform data verification for the specified cycle count
    while (h < NumCycle * 2) { // Blink twice per cycle
        for (f = 0; f < (int)g; f++); // Delay loop based on frequency

        P1OUT ^= BIT0; // Toggle LED1

        // SPI communication with slave device
        while (!(UCB0IFG & UCTXIFG)); // Wait for SPI TX buffer ready
        UCB0TXBUF = MST_Data; // Send master data to slave
        while (!(UCB0IFG & UCRXIFG)); // Wait for data back from slave

        // Check if received data matches expected slave data
        if (UCB0RXBUF == SLV_Data) {
            P1OUT |= BIT0; // Light LED1 if data matches
        } else {
            P1OUT &= ~BIT0; // Turn off LED1 if data does not match
        }

        P4OUT ^= BIT7; // Toggle LED2 as a heartbeat indicator

        // Update data values for next transmission
        MST_Data = (MST_Data + 1) % 50;
        SLV_Data = (SLV_Data + 1) % 50;
        h++; // Increment cycle counter
    }
}

void UART_setup(void) {
    // Configure UART communication on USCI_A1
    P4SEL |= BIT4 + BIT5; // Set P4.4 and P4.5 for UART
    UCA1CTL1 |= UCSWRST; // Put state machine in reset during setup
    UCA1CTL0 = 0; // Default UART configuration
    UCA1CTL1 |= UCSSEL_2; // Select SMCLK as UART clock source
    UCA1BR0 = 0x09; // Set baud rate to 115200 (SMCLK/115200)
    UCA1BR1 = 0x00;
    UCA1MCTL = 0x02; // Modulation settings
    UCA1CTL1 &= ~UCSWRST; // Initialize USCI state machine
}

void UARTA1_putchar(char c) {
    while (!(UCA1IFG & UCTXIFG)); // Wait until TX buffer is ready
    UCA1TXBUF = c; // Transmit character via UART
}

char UART_getChar() {
    while (!(UCA1IFG & UCRXIFG)); // Wait until RX buffer has data
    return UCA1RXBUF; // Return received character
}

void PrintString(char * msg) {
    // Print a string character-by-character via UART
    register int i = 0;
    for (i = 0; i < strlen(msg); i++) {
        UARTA1_putchar(msg[i]); // Send each character
    }
}

void UART_getLine(char* buf, int limit) {
    // Get a line of input from UART with character limit
    int i = 0;
    char ch;
    while (1) {
        ch = UART_getChar(); // Get character from UART

        if (ch == '\r' || ch == '\n') { // Enter key pressed
            buf[i] = '\0'; // Null-terminate string
            PrintString("\r\n"); // Print newline
            break;
        } else if (ch == '\b' || ch == 127) { // Backspace pressed
            if (i > 0) {
                i--; // Move back buffer pointer
                PrintString("\b \b"); // Clear character on screen
            }
        } else if (i < limit - 1) { // Normal character input
            buf[i++] = ch;
            UARTA1_putchar(ch); // Echo character back to UART
        }
    }
}
