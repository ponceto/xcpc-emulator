#ifndef __PPI_8255_H__
#define __PPI_8255_H__

typedef struct {
  byte control;
  byte port_a;
  byte port_b;
  byte port_c;
} PPI_8255;

extern PPI_8255 ppi_8255;

void ppi_8255_init(void);
void ppi_8255_reset(void);
void ppi_8255_exit(void);

#endif
