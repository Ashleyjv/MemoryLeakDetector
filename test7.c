#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;
char *y;


struct s{
    int value;
    char *p;
    char *q;
} s1, s2;

int main(){
  c = getc(stdin);
  d = getc(stdin);

  x = malloc(10);
  s1.p = malloc(10);

  s2 = s1;
  y = s2.p;
   
  free(x);
  free(y);
}
