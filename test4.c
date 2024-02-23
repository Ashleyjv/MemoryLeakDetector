#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;
char *y;

int main(){
  c = getc(stdin);
  d = getc(stdin);

  x = malloc(10);

  while (d < 50){
    
    if (c > 15)
      break;

    if (d < 10)
      continue;

  }
  
}
