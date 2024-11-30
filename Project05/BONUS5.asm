



            .cdecls C,LIST,"msp430.h"       ; Include device header file
			.def BONUS

            .text                           ; Assemble into program memory.
BONUS:		push R4							;save R4, the address of the str
			push R5							;save R5, the address of the place to store it
			push R6							;save R6, the element of the str
			mov.w 10(SP), R4				;put address of str into R4
			mov.w 8(SP), R5					;put address of result to R5
			clr.w MPY
			clr.w RESLO
			clr.w OP2

loop:		mov.b @R4+, &MPY					;put element into R6
			cmp	#0, &MPY						;is it the null value?
			jeq	done							;if it is, done
			sub	#0x30, &MPY
			mov.w	#0x1, &OP2
			nop
			nop
			nop
			mov RESLO, 0(R5)
			add	#0x2, R5


			jmp	loop
done:
			pop	R6
			pop	R5
			pop	R4
			ret
			.end
