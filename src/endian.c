#include <stdio.h>

typedef union {
  short word;
  struct {
    char b1;
    char b2;
  } byte;
} endian;

int main(int argc, char **argv)
{
endian value;

  value.word = 0xFF00;
  fprintf(stdout, "%s_FIRST\n", (value.byte.b1 == 0xFF ? "MSB" : "LSB"));
  return(0);
}
