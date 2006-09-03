#include "config.h"
#include "ppi_8255.h"

PPI_8255 ppi_8255;

void ppi_8255_init(void)
{
  ppi_8255_reset();
}

void ppi_8255_reset(void)
{
  ppi_8255.control = 0x00;
  ppi_8255.port_a = 0x00;
  ppi_8255.port_b = 0x00;
  ppi_8255.port_c = 0x00;
}

void ppi_8255_exit(void)
{
}
