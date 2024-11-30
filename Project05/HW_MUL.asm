;THIS IS MY HARDWARE MULTIPLICATION


;-------------------------------------------------------------------------------
				.cdecls C,LIST,"msp430.h"       ; Include device header file
;-------------------------------------------------------------------------------
				.def HW_MUL
;-------------------------------------------------------------------------------


HW_MUL:
				push R4							; pushing R4 onto the SP
				push R5							; pushing R5 onto the SP

				mov	8(SP), R4					; accessing the 8 index
				mov	6(SP), R5					; accessing the 6 index, lenght of array
				clr	R13							; clearing r13


HMultLoop:
				mov @R4+, MPY					; increment through the array, using the built ho
				mov MPY, OP2					; moving the value of MPY into OP2
				nop								; 1 clock cycle
				nop								; 2 clock cycle
				nop								; 3 clock cycle
				add RESLO, R13					; moiving the value of RESLO into r13 as specified
				dec	R5							; decremenets till i reach 0
				jnz HMultLoop					; if not equal to zero, hit the loop again
				pop R5							; else pop r5
				pop R4							; else pop r4

				ret
				.end






