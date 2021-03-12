/*
 * Emulator.c - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#ifndef WIDGET_CLASS
#define WIDGET_CLASS(widget_class) ((WidgetClass)(widget_class))
#endif

#ifndef XT_POINTER
#define XT_POINTER(pointer) ((XtPointer)(pointer))
#endif

#ifndef XT_EVENT_HANDLER
#define XT_EVENT_HANDLER(handler) ((XtEventHandler)(handler))
#endif

#ifndef XT_TIMER_CALLBACK_PROC
#define XT_TIMER_CALLBACK_PROC(proc) ((XtTimerCallbackProc)(proc))
#endif

#ifndef XT_INTERVAL_ID
#define XT_INTERVAL_ID(timer) ((XtIntervalId)(timer))
#endif

#ifndef XEM_EMULATOR_WIDGET
#define XEM_EMULATOR_WIDGET(widget) ((XemEmulatorWidget)(widget))
#endif

static void ClassInitialize(void);
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal* num_args);
static void Realize(Widget widget, XtValueMask* mask, XSetWindowAttributes* attributes);
static void Destroy(Widget widget);
static void Resize(Widget widget);
static void Redraw(Widget widget, XEvent* event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal* num_args);
static void OnKeyPress(Widget widget, XEvent* xevent, String* params, Cardinal* num_params);
static void OnKeyRelease(Widget widget, XEvent* xevent, String* params, Cardinal* num_params);
static void OnButtonPress(Widget widget, XEvent* xevent, String* params, Cardinal* num_params);
static void OnButtonRelease(Widget widget, XEvent* xevent, String* params, Cardinal* num_params);
static void OnConfigureNotify(Widget widget, XEvent* xevent, String* params, Cardinal* num_params);
static void TimerHnd(Widget widget, XtIntervalId* timer);

/**
 * XemEmulatorWidget::actions[]
 */
static XtActionsRec actions[] = {
    { "OnKeyPress"       , OnKeyPress        },
    { "OnKeyRelease"     , OnKeyRelease      },
    { "OnButtonPress"    , OnButtonPress     },
    { "OnButtonRelease"  , OnButtonRelease   },
    { "OnConfigureNotify", OnConfigureNotify },
};

/**
 * XemEmulatorWidget::translations[]
 */
static char translations[] = "\
<KeyPress>:        OnKeyPress()\n\
<KeyRelease>:      OnKeyRelease()\n\
<ButtonPress>:     OnButtonPress()\n\
<ButtonRelease>:   OnButtonRelease()\n\
<ConfigureNotify>: OnConfigureNotify()\n\
";

/**
 * XemEmulatorWidget::resources[]
 */
static XtResource resources[] = {
    /* XtNemuContext */ {
        XtNemuContext, XtCPointer, XtRPointer,
        sizeof(XtPointer), XtOffsetOf(XemEmulatorRec, emulator.context),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuCreateProc */ {
        XtNemuCreateProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.create_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuDestroyProc */ {
        XtNemuDestroyProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.destroy_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuRealizeProc */ {
        XtNemuRealizeProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.realize_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuResizeProc */ {
        XtNemuResizeProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.resize_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuRedrawProc */ {
        XtNemuRedrawProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.redraw_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuInputProc */ {
        XtNemuInputProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.input_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNemuTimerProc */ {
        XtNemuTimerProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.timer_proc),
        XtRImmediate, XT_POINTER(NULL)
    },
};

/*
 * XemEmulatorWidget::SuperClass
 */
static WidgetClass XemEmulatorSuperClass = WIDGET_CLASS(NULL);

/*
 * XemEmulatorWidget::Class
 */
externaldef(xememulatorclassrec) XemEmulatorClassRec xemEmulatorClassRec = {
    /* CoreClassPart */ {
        WIDGET_CLASS(&coreClassRec), /* superclass            */
        "XemEmulator",               /* class_name            */
        sizeof(XemEmulatorRec),      /* widget_size           */
        ClassInitialize,             /* class_initialize      */
        NULL,                        /* class_part_initialize */
        FALSE,                       /* class_inited          */
        Initialize,                  /* initialize            */
        NULL,                        /* initialize_hook       */
        Realize,                     /* realize               */
        actions,                     /* actions               */
        XtNumber(actions),           /* num_actions           */
        resources,                   /* resources             */
        XtNumber(resources),         /* num_resources         */
        NULLQUARK,                   /* xrm_class             */
        TRUE,                        /* compress_motion       */
        TRUE,                        /* compress_exposure     */
        TRUE,                        /* compress_enterleave   */
        FALSE,                       /* visible_interest      */
        Destroy,                     /* destroy               */
        Resize,                      /* resize                */
        Redraw,                      /* expose                */
        SetValues,                   /* set_values            */
        NULL,                        /* set_values_hook       */
        XtInheritSetValuesAlmost,    /* set_values_almost     */
        NULL,                        /* get_values_hook       */
        XtInheritAcceptFocus,        /* accept_focus          */
        XtVersion,                   /* version               */
        NULL,                        /* callback_private      */
        translations,                /* tm_table              */
        XtInheritQueryGeometry,      /* query_geometry        */
        XtInheritDisplayAccelerator, /* display_accelerator   */
        NULL                         /* extension             */
    },
    /* XemEmulatorClassPart */ {
        NULL                         /* extension             */
    }
};

externaldef(xememulatorwidgetclass) WidgetClass xemEmulatorWidgetClass = WIDGET_CLASS(&xemEmulatorClassRec);

/*
 * XemEmulatorWidget::FindShell()
 */
static Widget FindShell(Widget widget)
{
    while((widget != NULL) && (XtIsShell(widget) == FALSE)) {
        widget = XtParent(widget);
    }
    return widget;
}

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
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal* num_args)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    self->emulator.timer = XT_INTERVAL_ID(0);
    self->emulator.delay = 100UL;

    if(request->core.width == 0) {
        self->core.width = 640;
    }
    if(request->core.height == 0) {
        self->core.height = 480;
    }
    if(self->emulator.create_proc != NULL) {
        (void) (*self->emulator.create_proc)(widget, self->emulator.context, NULL);
    }
    self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), self->emulator.delay, XT_TIMER_CALLBACK_PROC(TimerHnd), XT_POINTER(widget));
}

/**
 * XemEmulatorWidget::Realize()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param mask specifies the attributes mask
 * @param attributes specifies the attributes
 */
static void Realize(Widget widget, XtValueMask* mask, XSetWindowAttributes* attributes)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(XemEmulatorSuperClass->core_class.realize != NULL) {
        (*XemEmulatorSuperClass->core_class.realize)(widget, mask, attributes);
    }
    if(XtGetKeyboardFocusWidget(widget) != widget) {
        XtSetKeyboardFocus(FindShell(widget), widget);
    }
    if(self->emulator.realize_proc != NULL) {
        (void) (*self->emulator.realize_proc)(widget, self->emulator.context, NULL);
    }
}

/**
 * XemEmulatorWidget::Destroy()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Destroy(Widget widget)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(self->emulator.timer != XT_INTERVAL_ID(0)) {
        XtRemoveTimeOut(self->emulator.timer);
        self->emulator.timer = XT_INTERVAL_ID(0);
        self->emulator.delay = 0UL;
    }
    if(self->emulator.destroy_proc != NULL) {
        (void) (*self->emulator.destroy_proc)(widget, self->emulator.context, NULL);
    }
}

/**
 * XemEmulatorWidget::Resize()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Resize(Widget widget)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(self->emulator.resize_proc != NULL) {
        (void) (*self->emulator.resize_proc)(widget, self->emulator.context, NULL);
    }
}

/**
 * XemEmulatorWidget::Redraw()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param region specifies the Region information
 */
static void Redraw(Widget widget, XEvent* xevent, Region region)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(self->emulator.redraw_proc != NULL) {
        (void) (*self->emulator.redraw_proc)(widget, self->emulator.context, xevent);
    }
}

/**
 * XemEmulatorWidget::SetValues()
 *
 * @param cur_w specifies the current XemEmulatorWidget instance
 * @param req_w specifies the requested XemEmulatorWidget instance
 * @param new_w specifies the new XemEmulatorWidget instance
 */
static Boolean SetValues(Widget ow, Widget rw, Widget nw, ArgList args, Cardinal* num_args)
{
    return FALSE;
}

/**
 * XemEmulatorWidget::OnKeyPress()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnKeyPress(Widget widget, XEvent* xevent, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(xevent->type != KeyPress) {
        return;
    }
    if(self->emulator.input_proc != NULL) {
        (void) (*self->emulator.input_proc)(widget, self->emulator.context, xevent);
    }
}

/**
 * XemEmulatorWidget::OnKeyRelease()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnKeyRelease(Widget widget, XEvent* xevent, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(xevent->type != KeyRelease) {
        return;
    }
    else {
        XEvent pending;
        if(XPending(xevent->xany.display) > 0) {
            (void) XPeekEvent(xevent->xany.display, &pending);
            if((pending.type         == KeyPress            )
            && (pending.xkey.display == xevent->xkey.display)
            && (pending.xkey.window  == xevent->xkey.window )
            && (pending.xkey.keycode == xevent->xkey.keycode)
            && ((pending.xkey.time - xevent->xkey.time) < 5)) {
                (void) XNextEvent(xevent->xany.display, &pending);
                return;
            }
        }
    }
    if(self->emulator.input_proc != NULL) {
        (void) (*self->emulator.input_proc)(widget, self->emulator.context, xevent);
    }
}

/**
 * XemEmulatorWidget::OnButtonPress()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnButtonPress(Widget widget, XEvent* xevent, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(xevent->type != ButtonPress) {
        return;
    }
    else {
        if(XtGetKeyboardFocusWidget(widget) != widget) {
            XtSetKeyboardFocus(FindShell(widget), widget);
        }
    }
    if(self->emulator.input_proc != NULL) {
        (void) (*self->emulator.input_proc)(widget, self->emulator.context, xevent);
    }
}

/**
 * XemEmulatorWidget::OnButtonRelease()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnButtonRelease(Widget widget, XEvent* xevent, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(xevent->type != ButtonRelease) {
        return;
    }
    if(self->emulator.input_proc != NULL) {
        (void) (*self->emulator.input_proc)(widget, self->emulator.context, xevent);
    }
}

/**
 * XemEmulatorWidget::OnConfigureNotify()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param xevent specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnConfigureNotify(Widget widget, XEvent* xevent, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(xevent->type != ConfigureNotify) {
        return;
    }
    else {
        Arg      arglist[2];
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
    }
    if(self->emulator.resize_proc != NULL) {
        (void) (*self->emulator.resize_proc)(widget, self->emulator.context, xevent);
    }
}

/**
 * XemEmulatorWidget::TimerHnd()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param timer specifies the XtIntervalId timer instance
 */
static void TimerHnd(Widget widget, XtIntervalId* timer)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if((self->emulator.timer_proc != NULL) && (self->core.sensitive != FALSE) && (self->core.ancestor_sensitive != FALSE)) {
        self->emulator.delay = (*self->emulator.timer_proc)(widget, self->emulator.context, NULL);
    }
    else {
        self->emulator.delay = 100UL;
    }
    self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), self->emulator.delay, XT_TIMER_CALLBACK_PROC(TimerHnd), XT_POINTER(widget));
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
    return XtCreateWidget(name, xemEmulatorWidgetClass, parent, args, num_args);
}
