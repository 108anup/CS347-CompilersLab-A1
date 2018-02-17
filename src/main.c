#include "code_gen.h"
#include "main.h"
#include <stdio.h>

void header(void)
{
  fprintf(fp, "    .global _start\n");
}

void prolog(void)
{
  fprintf(fp, "    .section\n    .text\n\n");
  fprintf(fp, "_start:\n");
}

void epilog(void)
{
  fprintf(fp, "\n    mov ebx, 0\n");
  fprintf(fp, "    mov eax, 1\n");
  fprintf(fp, "    int 0x80\n");
}

int main (void)
{
  fp = fopen("out.s", "w");
  lfp = fopen("lex.txt", "w");
  pfp = fopen("parse.txt", "w");
  header();
  prolog();
	statements();
  epilog();
  return 0;
}
