.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	a DW 1 DUP (0000H)
	b DW 1 DUP (0000H)
	c DW 1 DUP (0000H)
.CODE
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L1:
	MOV AX, 1       ; Line 5
	MOV [BP-2], AX
	PUSH AX
	POP AX
L2:
	MOV AX, 10       ; Line 5
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 5
	CMP AX, DX
	JLE L4
	JMP L6
L3:
	MOV AX, [BP-2]       ; Line 5
	PUSH AX
	INC AX
	MOV [BP-2], AX
	POP AX
	JMP L2
L4:
	MOV AX, [BP-2]       ; Line 6
	CALL print_output
	CALL new_line
L5:
	JMP L3
L6:
L7:
	ADD SP, 2
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
;-------------------------------
END main