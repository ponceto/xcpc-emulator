#ifndef __XAREAP_H__
#define __XAREAP_H__

#include "XArea.h"
#include <X11/CoreP.h>

typedef struct {
  int empty;
} XAreaClassPart;

typedef struct _XAreaClassRec {
  CoreClassPart core_class;
  XAreaClassPart xarea_class;
} XAreaClassRec;

extern XAreaClassRec xareaClassRec;

typedef struct {
  XtCallbackList KeyPressCbk;
  XtCallbackList KeyReleaseCbk;
  XtCallbackList ButtonPressCbk;
  XtCallbackList ButtonReleaseCbk;
  XtCallbackList MotionNotifyCbk;
  XtCallbackList EnterNotifyCbk;
  XtCallbackList LeaveNotifyCbk;
  XtCallbackList ExposeCbk;
} XAreaPart;

typedef struct _XAreaRec {
  CorePart core;
  XAreaPart xarea;
} XAreaRec;

#endif
