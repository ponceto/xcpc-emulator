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

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 640
#endif

#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 480
#endif

#ifndef DEFAULT_TIMEOUT
#define DEFAULT_TIMEOUT 100UL
#endif

#ifndef AUTO_KEY_THRESHOLD
#define AUTO_KEY_THRESHOLD 10
#endif

#ifndef WIDGET_CLASS
#define WIDGET_CLASS(widget_class) ((WidgetClass)(widget_class))
#endif

#ifndef POINTER
#define POINTER(pointer) ((XtPointer)(pointer))
#endif

#ifndef EVENT_HANDLER
#define EVENT_HANDLER(handler) ((XtEventHandler)(handler))
#endif

#ifndef TIMER_CALLBACK_PROC
#define TIMER_CALLBACK_PROC(proc) ((XtTimerCallbackProc)(proc))
#endif

#ifndef INTERVAL_ID
#define INTERVAL_ID(timer) ((XtIntervalId)(timer))
#endif

#ifndef EMULATOR_WIDGET
#define EMULATOR_WIDGET(widget) ((XemEmulatorWidget)(widget))
#endif

static void ClassInitialize(void);
static void ClassInitialize(void);
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal* num_args);
static void Realize(Widget widget, XtValueMask* mask, XSetWindowAttributes* attributes);
static void Destroy(Widget widget);
static void Resize(Widget widget);
static void Redraw(Widget widget, XEvent* event, Region region);
static Boolean SetValues(Widget cur_w, Widget req_w, Widget new_w, ArgList args, Cardinal* num_args);
static void SetFocus(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnKeyPress(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnKeyRelease(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnButtonPress(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnButtonRelease(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void OnConfigureNotify(Widget widget, XEvent* event, String* params, Cardinal* num_params);
static void TimeOutHandler(Widget widget, XtIntervalId* timer);

/**
 * XemEmulatorWidget::actions[]
 */
static XtActionsRec actions[] = {
    { "SetFocus"         , SetFocus          },
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
<ButtonPress>:     OnButtonPress() SetFocus()\n\
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
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuCreateProc */ {
        XtNemuCreateProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.create_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuDestroyProc */ {
        XtNemuDestroyProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.destroy_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuRealizeProc */ {
        XtNemuRealizeProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.realize_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuResizeProc */ {
        XtNemuResizeProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.resize_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuExposeProc */ {
        XtNemuExposeProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.expose_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuInputProc */ {
        XtNemuInputProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.input_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNemuTimerProc */ {
        XtNemuTimerProc, XtCFunction, XtRFunction,
        sizeof(XtWidgetProc), XtOffsetOf(XemEmulatorRec, emulator.timer_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNhotkeyCallback */ {
        XtNhotkeyCallback, XtCCallback, XtRCallback,
        sizeof(XtCallbackList), XtOffsetOf(XemEmulatorRec, emulator.hotkey_callback),
        XtRImmediate, (XtPointer) NULL
    }
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
 * XemEmulatorWidget::DefaultEmulatorProc()
 */
static unsigned long DefaultEmulatorProc(XtPointer data, XEvent* event)
{
    return DEFAULT_TIMEOUT;
}

/*
 * XemEmulatorWidget::SanitizeProperties()
 */
static void SanitizeProperties(Widget widget)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(self->emulator.create_proc == NULL) {
        self->emulator.create_proc = &DefaultEmulatorProc;
    }
    if(self->emulator.destroy_proc == NULL) {
        self->emulator.destroy_proc = &DefaultEmulatorProc;
    }
    if(self->emulator.realize_proc == NULL) {
        self->emulator.realize_proc = &DefaultEmulatorProc;
    }
    if(self->emulator.resize_proc == NULL) {
        self->emulator.resize_proc = &DefaultEmulatorProc;
    }
    if(self->emulator.expose_proc == NULL) {
        self->emulator.expose_proc = &DefaultEmulatorProc;
    }
    if(self->emulator.input_proc == NULL) {
        self->emulator.input_proc = &DefaultEmulatorProc;
    }
    if(self->emulator.timer_proc == NULL) {
        self->emulator.timer_proc = &DefaultEmulatorProc;
    }
}

/*
 * XemEmulatorWidget::CopyOrFillEvent()
 */
static XEvent* CopyOrFillEvent(Widget widget, XEvent* event)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(event != NULL) {
        self->emulator.event = *event;
    }
    else {
        self->emulator.event.xany.type       = GenericEvent;
        self->emulator.event.xany.serial     = 0UL;
        self->emulator.event.xany.send_event = True;
        self->emulator.event.xany.display    = XtDisplay(widget);
        self->emulator.event.xany.window     = XtWindow(widget);
    }
    return &self->emulator.event;
}

/*
 * XemEmulatorWidget::ThrottleInputEvent()
 */
static void ThrottleInputEvent(Widget widget, XEvent* event)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);
    unsigned int head = ((self->emulator.throttled_head + 0) % countof(self->emulator.throttled_data));
    unsigned int tail = ((self->emulator.throttled_tail + 1) % countof(self->emulator.throttled_data));

    /* auto-repeat detection */ {
        if(event->type == KeyPress) {
            XEvent* next = &self->emulator.event;
            if(XPending(event->xany.display) > 0) {
                (void) XPeekEvent(event->xany.display, next);
                if((next->type         == KeyPress           )
                && (next->xkey.display == event->xkey.display)
                && (next->xkey.window  == event->xkey.window )
                && (next->xkey.keycode == event->xkey.keycode)
                && ((next->xkey.time - event->xkey.time) < AUTO_KEY_THRESHOLD)) {
                    (void) XNextEvent(event->xany.display, next);
                    return;
                }
            }
            /* check some hotkeys */ {
                KeySym keysym = XLookupKeysym(&event->xkey, 0);
                if((keysym >= XK_F1) && (keysym <= XK_F35)) {
                    XtCallCallbackList(widget, self->emulator.hotkey_callback, &keysym);
                }
            }
        }
    }
    if(tail != head) {
        self->emulator.event = *event;
        self->emulator.throttled_data[self->emulator.throttled_tail] = *event;
        self->emulator.throttled_head = head;
        self->emulator.throttled_tail = tail;
    }
}

/*
 * XemEmulatorWidget::ProcessThrottledInputEvent()
 */
static void ProcessThrottledInputEvent(Widget widget)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(self->emulator.throttled_head != self->emulator.throttled_tail) {
        XEvent* event = &self->emulator.throttled_data[self->emulator.throttled_head];
        (void) (*self->emulator.input_proc)(self->emulator.context, CopyOrFillEvent(widget, event));
        self->emulator.throttled_head = ((self->emulator.throttled_head + 1) % countof(self->emulator.throttled_data));
        self->emulator.throttled_tail = ((self->emulator.throttled_tail + 0) % countof(self->emulator.throttled_data));
    }
}

/*
 * XemEmulatorWidget::Schedule()
 */
static void Schedule(Widget widget, unsigned long timeout)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(self->emulator.timer == INTERVAL_ID(0)) {
        self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), (timeout != 0UL ? timeout : 1UL), TIMER_CALLBACK_PROC(&TimeOutHandler), POINTER(widget));
    }
}

/*
 * XemEmulatorWidget::Unschedule()
 */
static void Unschedule(Widget widget)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(self->emulator.timer != INTERVAL_ID(0)) {
        XtRemoveTimeOut(self->emulator.timer);
        self->emulator.timer = INTERVAL_ID(0);
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
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* sanitize properties */ {
        SanitizeProperties(widget);
    }
    /* initialize geometry */ {
        if(request->core.width == 0) {
            self->core.width = DEFAULT_WIDTH;
        }
        if(request->core.height == 0) {
            self->core.height = DEFAULT_HEIGHT;
        }
    }
    /* initialize timer */ {
        self->emulator.timer = INTERVAL_ID(0);
    }
    /* throttled events */ {
        self->emulator.throttled_head = 0;
        self->emulator.throttled_tail = 0;
    }
    /* call create-proc */ {
        (void) (*self->emulator.create_proc)(self->emulator.context, CopyOrFillEvent(widget, NULL));
    }
    /* schedule timer */ {
        Schedule(widget, 0UL);
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
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* call superclass method */ {
        if(XemEmulatorSuperClass->core_class.realize != NULL) {
            (*XemEmulatorSuperClass->core_class.realize)(widget, mask, attributes);
        }
    }
    /* call realize-proc */ {
        (void) (*self->emulator.realize_proc)(self->emulator.context, CopyOrFillEvent(widget, NULL));
    }
}

/**
 * XemEmulatorWidget::Destroy()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Destroy(Widget widget)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* unschedule timer */ {
        Unschedule(widget);
    }
    /* call destroy-proc */ {
        (void) (*self->emulator.destroy_proc)(self->emulator.context, CopyOrFillEvent(widget, NULL));
    }
}

/**
 * XemEmulatorWidget::Resize()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Resize(Widget widget)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* call resize-proc */ {
        (void) (*self->emulator.resize_proc)(self->emulator.context, CopyOrFillEvent(widget, NULL));
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
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != Expose) {
            return;
        }
    }
    /* call expose-proc */ {
        (void) (*self->emulator.expose_proc)(self->emulator.context, CopyOrFillEvent(widget, event));
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
    /* sanitize properties */ {
        SanitizeProperties(widget);
    }
    return FALSE;
}

/**
 * XemEmulatorWidget::SetFocus()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param event specifies the XEvent structure
 * @param params specifies the parameters
 * @param num_params specifies the number of parameters
 */
static void SetFocus(Widget widget, XEvent* event, String* params, Cardinal* num_params)
{
    XtSetKeyboardFocus(FindShell(widget), widget);
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
    /* ensure event-type */ {
        if(event->type != KeyPress) {
            return;
        }
    }
    /* throttle event */ {
        ThrottleInputEvent(widget, event);
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
    /* ensure event-type */ {
        if(event->type != KeyRelease) {
            return;
        }
    }
    /* throttle event */ {
        ThrottleInputEvent(widget, event);
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
    /* ensure event-type */ {
        if(event->type != ButtonPress) {
            return;
        }
    }
    /* throttle event */ {
        ThrottleInputEvent(widget, event);
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
    /* ensure event-type */ {
        if(event->type != ButtonRelease) {
            return;
        }
    }
    /* throttle event */ {
        ThrottleInputEvent(widget, event);
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
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* ensure event-type */ {
        if(event->type != ConfigureNotify) {
            return;
        }
    }
    /* configure widget */ {
        Arg      arglist[2];
        Cardinal argcount = 0;
        if(event->xconfigure.width != self->core.width) {
            XtSetArg(arglist[argcount], XtNwidth, event->xconfigure.width); ++argcount;
        }
        if(event->xconfigure.height != self->core.height) {
            XtSetArg(arglist[argcount], XtNheight, event->xconfigure.height); ++argcount;
        }
        if(argcount > 0) {
            XtSetValues(widget, arglist, argcount);
        }
    }
    /* call resize-proc */ {
        (void) (*self->emulator.resize_proc)(self->emulator.context, CopyOrFillEvent(widget, event));
    }
}

/**
 * XemEmulatorWidget::TimeOutHandler()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param timer specifies the XtIntervalId timer instance
 */
static void TimeOutHandler(Widget widget, XtIntervalId* timer)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);
    unsigned long     timeout = 0UL;

    /* acknowledge timer */ {
        if(*timer == self->emulator.timer) {
            self->emulator.timer = INTERVAL_ID(0);
        }
    }
    /* call timer-proc */ {
        if((self->core.sensitive != FALSE) && (self->core.ancestor_sensitive != FALSE)) {
            timeout = (*self->emulator.timer_proc)(self->emulator.context, CopyOrFillEvent(widget, NULL));
        }
    }
    /* schedule timer */ {
        Schedule(widget, timeout);
    }
    /* process throttled input event */ {
        ProcessThrottledInputEvent(widget);
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
