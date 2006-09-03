#ifndef __AY_3_8910_H__
#define __AY_3_8910_H__

typedef struct {
  byte current;
  byte registers[16];
} AY_3_8910;

extern AY_3_8910 ay_3_8910;

void ay_3_8910_init(void);
void ay_3_8910_reset(void);
void ay_3_8910_exit(void);

#endif
