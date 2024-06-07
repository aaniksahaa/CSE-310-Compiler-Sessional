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
	JMP L2
L1:
L2:
	POP BP
	RET 2
f ENDP
g PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L3:
	MOV AX, [BP+6]       ; Line 7
	PUSH AX
	CALL f
	PUSH AX
	MOV AX, [BP+6]       ; Line 7
	MOV DX, AX
	POP AX       ; Line 7
	ADD AX, DX
	PUSH AX
	MOV AX, [BP+4]       ; Line 7
	MOV DX, AX
	POP AX       ; Line 7
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 7
	MOV [BP-2], AX
	PUSH AX
	POP AX
L4:
	MOV AX, [BP-2]       ; Line 8
	JMP L6
L5:
L6:
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
	SUB SP, 10
	SUB SP, 2
L7:
	MOV AX, 1       ; Line 13
	MOV [BP-2], AX
	PUSH AX
	POP AX
L8:
	MOV AX, 3       ; Line 14
	MOV DX, AX
	MOV AX, 2       ; Line 14
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 14
	MOV CX, AX
	MOV AX, 1       ; Line 14
	CWD
	MUL CX
	PUSH AX
	MOV AX, 3       ; Line 14
	MOV CX, AX
	POP AX       ; Line 14
	CWD
	DIV CX
	PUSH DX
	POP AX       ; Line 14
	MOV [BP-4], AX
	PUSH AX
	POP AX
L9:
	MOV AX, [BP-2]       ; Line 15
	PUSH AX
	MOV AX, [BP-4]       ; Line 15
	PUSH AX
	CALL g
	PUSH AX
	POP AX       ; Line 15
	MOV [BP-2], AX
	PUSH AX
	POP AX
L10:
	MOV AX, [BP-2]       ; Line 16
	CALL print_output
	CALL new_line
L11:
	MOV AX, 0       ; Line 17
	JMP L13
L12:
L13:
	ADD SP, 16
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main