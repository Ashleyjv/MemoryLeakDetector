#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;
char *y;

int main(){
  c = getc(stdin);
  d = getc(stdin);

  while (d < 50){
    x = malloc(10);

    if (c > 15)
      break;

    free(x);
    y = malloc(10);
    }
  free(y);
} 
