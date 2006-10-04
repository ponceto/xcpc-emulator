/*
 * Emulator.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include <Xem/StringDefs.h>
#include <Xem/EmulatorP.h>

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args);
static void Destroy(Widget widget);
static void Redisplay(Widget widget, XEvent *event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal *num_args);
static void ClockHnd(Widget widget);
static void MouseHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch);
static void KeybdHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch);

/**
 * XemEmulatorWidget::resources[]
 */
static XtResource resources[] = {
  /* XtNemuStartHandler */ {
    XtNemuStartHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.start_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuClockHandler */ {
    XtNemuClockHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.clock_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuCloseHandler */ {
    XtNemuCloseHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.close_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuKeybdHandler */ {
    XtNemuKeybdHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.keybd_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuMouseHandler */ {
    XtNemuMouseHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.mouse_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuPaintHandler */ {
    XtNemuPaintHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.paint_handler),
    XtRImmediate, (XtPointer) NULL
  },
};

/*
 * XemEmulatorWidget::Class
 */
externaldef(xememulatorclassrec) XemEmulatorClassRec xemEmulatorClassRec = {
  /* CoreClassPart */ {
    (WidgetClass) &coreClassRec,             /* superclass                   */
    "XemEmulator",                           /* class_name                   */
    sizeof(XemEmulatorRec),                  /* widget_size                  */
    NULL,                                    /* class_initialize             */
    NULL,                                    /* class_part_initialize        */
    FALSE,                                   /* class_inited                 */
    Initialize,                              /* initialize                   */
    NULL,                                    /* initialize_hook              */
    XtInheritRealize,                        /* realize                      */
    NULL,                                    /* actions                      */
    0,                                       /* num_actions                  */
    resources,                               /* resources                    */
    XtNumber(resources),                     /* num_resources                */
    NULLQUARK,                               /* xrm_class                    */
    TRUE,                                    /* compress_motion              */
    TRUE,                                    /* compress_exposure            */
    TRUE,                                    /* compress_enterleave          */
    FALSE,                                   /* visible_interest             */
    Destroy,                                 /* destroy                      */
    XtInheritResize,                         /* resize                       */
    Redisplay,                               /* expose                       */
    SetValues,                               /* set_values                   */
    NULL,                                    /* set_values_hook              */
    XtInheritSetValuesAlmost,                /* set_values_almost            */
    NULL,                                    /* get_values_hook              */
    XtInheritAcceptFocus,                    /* accept_focus                 */
    XtVersion,                               /* version                      */
    NULL,                                    /* callback_private             */
    XtInheritTranslations,                   /* tm_table                     */
    XtInheritQueryGeometry,                  /* query_geometry               */
    XtInheritDisplayAccelerator,             /* display_accelerator          */
    NULL                                     /* extension                    */
  },
  /* XemEmulatorClassPart */ {
    NULL                                     /* extension                    */
  }
};

externaldef(xememulatorwidgetclass) WidgetClass xemEmulatorWidgetClass = (WidgetClass) &xemEmulatorClassRec;

/**
 * XemEmulatorWidget::Initialize()
 *
 * @param request specifies the requested XemEmulatorWidget instance
 * @param widget specifies the XemEmulatorWidget instance
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 */
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;
  Widget shell = XtParent(widget);

  while((shell != NULL) && (XtIsShell(shell) == FALSE)) {
    shell = XtParent(shell);
  }
  XtAddEventHandler(widget, (KeyPressMask    | KeyReleaseMask   ), FALSE, (XtEventHandler) KeybdHnd, (XtPointer) shell);
  XtAddEventHandler(widget, (ButtonPressMask | ButtonReleaseMask), FALSE, (XtEventHandler) MouseHnd, (XtPointer) shell);
  if(self->emulator.start_handler != NULL) {
    (*self->emulator.start_handler)(widget, NULL);
  }
  self->emulator.interval_id = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 10, (XtTimerCallbackProc) ClockHnd, (XtPointer) widget);
}

/**
 * XemEmulatorWidget::Destroy()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Destroy(Widget widget)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;

  if(self->emulator.interval_id != (XtIntervalId) 0) {
    XtRemoveTimeOut(self->emulator.interval_id);
    self->emulator.interval_id = (XtIntervalId) 0;
  }
  if(self->emulator.close_handler != NULL) {
    (*self->emulator.close_handler)(widget, NULL);
  }
}

/**
 * XemEmulatorWidget::Redisplay()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent
 * @param region specifies the Region
 */
static void Redisplay(Widget widget, XEvent *xevent, Region region)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;

  if(self->emulator.paint_handler != NULL) {
    (*self->emulator.paint_handler)(widget, xevent);
  }
}

/**
 * XemEmulatorWidget::SetValues()
 *
 * @param cur_w specifies the current XemEmulatorWidget instance
 * @param req_w specifies the requested XemEmulatorWidget instance
 * @param new_w specifies the new XemEmulatorWidget instance
 */
static Boolean SetValues(Widget ow, Widget rw, Widget nw, ArgList args, Cardinal *num_args)
{
  return(FALSE);
}

/**
 * XemEmulatorWidget::ClockHnd()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void ClockHnd(Widget widget)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;
  unsigned long timeout = 10;

  if((self->emulator.clock_handler != NULL) && (self->core.sensitive != FALSE) && (self->core.ancestor_sensitive != FALSE)) {
    (*self->emulator.clock_handler)(widget, &timeout);
  }
  self->emulator.interval_id = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), timeout, (XtTimerCallbackProc) ClockHnd, (XtPointer) widget);
}

/**
 * XemEmulatorWidget::KeybdHnd()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param shell specifies the TopLevelShell instance
 * @param xevent specifies the XEvent
 * @param dispatch specifies the 'continue to dispatch' flag
 */
static void KeybdHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;
  Display *dpy = DisplayOfScreen(self->core.screen);
  XEvent pevent;

  if(xevent->type == KeyRelease) {
    if(XPending(dpy) > 0) {
      (void) XPeekEvent(dpy, &pevent);
      if((pevent.type == KeyPress)
      && (pevent.xkey.display == xevent->xkey.display)
      && (pevent.xkey.keycode == xevent->xkey.keycode)
      && ((pevent.xkey.time - xevent->xkey.time) < 5)) {
        (void) XNextEvent(dpy, &pevent); return;
      }
    }
    if(self->emulator.keybd_handler != NULL) {
      (*self->emulator.keybd_handler)(widget, xevent);
    }
  }
  else if(xevent->type == KeyPress) {
    if(self->emulator.keybd_handler != NULL) {
      (*self->emulator.keybd_handler)(widget, xevent);
    }
  }
}

/**
 * XemEmulatorWidget::MouseHnd()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param shell specifies the TopLevelShell instance
 * @param xevent specifies the XEvent
 * @param dispatch specifies the 'continue to dispatch' flag
 */
static void MouseHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;

  if(xevent->type == ButtonRelease) {
    if(self->emulator.mouse_handler != NULL) {
      (*self->emulator.mouse_handler)(widget, xevent);
    }
  }
  else if (xevent->type == ButtonPress) {
    XtSetKeyboardFocus(shell, widget);
    if(self->emulator.mouse_handler != NULL) {
      (*self->emulator.mouse_handler)(widget, xevent);
    }
  }
}

/**
 * XemEmulatorWidget::Create()
 *
 * @param parent specifies the parent widget
 * @param name specifies the name of the created widget
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 *
 * @return the XemEmulatorWidget instance
 */
Widget XemCreateEmulator(Widget parent, String name, ArgList args, Cardinal num_args)
{
  return(XtCreateWidget(name, xemEmulatorWidgetClass, parent, args, num_args));
}
