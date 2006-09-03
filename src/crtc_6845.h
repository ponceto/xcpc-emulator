#ifndef __CRTC_6845_H__
#define __CRTC_6845_H__

typedef struct {
  byte current;
  byte registers[18];
} CRTC_6845;

extern CRTC_6845 crtc_6845;

void crtc_6845_init(void);
void crtc_6845_reset(void);
void crtc_6845_exit(void);

#endif
