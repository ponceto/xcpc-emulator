#ifndef __FDC_765_H__
#define __FDC_765_H__

typedef struct {
  byte status;
  byte motor;
} FDC_765;

extern FDC_765 fdc_765;

void fdc_765_init(void);
void fdc_765_reset(void);
void fdc_765_exit(void);
void fdc_765_set_motor(byte);
byte fdc_765_get_motor(void);
byte fdc_765_get_status(void);

#endif
