#include "name.h"
#include "lex.h"

#include <stdio.h>
#include <stdlib.h>

char *Names[] = { "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7" };   
char **Namep  = Names;

char *Regs[] = {"r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
   
char *newname(void)   
{   
  if( Namep >= &Names[ sizeof(Names)/sizeof(*Names) ] )   
  {   
    fprintf( stderr, "%d: Expression too complex\n", yylineno );   
    exit( 1 );   
  }   
   
  return( *Namep++ );   
}   
   
void freename(char *s)   
{   
  if( Namep > Names )   
    *--Namep = s;   
  else   
    fprintf(stderr, "%d: (Internal error) Name stack underflow\n",   
            yylineno );   
}   

char *reg_name(char *s){
  int idx = ((void *)s - (void *)Names[0])/2;
  return Regs[idx];
}
