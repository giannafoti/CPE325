; THIS IS MY SOFTWARE MULTIPLICATION


;-------------------------------------------------------------------------------
				.cdecls C,LIST,"msp430.h"       ; Include device header file
;-------------------------------------------------------------------------------
				.def SW_MUL


SW_MUL:
				push R4							; saving registers on the stack
				push R5
				push R6
				push R7
				push R8
				push R11

				mov 16(SP), R4					; load address of array into R4
				mov 14(SP), R5					; load length of array into R5
				clr.w R12						; clear result register

LoopyLoop:
				jz EndyEnd						; jump to end if length is zero
				mov @R4+, R6					; load current element into R6 and increment pointer
				mov R6, R7						; copy element into R7 (multiplier)
				clr R11							; clearing the temp register thingy
				mov #16, R8						; set loop counter (bit length of multiplier)



AddyAdder:
        		bit.w   #1, R7					; check if LSB is 1
        		jz      ShiftyShifter			; if lsb is 0, then shift
        		add.w   R6, R11					; if result is 1 then add multiplicant to the result for multiplication


ShiftyShifter:
				rla.w   R6						; shift multiplicand (a) left
        		rra.w   R7						; shift multiplier (b) right
        		dec.w   R8						; decrementing the loopy counter
        		jnz    	AddyAdder 				; loop until the loopy counter is 0
        		bit.w 	#1, R7					; check if lsb of multiplier is negative
        		jz		ResultyResult			; if lsb is positive, go to the result
        		sub.w	R6, R11					; if lsb is negative, subtract multiplicand from result

ResultyResult:
				add.w   R11, R12				; add multiplication result to running sum (e.g., x^2 + y^2 + z^2)
        		dec.w	R5						; decrement array length
        		jmp     LoopyLoop				; repeat for next array element

EndyEnd:
				pop R11							; restore registers from stack
				pop R8
				pop R7
				pop R6
				pop R5
				pop R4
				ret
				.end
