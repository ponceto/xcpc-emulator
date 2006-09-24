/*
 * XArea.c - Copyright (c) 2006 Olivier Poncet
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

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args);
static void Destroy(Widget widget);
static void Redisplay(Widget widget, XEvent *event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal *num_args);
static void ClockHnd(Widget widget);
static void MouseHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch);
static void KeybdHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch);

static XtResource resources[] = {
  /* XtNemuStartHandler */ {
    XtNemuStartHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XAreaRec, xarea.start_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuClockHandler */ {
    XtNemuClockHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XAreaRec, xarea.clock_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuCloseHandler */ {
    XtNemuCloseHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XAreaRec, xarea.close_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuKeybdHandler */ {
    XtNemuKeybdHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XAreaRec, xarea.keybd_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuMouseHandler */ {
    XtNemuMouseHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XAreaRec, xarea.mouse_handler),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNemuPaintHandler */ {
    XtNemuPaintHandler, XtCFunction, XtRFunction,
    sizeof(XtWidgetProc), XtOffsetOf(XAreaRec, xarea.paint_handler),
    XtRImmediate, (XtPointer) NULL
  },
};

externaldef(xareaclassrec) XAreaClassRec xAreaClassRec = {
  /* CoreClassPart */ {
    (WidgetClass) &coreClassRec,            /* superclass                   */
    "XArea",                                /* class_name                   */
    sizeof(XAreaRec),                       /* widget_size                  */
    NULL,                                   /* class_initialize             */
    NULL,                                   /* class_part_initialize        */
    FALSE,                                  /* class_inited                 */
    Initialize,                             /* initialize                   */
    NULL,                                   /* initialize_hook              */
    XtInheritRealize,                       /* realize                      */
    NULL,                                   /* actions                      */
    0,                                      /* num_actions                  */
    resources,                              /* resources                    */
    XtNumber(resources),                    /* num_resources                */
    NULLQUARK,                              /* xrm_class                    */
    TRUE,                                   /* compress_motion              */
    TRUE,                                   /* compress_exposure            */
    TRUE,                                   /* compress_enterleave          */
    FALSE,                                  /* visible_interest             */
    Destroy,                                /* destroy                      */
    XtInheritResize,                        /* resize                       */
    Redisplay,                              /* expose                       */
    SetValues,                              /* set_values                   */
    NULL,                                   /* set_values_hook              */
    XtInheritSetValuesAlmost,               /* set_values_almost            */
    NULL,                                   /* get_values_hook              */
    XtInheritAcceptFocus,                   /* accept_focus                 */
    XtVersion,                              /* version                      */
    NULL,                                   /* callback_private             */
    XtInheritTranslations,                  /* tm_table                     */
    XtInheritQueryGeometry,                 /* query_geometry               */
    XtInheritDisplayAccelerator,            /* display_accelerator          */
    NULL                                    /* extension                    */
  },
  /* XAreaClassPart */ {
    NULL                                    /* extension                    */
  }
};

externaldef(xareawidgetclass) WidgetClass xAreaWidgetClass = (WidgetClass) &xAreaClassRec;

/**
 * XAreaWidget::Initialize()
 *
 * @param request specifies the requested XAreaWidget instance
 * @param widget specifies the XAreaWidget instance
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 */
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args)
{
  XAreaWidget self = (XAreaWidget) widget;
  Widget shell = XtParent(widget);

  while((shell != NULL) && (XtIsShell(shell) == FALSE)) {
    shell = XtParent(shell);
  }
  XtAddEventHandler(widget, (KeyPressMask    | KeyReleaseMask   ), FALSE, (XtEventHandler) KeybdHnd, (XtPointer) shell);
  XtAddEventHandler(widget, (ButtonPressMask | ButtonReleaseMask), FALSE, (XtEventHandler) MouseHnd, (XtPointer) shell);
  if(self->xarea.start_handler != NULL) {
    (*self->xarea.start_handler)(widget, NULL);
  }
  self->xarea.interval_id = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 10, (XtTimerCallbackProc) ClockHnd, (XtPointer) widget);
}

/**
 * XAreaWidget::Destroy()
 *
 * @param widget specifies the XAreaWidget instance
 */
static void Destroy(Widget widget)
{
  XAreaWidget self = (XAreaWidget) widget;

  if(self->xarea.interval_id != (XtIntervalId) 0) {
    XtRemoveTimeOut(self->xarea.interval_id);
    self->xarea.interval_id = (XtIntervalId) 0;
  }
  if(self->xarea.close_handler != NULL) {
    (*self->xarea.close_handler)(widget, NULL);
  }
}

/**
 * XAreaWidget::Redisplay()
 *
 * @param widget specifies the XAreaWidget instance
 * @param xevent specifies the XEvent
 * @param region specifies the Region
 */
static void Redisplay(Widget widget, XEvent *xevent, Region region)
{
  XAreaWidget self = (XAreaWidget) widget;

  if(self->xarea.paint_handler != NULL) {
    (*self->xarea.paint_handler)(widget, xevent);
  }
}

/**
 * XAreaWidget::SetValues()
 *
 * @param cur_w specifies the current XAreaWidget instance
 * @param req_w specifies the requested XAreaWidget instance
 * @param new_w specifies the new XAreaWidget instance
 */
static Boolean SetValues(Widget ow, Widget rw, Widget nw, ArgList args, Cardinal *num_args)
{
  return(FALSE);
}

/**
 * XAreaWidget::ClockHnd()
 *
 * @param widget specifies the XAreaWidget instance
 */
static void ClockHnd(Widget widget)
{
  XAreaWidget self = (XAreaWidget) widget;
  extern int paused;

  self->xarea.interval_id = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 10, (XtTimerCallbackProc) ClockHnd, (XtPointer) widget);
  if((self->xarea.clock_handler != NULL) && (paused == 0)) {
    (*self->xarea.clock_handler)(widget, NULL);
  }
}

/**
 * XAreaWidget::KeybdHnd()
 *
 * @param widget specifies the XAreaWidget instance
 * @param shell specifies the TopLevelShell instance
 * @param xevent specifies the XEvent
 * @param dispatch specifies the 'continue to dispatch' flag
 */
static void KeybdHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch)
{
  XAreaWidget self = (XAreaWidget) widget;
  XEvent pevent;
  KeySym keysym;
  Modifiers modifiers;

  if(xevent->type == KeyRelease) {
    if(XPending(XtDisplay(widget)) > 0) {
      if(XPeekEvent(XtDisplay(widget), &pevent) != 0) {
        if((pevent.type == KeyPress) && (pevent.xkey.keycode == xevent->xkey.keycode) && ((pevent.xkey.time - xevent->xkey.time) < 5)) {
          return;
        }
      }
    }
  }
  if((xevent->type == KeyPress) || (xevent->type == KeyRelease)) {
    XtTranslateKeycode(xevent->xany.display, xevent->xkey.keycode, xevent->xkey.state, &modifiers, &keysym);
    if(self->xarea.keybd_handler != NULL) {
      (*self->xarea.keybd_handler)(widget, xevent);
    }
  }
}

/**
 * XAreaWidget::MouseHnd()
 *
 * @param widget specifies the XAreaWidget instance
 * @param shell specifies the TopLevelShell instance
 * @param xevent specifies the XEvent
 * @param dispatch specifies the 'continue to dispatch' flag
 */
static void MouseHnd(Widget widget, Widget shell, XEvent *xevent, Boolean *dispatch)
{
  XAreaWidget self = (XAreaWidget) widget;

  if((xevent->type == ButtonPress) || (xevent->type == ButtonRelease)) {
    XtSetKeyboardFocus(shell, widget);
    if(self->xarea.mouse_handler != NULL) {
      (*self->xarea.mouse_handler)(widget, xevent);
    }
  }
}

/**
 * XAreaWidget::Create()
 *
 * @param parent specifies the parent widget
 * @param name specifies the name of the created widget
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 *
 * @return the XAreaWidget instance
 */
Widget XAreaCreate(Widget parent, String name, ArgList args, Cardinal num_args)
{
  return(XtCreateWidget(name, xAreaWidgetClass, parent, args, num_args));
}
