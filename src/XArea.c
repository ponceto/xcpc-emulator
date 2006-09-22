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
static void Resize(Widget widget);
static void Redisplay(Widget widget, XEvent *event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal *num_args);

static XtResource resources[] = {
  /* exposeCallback */ {
    "exposeCallback", XtCCallback, XtRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XAreaRec, xarea.expose_callback),
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
    Resize,                                 /* resize                       */
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
 * XArea::Initialize
 *
 * @param request specifies the XArea widget
 * @param widget specifies the XArea widget
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 */
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *num_args)
{
}

/**
 * XArea::Destroy
 *
 * @param widget specifies the XArea widget
 */
static void Destroy(Widget widget)
{
}

/**
 * XArea::Resize
 *
 * @param widget specifies the XArea widget
 */
static void Resize(Widget widget)
{
}

/**
 * XArea::Redisplay
 *
 * @param widget specifies the XArea widget
 * @param xevent specifies the XEvent
 * @param region specifies the Region
 */
static void Redisplay(Widget widget, XEvent *xevent, Region region)
{
}

/**
 * XArea::SetValues
 *
 * @param cur_w specifies the XArea widget
 * @param req_w specifies the XArea widget
 * @param new_w specifies the XArea widget
 */
static Boolean SetValues(Widget ow, Widget rw, Widget nw, ArgList args, Cardinal *num_args)
{
  return(FALSE);
}

/**
 * XArea::Create
 *
 * @param parent specifies the parent widget
 * @param name specifies the name of the created widget
 * @param args specifies the argument list
 * @param num_args specifies the argument count
 *
 * @return the XArea widget
 */
Widget XAreaCreate(Widget parent, String name, ArgList args, Cardinal num_args)
{
  return(XtCreateWidget(name, xAreaWidgetClass, parent, args, num_args));
}
