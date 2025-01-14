; ------------------------------------------------------------------------------
; File: Lab04_P1.asm
; Description: Counts the number of digits and total characters in a string
; Input: String
; Output: Number of digits in P1OUT | Number of characters in P2OUT
; Author: Gianna Foti
; ------------------------------------------------------------------------------
    .cdecls C, LIST, "msp430.h"     ; Include device header file
;-------------------------------------------------------------------------------
    .def    RESET                   ; Export program entry-point to linker
;-------------------------------------------------------------------------------
STRING:  .string "Welcome 2 the MSP430 Assembly!", ''  ; Input string
;-------------------------------------------------------------------------------
    .text                           ; Assemble into program memory
RESET:
    mov.w   #__STACK_END, SP        ; Initialize stack pointer
    mov.w   #WDTPW|WDTHOLD, &WDTCTL ; Stop watchdog timer
;-------------------------------------------------------------------------------
; MAIN PROGRAM
;-------------------------------------------------------------------------------
    bis.b   #0FFh, &P1DIR           ; Set all P1 pins to output
    bis.b   #0FFh, &P2DIR           ; Set all P2 pins to output
    mov.w   #STRING, R5         	; Load address of string into R5
    clr.b   R6                      ; Clear R6 (digit counter)
    clr.b   R7                      ; Clear R7 (character counter)

LOOP_GHF:
    mov.b   @R5, R8                 ; Load current character into R8
    cmp.b   #0, R8                  ; Check if it's the NULL terminator
    jeq     DONE                    ; If NULL, end loop
    inc.b   R7                      ; Increment character count
    cmp.b   #'0', R8                ; Compare with ASCII '0'
    jl      NEXT_CHAR               ; If less than '0', go to next character
    cmp.b   #'9', R8                ; Compare with ASCII '9'
    jge     NEXT_CHAR               ; If greater than or equal to '9', go to next character
    inc.b   R6                      ; If it's a digit, increment digit count

NEXT_CHAR:
    inc.w   R5                      ; Move to the next character in the string
    jmp     LOOP_GHF                    ; Repeat the loop

DONE:
    mov.b   R7, &P2OUT              ; Write character count to P2OUT
    mov.b   R6, &P1OUT              ; Write digit count to P1OUT
    bis.w   #LPM4, SR               ; Enter low-power mode
    nop                             ; Placeholder for debugger

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
    .global __STACK_END
    .sect   .stack

;-------------------------------------------------------------------------------
; Reset Vector
;-------------------------------------------------------------------------------
    .sect   ".reset"                ; MSP430 RESET Vector
    .short  RESET
    .end
