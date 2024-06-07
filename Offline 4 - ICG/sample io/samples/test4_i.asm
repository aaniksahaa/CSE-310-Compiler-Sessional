.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	a DW 1 DUP (0000H)
	b DW 1 DUP (0000H)
	c DW 1 DUP (0000H)
.CODE
func_a PROC
	PUSH BP
	MOV BP, SP
	MOV AX, 7       ; Line 4
	MOV a, AX
	PUSH AX
	POP AX
L1:
L2:
	POP BP
	RET 
func_a ENDP
foo PROC
	PUSH BP
	MOV BP, SP
	MOV AX, 3       ; Line 8
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 8
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 8
	MOV [BP+4], AX
	PUSH AX
	POP AX
L3:
	MOV AX, [BP+4]       ; Line 9
	JMP L5
L4:
L5:
	POP BP
	RET 2
foo ENDP
bar PROC
	PUSH BP
	MOV BP, SP
	MOV AX, [BP+6]       ; Line 14
	MOV CX, AX
	MOV AX, 4       ; Line 14
	CWD
	MUL CX
	PUSH AX
	MOV AX, [BP+4]       ; Line 14
	MOV CX, AX
	MOV AX, 2       ; Line 14
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 14
	MOV DX, AX
	POP AX       ; Line 14
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 14
	MOV c, AX
	PUSH AX
	POP AX
L6:
	MOV AX, c       ; Line 15
	JMP L8
L7:
L8:
	POP BP
	RET 4
bar ENDP
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
L9:
	MOV AX, 5       ; Line 22
	MOV [BP-2], AX
	PUSH AX
	POP AX
L10:
	MOV AX, 6       ; Line 23
	MOV [BP-4], AX
	PUSH AX
	POP AX
L11:
	CALL func_a       ; Line 25
	PUSH AX
	POP AX
L12:
	MOV AX, a       ; Line 26
	CALL print_output
	CALL new_line
L13:
	MOV AX, [BP-2]       ; Line 28
	PUSH AX
	CALL foo
	PUSH AX
	POP AX       ; Line 28
	MOV [BP-6], AX
	PUSH AX
	POP AX
L14:
	MOV AX, [BP-6]       ; Line 29
	CALL print_output
	CALL new_line
L15:
	MOV AX, [BP-2]       ; Line 31
	PUSH AX
	MOV AX, [BP-4]       ; Line 31
	PUSH AX
	CALL bar
	PUSH AX
	POP AX       ; Line 31
	MOV [BP-8], AX
	PUSH AX
	POP AX
L16:
	MOV AX, [BP-8]       ; Line 32
	CALL print_output
	CALL new_line
L17:
	MOV AX, [BP-2]       ; Line 34
	PUSH AX
	MOV AX, [BP-4]       ; Line 34
	PUSH AX
	CALL bar
	PUSH AX
	POP AX       ; Line 34
	MOV CX, AX
	MOV AX, 6       ; Line 34
	CWD
	MUL CX
	PUSH AX
	MOV AX, 2       ; Line 34
	MOV DX, AX
	POP AX       ; Line 34
	ADD AX, DX
	PUSH AX
	MOV AX, [BP-2]       ; Line 34
	PUSH AX
	CALL foo
	PUSH AX
	POP AX       ; Line 34
	MOV CX, AX
	MOV AX, 3       ; Line 34
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 34
	MOV DX, AX
	POP AX       ; Line 34
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 34
	MOV [BP-4], AX
	PUSH AX
	POP AX
L18:
	MOV AX, [BP-4]       ; Line 35
	CALL print_output
	CALL new_line
L19:
	MOV AX, 0       ; Line 38
	JMP L21
L20:
L21:
	ADD SP, 8
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main