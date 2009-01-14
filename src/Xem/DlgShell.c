/*
 * DlgShell.c - Copyright (c) 2001, 2006, 2007, 2008, 2009 Olivier Poncet
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
#include <Xem/DlgShellP.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

static void ClassInitialize(void);
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args);
static void Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attributes);
static void Destroy(Widget widget);
static void OnWMProtocols(Widget widget, XEvent *xevent, String *params, Cardinal *num_params);

/**
 * XemDlgShellWidget::actions[]
 */
static XtActionsRec actions[] = {
  { "OnWMProtocols", OnWMProtocols }
};

/**
 * XemDlgShellWidget::translations[]
 */
static char translations[] = "\
<Message>WM_PROTOCOLS: OnWMProtocols()\
";

/**
 * XemDlgShellWidget::resources[]
 */
static XtResource resources[] = {
  /* XtNwmCloseCallback */ {
    XtNwmCloseCallback, XtCCallback, XtRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XemDlgShellRec, dlg_shell.wm_close_callback),
    XtRImmediate, (XtPointer) NULL
  },
  /* XtNdropURICallback */ {
    XtNdropURICallback, XtCCallback, XtRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XemDlgShellRec, dlg_shell.drop_uri_callback),
    XtRImmediate, (XtPointer) NULL
  }
};

/*
 * XemDlgShellWidget::SuperClass
 */
static WidgetClass XemDlgShellSuperClass = (WidgetClass) NULL;

/*
 * XemDlgShellWidget::Class
 */
externaldef(xemdlgshellclassrec) XemDlgShellClassRec xemDlgShellClassRec = {
  /* CoreClassPart */ {
    (WidgetClass) &transientShellClassRec,   /* superclass                   */
    "XemDlgShell",                           /* class_name                   */
    sizeof(XemDlgShellRec),                  /* widget_size                  */
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
  /* TransientShellClassPart */ {
    NULL                                     /* extension                    */
  },
  /* XemDlgShellClassPart */ {
    NULL                                     /* extension                    */
  }
};

externaldef(xemdlgshellwidgetclass) WidgetClass xemDlgShellWidgetClass = (WidgetClass) &xemDlgShellClassRec;

/**
 * XemDlgShellWidget::ClassInitialize()
 */
static void ClassInitialize(void)
{
  XemDlgShellSuperClass = xemDlgShellClassRec.core_class.superclass;
}

/**
 * XemDlgShellWidget::Initialize()
 *
 * @param request specifies the requested XemDlgShellWidget instance
 * @param widget specifies the XemDlgShellWidget instance
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 */
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args)
{
  XemDlgShellWidget self = (XemDlgShellWidget) widget;

  self->dlg_shell.WM_PROTOCOLS     = XInternAtom(DisplayOfScreen(self->core.screen), "WM_PROTOCOLS",     FALSE);
  self->dlg_shell.WM_DELETE_WINDOW = XInternAtom(DisplayOfScreen(self->core.screen), "WM_DELETE_WINDOW", FALSE);
}

/**
 * XemDlgShellWidget::Realize()
 *
 * @param widget specifies the XemDlgShellWidget instance
 * @param mask specifies the attributes mask
 * @param attributes specifies the attributes
 */
static void Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attributes)
{
  XemDlgShellWidget self = (XemDlgShellWidget) widget;

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
  if(XemDlgShellSuperClass->core_class.realize != NULL) {
    (*XemDlgShellSuperClass->core_class.realize)(widget, mask, attributes);
  }
  if(self->core.window != None) {
    if(self->dlg_shell.WM_DELETE_WINDOW != None) {
      (void) XSetWMProtocols(DisplayOfScreen(self->core.screen), self->core.window, &self->dlg_shell.WM_DELETE_WINDOW, 1);
    }
  }
}

/**
 * XemDlgShellWidget::Destroy()
 *
 * @param widget specifies the XemDlgShellWidget instance
 */
static void Destroy(Widget widget)
{
}

/**
 * XemDlgShellWidget::OnWMProtocols()
 *
 * @param widget specifies the XemDlgShellWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameter list
 * @param num_params specifies the parameter count
 */
static void OnWMProtocols(Widget widget, XEvent *xevent, String *params, Cardinal *num_params)
{
  XemDlgShellWidget self = (XemDlgShellWidget) widget;

  if((xevent->type                 == ClientMessage                   )
  && (xevent->xclient.message_type == self->dlg_shell.WM_PROTOCOLS    )
  && (xevent->xclient.data.l[0]    == self->dlg_shell.WM_DELETE_WINDOW)) {
    if(XtHasCallbacks(widget, XtNwmCloseCallback) == XtCallbackHasSome) {
      XtCallCallbackList(widget, self->dlg_shell.wm_close_callback, NULL);
    }
    else {
      XtDestroyWidget(widget);
    }
  }
}

/**
 * XemDlgShellWidget::Create()
 *
 * @param parent specifies the parent widget
 * @param name specifies the name of the created widget
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 *
 * @return the XemDlgShellWidget instance
 */
Widget XemCreateDlgShell(Widget parent, String name, ArgList args, Cardinal num_args)
{
  return(XtCreateWidget(name, xemDlgShellWidgetClass, parent, args, num_args));
}
