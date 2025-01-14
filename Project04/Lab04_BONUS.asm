; ------------------------------------------------------------------------------
; File: Lab04_Bonus.asm
; Description: Converts uppercase letters to lowercase and counts the changes.
; Input: String
; Output: Updated string and number of changes in P3OUT
; Author(s): Gianna Foti
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
;-------------------------------------------------------------------------------
        .def    RESET                   ; Export program entry-point to
                                        ; make it known to linker.
;-------------------------------------------------------------------------------
STRING:  .string "WELCoME To MSP430 ASSEMBLY!", ''
        .data
RESULTS:     .space 64                  ; Allocate 64 bytes for outcome string
;-------------------------------------------------------------------------------
										; built in crap
        .text                           ; Assemble into program memory.
        .retain                         ; Override ELF conditional linking
                                        ; and retain current section.
        .retainrefs                     ; And retain any sections that have
                                        ; references to current section.
;-------------------------------------------------------------------------------
RESET:  mov.w   #__STACK_END,SP         ; Initialize stack pointer
        mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; stop watchdog timer
;-------------------------------------------------------------------------------
; MAIN LOOP HERE (START OF PROGRAM)
;-------------------------------------------------------------------------------
main:   bis.b   #0FFh, &P1DIR           ; set P1
        bis.b   #0FFh, &P3DIR           ; set P3 for output
        mov.w   #STRING, R4         	; loading the string address into R4
        mov.w   #RESULTS, R8            ; Load RESULTS address into R8
        clr.b   R5                      ; clear R5 (used for storing characters)
        clr.b   R6                      ; clear R6 (used as counter for changes)
        clr.b   R7                      ; clear R7 (length counter) this is extra, dont need the length count

LOOP:
        inc.w   R7                      ; Increment the string length counter
        mov.b   @R4+, R5                ; Load character from string into R5
        cmp     #0x00, R5               ; Check if null character
        jeq     DAEND                   ; If null, jump to end

        cmp.b   #'A', R5                ; Compare with 'A'
        jl      THANKU_NEXT             ; If less than 'A', skip to next character
        cmp.b   #'Z', R5                ; Compare with 'Z'
        jge      THANKU_NEXT            ; If greater than 'Z', skip to next character

        add.b   #32, R5                 ; Convert to lowercase by adding 32
        inc.b   R6                      ; Increment change counter

THANKU_NEXT:
        mov.b   R5, 0(R8)               ; Store the converted character in RESULTS buffer
        inc.w   R8                      ; Increment R8 to point to the next byte in RESULTS
        jmp     LOOP                    ; Continue loop to next character

DAEND:
        mov.b   R6, &P3OUT              ; Output the number of changes to P3
        bis.w   #LPM4, SR               ; Enter low power mode
        nop                             ; Required for debugger
;-------------------------------------------------------------------------------
; STACK POINTER DEFINITION
;-------------------------------------------------------------------------------
        .global __STACK_END
        .sect   .stack
;-------------------------------------------------------------------------------
; INTERRUPT VECTORS
;-------------------------------------------------------------------------------
        .sect   ".reset"                ; MSP430 RESET Vector
        .short  RESET
        .end

