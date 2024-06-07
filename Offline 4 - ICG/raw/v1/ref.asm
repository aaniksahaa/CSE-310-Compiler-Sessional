.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
f PROC
	PUSH BP
	MOV BP, SP
	MOV AX, [BP+4]       ; Line 2
	JMP L2
L1:
L2:
	POP BP
	RET 4
f ENDP
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
L3:
	MOV AX, 2       ; Line 6
	MOV [BP-2], AX
	PUSH AX
	POP AX
L4:
	MOV AX, 3       ; Line 7
	MOV [BP-4], AX
	PUSH AX
	POP AX
L5:
	MOV AX, [BP-2]       ; Line 8
	PUSH AX
	MOV AX, [BP-4]       ; Line 8
	PUSH AX
	CALL f
	PUSH AX
	POP AX       ; Line 8
	MOV [BP-6], AX
	PUSH AX
	POP AX
L6:
	MOV AX, [BP-6]       ; Line 9
	CALL print_output
	CALL new_line
L7:
L8:
	ADD SP, 6
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main