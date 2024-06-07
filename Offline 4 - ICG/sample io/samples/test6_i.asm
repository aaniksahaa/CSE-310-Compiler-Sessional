.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	w DW 10 DUP (0000H)
.CODE
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L1:
	SUB SP, 20
L2:
	MOV AX, 0       ; Line 6
	PUSH AX
	MOV AX, 2       ; Line 6
	NEG AX
	PUSH AX
	POP AX       ; Line 6
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	POP AX
	MOV w[BX], AX
	PUSH AX
	POP AX
L3:
	MOV AX, 0       ; Line 7
	PUSH AX
	MOV AX, 0       ; Line 7
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 7
	MUL BX
	MOV BX, AX
	MOV AX, w[BX]
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	MOV AX, 22
	SUB AX, BX
	MOV BX, AX
	POP AX
	MOV SI, BX
	NEG SI
	MOV [BP+SI], AX
	PUSH AX
	POP AX
L4:
	MOV AX, 0       ; Line 8
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 8
	MUL BX
	MOV BX, AX
	MOV AX, 22
	SUB AX, BX
	MOV BX, AX
	MOV SI, BX
	NEG SI
	MOV AX, [BP+SI]
	MOV [BP-2], AX
	PUSH AX
	POP AX
L5:
	MOV AX, [BP-2]       ; Line 9
	CALL print_output
	CALL new_line
L6:
	MOV AX, 1       ; Line 10
	PUSH AX
	MOV AX, 0       ; Line 10
	PUSH AX
	POP CX
	PUSH CX
	POP BX
	MOV AX, 2       ; Line 10
	MUL BX
	MOV BX, AX
	MOV AX, w[BX]
	PUSH AX
	INC AX
	PUSH CX
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	POP AX
	MOV w[BX], AX
	POP AX       ; Line 10
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	MOV AX, 22
	SUB AX, BX
	MOV BX, AX
	POP AX
	MOV SI, BX
	NEG SI
	MOV [BP+SI], AX
	PUSH AX
	POP AX
L7:
	MOV AX, 1       ; Line 11
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 11
	MUL BX
	MOV BX, AX
	MOV AX, 22
	SUB AX, BX
	MOV BX, AX
	MOV SI, BX
	NEG SI
	MOV AX, [BP+SI]
	MOV [BP-2], AX
	PUSH AX
	POP AX
L8:
	MOV AX, [BP-2]       ; Line 12
	CALL print_output
	CALL new_line
L9:
	MOV AX, 0       ; Line 13
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 13
	MUL BX
	MOV BX, AX
	MOV AX, w[BX]
	MOV [BP-2], AX
	PUSH AX
	POP AX
L10:
	MOV AX, [BP-2]       ; Line 14
	CALL print_output
	CALL new_line
L11:
	MOV AX, 0       ; Line 16
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 16
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 16
	MOV [BP-2], AX
	PUSH AX
	POP AX
L12:
	MOV AX, 0       ; Line 17
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 17
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 17
	MOV [BP-2], AX
	PUSH AX
	POP AX
L13:
	MOV AX, 1       ; Line 18
	MOV CX, AX
	MOV AX, [BP-2]       ; Line 18
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 18
	MOV [BP-2], AX
	PUSH AX
	POP AX
L14:
	MOV AX, [BP-2]       ; Line 19
	CALL print_output
	CALL new_line
L15:
	MOV AX, 0       ; Line 21
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 21
	CMP AX, DX
	JG L16
	JMP L17
L16:
	MOV AX, 10       ; Line 21
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 21
	CMP AX, DX
	JL L19
	JMP L17
L17:
	MOV AX, 0       ; Line 21
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 21
	CMP AX, DX
	JL L18
	JMP L20
L18:
	MOV AX, 10       ; Line 21
	NEG AX
	PUSH AX
	POP AX       ; Line 21
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 21
	CMP AX, DX
	JG L19
	JMP L20
L19:
	MOV AX, 100       ; Line 22
	MOV [BP-2], AX
	PUSH AX
	POP AX
	JMP L21
L20:
	MOV AX, 200       ; Line 24
	MOV [BP-2], AX
	PUSH AX
	POP AX
L21:
	MOV AX, [BP-2]       ; Line 25
	CALL print_output
	CALL new_line
L22:
	MOV AX, 0       ; Line 27
	JMP L24
L23:
L24:
	ADD SP, 22
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main