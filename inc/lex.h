#ifndef LEX_H
#define LEX_H
enum{
  EOI, /* End of input. */
  SEMI,/*;*/
  PLUS,/*+*/
  TIMES,/***/
  MINUS,
  DIV,
  LP,/*(*/
  RP,/*)*/
  EQUAL, /*=*/
  LT, GT, /* < > */
  ASSIGN, /* := */
  IF,
  THEN,
  WHILE,
  DO,
  BEGIN,
  END,
  NUM, ID /* Decimal Number or Identifier */
};

extern char *yytext;		/* in lex.c			*/
extern int yyleng;
extern int yylineno;

int match(int token);
void advance(void);
#endif
