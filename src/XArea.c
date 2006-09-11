/*
 * XArea.c - Copyright (c) 2001, 2006 Olivier Poncet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "XAreaP.h"

static XtResource resources[] = {
  { XtNkeyPressCallback,      XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.KeyPressCbk),      XtRCallback, NULL },
  { XtNkeyReleaseCallback,    XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.KeyReleaseCbk),    XtRCallback, NULL },
  { XtNbuttonPressCallback,   XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.ButtonPressCbk),   XtRCallback, NULL },
  { XtNbuttonReleaseCallback, XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.ButtonReleaseCbk), XtRCallback, NULL },
  { XtNmotionNotifyCallback,  XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.MotionNotifyCbk),  XtRCallback, NULL },
  { XtNenterNotifyCallback,   XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.EnterNotifyCbk),   XtRCallback, NULL },
  { XtNleaveNotifyCallback,   XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.LeaveNotifyCbk),   XtRCallback, NULL },
  { XtNexposeCallback,        XtCCallback, XtRCallback, sizeof(XtPointer), XtOffsetOf(XAreaRec, xarea.ExposeCbk),        XtRCallback, NULL },
};

static void XAreaXEvent(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  switch(event->type) {
    case KeyPress:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.KeyPressCbk, (XtPointer) event);
      break;
    case KeyRelease:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.KeyReleaseCbk, (XtPointer) event);
      break;
    case ButtonPress:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.ButtonPressCbk, (XtPointer) event);
      break;
    case ButtonRelease:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.ButtonReleaseCbk, (XtPointer) event);
      break;
    case MotionNotify:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.MotionNotifyCbk, (XtPointer) event);
      break;
    case EnterNotify:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.EnterNotifyCbk, (XtPointer) event);
      break;
    case LeaveNotify:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.LeaveNotifyCbk, (XtPointer) event);
      break;
    case Expose:
      XtCallCallbackList(widget, ((XAreaRec *) widget)->xarea.ExposeCbk, (XtPointer) event);
      break;
  }
}

static XtActionsRec actions[] = {
  { "xevent", XAreaXEvent },
};

static char translations[] = "\
<KeyDown>:  xevent()\n\
<KeyUp>:    xevent()\n\
<BtnDown>:  xevent()\n\
<BtnUp>:    xevent()\n\
<Motion>:   xevent()\n\
<Enter>:    xevent()\n\
<Leave>:    xevent()\n\
<Expose>:   xevent()\n\
";

XAreaClassRec xareaClassRec = {
  {                                /* core fields              */
    (WidgetClass) &widgetClassRec, /* superclass               */
    "XArea",                       /* class_name               */
    sizeof(XAreaRec),              /* widget_size              */
    NULL,                          /* class_initialize         */
    NULL,                          /* class_part_initialize    */
    FALSE,                         /* class_inited             */
    NULL,                          /* initialize               */
    NULL,                          /* initialize_hook          */
    XtInheritRealize,              /* realize                  */
    actions,                       /* actions                  */
    XtNumber(actions),             /* num_actions              */
    resources,                     /* resources                */
    XtNumber(resources),           /* num_resources            */
    NULLQUARK,                     /* xrm_class                */
    TRUE,                          /* compress_motion          */
    XtExposeCompressMaximal,       /* compress_exposure        */
    TRUE,                          /* compress_enterleave      */
    FALSE,                         /* visible_interest         */
    NULL,                          /* destroy                  */
    NULL,                          /* resize                   */
    NULL,                          /* expose                   */
    NULL,                          /* set_values               */
    NULL,                          /* set_values_hook          */
    XtInheritSetValuesAlmost,      /* set_values_almost        */
    NULL,                          /* get_values_hook          */
    NULL,                          /* accept_focus             */
    XtVersion,                     /* version                  */
    NULL,                          /* callback_private         */
    translations,                  /* tm_table                 */
    XtInheritQueryGeometry,        /* query_geometry           */
    XtInheritDisplayAccelerator,   /* display_accelerator      */
    NULL                           /* extension                */
  },
  {                                /* xarea fields             */
    0                              /* empty                    */
  }
};

WidgetClass xareaWidgetClass = (WidgetClass) &xareaClassRec;

Widget XtCreateXArea(Widget p, String name, ArgList args, Cardinal n)
{
  return(XtCreateWidget(name, xareaWidgetClass, p, args, n));
}
