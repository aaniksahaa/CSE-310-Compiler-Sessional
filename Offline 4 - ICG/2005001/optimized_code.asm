.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
main PROC
	MOV AX, @DATA			; Line 7
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2			; Line 8
L2:
	MOV AX, [BP-2]			; Line 9
	PUSH AX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JG L4
L6:
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, 10
	POP CX
	CMP CX, AX
	JL L4
	JMP L5
L4:
	MOV AX, 100			; Line 10
	MOV [BP-2], AX
	JMP L3
L5:
	MOV AX, 200			; Line 12
	MOV [BP-2], AX
L3:
	MOV AX, [BP-2]			; Line 14
	PUSH AX
	MOV AX, 20
	POP CX
	CMP CX, AX
	JG L10
	JMP L9
L10:
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, 30
	POP CX
	CMP CX, AX
	JL L8
	JMP L9
L8:
	MOV AX, 300			; Line 15
	MOV [BP-2], AX
	JMP L7
L9:
	MOV AX, 400			; Line 17
	MOV [BP-2], AX
L7:
	MOV AX, [BP-2]			; Line 19
	PUSH AX
	MOV AX, 40
	POP CX
	CMP CX, AX
	JG L15
	JMP L14
L15:
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, 50
	POP CX
	CMP CX, AX
	JL L12
L14:
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, 60
	POP CX
	CMP CX, AX
	JL L16
	JMP L13
L16:
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, 70
	POP CX
	CMP CX, AX
	JG L12
	JMP L13
L12:
	MOV AX, 500			; Line 20
	MOV [BP-2], AX
	JMP L11
L13:
	MOV AX, 600			; Line 22
	MOV [BP-2], AX
L11:
	MOV AX, [BP-2]			; Line 23
	CALL print_output
	CALL new_line
L17:
	MOV AX, 0			; Line 25
	ADD SP, 2
	JMP L1
L18:
	ADD SP, 2
L1:
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP

;-------------------------------
;         print library         
;-------------------------------

new_line proc
    push ax
    push dx
    mov ah,2
    mov dl,0Dh
    int 21h
    mov ah,2
    mov dl,0Ah
    int 21h
    pop dx
    pop ax
    ret
new_line endp
print_output proc  ;print what is in ax
    push ax
    push bx
    push cx
    push dx
    push si
    lea si,number
    mov bx,10
    add si,4
    cmp ax,0
    jnge negate
    print:
    xor dx,dx
    div bx
    mov [si],dl
    add [si],'0'
    dec si
    cmp ax,0
    jne print
    inc si
    lea dx,si
    mov ah,9
    int 21h
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
    negate:
    push ax
    mov ah,2
    mov dl,'-'
    int 21h
    pop ax
    neg ax
    jmp print
print_output endp

END main
