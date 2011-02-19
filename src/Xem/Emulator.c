/*
 * Emulator.c - Copyright (c) 2001, 2006, 2007, 2008, 2009, 2010, 2011 Olivier Poncet
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

static void ClassInitialize(void);
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args);
static void Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attributes);
static void Destroy(Widget widget);
static void Resize(Widget widget);
static void Redisplay(Widget widget, XEvent *event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal *num_args);
static void ClockHnd(Widget widget, XtIntervalId *timer);
static void EventHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch);

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
  /* XtNemuInputHandler */ {
    XtNemuInputHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.input_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuPaintHandler */ {
    XtNemuPaintHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.paint_handler),
    XtRImmediate, (XtPointer) NULL
  },
};

/*
 * XemEmulatorWidget::SuperClass
 */
static WidgetClass XemEmulatorSuperClass = (WidgetClass) NULL;

/*
 * XemEmulatorWidget::Class
 */
externaldef(xememulatorclassrec) XemEmulatorClassRec xemEmulatorClassRec = {
  /* CoreClassPart */ {
    (WidgetClass) &coreClassRec,             /* superclass                   */
    "XemEmulator",                           /* class_name                   */
    sizeof(XemEmulatorRec),                  /* widget_size                  */
    ClassInitialize,                         /* class_initialize             */
    NULL,                                    /* class_part_initialize        */
    FALSE,                                   /* class_inited                 */
    Initialize,                              /* initialize                   */
    NULL,                                    /* initialize_hook              */
    Realize,                                 /* realize                      */
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
    Resize,                                  /* resize                       */
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
 * XemEmulatorWidget::ClassInitialize()
 */
static void ClassInitialize(void)
{
  XemEmulatorSuperClass = xemEmulatorClassRec.core_class.superclass;
}

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

  if(request->core.width == 0) {
    self->core.width = 640;
  }
  if(request->core.height == 0) {
    self->core.height = 480;
  }
  while((shell != NULL) && (XtIsShell(shell) == FALSE)) {
    shell = XtParent(shell);
  }
  XtAddEventHandler(widget, (KeyPressMask    |    KeyReleaseMask), FALSE, (XtEventHandler) EventHnd, (XtPointer) shell);
  XtAddEventHandler(widget, (ButtonPressMask | ButtonReleaseMask), FALSE, (XtEventHandler) EventHnd, (XtPointer) shell);
  XtAddEventHandler(widget, (        StructureNotifyMask        ), FALSE, (XtEventHandler) EventHnd, (XtPointer) shell);
  if(self->emulator.start_handler != NULL) {
    (*self->emulator.start_handler)(widget, NULL);
  }
  self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), (self->emulator.delay = 100), (XtTimerCallbackProc) ClockHnd, (XtPointer) widget);
}

/**
 * XemEmulatorWidget::Realize()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param mask specifies the attributes mask
 * @param attributes specifies the attributes
 */
static void Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attributes)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;

  if(XemEmulatorSuperClass->core_class.realize != NULL) {
    (*XemEmulatorSuperClass->core_class.realize)(widget, mask, attributes);
  }
  if(self->core.window != None) {
    Widget shell = XtParent(widget);
    while((shell != NULL) && (XtIsShell(shell) == FALSE)) {
      shell = XtParent(shell);
    }
    if(shell != NULL) {
      XtSetKeyboardFocus(shell, widget);
    }
  }
}

/**
 * XemEmulatorWidget::Destroy()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Destroy(Widget widget)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;

  if(self->emulator.timer != (XtIntervalId) 0) {
    XtRemoveTimeOut(self->emulator.timer);
    self->emulator.timer = (XtIntervalId) 0;
    self->emulator.delay = (unsigned long) 0;
  }
  if(self->emulator.close_handler != NULL) {
    (*self->emulator.close_handler)(widget, NULL);
  }
}

/**
 * XemEmulatorWidget::Resize()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Resize(Widget widget)
{
}

/**
 * XemEmulatorWidget::Redisplay()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param region specifies the Region information
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
 * @param timer specifies the XtIntervalId timer instance
 */
static void ClockHnd(Widget widget, XtIntervalId *timer)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;

  if((self->emulator.clock_handler != NULL) && (self->core.sensitive != FALSE) && (self->core.ancestor_sensitive != FALSE)) {
    (*self->emulator.clock_handler)(widget, &self->emulator.delay);
  }
  else {
    self->emulator.delay = 100;
  }
  self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), self->emulator.delay, (XtTimerCallbackProc) ClockHnd, (XtPointer) widget);
}

/**
 * XemEmulatorWidget::EventHnd()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param shell specifies the TopLevelShell instance
 * @param xevent specifies the XEvent structure
 * @param dispatch specifies the 'continue to dispatch' flag
 */
static void EventHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch)
{
  XemEmulatorWidget self = (XemEmulatorWidget) widget;
  XEvent pevent;

  switch(xevent->type) {
    case KeyRelease:
      if(XPending(xevent->xany.display) > 0) {
        (void) XPeekEvent(xevent->xany.display, &pevent);
        if((pevent.type == KeyPress)
        && (pevent.xkey.display == xevent->xkey.display)
        && (pevent.xkey.keycode == xevent->xkey.keycode)
        && ((pevent.xkey.time - xevent->xkey.time) < 5)) {
          (void) XNextEvent(xevent->xany.display, &pevent); return;
        }
      }
    case KeyPress:
      if(self->emulator.input_handler != NULL) {
        (*self->emulator.input_handler)(widget, xevent);
      }
      break;
    case ButtonPress:
      if(shell != NULL) {
        XtSetKeyboardFocus(shell, widget);
      }
    case ButtonRelease:
      if(self->emulator.input_handler != NULL) {
        (*self->emulator.input_handler)(widget, xevent);
      }
      break;
    case ConfigureNotify:
      do {
        Arg arglist[2];
        Cardinal argcount = 0;
        if(xevent->xconfigure.width != self->core.width) {
          XtSetArg(arglist[argcount], XtNwidth, xevent->xconfigure.width); argcount++;
        }
        if(xevent->xconfigure.height != self->core.height) {
          XtSetArg(arglist[argcount], XtNheight, xevent->xconfigure.height); argcount++;
        }
        if(argcount > 0) {
          XtSetValues(widget, arglist, argcount);
        }
      } while(XCheckTypedWindowEvent(XtDisplay(widget), XtWindow(widget), ConfigureNotify, xevent) != False);
      break;
    default:
      break;
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
