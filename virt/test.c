#include <stdio.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned long uint;

uint getversion(void) {
  uint *ver = *(uint (*)[2])0x7FFE026C;
  return ver[0] * 10 + ver[1];
}

int main(void) {
  if (62 <= getversion()) {
    byte ve = (*(byte (*)[64])0x7FFE0274)[21];
    printf("Virtualization is %sabled.\n", 1 == ve ? "en" : "dis");
    return 0;
  }

  printf("Error: Windows 8 or higher is required.\n");
  return 1;
}
