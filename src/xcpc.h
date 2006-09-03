#ifndef __XCPC_H__
#define __XCPC_H__

typedef struct {
  Screen *screen;
  Display *display;
  Window window;
  GC gc;
  XImage *ximage;
} X11;

extern XtAppContext appcontext;
extern X11 x11;

int main(int, char **);

#endif
