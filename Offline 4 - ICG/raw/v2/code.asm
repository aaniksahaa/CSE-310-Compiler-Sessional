.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
f PROC
	PUSH BP
	MOV BP, SP
	MOV AX, 2
	PUSH AX
	MOV AX, [BP+4]
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	POP AX
	ADD SP, 0
	POP BP
	RET 2
f ENDP
g PROC
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	MOV AX, [BP+6]
	PUSH AX
	CALL f
	PUSH AX
	MOV AX, [BP+6]
	PUSH AX
	POP CX
	POP AX
	ADD AX, CX
	PUSH AX
	MOV AX, [BP+4]
	PUSH AX
	POP CX
	POP AX
	ADD AX, CX
	PUSH AX
	POP AX
	MOV [BP-2], AX
	PUSH AX
	POP AX
	MOV AX, [BP-2]
	PUSH AX
	POP AX
	ADD SP, 2
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
	SUB SP, 10
	SUB SP, 2
	MOV AX, 1
	PUSH AX
	POP AX
	MOV [BP-2], AX
	PUSH AX
	POP AX
	MOV AX, 1
	PUSH AX
	MOV AX, 2
	PUSH AX
	MOV AX, 3
	PUSH AX
	POP CX
	POP AX
	ADD AX, CX
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	MOV AX, 3
	PUSH AX
	POP CX
	POP AX
	CWD
	DIV CX
	PUSH DX
	POP AX
	MOV [BP-4], AX
	PUSH AX
	POP AX
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, [BP-4]
	PUSH AX
	CALL g
	PUSH AX
	POP AX
	MOV [BP-2], AX
	PUSH AX
	POP AX
	MOV AX, [BP-2]
	CALL print_output
	CALL new_line
	MOV AX, 0
	PUSH AX
	POP AX
	ADD SP, 16
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
