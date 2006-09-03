#include "config.h"
#include "ay_3_8910.h"

AY_3_8910 ay_3_8910;

void ay_3_8910_init(void)
{
  ay_3_8910_reset();
}

void ay_3_8910_reset(void)
{
int ix;

  ay_3_8910.current = 0x00;
  for(ix = 0; ix < 16; ix++) {
    ay_3_8910.registers[ix] = 0x00;
  }
}

void ay_3_8910_exit(void)
{
}
