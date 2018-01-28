#include "lex.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

char* yytext = ""; /* Lexeme (not '\0' terminated). */
int yyleng   = 0;  /* Lexeme length. */
int yylineno = 0;  /* Input line number. */

static int lex(void);

int lex(void){

  static char input_buffer[1024];
  char        *current;

  current = yytext + yyleng; /* Skip current
                                lexeme        */

  while(1){       /* Get the next one         */
    while(!*current ){
      /* Get new lines, skipping any leading
       * white space on the line,
       * until a nonblank line is found.
       */

      current = input_buffer;
      if(scanf(" %[^\n]", input_buffer) < 1){
        *current = '\0' ;
        return EOI;
      }
      ++yylineno;
      while(isspace(*current))
        ++current;
    }
    for(; *current; ++current){
      /* Get the next token */
      yytext = current;
      yyleng = 1;
      switch( *current ){
      case ';':
        return SEMI;
      case '+':
        return PLUS;
      case '-':
        return MINUS;
      case '*':
        return TIMES;
      case '/':
        return DIV;
      case '>':
        return GT;
      case '<':
        return LT;
      case '=':
        return EQUAL;
      case '(':
        return LP;
      case ')':
        return RP;
      case '\n':
      case '\t':
      case ' ' :
        break;
      default:
        if(!isalnum(*current)){
          if(*current == ':' && *(current+1) == '='){
            yyleng = 2;
            yytext = current;
            return ASSIGN;
          }
          else
            fprintf(stderr, "Not alphanumeric <%c>\n", *current);
        }
        else{
          while(isalnum(*current))
            ++current;
          yyleng = current - yytext;
               
          if(strncmp (current, "if", 2) == 0)
            return IF;
          else if(strncmp(current, "then", 4) == 0)
            return THEN;
          else if(strncmp(current, "while", 5) == 0)
            return WHILE;
          else if(strncmp(current, "do", 2) == 0)
            return DO;
          else if(strncmp(current, "begin", 5) == 0)
            return BEGIN;
          else if(strncmp(current, "END", 3) == 0)
            return END;
          return NUM_OR_ID;
        }
        break;
      }
    }
  }
}


static int Lookahead = -1; /* Lookahead token  */

int match(int token){
  /* Return true if "token" matches the
     current lookahead symbol.                */

  if(Lookahead == -1)
    Lookahead = lex();

  return token == Lookahead;
}

void advance(void){
/* Advance the lookahead to the next
   input symbol.                               */

  Lookahead = lex();
}
