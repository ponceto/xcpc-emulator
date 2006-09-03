#include "config.h"
#include "fdc_765.h"

FDC_765 fdc_765;

void fdc_765_init(void)
{
  fdc_765_reset();
}

void fdc_765_reset(void)
{
  fdc_765.status = 0x80;
  fdc_765.motor = 0x00;
}

void fdc_765_exit(void)
{
}

void fdc_765_set_motor(byte state)
{
  fdc_765.motor = state & 0x01;
}

byte fdc_765_get_motor(void)
{
  return(fdc_765.motor);
}

byte fdc_765_get_status(void)
{
  return(fdc_765.status);
}
