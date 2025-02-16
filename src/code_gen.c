#include "code_gen.h"
#include "name.h"
#include "lex.h"
#include "main.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#define MAXFIRST 16
#define SYNCH	 SEMI

static char *factor(int);
static char *term(int);
static char *expression(int);
static char *condition(int);
static bool statement(int);
static void statement_list(int);
static int legal_lookahead(int, ...);

static int lbl_count = 0;
static int loop_count = 0;

/*
  Precedence:
  * /
  + -
  > < =

  Left to right associative
*/

static void print_spaces(int spaces){
  for(int i= 0; i<spaces; i++){
    fprintf(pfp, " ");
  }
}

static void skip_statement(void){
  //while(!legal_lookahead(SEMI, EOI, WHILE, IF, BEGIN)){
  while(!match(SEMI) && !match(EOI) && !match(WHILE)
        && !match(IF) && !match(BEGIN)){
    if(match(ID)){
      advance();
      if(match(SEMI))
        break;
      if(!match(ASSIGN)){
        advance();
      }
      else{
        goback();
        break;
      }
    }
    else
      advance();
  }
}

void statements()
{
  /*
    statements -> statement; statements | epsilon
  */
  int spaces = 0;
  fprintf(pfp, "statement's'\n");
  spaces = 2;
  
  while(!match(EOI))
  {
    if(!statement(spaces))
      skip_statement();

    if(match(SEMI))
      advance();
    else 
      fprintf( stderr, "%d: Inserting expected missing ';' on or before"
               " '%.*s'\n", yylineno, yyleng, yytext);
 }
}

void statement_list(int spaces)
{
  /*
    statement_list -> statement; statement_list | epsilon
  */
  print_spaces(spaces);
  fprintf(pfp, "statement_list\n");
  spaces += 2;

  while(true)
  {
    if(match(EOI)){
      fprintf( stderr, "%d: Invalid begin end block\n", yylineno );
      break;
    }
    if(!match(END)){
      if(!statement(spaces))
        skip_statement();
      
      if(match(SEMI))
        advance();
      else
        fprintf( stderr, "%d: Inserting expected missing ';' on or before"
                 " '%.*s'\n", yylineno, yyleng, yytext);
    }
    else{
      print_spaces(spaces);
      fprintf(pfp, "end\n");
      advance();
      break;
    }
  }
}

bool statement(int spaces){
  /*
    statement -> ID ASSIGN expression  
    | if expression then statement
    | while expression do statement
    | begin statment_list end
  */
  print_spaces(spaces);
  spaces+=2;
  fprintf(pfp, "statement\n");
  
  char *tempvar, *tempvar2;
  if(match(ID)){
    char tempv[1024];
    int tempvarlen = yyleng;
    strncpy(tempv, yytext, yyleng);
    print_spaces(spaces);
    fprintf(pfp, "ID: %.*s\n", yyleng, yytext);
    advance();
    if(!match(ASSIGN)){
      fprintf(stderr, "%d: Invalid assignment\n", yylineno);
      return false;
    }
    else{
      print_spaces(spaces);
      fprintf(pfp, ":=\n");
      advance();
      tempvar2 = expression(spaces);
      fprintf(fp, "    mov %.*s, %s\n", tempvarlen, tempv, reg_name(tempvar2));
      printf("    %.*s = %s\n", tempvarlen, tempv, tempvar2);
      freename(tempvar2);
    }
  }
  else if(match(IF)){
    print_spaces(spaces);
    fprintf(pfp, "if\n");
    advance();
    printf("\n");
    tempvar = expression(spaces);
    int lbl_num_used = lbl_count++;
    fprintf(fp, "    cmp %s, 0\n", reg_name(tempvar));
    fprintf(fp, "    jz _LBL%d\n\n", lbl_num_used);
    printf("    if not %s goto _LBL%d\n\n", tempvar, lbl_num_used);
    freename(tempvar);
    if(!match(THEN)){
      fprintf(stderr, "%d: Invalid 'if' statement "
              "(expected 'then' before '%.*s')\n", yylineno, yyleng, yytext);
      return false;
    }
    else{
      print_spaces(spaces);
      fprintf(pfp, "then\n");
      advance();
      int ret = statement(spaces);
      printf("\n_LBL%d: \n", lbl_num_used);
      fprintf(fp, "\n_LBL%d: \n", lbl_num_used);
      return ret;
    }
  }
  else if(match(WHILE)){
    print_spaces(spaces);
    fprintf(pfp, "while\n");
    advance();
    int loop_num_used = loop_count++;
    fprintf (fp, "_LOOP%d:\n", loop_num_used);
    printf ("_LOOP%d:\n", loop_num_used);
    tempvar = expression(spaces);
    int lbl_num_used = lbl_count++;
    fprintf(fp, "    cmp %s, 0\n", reg_name(tempvar));
    fprintf(fp, "    jz _LBL%d\n\n", lbl_num_used);
    printf("    if not %s goto _LBL%d\n\n", tempvar, lbl_num_used);
    freename(tempvar);
    if(!match(DO)){
      fprintf(stderr, "%d: Invalid 'while' statement "
              "(expected 'do' before '%.*s')\n", yylineno, yyleng, yytext);
      return false;
    }
    else{
      print_spaces(spaces);
      fprintf(pfp, "do\n");
      advance();
      statement(spaces);
      fprintf(fp, "    jmp _LOOP%d\n\n_LBL%d: \n",
              loop_num_used, lbl_num_used);
      printf("    goto _LOOP%d\n\n_LBL%d: \n", loop_num_used, lbl_num_used);
    }
  }
  else if(match(BEGIN)){
    print_spaces(spaces);
    fprintf(pfp, "begin\n");
    advance();
    statement_list(spaces);
  }
  else{
    return false;
  }
  return true;
}

char *expression(int spaces)
{
  /* expression -> condition expression'
   * expression' -> LT / GT / EQUAL condition expression' |  epsilon
   */

  print_spaces(spaces);
  fprintf(pfp, "expression\n");
  spaces += 2;
  
  int op;
  int to_match[] = {LT, GT, EQUAL};
  char *symbols[] = {"<", ">", "=="};
  int num_to_match = sizeof(to_match)/sizeof(int);
  char  *tempvar, *tempvar2;

  tempvar = condition(spaces);
  while(true)
  {
    print_spaces(spaces);
    fprintf(pfp, "expression'\n");
    spaces += 2;
    
    op = -1;
    for(int i = 0; i<num_to_match; i++){
      if(match(to_match[i])){
        op = i;
        break;
      }
    }
    if(op == -1)
      break;

    print_spaces(spaces);
    fprintf(pfp, "%s\n", symbols[op]);
    advance();
    tempvar2 = condition(spaces);
    printf("    %s = %s %s %s\n", tempvar, tempvar, symbols[op], tempvar2);
    fprintf(fp, "    cmp %s, %s\n", reg_name(tempvar), reg_name(tempvar2));
    if(op == 1){
      fprintf(fp, "    setg %s\n", reg_name(tempvar));
    }
    else if(op == 0){
      fprintf(fp, "    setl %s\n", reg_name(tempvar));
    }
    else if(op == 2){
      fprintf(fp, "    sete %s\n", reg_name(tempvar));
    }
    else{
      fprintf(fp, "%d", op);
    }
    freename( tempvar2 );
  }
  return tempvar;
}

char *condition(int spaces)
{
  /* condition -> term condition'
   * condition' -> PLUS / MINUS term condition' |  epsilon
   */

  print_spaces(spaces);
  fprintf(pfp, "condition\n");
  spaces += 2;
  
  int op;
  int to_match[] = {PLUS, MINUS};
  char *symbols[] = {"+", "-"};
  int num_to_match = sizeof(to_match)/sizeof(int);
  char  *tempvar, *tempvar2;

  tempvar = term(spaces);
  while(true)
  {
    print_spaces(spaces);
    fprintf(pfp, "condition'\n");
    spaces += 2;
    
    op = -1;
    for(int i = 0; i<num_to_match; i++){
      if(match(to_match[i])){
        op = i;
        break;
      }
    }
    if(op == -1)
      break;

    print_spaces(spaces);
    fprintf(pfp, "%s\n", symbols[op]);

    advance();
    tempvar2 = term(spaces);
    printf("    %s = %s %s %s\n", tempvar, tempvar, symbols[op], tempvar2);
    if (op == 0){
      fprintf(fp, "    add	%s, %s\n", reg_name(tempvar), reg_name(tempvar2));
    }
    else if (op == 1){
      fprintf(fp, "    sub	%s, %s\n", reg_name(tempvar), reg_name(tempvar2));
    }
    freename( tempvar2 );
  }
  return tempvar;
}

char *term(int spaces)
{
  /* term -> factor term'
   * term' -> TIMES / DIV / MOD factor term' |  epsilon
   */

  print_spaces(spaces);
  fprintf(pfp, "term\n");
  spaces += 2;

  int op;
  int to_match[] = {TIMES, DIV, MOD};
  char *symbols[] = {"*", "/", "%"};
  int num_to_match = sizeof(to_match)/sizeof(int);
  char  *tempvar, *tempvar2;

  tempvar = factor(spaces);
  while(true)
  {
    print_spaces(spaces);
    fprintf(pfp, "term'\n");
    spaces += 2;
    
    op = -1;
    for(int i = 0; i<num_to_match; i++){
      if(match(to_match[i])){
        op = i;
        break;
      }
    }
    if(op == -1)
      break;

    print_spaces(spaces);
    fprintf(pfp, "%s\n", symbols[op]);
    advance();
    tempvar2 = factor(spaces);
    printf("    %s = %s %s %s\n", tempvar, tempvar, symbols[op], tempvar2);
    if (op == 0){
      fprintf(fp, "    imul %s, %s\n", reg_name(tempvar), reg_name(tempvar2));
    }
    else if (op == 1){
      fprintf(fp, "    xor edx, edx\n");
      fprintf(fp, "    mov eax, %s\n", reg_name(tempvar));
      fprintf(fp, "    idiv %s\n", reg_name(tempvar2));
      fprintf(fp, "    mov %s, eax\n", reg_name(tempvar));
    }
    else if (op == 2){
      fprintf(fp, "    xor edx, edx\n");
      fprintf(fp, "    mov eax, %s\n", reg_name(tempvar));
      fprintf(fp, "    idiv %s\n", reg_name(tempvar2));
      fprintf(fp, "    mov %s, edx\n", reg_name(tempvar));
    }
    freename(tempvar2);
  }
  return tempvar;
}

char *factor(int spaces)
{
  
  print_spaces(spaces);
  fprintf(pfp, "factor\n");
  spaces += 2;
  
  char *tempvar = NULL;

  if(match(NUM) || match(ID))
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
    fprintf(fp, "    mov %s, %.*s\n", reg_name(tempvar), yyleng, yytext);
    print_spaces(spaces);
    fprintf(pfp, " %.*s\n", yyleng, yytext);
    advance();
  }
  else if( match(LP) )
  {
    print_spaces(spaces);
    fprintf(pfp, "(\n");
    advance();
    tempvar = expression(spaces);
    if( match(RP) ){
      print_spaces(spaces);
      fprintf(pfp, ")\n");
      advance();
    }
    else
      fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno );
  }
  else{
    fprintf( stderr, "%d: Number or identifier expected"
             " on or before '%.*s'\n", yylineno, yyleng, yytext);
    printf("    %s = %s\n", tempvar = newname(), "EXPECTED ID");
    fprintf(fp, "    mov %s, %s\n", reg_name(tempvar), "EXPECTED ID");
    if(match(-2))
      advance();
  }

  return tempvar;
}

int	legal_lookahead(int first_arg, ...)
{
  /* Simple error detection and recovery. Arguments are a 0-terminated list of
   * those tokens that can legitimately come next in the input. If the list is
   * empty, the end of file must come next. Print an error message if
   * necessary. Error recovery is performed by discarding all input symbols
   * until one that's in the input list is found
   *
   * Return true if there's no error or if we recovered from the error,
   * false if we can't recover.
   */

  va_list  	args;
  int		tok;
  int		lookaheads[MAXFIRST], *p = lookaheads, *current;
  int		error_printed = 0;
  int		rval	      = 0;

  va_start( args, first_arg );

  if( !first_arg )
  {
    if( match(EOI) )
	    rval = 1;
  }
  else
  {
    *p++ = first_arg;
    while( (tok = va_arg(args, int)) && p < &lookaheads[MAXFIRST] )
	    *p++ = tok;

    while( !match( SYNCH ) )
    {
	    for( current = lookaheads; current < p ; ++current )
        if( match( *current ) )
        {
          rval = 1;
          goto exit;
        }
      
	    if( !error_printed )
	    {
        fprintf( stderr, "Line %d: Syntax error\n", yylineno );
        error_printed = 1;
	    }

	    advance();
    }
  }

exit:
  va_end( args );
  return rval;
}
