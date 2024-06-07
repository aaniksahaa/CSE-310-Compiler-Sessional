.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
func PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L1:
	MOV AX, 0       ; Line 3
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 3
	CMP AX, DX
	JE L2
	JMP L3
L2:
	MOV AX, 0       ; Line 3
	JMP L6
L3:
	MOV AX, [BP+4]       ; Line 4
	MOV [BP-2], AX
	PUSH AX
	POP AX
L4:
	MOV AX, 1       ; Line 5
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 5
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 5
	PUSH AX
	CALL func
	PUSH AX
	MOV AX, [BP-2]       ; Line 5
	MOV DX, AX
	POP AX       ; Line 5
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 5
	JMP L6
L5:
L6:
	ADD SP, 2
	POP BP
	RET 2
func ENDP
func2 PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L7:
	MOV AX, 0       ; Line 10
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 10
	CMP AX, DX
	JE L8
	JMP L9
L8:
	MOV AX, 0       ; Line 10
	JMP L12
L9:
	MOV AX, [BP+4]       ; Line 11
	MOV [BP-2], AX
	PUSH AX
	POP AX
L10:
	MOV AX, 1       ; Line 12
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 12
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 12
	PUSH AX
	CALL func
	PUSH AX
	MOV AX, [BP-2]       ; Line 12
	MOV DX, AX
	POP AX       ; Line 12
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 12
	JMP L12
L11:
L12:
	ADD SP, 2
	POP BP
	RET 2
func2 ENDP
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L13:
	MOV AX, 7       ; Line 17
	PUSH AX
	CALL func
	PUSH AX
	POP AX       ; Line 17
	MOV [BP-2], AX
	PUSH AX
	POP AX
L14:
	MOV AX, [BP-2]       ; Line 18
	CALL print_output
	CALL new_line
L15:
	MOV AX, 0       ; Line 19
	JMP L17
L16:
L17:
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