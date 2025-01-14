; ------------------------------------------------------------------------------
; File: Lab04_P2.asm
; Description: This program determines if an integer value is even or odd.
;              It outputs the ASCII string "Even" or "Odd" based on the value of TACO_LOVER.
; Input: An integer stored in TACO_LOVER
; Output: ASCII "Even" or "Odd" stored in memory at RESULT_GHF
; Author(s): Gianna Foti
; ------------------------------------------------------------------------------

        .cdecls C, LIST, "msp430.h"     ; Include device header file

;-------------------------------------------------------------------------------
        .def    RESET                   ; Export program entry-point to make it known to linker.

;-------------------------------------------------------------------------------
TACO_LOVER:  .int 20                    ; Initialize TACO_LOVER with the value 20 or 21 for even or odd
		.data
RESULT_GHF:     .space 30                ; Allocate more than 4 bytes of memory for the output string

;-------------------------------------------------------------------------------
        .text                           ; Assemble into program memory.
        .retain                         ; Override ELF conditional linking
                                        ; and retain current section.
        .retainrefs                     ; Retain any sections that have references to current section.

;-------------------------------------------------------------------------------
RESET:  mov.w   #__STACK_END,SP         ; Initialize the stack pointer
        mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop the watchdog timer

;-------------------------------------------------------------------------------
; Main Programmmm
;-------------------------------------------------------------------------------
main:
        bis.b   #0FFh, &P1DIR           ; Set Port 1 to output mode
        clr.b   R5                      ; Clear register R5

        mov.w   TACO_LOVER, R5          ; Load TACO_LOVER into R5
        and.w   #1, R5                  ; Perform bitwise AND with 1 to check the least significant bit

        jz      EVEN                    ; If LSB is 0 (even number), jump to EVEN
        jnz     ODD                     ; If LSB is 1 (odd number), jump to ODD

; Store the characters for "Odd" in memory allocated at RESULT_GHF
ODD:
		; O in hex
        mov.b   #0x4F, RESULT_GHF
        ; d in hex
        mov.b   #0x64, RESULT_GHF+1
        ; d in hex
        mov.b   #0x64, RESULT_GHF+2
        ; Null terminator for the string
        mov.b   #0x0, RESULT_GHF+3
        ; Jump to DAEND
        jmp     DAEND

; Store the characters for "Even" in memory allocated at RESULT_GHF
EVEN:

        mov.b   #0x45, RESULT_GHF
        mov.b   #0x76, RESULT_GHF+1
        mov.b   #0x65, RESULT_GHF+2
        mov.b   #0x6E, RESULT_GHF+3
        ; Jump to daend
        jmp     DAEND



DAEND:
		; Enter Low Power Mode 4
        bis.w #LPM4, SR
        ; No operation (typically used for debugging)
        nop

;-------------------------------------------------------------------------------
; Stack Pointer Definition
;-------------------------------------------------------------------------------
        .global __STACK_END
        .sect   .stack

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
        .sect   ".reset"                ; MSP430 RESET Vector
        .short  RESET
        .end
