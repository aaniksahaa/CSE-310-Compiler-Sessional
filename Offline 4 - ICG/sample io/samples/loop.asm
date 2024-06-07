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
	SUB SP, 2
	SUB SP, 2
L1:
	MOV AX, 0       ; Line 3
	MOV [BP-4], AX
	PUSH AX
	POP AX
L2:
	MOV AX, 1       ; Line 4
	MOV [BP-6], AX
	PUSH AX
	POP AX
L3:
	MOV AX, 0       ; Line 5
	MOV [BP-8], AX
	PUSH AX
	POP AX
L4:
	MOV AX, 4       ; Line 5
	MOV DX, AX
	MOV AX, [BP-8]       ; Line 5
	CMP AX, DX
	JL L6
	JMP L12
L5:
	MOV AX, [BP-8]       ; Line 5
	PUSH AX
	INC AX
	MOV [BP-8], AX
	POP AX
	JMP L4
L6:
	MOV AX, 3       ; Line 6
	MOV [BP-2], AX
	PUSH AX
	POP AX
L7:
L8:
	MOV AX, [BP-2]       ; Line 7
	PUSH AX
	DEC AX
	MOV [BP-2], AX
	POP AX       ; Line 7
	CMP AX, 0
	JNE L9
	JMP L5
L9:
	MOV AX, [BP-4]       ; Line 8
	PUSH AX
	INC AX
	MOV [BP-4], AX
	POP AX
L10:
	JMP L8
L11:
	JMP L5
L12:
	MOV AX, [BP-2]       ; Line 11
	CALL print_output
	CALL new_line
L13:
	MOV AX, [BP-4]       ; Line 12
	CALL print_output
	CALL new_line
L14:
	MOV AX, [BP-6]       ; Line 13
	CALL print_output
	CALL new_line
L15:
L16:
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