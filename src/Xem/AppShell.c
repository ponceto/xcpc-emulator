/*
 * AppShell.c - Copyright (c) 2001, 2006, 2007 Olivier Poncet
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
#include <Xem/AppShellP.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

static void ClassInitialize(void);
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args);
static void Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attributes);
static void Destroy(Widget widget);
static void OnWMProtocols(Widget widget, XEvent *xevent, String *params, Cardinal *num_params);
static void OnXdndEnter(Widget widget, XEvent *xevent, String *params, Cardinal *num_params);
static void OnXdndLeave(Widget widget, XEvent *xevent, String *params, Cardinal *num_params);
static void OnXdndPosition(Widget widget, XEvent *xevent, String *params, Cardinal *num_params);
static void OnXdndDrop(Widget widget, XEvent *xevent, String *params, Cardinal *num_params);

/**
 * XemAppShellWidget::actions[]
 */
static XtActionsRec actions[] = {
  { "OnWMProtocols",  OnWMProtocols  },
  { "OnXdndEnter",    OnXdndEnter    },
  { "OnXdndLeave",    OnXdndLeave    },
  { "OnXdndPosition", OnXdndPosition },
  { "OnXdndDrop",     OnXdndDrop     }
};

/**
 * XemAppShellWidget::translations[]
 */
static char translations[] = "\
<Message>WM_PROTOCOLS: OnWMProtocols()\n\
<Message>XdndEnter:    OnXdndEnter()\n\
<Message>XdndLeave:    OnXdndLeave()\n\
<Message>XdndPosition: OnXdndPosition()\n\
<Message>XdndDrop:     OnXdndDrop()\n\
";

/**
 * XemAppShellWidget::resources[]
 */
static XtResource resources[] = {
  /* XtNwmCloseCallback */ {
    XtNwmCloseCallback, XtCCallback, XtRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XemAppShellRec, app_shell.wm_close_callback),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNdropURICallback */ {
    XtNdropURICallback, XtCCallback, XtRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XemAppShellRec, app_shell.drop_uri_callback),
    XtRImmediate, (XtPointer) NULL
  }
};

/*
 * XemAppShellWidget::SuperClass
 */
static WidgetClass XemAppShellSuperClass = (WidgetClass) NULL;

/*
 * XemAppShellWidget::Class
 */
externaldef(xemappshellclassrec) XemAppShellClassRec xemAppShellClassRec = {
  /* CoreClassPart */ {
    (WidgetClass) &applicationShellClassRec, /* superclass                   */
    "XemAppShell",                           /* class_name                   */
    sizeof(XemAppShellRec),                  /* widget_size                  */
    ClassInitialize,                         /* class_initialize             */
    NULL,                                    /* class_part_initialize        */
    FALSE,                                   /* class_inited                 */
    Initialize,                              /* initialize                   */
    NULL,                                    /* initialize_hook              */
    Realize,                                 /* realize                      */
    actions,                                 /* actions                      */
    XtNumber(actions),                       /* num_actions                  */
    resources,                               /* resources                    */
    XtNumber(resources),                     /* num_resources                */
    NULLQUARK,                               /* xrm_class                    */
    TRUE,                                    /* compress_motion              */
    TRUE,                                    /* compress_exposure            */
    TRUE,                                    /* compress_enterleave          */
    FALSE,                                   /* visible_interest             */
    Destroy,                                 /* destroy                      */
    XtInheritResize,                         /* resize                       */
    XtInheritExpose,                         /* expose                       */
    NULL,                                    /* set_values                   */
    NULL,                                    /* set_values_hook              */
    XtInheritSetValuesAlmost,                /* set_values_almost            */
    NULL,                                    /* get_values_hook              */
    XtInheritAcceptFocus,                    /* accept_focus                 */
    XtVersion,                               /* version                      */
    NULL,                                    /* callback_private             */
    translations,                            /* tm_table                     */
    XtInheritQueryGeometry,                  /* query_geometry               */
    XtInheritDisplayAccelerator,             /* display_accelerator          */
    NULL                                     /* extension                    */
  },
  /* CompositeClassPart */ {
    XtInheritGeometryManager,                /* geometry_manager             */
    XtInheritChangeManaged,                  /* change_managed               */
    XtInheritInsertChild,                    /* insert_child                 */
    XtInheritDeleteChild,                    /* delete_child                 */
    NULL                                     /* extension                    */
  },
  /* ShellClassPart */ {
    NULL                                     /* extension                    */
  },
  /* WMShellClassPart */ {
    NULL                                     /* extension                    */
  },
  /* VendorShellClassPart */ {
    NULL                                     /* extension                    */
  },
  /* TopLevelShellClassPart */ {
    NULL                                     /* extension                    */
  },
  /* ApplicationShellClassPart */ {
    NULL                                     /* extension                    */
  },
  /* XemAppShellClassPart */ {
    NULL                                     /* extension                    */
  }
};

externaldef(xemappshellwidgetclass) WidgetClass xemAppShellWidgetClass = (WidgetClass) &xemAppShellClassRec;

/**
 * XemAppShellWidget::ClassInitialize()
 */
static void ClassInitialize(void)
{
  XemAppShellSuperClass = xemAppShellClassRec.core_class.superclass;
}

/**
 * XemAppShellWidget::Initialize()
 *
 * @param request specifies the requested XemAppShellWidget instance
 * @param widget specifies the XemAppShellWidget instance
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 */
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;

  self->app_shell.WM_PROTOCOLS      = XInternAtom(DisplayOfScreen(self->core.screen), "WM_PROTOCOLS",      FALSE);
  self->app_shell.WM_DELETE_WINDOW  = XInternAtom(DisplayOfScreen(self->core.screen), "WM_DELETE_WINDOW",  FALSE);
  self->app_shell.XdndAware         = XInternAtom(DisplayOfScreen(self->core.screen), "XdndAware",         FALSE);
  self->app_shell.XdndSelection     = XInternAtom(DisplayOfScreen(self->core.screen), "XdndSelection",     FALSE);
  self->app_shell.XdndEnter         = XInternAtom(DisplayOfScreen(self->core.screen), "XdndEnter",         FALSE);
  self->app_shell.XdndLeave         = XInternAtom(DisplayOfScreen(self->core.screen), "XdndLeave",         FALSE);
  self->app_shell.XdndPosition      = XInternAtom(DisplayOfScreen(self->core.screen), "XdndPosition",      FALSE);
  self->app_shell.XdndDrop          = XInternAtom(DisplayOfScreen(self->core.screen), "XdndDrop",          FALSE);
  self->app_shell.XdndStatus        = XInternAtom(DisplayOfScreen(self->core.screen), "XdndStatus",        FALSE);
  self->app_shell.XdndFinished      = XInternAtom(DisplayOfScreen(self->core.screen), "XdndFinished",      FALSE);
  self->app_shell.XdndActionCopy    = XInternAtom(DisplayOfScreen(self->core.screen), "XdndActionCopy",    FALSE);
  self->app_shell.XdndActionMove    = XInternAtom(DisplayOfScreen(self->core.screen), "XdndActionMove",    FALSE);
  self->app_shell.XdndActionLink    = XInternAtom(DisplayOfScreen(self->core.screen), "XdndActionLink",    FALSE);
  self->app_shell.XdndActionAsk     = XInternAtom(DisplayOfScreen(self->core.screen), "XdndActionAsk",     FALSE);
  self->app_shell.XdndActionPrivate = XInternAtom(DisplayOfScreen(self->core.screen), "XdndActionPrivate", FALSE);
  self->app_shell.XdndSource        = None;
  self->app_shell.XdndDataT1        = None;
  self->app_shell.XdndDataT2        = None;
  self->app_shell.XdndDataT3        = None;
  self->app_shell.XdndDataT4        = None;
}

/**
 * XemAppShellWidget::Realize()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param mask specifies the attributes mask
 * @param attributes specifies the attributes
 */
static void Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attributes)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;
  unsigned int xdnd_version = 5;

  if(self->core.parent != NULL) {
    XtWidgetGeometry request;
    request.request_mode = CWX | CWY;
    if(self->core.parent->core.width > self->core.width) {
      request.x = self->core.parent->core.x + ((self->core.parent->core.width - self->core.width) / 2);
    }
    else {
      request.x = self->core.parent->core.x - ((self->core.width - self->core.parent->core.width) / 2);
    }
    if((request.x + self->core.width) > WidthOfScreen(self->core.screen)) {
      request.x = WidthOfScreen(self->core.screen) - self->core.width;
    }
    if(request.x < 0) {
      request.x = 0;
    }
    if(self->core.parent->core.height > self->core.height) {
      request.y = self->core.parent->core.y + ((self->core.parent->core.height - self->core.height) / 2);
    }
    else {
      request.y = self->core.parent->core.y - ((self->core.height - self->core.parent->core.height) / 2);
    }
    if((request.y + self->core.height) > HeightOfScreen(self->core.screen)) {
      request.y = HeightOfScreen(self->core.screen) - self->core.height;
    }
    if(request.y < 0) {
      request.y = 0;
    }
    (void) XtMakeGeometryRequest(widget, &request, &request);
  }
  if(XemAppShellSuperClass->core_class.realize != NULL) {
    (*XemAppShellSuperClass->core_class.realize)(widget, mask, attributes);
  }
  if(self->core.window != None) {
    if(self->app_shell.WM_DELETE_WINDOW != None) {
      (void) XSetWMProtocols(DisplayOfScreen(self->core.screen), self->core.window, &self->app_shell.WM_DELETE_WINDOW, 1);
    }
    if(self->app_shell.XdndAware != None) {
      (void) XChangeProperty(XtDisplay(widget), XtWindow(widget), self->app_shell.XdndAware, XA_ATOM, 32, PropModeReplace, (unsigned char *) &xdnd_version, 1);
    }
  }
}

/**
 * XemAppShellWidget::Destroy()
 *
 * @param widget specifies the XemAppShellWidget instance
 */
static void Destroy(Widget widget)
{
}

/**
 * XemAppShellWidget::OnWMProtocols()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void OnWMProtocols(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;

  if((xevent->type                 == ClientMessage                   )
  && (xevent->xclient.message_type == self->app_shell.WM_PROTOCOLS    )
  && (xevent->xclient.data.l[0]    == self->app_shell.WM_DELETE_WINDOW)) {
    if(XtHasCallbacks(widget, XtNwmCloseCallback) == XtCallbackHasSome) {
      XtCallCallbackList(widget, self->app_shell.wm_close_callback, NULL);
    }
    else {
      XtDestroyWidget(widget);
    }
  }
}

/**
 * XemAppShellWidget::ConvertSelection()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param data specifies the user data
 * @param selection specifies the selection atom
 * @param type specifies the type atom
 * @param value specifies the converted value
 * @param length specifies the value length
 * @param format specifies the value format
 */
static void ConvertSelection(
  Widget         widget,
  XtPointer      data,
  Atom          *selection,
  Atom          *type,
  XtPointer      value,
  unsigned long *length,
  int           *format
)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;

  if(value != NULL) {
    XtCallCallbackList(widget, self->app_shell.drop_uri_callback, value);
    XtFree((char *) value); value = NULL;
  }
}

/**
 * XemAppShellWidget::OnXdndEnter()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void OnXdndEnter(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;

  if((xevent->type == ClientMessage) && (xevent->xclient.message_type == self->app_shell.XdndEnter)) {
    if(self->app_shell.XdndSource != xevent->xclient.data.l[0]) {
      self->app_shell.XdndSource = xevent->xclient.data.l[0];
      self->app_shell.XdndDataT1 = xevent->xclient.data.l[2];
      self->app_shell.XdndDataT2 = xevent->xclient.data.l[3];
      self->app_shell.XdndDataT3 = xevent->xclient.data.l[4];
      self->app_shell.XdndDataT4 = XA_STRING;
    }
    else {
      XtAppWarning(XtWidgetToApplicationContext(widget), "XemAppShellWidget::OnXdndEnter()");
    }
  }
}

/**
 * XemAppShellWidget::OnXdndLeave()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void OnXdndLeave(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;

  if((xevent->type == ClientMessage) && (xevent->xclient.message_type == self->app_shell.XdndLeave)) {
    if(self->app_shell.XdndSource == xevent->xclient.data.l[0]) {
      self->app_shell.XdndSource = None;
      self->app_shell.XdndDataT1 = None;
      self->app_shell.XdndDataT2 = None;
      self->app_shell.XdndDataT3 = None;
      self->app_shell.XdndDataT4 = None;
    }
    else {
      XtAppWarning(XtWidgetToApplicationContext(widget), "XemAppShellWidget::OnXdndLeave()");
    }
  }
}

/**
 * XemAppShellWidget::OnXdndPosition()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void OnXdndPosition(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;
  XEvent reply;

  if((xevent->type == ClientMessage) && (xevent->xclient.message_type == self->app_shell.XdndPosition)) {
    if(self->app_shell.XdndSource == xevent->xclient.data.l[0]) {
      Display *dpy = XtDisplay(widget);
      Window src = RootWindowOfScreen(XtScreen(widget));
      Window dst = XtWindow(widget), kid = None;
      int sx = (xevent->xclient.data.l[2] >> 16) & 0xffff, dx = 0;
      int sy = (xevent->xclient.data.l[2] >>  0) & 0xffff, dy = 0;
      while((src != None) && (dst != None) && (XTranslateCoordinates(dpy, src, dst, sx, sy, &dx, &dy, &kid) != False)) {
        src = dst;
        sx  = dx;
        sy  = dy;
        dst = kid;
      }
      /*
      if(src != None) {
        (void) printf("XdndPosition => %s: %4d - %4d\n\n", XtName(XtWindowToWidget(dpy, src)), sx, sy);
      }
      */
      reply.xclient.type         = ClientMessage;
      reply.xclient.serial       = 0;
      reply.xclient.send_event   = True;
      reply.xclient.display      = XtDisplay(widget);
      reply.xclient.window       = xevent->xclient.data.l[0];
      reply.xclient.message_type = self->app_shell.XdndStatus;
      reply.xclient.format       = 32;
      reply.xclient.data.l[0]    = XtWindow(widget);
      reply.xclient.data.l[1]    = 0x03;
      reply.xclient.data.l[2]    = 0;
      reply.xclient.data.l[3]    = 0;
      reply.xclient.data.l[4]    = self->app_shell.XdndActionCopy;
      (void) XSendEvent(XtDisplay(widget), self->app_shell.XdndSource, False, 0, &reply);
    }
    else {
      XtAppWarning(XtWidgetToApplicationContext(widget), "XemAppShellWidget::OnXdndPosition()");
    }
  }
}

/**
 * XemAppShellWidget::OnXdndDrop()
 *
 * @param widget specifies the XemAppShellWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void OnXdndDrop(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  XemAppShellWidget self = (XemAppShellWidget) widget;
  XEvent reply;

  if((xevent->type == ClientMessage) && (xevent->xclient.message_type == self->app_shell.XdndDrop)) {
    if(self->app_shell.XdndSource == xevent->xclient.data.l[0]) {
      if(self->app_shell.XdndDataT1 != None) {
        XtGetSelectionValue(widget, self->app_shell.XdndSelection, self->app_shell.XdndDataT1, ConvertSelection, NULL, xevent->xclient.data.l[2]);
      }
      if(self->app_shell.XdndDataT2 != None) {
        XtGetSelectionValue(widget, self->app_shell.XdndSelection, self->app_shell.XdndDataT2, ConvertSelection, NULL, xevent->xclient.data.l[2]);
      }
      if(self->app_shell.XdndDataT3 != None) {
        XtGetSelectionValue(widget, self->app_shell.XdndSelection, self->app_shell.XdndDataT3, ConvertSelection, NULL, xevent->xclient.data.l[2]);
      }
      if(self->app_shell.XdndDataT4 != None) {
        XtGetSelectionValue(widget, self->app_shell.XdndSelection, self->app_shell.XdndDataT4, ConvertSelection, NULL, xevent->xclient.data.l[2]);
      }
      reply.xclient.type         = ClientMessage;
      reply.xclient.serial       = 0;
      reply.xclient.send_event   = True;
      reply.xclient.display      = XtDisplay(widget);
      reply.xclient.window       = xevent->xclient.data.l[0];
      reply.xclient.message_type = self->app_shell.XdndFinished;
      reply.xclient.format       = 32;
      reply.xclient.data.l[0]    = XtWindow(widget);
      reply.xclient.data.l[1]    = 0x01;
      reply.xclient.data.l[2]    = self->app_shell.XdndActionCopy;
      reply.xclient.data.l[3]    = 0;
      reply.xclient.data.l[4]    = 0;
      (void) XSendEvent(XtDisplay(widget), self->app_shell.XdndSource, False, 0, &reply);
      self->app_shell.XdndSource = None;
      self->app_shell.XdndDataT1 = None;
      self->app_shell.XdndDataT2 = None;
      self->app_shell.XdndDataT3 = None;
      self->app_shell.XdndDataT4 = None;
    }
    else {
      XtAppWarning(XtWidgetToApplicationContext(widget), "XemAppShellWidget::OnXdndDrop()");
    }
  }
}

/**
 * XemAppShellWidget::Create()
 *
 * @param parent specifies the parent widget
 * @param name specifies the name of the created widget
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 *
 * @return the XemAppShellWidget instance
 */
Widget XemCreateAppShell(Widget parent, String name, ArgList args, Cardinal num_args)
{
  return(XtCreateWidget(name, xemAppShellWidgetClass, parent, args, num_args));
}
