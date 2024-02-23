#include <stdio.h>
#include <stdlib.h>

char c, d;
char *x;


int main(){
  c = getc(stdin);
  d = getc(stdin);
  
  x = malloc(10);

  while (d < 50){
    if (d > 5)
       c += 1;
  }
 
  while (c > 15)
  {
    if (c > 5)
      d-=1;
  }
  d -= 1;

}
