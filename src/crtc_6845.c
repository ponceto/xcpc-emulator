#include "config.h"
#include "crtc_6845.h"

CRTC_6845 crtc_6845;

void crtc_6845_init(void)
{
  crtc_6845_reset();
}

void crtc_6845_reset(void)
{
int ix;

  crtc_6845.current = 0x00;
  for(ix = 0; ix < 18; ix++) {
    crtc_6845.registers[ix] = 0x00;
  }
}

void crtc_6845_exit(void)
{
}
