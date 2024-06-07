.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
f PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L1:
	MOV AX, 5       ; Line 3
	MOV [BP-2], AX
	PUSH AX
	POP AX
L2:
L3:
	MOV AX, 0       ; Line 4
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 4
	CMP AX, DX
	JG L4
	JMP L7
L4:
	MOV AX, [BP+4]       ; Line 5
	PUSH AX
	INC AX
	MOV [BP+4], AX
	POP AX
L5:
	MOV AX, [BP-2]       ; Line 6
	PUSH AX
	DEC AX
	MOV [BP-2], AX
	POP AX
L6:
	JMP L3
L7:
	MOV AX, [BP+4]       ; Line 8
	MOV CX, AX
	MOV AX, 3       ; Line 8
	CWD
	MUL CX
	PUSH AX
	MOV AX, 7       ; Line 8
	MOV DX, AX
	POP AX       ; Line 8
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 8
	JMP L10
L8:
	MOV AX, 9       ; Line 9
	MOV [BP+4], AX
	PUSH AX
	POP AX
L9:
L10:
	ADD SP, 2
	POP BP
	RET 2
f ENDP
g PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
L11:
	MOV AX, [BP+6]       ; Line 15
	PUSH AX
	CALL f
	PUSH AX
	MOV AX, [BP+6]       ; Line 15
	MOV DX, AX
	POP AX       ; Line 15
	ADD AX, DX
	PUSH AX
	MOV AX, [BP+4]       ; Line 15
	MOV DX, AX
	POP AX       ; Line 15
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 15
	MOV [BP-2], AX
	PUSH AX
	POP AX
L12:
	MOV AX, 0       ; Line 17
	MOV [BP-4], AX
	PUSH AX
	POP AX
L13:
	MOV AX, 7       ; Line 17
	MOV DX, AX
	MOV AX, [BP-4]       ; Line 17
	CMP AX, DX
	JL L15
	JMP L21
L14:
	MOV AX, [BP-4]       ; Line 17
	PUSH AX
	INC AX
	MOV [BP-4], AX
	POP AX
	JMP L13
L15:
	MOV AX, 3       ; Line 18
	MOV CX, AX
	MOV AX, [BP-4]       ; Line 18
	CWD
	DIV CX
	PUSH DX
	MOV AX, 0       ; Line 18
	MOV DX, AX
	POP AX       ; Line 18
	CMP AX, DX
	JE L16
	JMP L18
L16:
	MOV AX, 5       ; Line 19
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 19
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 19
	MOV [BP-2], AX
	PUSH AX
	POP AX
L17:
	JMP L14
L18:
	MOV AX, 1       ; Line 22
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 22
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 22
	MOV [BP-2], AX
	PUSH AX
	POP AX
L19:
L20:
	JMP L14
L21:
	MOV AX, [BP-2]       ; Line 26
	JMP L23
L22:
L23:
	ADD SP, 4
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
	SUB SP, 2
L24:
	MOV AX, 1       ; Line 31
	MOV [BP-2], AX
	PUSH AX
	POP AX
L25:
	MOV AX, 2       ; Line 32
	MOV [BP-4], AX
	PUSH AX
	POP AX
L26:
	MOV AX, [BP-2]       ; Line 33
	PUSH AX
	MOV AX, [BP-4]       ; Line 33
	PUSH AX
	CALL g
	PUSH AX
	POP AX       ; Line 33
	MOV [BP-2], AX
	PUSH AX
	POP AX
L27:
	MOV AX, [BP-2]       ; Line 34
	CALL print_output
	CALL new_line
L28:
	MOV AX, 0       ; Line 35
	MOV [BP-6], AX
	PUSH AX
	POP AX
L29:
	MOV AX, 4       ; Line 35
	MOV DX, AX
	MOV AX, [BP-6]       ; Line 35
	CMP AX, DX
	JL L31
	JMP L38
L30:
	MOV AX, [BP-6]       ; Line 35
	PUSH AX
	INC AX
	MOV [BP-6], AX
	POP AX
	JMP L29
L31:
	MOV AX, 3       ; Line 36
	MOV [BP-2], AX
	PUSH AX
	POP AX
L32:
L33:
	MOV AX, 0       ; Line 37
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 37
	CMP AX, DX
	JG L34
	JMP L30
L34:
	MOV AX, [BP-4]       ; Line 38
	PUSH AX
	INC AX
	MOV [BP-4], AX
	POP AX
L35:
	MOV AX, [BP-2]       ; Line 39
	PUSH AX
	DEC AX
	MOV [BP-2], AX
	POP AX
L36:
	JMP L33
L37:
	JMP L30
L38:
	MOV AX, [BP-2]       ; Line 42
	CALL print_output
	CALL new_line
L39:
	MOV AX, [BP-4]       ; Line 43
	CALL print_output
	CALL new_line
L40:
	MOV AX, [BP-6]       ; Line 44
	CALL print_output
	CALL new_line
L41:
	MOV AX, 0       ; Line 45
	JMP L43
L42:
L43:
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