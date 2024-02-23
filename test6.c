#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;
char *y;


struct s{
    int value;
    char *p;
    char *q;
} s1, s0;

int main(){
  c = getc(stdin);
  d = getc(stdin);

  x = malloc(10);
  s1.p = malloc(10);
  y = s1.p;
   
  free(x);
  free(y);
}
