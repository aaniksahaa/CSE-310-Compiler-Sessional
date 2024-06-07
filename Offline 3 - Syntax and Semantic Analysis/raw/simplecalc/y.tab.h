#ifndef _yy_defines_h_
#define _yy_defines_h_

#define NUMBER 257
#define NEWLINE 258
#define PLUS 259
#define MINUS 260
#define SLASH 261
#define ASTERISK 262
#define LPAREN 263
#define RPAREN 264
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE {
    double value;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
