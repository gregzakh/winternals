#include <stdio.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned long uint;

float getversion(void) {
  uint *ver = *(uint (*)[2])0x7FFE026C;
  int  decs = log10(ver[1]) + 1;
  return ver[0] + ver[1] * pow(10.0, -decs);
}

int main(void) {
  if (6.2 <= getversion()) {
    byte ve = (*(byte (*)[64])0x7FFE0274)[21];
    printf("Virtualization is %sabled.\n", 1 == ve ? "en" : "dis");
    return 0;
  }

  printf("Error: Windows 8 or higher is required.\n");
  return 1;
}
