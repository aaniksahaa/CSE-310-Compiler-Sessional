.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	i DW 1 DUP (0000H)
	j DW 1 DUP (0000H)
.CODE
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
L1:
	MOV AX, 1       ; Line 6
	MOV i, AX
	PUSH AX
	POP AX
L2:
	MOV AX, i       ; Line 7
	CALL print_output
	CALL new_line
L3:
	MOV AX, 8       ; Line 9
	MOV DX, AX
	MOV AX, 5       ; Line 9
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 9
	MOV j, AX
	PUSH AX
	POP AX
L4:
	MOV AX, j       ; Line 10
	CALL print_output
	CALL new_line
L5:
	MOV AX, j       ; Line 12
	MOV CX, AX
	MOV AX, 2       ; Line 12
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 12
	MOV DX, AX
	MOV AX, i       ; Line 12
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 12
	MOV [BP-2], AX
	PUSH AX
	POP AX
L6:
	MOV AX, [BP-2]       ; Line 13
	CALL print_output
	CALL new_line
L7:
	MOV AX, 9       ; Line 15
	MOV CX, AX
	MOV AX, [BP-2]       ; Line 15
	CWD
	DIV CX
	PUSH DX
	POP AX       ; Line 15
	MOV [BP-6], AX
	PUSH AX
	POP AX
L8:
	MOV AX, [BP-6]       ; Line 16
	CALL print_output
	CALL new_line
L9:
	MOV AX, [BP-4]       ; Line 18
	MOV DX, AX
	MOV AX, [BP-6]       ; Line 18
	CMP AX, DX
	JLE L10
	JMP L12
L10:
	MOV AX, 1       ; Line 18
	JMP L11
L12:
	MOV AX, 0
L11:
	MOV [BP-8], AX
	PUSH AX
	POP AX
L13:
	MOV AX, [BP-8]       ; Line 19
	CALL print_output
	CALL new_line
L14:
	MOV AX, j       ; Line 21
	MOV DX, AX
	MOV AX, i       ; Line 21
	CMP AX, DX
	JNE L15
	JMP L17
L15:
	MOV AX, 1       ; Line 21
	JMP L16
L17:
	MOV AX, 0
L16:
	MOV [BP-10], AX
	PUSH AX
	POP AX
L18:
	MOV AX, [BP-10]       ; Line 22
	CALL print_output
	CALL new_line
L19:
	MOV AX, [BP-8]       ; Line 24
	CMP AX, 0
	JNE L21
	JMP L20
L20:
	MOV AX, [BP-10]       ; Line 24
	CMP AX, 0
	JNE L21
	JMP L23
L21:
	MOV AX, 1       ; Line 24
	JMP L22
L23:
	MOV AX, 0
L22:
	MOV [BP-12], AX
	PUSH AX
	POP AX
L24:
	MOV AX, [BP-12]       ; Line 25
	CALL print_output
	CALL new_line
L25:
	MOV AX, [BP-8]       ; Line 27
	CMP AX, 0
	JNE L26
	JMP L29
L26:
	MOV AX, [BP-10]       ; Line 27
	CMP AX, 0
	JNE L27
	JMP L29
L27:
	MOV AX, 1       ; Line 27
	JMP L28
L29:
	MOV AX, 0
L28:
	MOV [BP-12], AX
	PUSH AX
	POP AX
L30:
	MOV AX, [BP-12]       ; Line 28
	CALL print_output
	CALL new_line
L31:
	MOV AX, [BP-12]       ; Line 30
	PUSH AX
	INC AX
	MOV [BP-12], AX
	POP AX
L32:
	MOV AX, [BP-12]       ; Line 31
	CALL print_output
	CALL new_line
L33:
	MOV AX, [BP-12]       ; Line 33
	NEG AX
	PUSH AX
	POP AX       ; Line 33
	MOV [BP-2], AX
	PUSH AX
	POP AX
L34:
	MOV AX, [BP-2]       ; Line 34
	CALL print_output
	CALL new_line
L35:
	MOV AX, 0       ; Line 36
	JMP L37
L36:
L37:
	ADD SP, 12
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main