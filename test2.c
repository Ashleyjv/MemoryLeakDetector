#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;


int main(){
  c = getc(stdin);
  d = getc(stdin);
  
  x = malloc(10);

  while (d < 50){

    c += 1;

    while (c > 15)
    {
      if (c > d)
        break;
    }

    d -= 1;
  } 
  
}
