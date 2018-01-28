#include "code_gen.h"
#include "name.h"
#include "lex.h"

#include <stdio.h>
#include <stdbool.h>

static char *factor(void);
static char *term(void);
static char *expression(void);
static char *condition(void);

/*
  Precedence:
  * /
  + -
  > < =

  Left to right associative
*/

void statements()
{
  /*  statements -> expression SEMI  |  expression SEMI statements | 
      epsilon  */

  char *tempvar = NULL;

  while( !match(EOI) )
  {
    tempvar = expression();

    if (tempvar == NULL)
      break;
    
    if( match( SEMI ) )
      advance();
    else
      fprintf( stderr, "%d: Inserting missing semicolon\n", yylineno );

    freename( tempvar );
  }
}

char *expression()
{
  /* expression -> condition expression'
   * expression' -> LT / GT / EQUAL condition expression' |  epsilon
   */

  int op;
  int to_match[] = {LT, GT, EQUAL};
  char *symbols[] = {"<", ">", "=="};
  int num_to_match = sizeof(to_match)/sizeof(int);
  char  *tempvar, *tempvar2;

  tempvar = condition();
  while(true)
  {
    op = -1;
    for(int i = 0; i<num_to_match; i++){
      if(match(to_match[i])){
        op = i;
        break;
      }
    }
    if(op == -1)
      break;
    
    advance();
    tempvar2 = condition();
    printf("    %s = %s %s %s\n", tempvar, tempvar, symbols[op], tempvar2);
    freename( tempvar2 );
  }
  return tempvar;
}

char *condition()
{
  /* condition -> term condition'
   * condition' -> PLUS / MINUS term condition' |  epsilon
   */

  int op;
  int to_match[] = {PLUS, MINUS};
  char *symbols[] = {"+", "-"};
  int num_to_match = sizeof(to_match)/sizeof(int);
  char  *tempvar, *tempvar2;

  tempvar = term();
  while(true)
  {
    op = -1;
    for(int i = 0; i<num_to_match; i++){
      if(match(to_match[i])){
        op = i;
        break;
      }
    }
    if(op == -1)
      break;
    
    advance();
    tempvar2 = term();
    printf("    %s = %s %s %s\n", tempvar, tempvar, symbols[op], tempvar2);
    freename( tempvar2 );
  }
  return tempvar;
}

char *term()
{
  /* term -> factor term'
   * term' -> TIMES / DIV factor term' |  epsilon
   */

  int op;
  int to_match[] = {TIMES, DIV};
  char *symbols[] = {"*", "/"};
  int num_to_match = sizeof(to_match)/sizeof(int);
  char  *tempvar, *tempvar2;

  tempvar = factor();
  while(true)
  {
    op = -1;
    for(int i = 0; i<num_to_match; i++){
      if(match(to_match[i])){
        op = i;
        break;
      }
    }
    if(op == -1)
      break;
    
    advance();
    tempvar2 = factor();
    printf("    %s = %s %s %s\n", tempvar, tempvar, symbols[op], tempvar2);
    freename(tempvar2);
  }
  return tempvar;
}

char *factor()
{
  char *tempvar;

  if( match(NUM_OR_ID) )
  {
    /* Print the assignment instruction. The %0.*s conversion is a form of
     * %X.Ys, where X is the field width and Y is the maximum number of
     * characters that will be printed (even if the string is longer). I'm
     * using the %0.*s to print the string because it's not \0 terminated.
     * The field has a default width of 0, but it will grow the size needed
     * to print the string. The ".*" tells printf() to take the maximum-
     * number-of-characters count from the next argument (yyleng).
     */

    printf("    %s = %.*s\n", tempvar = newname(), yyleng, yytext );
    advance();
  }
  else if( match(LP) )
  {
    advance();
    tempvar = expression();
    if( match(RP) )
      advance();
    else
      fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno );
  }
  else
    fprintf( stderr, "%d: Number or identifier expected\n", yylineno );

  return tempvar;
}
