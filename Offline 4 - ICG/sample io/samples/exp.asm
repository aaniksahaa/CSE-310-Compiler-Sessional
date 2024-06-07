.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
	SUB SP, 6
L1:
	MOV AX, 3       ; Line 3
	MOV DX, AX
	MOV AX, 2       ; Line 3
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 3
	MOV CX, AX
	MOV AX, 1       ; Line 3
	CWD
	MUL CX
	PUSH AX
	MOV AX, 3       ; Line 3
	MOV CX, AX
	POP AX       ; Line 3
	CWD
	DIV CX
	PUSH DX
	POP AX       ; Line 3
	MOV [BP-2], AX
	PUSH AX
	POP AX
L2:
	MOV AX, 5       ; Line 4
	MOV DX, AX
	MOV AX, 1       ; Line 4
	CMP AX, DX
	JL L3
	JMP L5
L3:
	MOV AX, 1       ; Line 4
	JMP L4
L5:
	MOV AX, 0
L4:
	MOV [BP-4], AX
	PUSH AX
	POP AX
L6:
	MOV AX, 0       ; Line 5
	PUSH AX
	MOV AX, 2       ; Line 5
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	MOV AX, 10
	SUB AX, BX
	MOV BX, AX
	POP AX
	MOV SI, BX
	NEG SI
	MOV [BP+SI], AX
	PUSH AX
	POP AX
L7:
	MOV AX, [BP-2]       ; Line 6
	CMP AX, 0
	JNE L8
	JMP L10
L8:
	MOV AX, [BP-4]       ; Line 6
	CMP AX, 0
	JNE L9
	JMP L10
L9:
	MOV AX, 0       ; Line 7
	PUSH AX
	POP CX
	PUSH CX
	POP BX
	MOV AX, 2       ; Line 7
	MUL BX
	MOV BX, AX
	MOV AX, 10
	SUB AX, BX
	MOV BX, AX
	MOV SI, BX
	NEG SI
	MOV AX, [BP+SI]
	PUSH AX
	INC AX
	PUSH CX
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	MOV AX, 10
	SUB AX, BX
	MOV BX, AX
	POP AX
	MOV SI, BX
	NEG SI
	MOV [BP+SI], AX
	POP AX
	JMP L11
L10:
	MOV AX, 1       ; Line 9
	PUSH AX
	MOV AX, 0       ; Line 9
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 9
	MUL BX
	MOV BX, AX
	MOV AX, 10
	SUB AX, BX
	MOV BX, AX
	MOV SI, BX
	NEG SI
	MOV AX, [BP+SI]
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	MOV AX, 10
	SUB AX, BX
	MOV BX, AX
	POP AX
	MOV SI, BX
	NEG SI
	MOV [BP+SI], AX
	PUSH AX
	POP AX
L11:
	MOV AX, [BP-2]       ; Line 10
	CALL print_output
	CALL new_line
L12:
	MOV AX, [BP-4]       ; Line 11
	CALL print_output
	CALL new_line
L13:
L14:
	ADD SP, 10
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main