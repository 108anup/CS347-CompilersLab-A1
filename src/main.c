#include "code_gen.h"
#include "main.h"
#include <stdio.h>

int main (void)
{
  fp = fopen("out.s", "w");
	statements ();
  return 0;
}
