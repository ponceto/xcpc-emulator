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
static void ClassInitialize(void);
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal* num_args);
static void Realize(Widget widget, XtValueMask* mask, XSetWindowAttributes* attributes);
static void Destroy(Widget widget);
static void Resize(Widget widget);
static void Redraw(Widget widget, XEvent* event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal* num_args);
static void OnKeyPress(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnKeyRelease(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnButtonPress(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnButtonRelease(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnConfigureNotify(Widget widget, XEvent* event, String* params, Cardinal* num_params);
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
    /* XtNemuExposeProc */ {
        XtNemuExposeProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.expose_proc),
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

/*
 * XemEmulatorWidget::DefaultProc()
 */
static unsigned long DefaultProc(XtPointer data, XEvent* event)
{
    return 100UL;
}

/*
 * XemEmulatorWidget::Sanitize()
 */
static void Sanitize(Widget widget)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    if(self->emulator.create_proc == NULL) {
        self->emulator.create_proc = &DefaultProc;
    }
    if(self->emulator.destroy_proc == NULL) {
        self->emulator.destroy_proc = &DefaultProc;
    }
    if(self->emulator.realize_proc == NULL) {
        self->emulator.realize_proc = &DefaultProc;
    }
    if(self->emulator.resize_proc == NULL) {
        self->emulator.resize_proc = &DefaultProc;
    }
    if(self->emulator.expose_proc == NULL) {
        self->emulator.expose_proc = &DefaultProc;
    }
    if(self->emulator.input_proc == NULL) {
        self->emulator.input_proc = &DefaultProc;
    }
    if(self->emulator.timer_proc == NULL) {
        self->emulator.timer_proc = &DefaultProc;
    }
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

    /* sanitize */ {
        Sanitize(widget);
    }
    /* initialize geometry */ {
        if(request->core.width == 0) {
            self->core.width = 640;
        }
        if(request->core.height == 0) {
            self->core.height = 480;
        }
    }
    /* initialize timer */ {
        self->emulator.timer = XT_INTERVAL_ID(0);
        self->emulator.delay = 100UL;
    }
    /* init event */ {
        self->emulator.event.xany.type       = GenericEvent;
        self->emulator.event.xany.serial     = 0UL;
        self->emulator.event.xany.send_event = True;
        self->emulator.event.xany.display    = XtDisplay(widget);
        self->emulator.event.xany.window     = XtWindow(widget);
    }
    /* call create-proc */ {
        (void) (*self->emulator.create_proc)(self->emulator.context, &self->emulator.event);
    }
    /* schedule timer */ {
        self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), self->emulator.delay, XT_TIMER_CALLBACK_PROC(TimerHnd), XT_POINTER(widget));
    }
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

    /* call superclass method */ {
        if(XemEmulatorSuperClass->core_class.realize != NULL) {
            (*XemEmulatorSuperClass->core_class.realize)(widget, mask, attributes);
        }
    }
    /* init event */ {
        self->emulator.event.xany.type       = GenericEvent;
        self->emulator.event.xany.serial     = 0UL;
        self->emulator.event.xany.send_event = True;
        self->emulator.event.xany.display    = XtDisplay(widget);
        self->emulator.event.xany.window     = XtWindow(widget);
    }
    /* call realize-proc */ {
        (void) (*self->emulator.realize_proc)(self->emulator.context, &self->emulator.event);
    }
    /* set keyboard focus */ {
        XtSetKeyboardFocus(FindShell(widget), widget);
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

    /* finalize timer */ {
        if(self->emulator.timer != XT_INTERVAL_ID(0)) {
            XtRemoveTimeOut(self->emulator.timer);
            self->emulator.timer = XT_INTERVAL_ID(0);
            self->emulator.delay = 0UL;
        }
    }
    /* init event */ {
        self->emulator.event.xany.type       = GenericEvent;
        self->emulator.event.xany.serial     = 0UL;
        self->emulator.event.xany.send_event = True;
        self->emulator.event.xany.display    = XtDisplay(widget);
        self->emulator.event.xany.window     = XtWindow(widget);
    }
    /* call destroy-proc */ {
        (void) (*self->emulator.destroy_proc)(self->emulator.context, &self->emulator.event);
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

    /* init event */ {
        self->emulator.event.xany.type       = GenericEvent;
        self->emulator.event.xany.serial     = 0UL;
        self->emulator.event.xany.send_event = True;
        self->emulator.event.xany.display    = XtDisplay(widget);
        self->emulator.event.xany.window     = XtWindow(widget);
    }
    /* call resize-proc */ {
        (void) (*self->emulator.resize_proc)(self->emulator.context, &self->emulator.event);
    }
}

/**
 * XemEmulatorWidget::Redraw()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param region specifies the Region information
 */
static void Redraw(Widget widget, XEvent* event, Region region)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    /* store event */ {
        self->emulator.event = *event;
    }
    /* call expose-proc */ {
        (void) (*self->emulator.expose_proc)(self->emulator.context, &self->emulator.event);
    }
}

/**
 * XemEmulatorWidget::SetValues()
 *
 * @param current specifies the current XemEmulatorWidget instance
 * @param request specifies the requested XemEmulatorWidget instance
 * @param widget specifies the new XemEmulatorWidget instance
 */
static Boolean SetValues(Widget current, Widget request, Widget widget, ArgList args, Cardinal* num_args)
{
    /* sanitize */ {
        Sanitize(widget);
    }
    return FALSE;
}

/**
 * XemEmulatorWidget::OnKeyPress()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnKeyPress(Widget widget, XEvent* event, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != KeyPress) {
            return;
        }
    }
    /* store event */ {
        self->emulator.event = *event;
    }
    /* call input-proc */ {
        (void) (*self->emulator.input_proc)(self->emulator.context, &self->emulator.event);
    }
}

/**
 * XemEmulatorWidget::OnKeyRelease()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnKeyRelease(Widget widget, XEvent* event, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != KeyRelease) {
            return;
        }
    }
    /* fast key detection */ {
        if((self->emulator.event.type         == KeyPress           )
        && (self->emulator.event.xkey.display == event->xkey.display)
        && (self->emulator.event.xkey.window  == event->xkey.window )
        && (self->emulator.event.xkey.keycode == event->xkey.keycode)
        && ((event->xkey.time - self->emulator.event.xkey.time) < 10)) {
            /* todo : throttle event */
        }
    }
    /* auto-repeat detection */ {
        XEvent pending;
        if(XPending(event->xany.display) > 0) {
            (void) XPeekEvent(event->xany.display, &pending);
            if((pending.type         == KeyPress           )
            && (pending.xkey.display == event->xkey.display)
            && (pending.xkey.window  == event->xkey.window )
            && (pending.xkey.keycode == event->xkey.keycode)
            && ((pending.xkey.time - event->xkey.time) < 10)) {
                (void) XNextEvent(event->xany.display, &pending);
                return;
            }
        }
    }
    /* store event */ {
        self->emulator.event = *event;
    }
    /* call input-proc */ {
        (void) (*self->emulator.input_proc)(self->emulator.context, &self->emulator.event);
    }
}

/**
 * XemEmulatorWidget::OnButtonPress()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnButtonPress(Widget widget, XEvent* event, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != ButtonPress) {
            return;
        }
    }
    /* store event */ {
        self->emulator.event = *event;
    }
    /* call input-proc */ {
        (void) (*self->emulator.input_proc)(self->emulator.context, &self->emulator.event);
    }
    /* set keyboard focus */ {
        XtSetKeyboardFocus(FindShell(widget), widget);
    }
}

/**
 * XemEmulatorWidget::OnButtonRelease()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnButtonRelease(Widget widget, XEvent* event, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != ButtonRelease) {
            return;
        }
    }
    /* store event */ {
        self->emulator.event = *event;
    }
    /* call input-proc */ {
        (void) (*self->emulator.input_proc)(self->emulator.context, &self->emulator.event);
    }
}

/**
 * XemEmulatorWidget::OnConfigureNotify()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void OnConfigureNotify(Widget widget, XEvent* event, String* params, Cardinal* num_params)
{
    XemEmulatorWidget self = XEM_EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != ConfigureNotify) {
            return;
        }
    }
    /* configure widget */ {
        Arg      arglist[2];
        Cardinal argcount = 0;
        if(event->xconfigure.width != self->core.width) {
            XtSetArg(arglist[argcount], XtNwidth, event->xconfigure.width); argcount++;
        }
        if(event->xconfigure.height != self->core.height) {
            XtSetArg(arglist[argcount], XtNheight, event->xconfigure.height); argcount++;
        }
        if(argcount > 0) {
            XtSetValues(widget, arglist, argcount);
        }
    }
    /* store event */ {
        self->emulator.event = *event;
    }
    /* call resize-proc */ {
        (void) (*self->emulator.resize_proc)(self->emulator.context, &self->emulator.event);
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

    /* init event */ {
        self->emulator.event.xany.type       = GenericEvent;
        self->emulator.event.xany.serial     = 0UL;
        self->emulator.event.xany.send_event = True;
        self->emulator.event.xany.display    = XtDisplay(widget);
        self->emulator.event.xany.window     = XtWindow(widget);
    }
    /* call timer-proc */ {
        if((self->core.sensitive != FALSE) && (self->core.ancestor_sensitive != FALSE)) {
            self->emulator.delay = (*self->emulator.timer_proc)(self->emulator.context, &self->emulator.event);
        }
        else {
            self->emulator.delay = 100UL;
        }
    }
    /* schedule timer */ {
        self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), self->emulator.delay, XT_TIMER_CALLBACK_PROC(TimerHnd), XT_POINTER(widget));
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
    return XtCreateWidget(name, xemEmulatorWidgetClass, parent, args, num_args);
}
