#define ARR_SIZE 5
#include <stdio.h>

int main() {
  int x = -23;
  // if(x < 0) {
  //   x = x - 0b1;
  // }
  printf("x = %d\n", x);
  int count = 0;
  int size = sizeof(x) * 8;
  for (int i = 0; i < size; i++) {
    if( x & 1 ) {
      count ++;
    }

    x = x >> 1;
  }
  printf("count = %d\n", count);
  return 0;
}