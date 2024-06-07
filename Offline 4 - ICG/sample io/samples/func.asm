.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
f PROC
	PUSH BP
	MOV BP, SP
	MOV AX, [BP+4]       ; Line 2
	MOV CX, AX
	MOV AX, 2       ; Line 2
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 2
	JMP L3
L1:
	MOV AX, 9       ; Line 3
	MOV [BP+4], AX
	PUSH AX
	POP AX
L2:
L3:
	POP BP
	RET 2
f ENDP
g PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L4:
	MOV AX, [BP+6]       ; Line 8
	PUSH AX
	CALL f
	PUSH AX
	MOV AX, [BP+6]       ; Line 8
	MOV DX, AX
	POP AX       ; Line 8
	ADD AX, DX
	PUSH AX
	MOV AX, [BP+4]       ; Line 8
	MOV DX, AX
	POP AX       ; Line 8
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 8
	MOV [BP-2], AX
	PUSH AX
	POP AX
L5:
	MOV AX, [BP-2]       ; Line 9
	JMP L7
L6:
L7:
	ADD SP, 2
	POP BP
	RET 4
g ENDP
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
L8:
	MOV AX, 1       ; Line 14
	MOV [BP-2], AX
	PUSH AX
	POP AX
L9:
	MOV AX, 2       ; Line 15
	MOV [BP-4], AX
	PUSH AX
	POP AX
L10:
	MOV AX, [BP-2]       ; Line 16
	PUSH AX
	MOV AX, [BP-4]       ; Line 16
	PUSH AX
	CALL g
	PUSH AX
	POP AX       ; Line 16
	MOV [BP-2], AX
	PUSH AX
	POP AX
L11:
	MOV AX, [BP-2]       ; Line 17
	CALL print_output
	CALL new_line
L12:
	MOV AX, 0       ; Line 18
	JMP L14
L13:
L14:
	ADD SP, 4
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main