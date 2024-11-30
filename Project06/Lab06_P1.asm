; ---------------------------------------------------------------------------------------------------------------------
; file:        Lab06_P1.asm
; description: assembly program interfaces w/ switches 1 & 2, and leds 1 & 2 aka red and green
;               1. turn on both leds
;               2. toggle led2 when sw1 is pressed
;               3. when sw2 is pressed, toggle led1 4x & toggle led2 1x at 1 Hz
; input:       p2.1 - sw1 | p1.1 - sw2
; output:      p1.0 - red led | p4.7 - green led
; author:      gianna foti
; ---------------------------------------------------------------------------------------------------------------------
                .cdecls C, LIST, "msp430.h"       	; include device header file
; ---------------------------------------------------------------------------------------------------------------------
                .def    RESET                     	; export program entry-point to make it known to linker
                .def    SW2_ISR                   	; define sw2 interrupt
                .def    SW1_ISR                   	; define sw1 interrupt
; ---------------------------------------------------------------------------------------------------------------------
                .text                            	; assemble into program memory
                .retain                          	; override elf conditional linking and retain current section
                .retainrefs                      	; and retain any sections that have references to current section
; ---------------------------------------------------------------------------------------------------------------------
RESET:          mov.w   #__STACK_END, SP         	; initialize da stack pointer
                mov.w   #WDTPW|WDTHOLD, &WDTCTL   	; stoppy stop da watchdog timer
; ---------------------------------------------------------------------------------------------------------------------
; all the boring stuff, inititialization or whatever
MainyMain:
                bis.b   #BIT0, &P1DIR             	; set p1.0 as output (red LED)
                bis.b   #BIT0, &P1OUT             	; turn on led1 (p1.0)

                bis.b   #BIT7, &P4DIR             	; set p4.7 as output (green LED)
                bis.b   #BIT7, &P4OUT             	; turn on led2 (p4.7)

                bic.b   #BIT1, &P1DIR             	; set p1.1 as input for sw2
                bis.b   #BIT1, &P1REN             	; enable pull-up resistor at p1.1
                bis.b   #BIT1, &P1OUT             	; configure pull-up

                bic.b   #BIT1, &P2DIR              	; set p2.1 as input for sw1
                bis.b   #BIT1, &P2REN              	; enable pull-up resistor at p2.1
                bis.b   #BIT1, &P2OUT              	; configure pull-up

                bis.w   #GIE, SR                   	; enable global interrupts
                bis.b   #BIT1, &P1IE               	; enable interrupt for p1.1 (sw2)
                bis.b   #BIT1, &P1IES              	; set interrupt to trigger on high-to-low transition
                bic.b   #BIT1, &P1IFG              	; clear interrupt flag

                bis.b   #BIT1, &P2IE               	; enable interrupt for p2.1 (sw1)
                bis.b   #BIT1, &P2IES              	; set interrupt to trigger on high-to-low transition
                bic.b   #BIT1, &P2IFG              	; clear interrupt flag

InfinityInfiniteLoop:
                jmp     $                          	; infinite loop, to infinity and beyondddd
; ---------------------------------------------------------------------------------------------------------------------
; SW2 Interrupt Service Routine (P1.1)
; Handles toggling of LED1 (red) and LED2 (green) when switch 2 is pressed.
; ---------------------------------------------------------------------------------------------------------------------
SW2_ISR:
                bic.b   #BIT1, &P1IFG              	; clear interrupt flag, bye bye

CheckyCheckSW2:
                bit.b   #BIT1, &P1IN               	; check if sw2 is still pressed
                jnz     EndyEndSW2ISR               ; exit ISR if sw2 is not pressed

DebounceyDebounceSW2:
                mov.w   #2000, R15                 	; debounce delay count

DebouncyLoopSW2:
                dec.w   R15
                nop
                nop
                nop
                nop
                jnz     DebouncyLoopSW2          	; wait for debounce period
                bit.b   #BIT1, &P1IN               	; check if sw2 is still pressed
                jnz     EndyEndSW2ISR               ; exit ISR if sw2 is not pressed

                mov.w   #8, R4                     	; initialize counter for 8 toggles (4 blinks)

ToggleyToggleLED4:
                xor.b   #BIT0, &P1OUT              	; toggle LED1 (red)
                mov.w   #50000, R15                	; delay count for ~500 ms

Delay_500ms_SW2:
                dec.w   R15
                nop
                nop
                nop
                nop
                jnz     Delay_500ms_SW2            	; wait for 500 ms
                dec.w   R4                         	; decrement toggle counter
                jnz     ToggleyToggleLED4           ; repeat toggling 4 times
                xor.b   #BIT7, &P4OUT              	; toggle LED2 (green)

WaityWaitSW2:
                bit.b   #BIT1, &P1IN               	; check if sw2 is released
                jz      WaityWaitSW2           		; wait until sw2 is released

EndyEndSW2ISR:
                reti                              	; return from interrupt
; ---------------------------------------------------------------------------------------------------------------------
; SW1 Interrupt Service Routine (P2.1)
; Handles toggling of LED2 (green) when switch 1 is pressed.
; ---------------------------------------------------------------------------------------------------------------------
SW1_ISR:
                bic.b   #BIT1, &P2IFG              ; clear interrupt flag

CheckyCheckSW1:
                bit.b   #BIT1, &P2IN               ; check if sw1 is still pressed
                jnz     EndyEndSW1ISR              ; exit ISR if sw1 is not pressed

DebouncyBounceSW1:
                mov.w   #2000, R15                 ; debounce da delay count

DebouncyLoopySW1:
                dec.w   R15
                nop
                nop
                nop
                nop
                jnz     DebouncyLoopySW1			; wait for da debounce period
                bit.b   #BIT1, &P2IN               	; check if sw1 is still pressed
                jnz     EndyEndSW1ISR              	; exit ISR if sw1 is not pressed

ToggleyToggleLED2:
                xor.b   #BIT7, &P4OUT             	; toggle LED2 (green)

WaityReleaseSW1:
                bit.b   #BIT1, &P2IN              	; check if sw1 is released
                jz      WaityReleaseSW1          	; wait until sw1 is released

EndyEndSW1ISR:
                reti                             	; return from interrupt
; ---------------------------------------------------------------------------------------------------------------------
; Stack Pointer Definition
; ---------------------------------------------------------------------------------------------------------------------
                .global __STACK_END
                .sect   .stack
; ---------------------------------------------------------------------------------------------------------------------
; Interrupt Vectors
; ---------------------------------------------------------------------------------------------------------------------
                .sect   ".reset"                   	; MSP430 reset vector
                .short  RESET
                .sect   ".int47"                   	; Port1 vector
                .short  SW2_ISR                    	; ISR for SW2
                .sect   ".int42"                   	; Port2 vector
                .short  SW1_ISR                   	; ISR for SW1
                .end
