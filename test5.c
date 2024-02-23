#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;
char *y;
char *z;

int main(){
  c = getc(stdin);
  d = getc(stdin);

  x = malloc(10);
  y = malloc(10);

  z = y;
  
  free(x);
  free(z);
}
 