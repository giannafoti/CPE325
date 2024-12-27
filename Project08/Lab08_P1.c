/*----------------------------------------------------------------------------------------------
 * File: Lab08_p1.c
 * Description: Uses a 115,200 baud connection to create an interactive chatbot program
 * Input: User input
 * Output: Chat output
 * Author: Gianna Foti
 *----------------------------------------------------------------------------------------------*/

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Me doing the bonus colourssss
#define colorReset   "\x1b[0m"
#define colorBot     "\x1b[31m"  // Market Bot in red like example
#define colorUser    "\x1b[35m"  // User in purple

//these are all of my function prototypes
void UART_SETUP();
void UART_sendChar(char bip);
char UART_getChar();
void UART_sendString(char* str);
void UART_getLine(char* buf, int limit);

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
    UART_SETUP();  // Setup UART

    char userName[50];
    char item[10];
    char response[10];
    char message[100];
    int totalPrice = 0;

    while (1) {
        // Ask for User's Name (First Interaction)
        UART_sendString(colorBot "[Market Bot]: " colorReset);
        UART_sendString("\r\nHi, I am Market Bot. What is the name for your order?\r\n");

        UART_sendString(colorUser "[User]: " colorReset);
        UART_getLine(userName, 50);  // Get the user's name

        // Greeting after receiving User's Name
        snprintf(message, sizeof(message),
                 "Hello %s! Today we have eggs, chicken, and milk. What would you like to order?\r\n",
                 userName);
        UART_sendString(colorBot "[Market Bot]: " colorReset);
        UART_sendString(message);

        totalPrice = 0;  // Reset total price for new order

        do {
            // User selecting an item
            UART_sendString(colorUser "[User]: " colorReset);
            UART_getLine(item, 10);

            while (strcmp(item, "eggs") != 0 && strcmp(item, "chicken") != 0 && strcmp(item, "milk") != 0) {
                snprintf(message, sizeof(message),
                         "%s are not available today. Today we have eggs, chicken, and milk. What would you like to order?\r\n",
                         item);
                UART_sendString(colorBot "[Market Bot]: " colorReset);
                UART_sendString(message);

                UART_sendString(colorUser "[User]: " colorReset);
                UART_getLine(item, 10);
            }

            // Handle item price and quantity
            //Ternary Operator
            //
            int PRICE = (strcmp(item, "eggs") == 0) ? 2 :
                        (strcmp(item, "chicken") == 0) ? 1 : 2;

            snprintf(message, sizeof(message),
                     "%s costs $%d. How many would you like?\r\n", item, PRICE);
            UART_sendString(colorBot "[Market Bot]: " colorReset);
            UART_sendString(message);

            UART_sendString(colorUser "[User]: " colorReset);
            UART_getLine(response, 10);  // Get quantity as input
            int QUANTITY = atoi(response);  // Convert to integer

            totalPrice += PRICE * QUANTITY;  // Update total price

            // Ask if the User Wants to Order More
            UART_sendString(colorBot "[Market Bot]: " colorReset);
            UART_sendString("Do you want to order more (yes/no)?\r\n");

            UART_sendString(colorUser "[User]: " colorReset);
            UART_getLine(response, 10);  // Get yes/no response

            // Validate yes/no response
            while (strcmp(response, "yes") != 0 && strcmp(response, "no") != 0) {
                UART_sendString(colorBot "[Market Bot]: " colorReset);
                UART_sendString("Please respond with 'yes' or 'no'.\r\n");

                UART_sendString(colorUser "[User]: " colorReset);
                UART_getLine(response, 10);
            }

            // If "yes", re-prompt for the next item
            if (strcmp(response, "yes") == 0) {
                UART_sendString(colorBot "[Market Bot]: " colorReset);
                UART_sendString("Today we have eggs, chicken, and milk. What would you like to order?\r\n");
            }

        } while (strcmp(response, "yes") == 0);

        // Show total price and thank the user
        snprintf(message, sizeof(message),
                 "The total for all items is $%d. Thank you for shopping with me, %s!\r\n\n",
                 totalPrice, userName);
        UART_sendString(colorBot "[Market Bot]: " colorReset);
        UART_sendString(message);
    }
}

// UART Setup
void UART_SETUP() {
    P3SEL |= BIT3 + BIT4;  // Set RXD/TXD pins
    UCA0CTL1 |= UCSWRST;   // Software reset
    UCA0CTL0 = 0;          // Control register
    UCA0CTL1 |= UCSSEL_2;  // Use SMCLK

    UCA0BR0 = 0x09;  // Baud rate 115200
    UCA0BR1 = 0x00;
    UCA0MCTL = 0x02;

    UCA0CTL1 &= ~UCSWRST;  // Initialize UART state machine
}

// Send a character via UART
void UART_sendChar(char bip) {
    while (!(UCA0IFG & UCTXIFG));  // Wait for TX buffer to be ready
    UCA0TXBUF = bip;
}

// Receive a character via UART
char UART_getChar() {
    while (!(UCA0IFG & UCRXIFG));  // Wait for RX buffer to be ready
    return UCA0RXBUF;
}

// Send a string via UART
void UART_sendString(char* str) {
    while (*str) {
        UART_sendChar(*str++);
    }
}

// Receive a line of input via UART with backspace handling
void UART_getLine(char* buf, int limit) {
    int i = 0;
    char ch;

    while (1) {
        ch = UART_getChar();

        if (ch == '\r' || ch == '\n') {  // Enter key pressed
            buf[i] = '\0';  // Null-terminate the string
            UART_sendString("\r\n");
            break;
        } else if (ch == '\b' || ch == 127) {  // Backspace pressed
            if (i > 0) {
                i--;  // Move buffer pointer back
                UART_sendString("\b \b");  // Clear character on screen
            }
        } else if (i < limit - 1) {  // Normal character input
            buf[i++] = ch;
            UART_sendChar(ch);  // Echo the character back
        }
    }
}
