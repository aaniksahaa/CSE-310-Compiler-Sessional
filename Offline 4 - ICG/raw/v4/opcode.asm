.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	y DW 5 DUP (0000H)
	i DW 1 DUP (0000H)
	x DW 1 DUP (0000H)
.CODE
fact PROC
	PUSH BP			; Line 39
	MOV BP, SP
	MOV AX, [BP+4]			; Line 40
	PUSH AX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JE L3
	JMP L2
L3:
	MOV AX, 1			; Line 41
	JMP L1
L4:
L2:
	MOV AX, [BP+4]			; Line 43
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	CALL fact
	PUSH AX
	MOV AX, [BP+4]
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	JMP L1
L5:
L1:
	POP BP
	RET 2
fact ENDP
foo PROC
	PUSH BP			; Line 50
	MOV BP, SP
	MOV AX, [BP+6]			; Line 51
	PUSH AX
	MOV AX, [BP+4]
	POP CX
	ADD CX, AX
	PUSH CX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JLE L8
	JMP L7
L8:
	MOV AX, 7			; Line 52
	JMP L6
L9:
L7:
	MOV AX, [BP+6]			; Line 54
	PUSH AX
	MOV AX, 2
	POP CX
	SUB CX, AX
	PUSH CX
	MOV AX, [BP+4]
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	CALL foo
	PUSH AX
	MOV AX, 2
	PUSH AX
	MOV AX, [BP+6]
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	MOV AX, [BP+4]
	PUSH AX
	MOV AX, 2
	POP CX
	SUB CX, AX
	PUSH CX
	CALL foo
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	JMP L6
L10:
L6:
	POP BP
	RET 4
foo ENDP
h PROC
	PUSH BP			; Line 59
	MOV BP, SP
	SUB SP, 2			; Line 60
L12:
	MOV AX, 22			; Line 61
	MOV [BP-2], AX
L13:
	MOV AX, [BP-2]			; Line 62
	CALL print_output
	CALL new_line
L14:
	ADD SP, 2
L11:
	POP BP
	RET 
h ENDP
g PROC
	PUSH BP			; Line 65
	MOV BP, SP
L15:
	POP BP
	RET 
g ENDP
gg PROC
	PUSH BP			; Line 69
	MOV BP, SP
	MOV AX, [BP+6]			; Line 70
	PUSH AX
	MOV AX, [BP+4]
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH DX
	POP AX
	JMP L16
L17:
L16:
	POP BP
	RET 4
gg ENDP
hh PROC
	PUSH BP			; Line 73
	MOV BP, SP
	MOV AX, 0			; Line 74
	MOV i, AX
L20:
	MOV AX, i
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JLE L22
	JMP L19
L21:
	MOV AX, i
	PUSH AX
	INC AX
	MOV i, AX
	POP AX
	JMP L20
L22:
	MOV AX, 101			; Line 75
	PUSH AX
	MOV AX, i
	POP CX
	ADD CX, AX
	PUSH CX
	MOV AX, i
	PUSH AX
	POP BX
	SHL BX, 1
	POP AX
	MOV y[BX], AX
L24:
L23:
	JMP L21
L19:
L18:
	POP BP
	RET 
hh ENDP
f2 PROC
	PUSH BP			; Line 79
	MOV BP, SP
	SUB SP, 2			; Line 80
L26:
	MOV AX, 1			; Line 81
	ADD SP, 2
	JMP L25
L27:
	SUB SP, 10			; Line 82
L28:
	MOV AX, 2			; Line 83
	ADD SP, 12
	JMP L25
L29:
	SUB SP, 20			; Line 84
L30:
	MOV AX, 3			; Line 85
	ADD SP, 32
	JMP L25
L31:
	ADD SP, 32
L25:
	POP BP
	RET 
f2 ENDP
func PROC
	PUSH BP			; Line 88
	MOV BP, SP
	SUB SP, 2			; Line 89
L33:
	MOV AX, [BP+4]			; Line 90
	PUSH AX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JE L35
	JMP L34
L35:
	MOV AX, 0
	ADD SP, 2
	JMP L32
L34:
	MOV AX, [BP+4]			; Line 91
	MOV [BP-2], AX
L36:
	MOV AX, [BP+4]			; Line 92
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	CALL func
	PUSH AX
	MOV AX, [BP-2]
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	ADD SP, 2
	JMP L32
L37:
	ADD SP, 2
L32:
	POP BP
	RET 2
func ENDP
func2 PROC
	PUSH BP			; Line 95
	MOV BP, SP
	SUB SP, 2			; Line 96
L39:
	MOV AX, [BP+4]			; Line 97
	PUSH AX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JE L41
	JMP L40
L41:
	MOV AX, 0
	ADD SP, 2
	JMP L38
L40:
	MOV AX, [BP+4]			; Line 98
	MOV [BP-2], AX
L42:
	MOV AX, [BP+4]			; Line 99
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	CALL func
	PUSH AX
	MOV AX, [BP-2]
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	ADD SP, 2
	JMP L38
L43:
	ADD SP, 2
L38:
	POP BP
	RET 2
func2 ENDP
main PROC
	MOV AX, @DATA			; Line 102
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2			; Line 103
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
L45:
	MOV AX, 4			; Line 104
	PUSH AX
	CALL fact
	MOV [BP-2], AX
L46:
	MOV AX, [BP-2]			; Line 105
	CALL print_output
	CALL new_line
L47:
	MOV AX, 14			; Line 106
	PUSH AX
	MOV AX, 2
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH AX
	MOV AX, 15
	NEG AX
	PUSH AX
	MOV AX, 3
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH DX
	MOV AX, 3
	POP CX
	ADD CX, AX
	PUSH CX
	CALL foo
	MOV [BP-4], AX
L48:
	MOV AX, [BP-4]			; Line 107
	CALL print_output
	CALL new_line
L49:
	CALL h			; Line 108
L50:
	SUB SP, 10			; Line 109
L51:
	MOV AX, 0			; Line 111
	MOV [BP-16], AX
L53:
	MOV AX, [BP-16]
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JL L55
	JMP L52
L54:
	MOV AX, [BP-16]
	PUSH AX
	INC AX
	MOV [BP-16], AX
	POP AX
	JMP L53
L55:
	MOV AX, [BP-16]			; Line 112
	PUSH AX
	MOV AX, [BP-16]
	PUSH AX
	POP BX
	SHL BX, 1
	POP AX
	MOV y[BX], AX
L57:
L56:
	JMP L54
L52:
	MOV AX, 4			; Line 114
	MOV [BP-18], AX
L58:
L60:
	MOV AX, [BP-18]			; Line 115
	PUSH AX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JGE L61
	JMP L59
L61:
	MOV AX, [BP-18]			; Line 116
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	PUSH AX
	MOV AX, [BP-18]
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -30
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	POP AX
	MOV [BP+SI], AX
L62:
	MOV AX, [BP-18]			; Line 117
	PUSH AX
	DEC AX
	MOV [BP-18], AX
	POP AX
L63:
	JMP L60
L59:
	SUB SP, 2			; Line 119
L64:
	MOV AX, 0			; Line 120
	MOV [BP-32], AX
L65:
	MOV AX, 0			; Line 121
	MOV [BP-16], AX
L67:
	MOV AX, [BP-16]
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JL L69
	JMP L66
L68:
	MOV AX, [BP-16]
	PUSH AX
	INC AX
	MOV [BP-16], AX
	POP AX
	JMP L67
L69:
	MOV AX, [BP-16]			; Line 124
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -30
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	MOV AX, [BP+SI]
	PUSH AX
	MOV AX, 2
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH DX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JE L74
	JMP L75
L74:
	MOV AX, [BP-32]			; Line 126
	PUSH AX
	MOV AX, [BP-16]
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-32], AX
L77:
L76:
	JMP L73
L75:
	MOV AX, [BP-32]			; Line 130
	PUSH AX
	MOV AX, 10
	PUSH AX
	MOV AX, [BP-16]
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-32], AX
L78:
L73:
L72:
L71:
L70:
	JMP L68
L66:
	MOV AX, [BP-32]			; Line 136
	CALL print_output
	CALL new_line
L79:
	CALL g			; Line 138
L80:
	CALL g			; Line 139
L81:
	MOV AX, 2			; Line 141
	MOV [BP-2], AX
L82:
	MOV AX, [BP-2]			; Line 142
	PUSH AX
	MOV AX, 3
	POP CX
	CMP CX, AX
	JLE L84
	JMP L83
L84:
	SUB SP, 2			; Line 143
L85:
	MOV AX, 4			; Line 144
	MOV [BP-34], AX
L86:
	MOV AX, [BP-34]			; Line 145
	PUSH AX
	MOV AX, 2
	POP CX
	CMP CX, AX
	JGE L88
	JMP L87
L88:
	SUB SP, 2			; Line 147
L90:
	MOV AX, 7			; Line 148
	MOV [BP-36], AX
L91:
	SUB SP, 2			; Line 150
L93:
	MOV AX, 1			; Line 151
	MOV [BP-38], AX
L95:
	MOV AX, [BP-38]
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JLE L97
	JMP L94
L96:
	MOV AX, [BP-38]
	PUSH AX
	INC AX
	MOV [BP-38], AX
	POP AX
	JMP L95
L97:
	MOV AX, [BP-38]			; Line 152
	PUSH AX
	MOV AX, 2
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH DX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JE L100
	JMP L99
L100:
	MOV AX, [BP-38]			; Line 153
	CALL print_output
	CALL new_line
L101:
L103:
L102:
L99:
L98:
	JMP L96
L94:
	ADD SP, 2
L92:
	ADD SP, 2
L89:
L87:
	ADD SP, 2
L83:
	CALL f2			; Line 166
	MOV [BP-14], AX
L104:
	MOV AX, [BP-14]			; Line 167
	CALL print_output
	CALL new_line
L105:
	MOV AX, [BP-2]			; Line 169
	PUSH AX
	MOV AX, 15
	POP CX
	CMP CX, AX
	JG L109
	JMP L108
L109:
	MOV AX, [BP-4]
	PUSH AX
	MOV AX, 50
	POP CX
	CMP CX, AX
	JLE L107
L110:
	MOV AX, [BP-6]
	PUSH AX
	MOV AX, 67
	POP CX
	CMP CX, AX
	JE L107
	JMP L108
L107:
	MOV AX, 1
	JMP L111
L108:
	MOV AX, 0
L111:
	MOV [BP-14], AX
L106:
	MOV AX, [BP-14]			; Line 170
	CALL print_output
	CALL new_line
L112:
	MOV AX, [BP-2]			; Line 171
	PUSH AX
	MOV AX, 15
	POP CX
	CMP CX, AX
	JG L116
	JMP L115
L116:
	MOV AX, [BP-4]
	PUSH AX
	MOV AX, 50
	POP CX
	CMP CX, AX
	JLE L114
L117:
	MOV AX, [BP-6]
	PUSH AX
	MOV AX, 67
	POP CX
	CMP CX, AX
	JE L114
	JMP L115
L114:
	MOV AX, [BP-2]			; Line 172
	CALL print_output
	CALL new_line
L118:
	JMP L113
L115:
	SUB SP, 2			; Line 174
L119:
	MOV AX, 67			; Line 175
	MOV [BP-34], AX
L120:
	MOV AX, [BP-34]			; Line 178
	CALL print_output
	CALL new_line
L123:
L122:
L121:
	ADD SP, 2
L113:
	MOV AX, 0			; Line 183
	MOV [BP-14], AX
L124:
	MOV AX, 1			; Line 185
	MOV [BP-2], AX
L126:
	MOV AX, [BP-2]
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JLE L128
	JMP L125
L127:
	MOV AX, [BP-2]
	PUSH AX
	INC AX
	MOV [BP-2], AX
	POP AX
	JMP L126
L128:
	SUB SP, 2			; Line 186
L130:
	MOV AX, 0			; Line 187
	MOV [BP-34], AX
L131:
	MOV AX, 1			; Line 188
	MOV [BP-4], AX
L132:
L134:
	MOV AX, [BP-4]			; Line 189
	PUSH AX
	MOV AX, [BP-2]
	POP CX
	CMP CX, AX
	JLE L135
	JMP L133
L135:
	MOV AX, [BP-34]			; Line 190
	PUSH AX
	MOV AX, [BP-4]
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-34], AX
L136:
	MOV AX, [BP-4]			; Line 191
	PUSH AX
	MOV AX, 2
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH DX
	MOV AX, 0
	POP CX
	CMP CX, AX
	JE L138
	JMP L137
L138:
	MOV AX, [BP-34]			; Line 192
	PUSH AX
	MOV AX, 10
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-34], AX
L139:
L137:
	MOV AX, [BP-4]			; Line 194
	PUSH AX
	INC AX
	MOV [BP-4], AX
	POP AX
L140:
	JMP L134
L133:
	MOV AX, [BP-14]			; Line 196
	PUSH AX
	MOV AX, [BP-34]
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-14], AX
L141:
	ADD SP, 2
L129:
	JMP L127
L125:
	MOV AX, [BP-14]			; Line 199
	CALL print_output
	CALL new_line
L142:
	MOV AX, 4			; Line 202
	MOV [BP-12], AX
L143:
	MOV AX, 1			; Line 203
	MOV [BP-16], AX
L145:
	MOV AX, [BP-16]
	PUSH AX
	MOV AX, [BP-12]
	POP CX
	CMP CX, AX
	JLE L147
	JMP L144
L146:
	MOV AX, [BP-16]
	PUSH AX
	INC AX
	MOV [BP-16], AX
	POP AX
	JMP L145
L147:
	MOV AX, 0			; Line 204
	MOV [BP-14], AX
L149:
	MOV AX, 1			; Line 205
	MOV [BP-18], AX
L150:
L152:
	MOV AX, [BP-18]			; Line 206
	PUSH AX
	MOV AX, [BP-16]
	POP CX
	CMP CX, AX
	JLE L153
	JMP L151
L153:
	MOV AX, [BP-14]			; Line 207
	PUSH AX
	MOV AX, 10
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	MOV AX, 1
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-14], AX
L154:
	MOV AX, [BP-18]			; Line 208
	PUSH AX
	INC AX
	MOV [BP-18], AX
	POP AX
L155:
	JMP L152
L151:
	MOV AX, [BP-14]			; Line 210
	CALL print_output
	CALL new_line
L156:
L148:
	JMP L146
L144:
	MOV AX, [BP-12]			; Line 212
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	POP AX
	MOV [BP-16], AX
L158:
	MOV AX, [BP-16]
	PUSH AX
	MOV AX, 1
	POP CX
	CMP CX, AX
	JGE L160
	JMP L157
L159:
	MOV AX, [BP-16]
	PUSH AX
	DEC AX
	MOV [BP-16], AX
	POP AX
	JMP L158
L160:
	MOV AX, 0			; Line 213
	MOV [BP-14], AX
L162:
	MOV AX, 1			; Line 214
	MOV [BP-18], AX
L163:
L165:
	MOV AX, [BP-18]			; Line 215
	PUSH AX
	MOV AX, [BP-16]
	POP CX
	CMP CX, AX
	JLE L166
	JMP L164
L166:
	MOV AX, [BP-14]			; Line 216
	PUSH AX
	MOV AX, 10
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	MOV AX, 1
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-14], AX
L167:
	MOV AX, [BP-18]			; Line 217
	PUSH AX
	INC AX
	MOV [BP-18], AX
	POP AX
L168:
	JMP L165
L164:
	MOV AX, [BP-14]			; Line 219
	CALL print_output
	CALL new_line
L169:
L161:
	JMP L159
L157:
	MOV AX, 78			; Line 222
	MOV [BP-8], AX
L170:
	MOV AX, 2			; Line 223
	PUSH AX
	MOV AX, 2
	POP CX
	ADD CX, AX
	PUSH CX
	MOV AX, 6
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JG L173
	JMP L174
L173:
	MOV AX, 1
	JMP L175
L174:
	MOV AX, 0
L175:
	PUSH AX
	MOV AX, 6
	PUSH AX
	MOV AX, 7
	POP CX
	CMP CX, AX
	JL L176
	JMP L177
L176:
	MOV AX, 1
	JMP L178
L177:
	MOV AX, 0
L178:
	POP CX
	ADD CX, AX
	PUSH CX
	MOV AX, 2
	PUSH AX
	MOV AX, 3
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	MOV AX, 7
	PUSH AX
	MOV AX, 1
	POP CX
	SUB CX, AX
	PUSH CX
	POP AX
	POP CX
	CMP CX, AX
	JGE L179
	JMP L180
L179:
	MOV AX, 1
	JMP L181
L180:
	MOV AX, 0
L181:
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	CALL gg
	CMP AX, 0
	JNE L171
L172:
	MOV AX, [BP-8]			; Line 224
	CALL print_output
	CALL new_line
L182:
L171:
	MOV AX, 3			; Line 227
	PUSH AX
	CALL fact
	PUSH AX
	CALL fact
	MOV [BP-8], AX
L183:
	MOV AX, [BP-8]			; Line 229
	CALL print_output
	CALL new_line
L184:
	CALL hh			; Line 231
L185:
	MOV AX, 0			; Line 233
	MOV [BP-14], AX
L186:
	MOV AX, 0			; Line 234
	MOV [BP-18], AX
L187:
L189:
	MOV AX, [BP-18]			; Line 235
	PUSH AX
	MOV AX, 5
	POP CX
	CMP CX, AX
	JL L190
	JMP L188
L190:
	MOV AX, [BP-14]			; Line 236
	PUSH AX
	MOV AX, [BP-18]
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	POP CX
	ADD CX, AX
	PUSH CX
	POP AX
	MOV [BP-14], AX
L191:
	MOV AX, [BP-18]			; Line 237
	PUSH AX
	INC AX
	MOV [BP-18], AX
	POP AX
L192:
	JMP L189
L188:
	MOV AX, [BP-14]			; Line 240
	CALL print_output
	CALL new_line
L193:
	MOV AX, 3			; Line 242
	PUSH AX
	CALL fact
	PUSH AX
	MOV AX, 12
	NEG AX
	PUSH AX
	MOV AX, 3
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	PUSH DX
	POP AX
	POP CX
	ADD CX, AX
	PUSH CX
	MOV AX, 1
	PUSH AX
	MOV AX, 3
	PUSH AX
	POP CX
	POP AX
	CWD
	MUL CX
	PUSH AX
	MOV AX, 1
	PUSH AX
	POP CX
	POP AX
	CWD
	IDIV CX
	POP CX
	SUB CX, AX
	PUSH CX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	MOV [BP-14], AX
L194:
	MOV AX, [BP-14]			; Line 244
	CALL print_output
	CALL new_line
L195:
	MOV AX, 2			; Line 246
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	PUSH AX
	MOV AX, 101
	POP CX
	SUB CX, AX
	PUSH CX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	PUSH AX
	MOV AX, 101
	POP CX
	SUB CX, AX
	PUSH CX
	POP BX
	SHL BX, 1
	MOV AX, y[BX]
	MOV [BP-14], AX
L196:
	MOV AX, [BP-14]			; Line 248
	CALL print_output
	CALL new_line
L197:
	SUB SP, 200			; Line 250
L198:
	MOV AX, 5			; Line 252
	PUSH AX
	MOV AX, 0
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -232
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	POP AX
	MOV [BP+SI], AX
L199:
	MOV AX, 0			; Line 253
	PUSH AX
	MOV AX, 5
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -232
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	POP AX
	MOV [BP+SI], AX
L200:
	MOV AX, 0			; Line 255
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -232
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	MOV AX, [BP+SI]
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -232
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	MOV AX, [BP+SI]
	PUSH AX
	POP BX
	SHL BX, 1
	MOV AX, -232
	ADD AX, BX
	MOV BX, AX
	MOV SI, BX
	MOV AX, [BP+SI]
	MOV [BP-14], AX
L201:
	MOV AX, [BP-14]			; Line 257
	CALL print_output
	CALL new_line
L202:
	MOV AX, 7			; Line 259
	PUSH AX
	CALL func
	MOV [BP-14], AX
L203:
	MOV AX, [BP-14]			; Line 261
	CALL print_output
	CALL new_line
L204:
	MOV AX, 0			; Line 263
	MOV [BP-4], AX
L205:
	MOV AX, 1			; Line 264
	MOV [BP-6], AX
L206:
	MOV AX, 0			; Line 265
	MOV [BP-16], AX
L208:
	MOV AX, [BP-16]
	PUSH AX
	MOV AX, 4
	POP CX
	CMP CX, AX
	JL L210
	JMP L207
L209:
	MOV AX, [BP-16]
	PUSH AX
	INC AX
	MOV [BP-16], AX
	POP AX
	JMP L208
L210:
	MOV AX, 3			; Line 266
	MOV [BP-2], AX
L212:
L214:
	MOV AX, [BP-2]			; Line 267
	PUSH AX
	DEC AX
	MOV [BP-2], AX
	POP AX
	CMP AX, 0
	JNE L215
	JMP L213
L215:
	MOV AX, [BP-4]			; Line 268
	PUSH AX
	INC AX
	MOV [BP-4], AX
	POP AX
L216:
	JMP L214
L213:
L211:
	JMP L209
L207:
	MOV AX, [BP-2]			; Line 271
	CALL print_output
	CALL new_line
L217:
	MOV AX, [BP-4]			; Line 272
	CALL print_output
	CALL new_line
L218:
	MOV AX, [BP-6]			; Line 273
	CALL print_output
	CALL new_line
L219:
	MOV AX, 3			; Line 275
	MOV [BP-2], AX
L220:
	MOV AX, 100			; Line 276
	MOV [BP-4], AX
L221:
	MOV AX, 101			; Line 277
	MOV [BP-6], AX
L222:
	MOV AX, 102			; Line 278
	MOV [BP-8], AX
L223:
	MOV AX, 103			; Line 279
	MOV [BP-10], AX
L224:
	MOV AX, [BP-2]			; Line 281
	PUSH AX
	MOV AX, 1
	POP CX
	CMP CX, AX
	JE L226
	JMP L227
L226:
	MOV AX, [BP-4]			; Line 282
	CALL print_output
	CALL new_line
L228:
	JMP L225
L227:
	MOV AX, [BP-2]			; Line 283
	PUSH AX
	MOV AX, 2
	POP CX
	CMP CX, AX
	JE L229
	JMP L230
L229:
	MOV AX, [BP-6]			; Line 284
	CALL print_output
	CALL new_line
L231:
	JMP L225
L230:
	MOV AX, [BP-2]			; Line 285
	PUSH AX
	MOV AX, 3
	POP CX
	CMP CX, AX
	JE L232
	JMP L233
L232:
	MOV AX, [BP-8]			; Line 286
	CALL print_output
	CALL new_line
L234:
	JMP L225
L233:
	MOV AX, [BP-10]			; Line 288
	CALL print_output
	CALL new_line
L235:
L225:
	MOV AX, 0			; Line 292
	ADD SP, 232
	JMP L44
L236:
	ADD SP, 232
L44:
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