//Input: Bytes
//Output: Comma-separated list of numbers

#include <stdio.h>

int main() {
  int x;
  if((x=getchar())!=EOF) printf("={%d", x);
  while((x=getchar())!=EOF) printf(",%d", x);
  printf("};\n");
  return(0);
}
