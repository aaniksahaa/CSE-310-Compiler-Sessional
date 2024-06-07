#ifndef _yy_defines_h_
#define _yy_defines_h_

#define IF 257
#define FLOAT 258
#define FOR 259
#define INT 260
#define VOID 261
#define ELSE 262
#define WHILE 263
#define RETURN 264
#define PRINTLN 265
#define ADDOP 266
#define MULOP 267
#define INCOP 268
#define DECOP 269
#define RELOP 270
#define ASSIGNOP 271
#define LOGICOP 272
#define BITOP 273
#define NOT 274
#define LPAREN 275
#define RPAREN 276
#define LCURL 277
#define RCURL 278
#define LTHIRD 279
#define RTHIRD 280
#define COMMA 281
#define SEMICOLON 282
#define CONST_INT 283
#define CONST_FLOAT 284
#define ID 285
#define CONST_CHAR 286
#define SINGLE_LINE_STRING 287
#define MULTI_LINE_STRING 288
#define LOWER_THAN_ELSE 289
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE {
	SymbolInfo* info;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
