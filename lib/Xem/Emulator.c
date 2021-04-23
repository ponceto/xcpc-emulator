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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_LINUX_JOYSTICK_H
#include <linux/joystick.h>
#include <sys/ioctl.h>
#endif
#include <X11/IntrinsicP.h>
#include <Xem/StringDefs.h>
#include <Xem/EmulatorP.h>

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

#ifndef JS_EVENT_BUTTON
#define JS_EVENT_BUTTON 0x01
#endif

#ifndef JS_EVENT_AXIS
#define JS_EVENT_AXIS 0x02
#endif

#ifndef JS_EVENT_INIT
#define JS_EVENT_INIT 0x80
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

#ifndef KEY_DELAY_THRESHOLD
#define KEY_DELAY_THRESHOLD 20
#endif

#ifndef KEY_REPEAT_THRESHOLD
#define KEY_REPEAT_THRESHOLD 10
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

#ifndef INPUT_CALLBACK_PROC
#define INPUT_CALLBACK_PROC(proc) ((XtInputCallbackProc)(proc))
#endif

#ifndef INTERVAL_ID
#define INTERVAL_ID(interval_id) ((XtIntervalId)(interval_id))
#endif

#ifndef INPUT_ID
#define INPUT_ID(input_id) ((XtInputId)(input_id))
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
    /* XtNmachineInstance */ {
        XtNmachineInstance, XtCPointer, XtRPointer,
        sizeof(XemEmulatorData), XtOffsetOf(XemEmulatorRec, emulator.machine.instance),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineCreateProc */ {
        XtNmachineCreateProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.create_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineDestroyProc */ {
        XtNmachineDestroyProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.destroy_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineRealizeProc */ {
        XtNmachineRealizeProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.realize_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineResizeProc */ {
        XtNmachineResizeProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.resize_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineExposeProc */ {
        XtNmachineExposeProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.expose_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineInputProc */ {
        XtNmachineInputProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.input_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNmachineTimerProc */ {
        XtNmachineTimerProc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorProc), XtOffsetOf(XemEmulatorRec, emulator.machine.timer_proc),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNjoystick0 */ {
        XtNjoystick0, XtCJoystick, XtRString,
        sizeof(String), XtOffsetOf(XemEmulatorRec, emulator.joystick0.device),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNjoystick1 */ {
        XtNjoystick1, XtCJoystick, XtRString,
        sizeof(String), XtOffsetOf(XemEmulatorRec, emulator.joystick1.device),
        XtRImmediate, POINTER(NULL)
    },
    /* XtNhotkeyCallback */ {
        XtNhotkeyCallback, XtCCallback, XtRCallback,
        sizeof(XtCallbackList), XtOffsetOf(XemEmulatorRec, emulator.hotkey_callback),
        XtRImmediate, POINTER(NULL)
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
 * XemEmulatorWidget::DefaultMachineProc()
 */
static unsigned long DefaultMachineProc(XtPointer data, XEvent* event)
{
    return DEFAULT_TIMEOUT;
}

/*
 * XemEmulatorWidget::SanitizeMachine()
 */
static void SanitizeMachine(Widget widget)
{
    XemEmulatorWidget self    = EMULATOR_WIDGET(widget);
    XemMachine*       machine = &self->emulator.machine;

    if(machine->create_proc == NULL) {
        machine->create_proc = &DefaultMachineProc;
    }
    if(machine->destroy_proc == NULL) {
        machine->destroy_proc = &DefaultMachineProc;
    }
    if(machine->realize_proc == NULL) {
        machine->realize_proc = &DefaultMachineProc;
    }
    if(machine->resize_proc == NULL) {
        machine->resize_proc = &DefaultMachineProc;
    }
    if(machine->expose_proc == NULL) {
        machine->expose_proc = &DefaultMachineProc;
    }
    if(machine->input_proc == NULL) {
        machine->input_proc = &DefaultMachineProc;
    }
    if(machine->timer_proc == NULL) {
        machine->timer_proc = &DefaultMachineProc;
    }
}

/*
 * XemEmulatorWidget::CopyOrFillEvent()
 */
static XEvent* CopyOrFillEvent(Widget widget, XEvent* event)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(event != NULL) {
        self->emulator.last_rcv_event = *event;
        if((event->type == KeyPress)
        || (event->type == KeyRelease)) {
            self->emulator.last_key_event = *event;
        }
    }
    else {
        self->emulator.last_rcv_event.xany.type       = GenericEvent;
        self->emulator.last_rcv_event.xany.serial     = 0UL;
        self->emulator.last_rcv_event.xany.send_event = True;
        self->emulator.last_rcv_event.xany.display    = XtDisplay(widget);
        self->emulator.last_rcv_event.xany.window     = XtWindow(widget);
    }
    return &self->emulator.last_rcv_event;
}

/*
 * XemEmulatorWidget::ThrottleInputEvent()
 */
static void ThrottleInputEvent(Widget widget, XEvent* event)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);
    unsigned int      head = ((self->emulator.throttled.head + 0) % countof(self->emulator.throttled.list));
    unsigned int      tail = ((self->emulator.throttled.tail + 1) % countof(self->emulator.throttled.list));

    if(tail != head) {
        self->emulator.throttled.list[self->emulator.throttled.tail] = *event;
        self->emulator.throttled.head = head;
        self->emulator.throttled.tail = tail;
    }
}

/*
 * XemEmulatorWidget::ProcessThrottledInputEvent()
 */
static void ProcessThrottledInputEvent(Widget widget)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);
    int event_type = 0;

    while(self->emulator.throttled.head != self->emulator.throttled.tail) {
        XEvent* event = &self->emulator.throttled.list[self->emulator.throttled.head];
        if(event_type == 0) {
            event_type = event->type;
        }
        if(event->type == event_type) {
            (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, event);
            self->emulator.throttled.head = ((self->emulator.throttled.head + 1) % countof(self->emulator.throttled.list));
            self->emulator.throttled.tail = ((self->emulator.throttled.tail + 0) % countof(self->emulator.throttled.list));
        }
        else {
            break;
        }
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

    /* sanitize machine */ {
        SanitizeMachine(widget);
    }
    /* keyboard */ {
        XemKeyboardConstruct(widget, &self->emulator.keyboard, 0);
    }
    /* joysticks */ {
        XemJoystickConstruct(widget, &self->emulator.joystick0, self->emulator.joystick0.device, 0);
        XemJoystickConstruct(widget, &self->emulator.joystick1, self->emulator.joystick1.device, 1);
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
        self->emulator.throttled.head = 0;
        self->emulator.throttled.tail = 0;
    }
    /* call create-proc */ {
        (void) (*self->emulator.machine.create_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, NULL));
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
        (void) (*self->emulator.machine.realize_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, NULL));
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
        (void) (*self->emulator.machine.destroy_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, NULL));
    }
    /* joysticks */ {
        XemJoystickDestruct(widget, &self->emulator.joystick0);
        XemJoystickDestruct(widget, &self->emulator.joystick1);
    }
    /* keyboard */ {
        XemKeyboardDestruct(widget, &self->emulator.keyboard);
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
        (void) (*self->emulator.machine.resize_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, NULL));
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

    if(event->type == Expose) {
        (void) (*self->emulator.machine.expose_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, event));
    }
}

/**
 * XemEmulatorWidget::SetValues()
 *
 * @param prev_widget specifies the prev_widget XemEmulatorWidget instance
 * @param next_widget specifies the requested XemEmulatorWidget instance
 * @param widget specifies the new XemEmulatorWidget instance
 */
static Boolean SetValues(Widget prev_widget, Widget next_widget, Widget widget, ArgList args, Cardinal* num_args)
{
    XemEmulatorWidget prev = EMULATOR_WIDGET(prev_widget);
    XemEmulatorWidget next = EMULATOR_WIDGET(next_widget);
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    /* joystick0 */ {
        if(prev->emulator.joystick0.device != next->emulator.joystick0.device) {
            XemJoystickDestruct(prev_widget, &prev->emulator.joystick0);
            XemJoystickConstruct(widget, &self->emulator.joystick0, next->emulator.joystick0.device, 0);
        }
    }
    /* joystick1 */ {
        if(prev->emulator.joystick1.device != next->emulator.joystick1.device) {
            XemJoystickDestruct(prev_widget, &prev->emulator.joystick1);
            XemJoystickConstruct(widget, &self->emulator.joystick1, next->emulator.joystick1.device, 1);
        }
    }
    /* sanitize machine */ {
        SanitizeMachine(widget);
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
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(event->type == KeyPress) {
        /* preprocess keyboard event */ {
            if(XemKeyboardPreprocessEvent(widget, &self->emulator.keyboard, event) != FALSE) {
                return;
            }
        }
        /* check for same successive keypress/keyrelease */ {
            XEvent* prev = &self->emulator.last_key_event;
            if((prev->type == KeyPress) || (prev->type == KeyRelease)) {
                if((prev->xkey.display == event->xkey.display)
                && (prev->xkey.window  == event->xkey.window )
                && (prev->xkey.keycode == event->xkey.keycode)
                && ((event->xkey.time - prev->xkey.time) < KEY_DELAY_THRESHOLD)) {
                    ThrottleInputEvent(widget, CopyOrFillEvent(widget, NULL));
                }
            }
        }
        ThrottleInputEvent(widget, CopyOrFillEvent(widget, event));
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
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(event->type == KeyRelease) {
        /* check for auto-repeat */ {
            if(XPending(event->xany.display) > 0) {
                XEvent next;
                (void) XPeekEvent(event->xany.display, &next);
                if((next.type         == KeyPress           )
                && (next.xkey.display == event->xkey.display)
                && (next.xkey.window  == event->xkey.window )
                && (next.xkey.keycode == event->xkey.keycode)
                && ((next.xkey.time - event->xkey.time) < KEY_REPEAT_THRESHOLD)) {
                    (void) XNextEvent(event->xany.display, &next);
                    return;
                }
            }
        }
        /* preprocess keyboard event */ {
            if(XemKeyboardPreprocessEvent(widget, &self->emulator.keyboard, event) != FALSE) {
                return;
            }
        }
        ThrottleInputEvent(widget, CopyOrFillEvent(widget, event));
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
    if(event->type == ButtonPress) {
        ThrottleInputEvent(widget, CopyOrFillEvent(widget, event));
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
    if(event->type == ButtonRelease) {
        ThrottleInputEvent(widget, CopyOrFillEvent(widget, event));
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

    if(event->type == ConfigureNotify) {
        /* reconfigure widget */ {
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
            (void) (*self->emulator.machine.resize_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, event));
        }
    }
}

/**
 * XemEmulatorWidget::TimeOutHandler()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param timer specifies the XtIntervalId instance
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
            timeout = (*self->emulator.machine.timer_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, NULL));
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

/**
 * XemKeyboard::Construct()
 */
void XemKeyboardConstruct(Widget widget, XemKeyboard* keyboard, int id)
{
    /* initialize */ {
        keyboard->js_enabled = FALSE;
        keyboard->js_id      = id;
        keyboard->js_axis_x  = 0;
        keyboard->js_axis_y  = 0;
        keyboard->js_button0 = 0;
        keyboard->js_button1 = 0;
    }
}

/**
 * XemKeyboard::Destruct()
 */
void XemKeyboardDestruct(Widget widget, XemKeyboard* keyboard)
{
    /* finalize */ {
        keyboard->js_enabled = FALSE;
        keyboard->js_id      = -1;
        keyboard->js_axis_x  = 0;
        keyboard->js_axis_y  = 0;
        keyboard->js_button0 = 0;
        keyboard->js_button1 = 0;
    }
}

/**
 * XemKeyboard::PreprocessEvent()
 */
static Boolean XemKeyboardPreprocessEvent(Widget widget, XemKeyboard* keyboard, XEvent* event)
{
    XemEmulatorWidget self   = EMULATOR_WIDGET(widget);
    KeySym            keysym = XLookupKeysym(&event->xkey, 0);

    if(event->type == KeyPress) {
        if((keysym == XK_Home) || (keysym == XK_End)) {
            if(keyboard->js_enabled == FALSE) {
                keyboard->js_enabled = TRUE;
                keyboard->js_axis_x  = 0;
                keyboard->js_axis_y  = 0;
                keyboard->js_button0 = 0;
                keyboard->js_button1 = 0;
            }
            else {
                keyboard->js_enabled = FALSE;
                keyboard->js_axis_x  = 0;
                keyboard->js_axis_y  = 0;
                keyboard->js_button0 = 0;
                keyboard->js_button1 = 0;
            }
            return TRUE;
        }
        if((keysym >= XK_F1) && (keysym <= XK_F35)) {
            XtCallCallbackList(widget, self->emulator.hotkey_callback, &keysym);
            return TRUE;
        }
    }
    if(keyboard->js_enabled != FALSE) {
        unsigned char event_type   = 0;
        unsigned char event_number = 0;
        short         event_value  = 0;
        /* check for emulated joystick event */ {
            switch(keysym) {
                case XK_Up:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 1;
                        event_value  = (event->type == KeyPress ? -32767 : 0);
                    }
                    break;
                case XK_Down:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 1;
                        event_value  = (event->type == KeyPress ? +32767 : 0);
                    }
                    break;
                case XK_Left:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 0;
                        event_value  = (event->type == KeyPress ? -32767 : 0);
                    }
                    break;
                case XK_Right:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 0;
                        event_value  = (event->type == KeyPress ? +32767 : 0);
                    }
                    break;
                case XK_Control_L:
                    {
                        event_type   = JS_EVENT_BUTTON;
                        event_number = 0;
                        event_value  = (event->type == KeyPress ? 1 : 0);
                    }
                    break;
                case XK_Alt_L:
                    {
                        event_type   = JS_EVENT_BUTTON;
                        event_number = 1;
                        event_value  = (event->type == KeyPress ? 1 : 0);
                    }
                    break;
                default:
                    break;
            }
        }
        /* decode emulated joystick event */ {
            switch(event_type) {
                case JS_EVENT_BUTTON:
                    {
                        XEvent xevent;
                        /* update keyboard */ {
                            if((event_number &= 1) == 0) {
                                keyboard->js_button0 = event_value;
                            }
                            else {
                                keyboard->js_button1 = event_value;
                            }
                        }
                        /* initialize button event */ {
                            xevent.xbutton.type        = (event_value != 0 ? ButtonPress : ButtonRelease);
                            xevent.xbutton.serial      = 0UL;
                            xevent.xbutton.send_event  = True;
                            xevent.xbutton.display     = XtDisplay(widget);
                            xevent.xbutton.window      = XtWindow(widget);
                            xevent.xbutton.root        = None;
                            xevent.xbutton.subwindow   = None;
                            xevent.xbutton.time        = 0UL;
                            xevent.xbutton.x           = keyboard->js_axis_x;
                            xevent.xbutton.y           = keyboard->js_axis_y;
                            xevent.xbutton.x_root      = 0;
                            xevent.xbutton.y_root      = 0;
                            xevent.xbutton.state       = AnyModifier << (keyboard->js_id + 1);
                            xevent.xbutton.button      = event_number;
                            xevent.xbutton.same_screen = True;
                        }
                        /* call input-proc */ {
                            (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, &xevent));
                        }
                    }
                    return TRUE;
                case JS_EVENT_AXIS:
                    {
                        XEvent xevent;
                        /* update keyboard */ {
                            if((event_number &= 1) == 0) {
                                keyboard->js_axis_x = event_value;
                            }
                            else {
                                keyboard->js_axis_y = event_value;
                            }
                        }
                        /* initialize motion event */ {
                            xevent.xmotion.type        = MotionNotify;
                            xevent.xmotion.serial      = 0UL;
                            xevent.xmotion.send_event  = True;
                            xevent.xmotion.display     = XtDisplay(widget);
                            xevent.xmotion.window      = XtWindow(widget);
                            xevent.xmotion.root        = None;
                            xevent.xmotion.subwindow   = None;
                            xevent.xmotion.time        = 0UL;
                            xevent.xmotion.x           = keyboard->js_axis_x;
                            xevent.xmotion.y           = keyboard->js_axis_y;
                            xevent.xmotion.x_root      = 0;
                            xevent.xmotion.y_root      = 0;
                            xevent.xmotion.state       = AnyModifier << (keyboard->js_id + 1);
                            xevent.xmotion.is_hint     = 0;
                            xevent.xmotion.same_screen = True;
                        }
                        /* call input-proc */ {
                            (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, &xevent));
                        }
                    }
                    return TRUE;
                default:
                    break;
            }
        }
    }
    return FALSE;
}

/**
 * XemJoystick::Construct()
 */
void XemJoystickConstruct(Widget widget, XemJoystick* joystick, const char* device, int id)
{
    /* initialize */ {
        joystick->device     = NULL;
        joystick->identifier = NULL;
        joystick->fd         = -1;
        joystick->input_id   = INPUT_ID(0);
        joystick->js_id      = id;
        joystick->js_axis_x  = 0;
        joystick->js_axis_y  = 0;
        joystick->js_button0 = 0;
        joystick->js_button1 = 0;
        joystick->js_buttons = 0;
    }
    /* check device name */ {
        if((device != NULL) && (*device != '\0')) {
            joystick->device = XtNewString(device);
        }
    }
    /* open joystick */ {
        if(joystick->device != NULL) {
            joystick->fd = open(joystick->device, O_RDONLY);
            if(joystick->fd != -1) {
                joystick->input_id = XtAppAddInput(XtWidgetToApplicationContext(widget), joystick->fd, POINTER(XtInputReadMask | XtInputExceptMask), INPUT_CALLBACK_PROC(&XemJoystickHandler), POINTER(widget));
            }
            else {
                joystick->device = (XtFree(joystick->device), NULL);
            }
        }
    }
#ifdef HAVE_LINUX_JOYSTICK_H
    /* get the joystick name */ {
        if(joystick->fd != -1) {
            char buffer[256];
            if(ioctl(joystick->fd, JSIOCGNAME(sizeof(buffer)), buffer) != -1) {
                joystick->identifier = XtNewString(buffer);
            }
        }
    }
#endif
#ifdef HAVE_LINUX_JOYSTICK_H
    /* get the joystick mapping */ {
        unsigned char count = 0;
        if(joystick->fd != -1) {
            if(ioctl(joystick->fd, JSIOCGBUTTONS, &count) == -1) {
                (void) memset(joystick->js_mapping, 0, sizeof(joystick->js_mapping));
            }
            if(ioctl(joystick->fd, JSIOCGBTNMAP, joystick->js_mapping) == -1) {
                (void) memset(joystick->js_mapping, 0, sizeof(joystick->js_mapping));
            }
            if(count != 0) {
                joystick->js_buttons = count;
            }
        }
    }
#endif
}

/**
 * XemJoystick::Destruct()
 */
void XemJoystickDestruct(Widget widget, XemJoystick* joystick)
{
    /* finalize */ {
        joystick->js_id      = 0;
        joystick->js_axis_x  = 0;
        joystick->js_axis_y  = 0;
        joystick->js_button0 = 0;
        joystick->js_button1 = 0;
        joystick->js_buttons = 0;
    }
    /* destroy input_id */ {
        if(joystick->input_id != INPUT_ID(0)) {
            joystick->input_id = (XtRemoveInput(joystick->input_id), INPUT_ID(0));
        }
    }
    /* close joystick */ {
        if(joystick->fd != -1) {
            joystick->fd = ((void) close(joystick->fd), -1);
        }
    }
    /* finalize structure */ {
        if(joystick->identifier != NULL) {
            joystick->identifier = (XtFree(joystick->identifier), NULL);
        }
        if(joystick->device != NULL) {
            joystick->device = (XtFree(joystick->device), NULL);
        }
    }
}

/**
 * XemJoystick::LookupByFd()
 */
XemJoystick* XemJoystickLookupByFd(Widget widget, int fd)
{
    XemEmulatorWidget self = EMULATOR_WIDGET(widget);

    if(fd == self->emulator.joystick0.fd) {
        return &self->emulator.joystick0;
    }
    if(fd == self->emulator.joystick1.fd) {
        return &self->emulator.joystick1;
    }
    return NULL;
}

/**
 * XemJoystick::Handler()
 */
void XemJoystickHandler(Widget widget, int* source, XtInputId* input_id)
{
    XemEmulatorWidget self     = EMULATOR_WIDGET(widget);
    XemJoystick*      joystick = XemJoystickLookupByFd(widget, *source);

    /* joystick was not found */ {
        if(joystick == NULL) {
            XtRemoveInput(*input_id);
            return;
        }
    }
#ifdef HAVE_LINUX_JOYSTICK_H
    /* linux joystick */ {
        struct js_event event;
        /* read joystick event */ {
            const ssize_t bytes = read(joystick->fd, &event, sizeof(event));
            if(bytes != sizeof(event)) {
                char buffer[256];
                (void) snprintf ( buffer, sizeof(buffer)
                                , "an unexpected error occured while reading joystick #%d (%s)"
                                , joystick->js_id
                                , (joystick->identifier != NULL ? joystick->identifier : "unknown joystick") );
                XtWarning(buffer);
                joystick->input_id = (XtRemoveInput(joystick->input_id), INPUT_ID(0));
                return;
            }
        }
        /* decode joystick event */ {
            switch(event.type) {
                case JS_EVENT_BUTTON:
                    {
                        XEvent xevent;
                        /* check for special button */ {
                            if(event.value != 0) {
                                unsigned short code = joystick->js_mapping[event.number];
                                if(code == BTN_MODE) {
                                    return;
                                }
                                else if((code == BTN_SELECT) || (code == BTN_START)) {
                                    KeySym keysym = XK_Pause;
                                    XtCallCallbackList(widget, self->emulator.hotkey_callback, &keysym);
                                    return;
                                }
                            }
                        }
                        /* update joystick */ {
                            if((event.number &= 1) == 0) {
                                joystick->js_button0 = event.value;
                            }
                            else {
                                joystick->js_button1 = event.value;
                            }
                        }
                        /* initialize button event */ {
                            xevent.xbutton.type        = (event.value != 0 ? ButtonPress : ButtonRelease);
                            xevent.xbutton.serial      = 0UL;
                            xevent.xbutton.send_event  = True;
                            xevent.xbutton.display     = XtDisplay(widget);
                            xevent.xbutton.window      = XtWindow(widget);
                            xevent.xbutton.root        = None;
                            xevent.xbutton.subwindow   = None;
                            xevent.xbutton.time        = 0UL;
                            xevent.xbutton.x           = joystick->js_axis_x;
                            xevent.xbutton.y           = joystick->js_axis_y;
                            xevent.xbutton.x_root      = 0;
                            xevent.xbutton.y_root      = 0;
                            xevent.xmotion.state       = AnyModifier << (joystick->js_id + 1);
                            xevent.xbutton.button      = event.number;
                            xevent.xbutton.same_screen = True;
                        }
                        /* call input-proc */ {
                            (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, &xevent));
                        }
                    }
                    break;
                case JS_EVENT_AXIS:
                    {
                        XEvent xevent;
                        /* update joystick */ {
                            if((event.number &= 1) == 0) {
                                joystick->js_axis_x = event.value;
                            }
                            else {
                                joystick->js_axis_y = event.value;
                            }
                        }
                        /* initialize motion event */ {
                            xevent.xmotion.type        = MotionNotify;
                            xevent.xmotion.serial      = 0UL;
                            xevent.xmotion.send_event  = True;
                            xevent.xmotion.display     = XtDisplay(widget);
                            xevent.xmotion.window      = XtWindow(widget);
                            xevent.xmotion.root        = None;
                            xevent.xmotion.subwindow   = None;
                            xevent.xmotion.time        = 0UL;
                            xevent.xmotion.x           = joystick->js_axis_x;
                            xevent.xmotion.y           = joystick->js_axis_y;
                            xevent.xmotion.x_root      = 0;
                            xevent.xmotion.y_root      = 0;
                            xevent.xmotion.state       = AnyModifier << (joystick->js_id + 1);
                            xevent.xmotion.is_hint     = 0;
                            xevent.xmotion.same_screen = True;
                        }
                        /* call input-proc */ {
                            (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, &xevent));
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        return;
    }
#endif
    /* unknown joystick support */ {
        XemJoystickDestruct(widget, joystick);
    }
}

/**
 * XemJoystick::ButtonName()
 */
const char* XemJoystickButtonName(unsigned short code)
{
#ifdef HAVE_LINUX_JOYSTICK_H
    switch(code) {
        case BTN_A      : return "BTN_A";
        case BTN_B      : return "BTN_B";
        case BTN_C      : return "BTN_C";
        case BTN_X      : return "BTN_X";
        case BTN_Y      : return "BTN_Y";
        case BTN_Z      : return "BTN_Z";
        case BTN_TL     : return "BTN_TL";
        case BTN_TR     : return "BTN_TR";
        case BTN_TL2    : return "BTN_TL2";
        case BTN_TR2    : return "BTN_TR2";
        case BTN_SELECT : return "BTN_SELECT";
        case BTN_START  : return "BTN_START";
        case BTN_MODE   : return "BTN_MODE";
        case BTN_THUMBL : return "BTN_THUMBL";
        case BTN_THUMBR : return "BTN_THUMBR";
        default:
            break;
    }
#endif
    return "BTN_UNKNOWN";
}

/**
 * XemJoystick::Dump()
 */
void XemJoystickDump(Widget widget, XemJoystick* joystick, unsigned char button)
{
    int index = 0;
    int count = joystick->js_buttons;

    (void) fprintf(stderr, "%s\n", joystick->identifier);
    if(button != -1) {
        unsigned short code = joystick->js_mapping[button];
        (void) fprintf(stderr, "  - Button %2d event   = 0x%3x (%s)\n", button, code, XemJoystickButtonName(code));
    }
    for(index = 0; index < count; ++index) {
        unsigned short code = joystick->js_mapping[index];
        (void) fprintf(stderr, "  - Button %2d mapping = 0x%3x (%s)\n", index, code, XemJoystickButtonName(code));
    }
    (void) fprintf(stderr, "\n");
    (void) fflush(stderr);
}
