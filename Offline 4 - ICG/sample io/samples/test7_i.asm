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
L1:
	MOV AX, 0       ; Line 3
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 3
	CMP AX, DX
	JG L3
	JMP L2
L2:
	MOV AX, 10       ; Line 3
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 3
	CMP AX, DX
	JL L3
	JMP L4
L3:
	MOV AX, 100       ; Line 4
	MOV [BP-2], AX
	PUSH AX
	POP AX
	JMP L5
L4:
	MOV AX, 200       ; Line 6
	MOV [BP-2], AX
	PUSH AX
	POP AX
L5:
	MOV AX, 20       ; Line 8
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 8
	CMP AX, DX
	JG L6
	JMP L8
L6:
	MOV AX, 30       ; Line 8
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 8
	CMP AX, DX
	JL L7
	JMP L8
L7:
	MOV AX, 300       ; Line 9
	MOV [BP-2], AX
	PUSH AX
	POP AX
	JMP L9
L8:
	MOV AX, 400       ; Line 11
	MOV [BP-2], AX
	PUSH AX
	POP AX
L9:
	MOV AX, 40       ; Line 13
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 13
	CMP AX, DX
	JG L10
	JMP L11
L10:
	MOV AX, 50       ; Line 13
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 13
	CMP AX, DX
	JL L13
	JMP L11
L11:
	MOV AX, 60       ; Line 13
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 13
	CMP AX, DX
	JL L12
	JMP L14
L12:
	MOV AX, 70       ; Line 13
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 13
	CMP AX, DX
	JG L13
	JMP L14
L13:
	MOV AX, 500       ; Line 14
	MOV [BP-2], AX
	PUSH AX
	POP AX
	JMP L15
L14:
	MOV AX, 600       ; Line 16
	MOV [BP-2], AX
	PUSH AX
	POP AX
L15:
	MOV AX, [BP-2]       ; Line 17
	CALL print_output
	CALL new_line
L16:
	MOV AX, 0       ; Line 19
	JMP L18
L17:
L18:
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