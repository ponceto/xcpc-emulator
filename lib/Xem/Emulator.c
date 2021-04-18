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
#endif
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
static void JoystickHandler(Widget widget, int* source, XtInputId* input_id);

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
    unsigned int head = ((self->emulator.throttled_head + 0) % countof(self->emulator.throttled_list));
    unsigned int tail = ((self->emulator.throttled_tail + 1) % countof(self->emulator.throttled_list));

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
        self->emulator.throttled_list[self->emulator.throttled_tail] = *event;
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
        XEvent* event = &self->emulator.throttled_list[self->emulator.throttled_head];
        (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, event));
        self->emulator.throttled_head = ((self->emulator.throttled_head + 1) % countof(self->emulator.throttled_list));
        self->emulator.throttled_tail = ((self->emulator.throttled_tail + 0) % countof(self->emulator.throttled_list));
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
 * XemEmulatorWidget::InitializeJoystick()
 */
static void InitializeJoystick(Widget widget, XemJoystick* joystick, const char* device, int id)
{
    /* initialize structure */ {
        if((device != NULL) && (*device != '\0')) {
            joystick->device   = XtNewString(device);
            joystick->id       = id;
            joystick->fd       = -1;
            joystick->input_id = INPUT_ID(0);
            joystick->x        = 0;
            joystick->y        = 0;
        }
        else {
            joystick->device   = NULL;
            joystick->id       = id;
            joystick->fd       = -1;
            joystick->input_id = INPUT_ID(0);
            joystick->x        = 0;
            joystick->y        = 0;
        }
    }
    /* open joystick */ {
        if(joystick->device != NULL) {
            joystick->fd = open(joystick->device, O_RDONLY);
            if(joystick->fd == -1) {
                joystick->device = (XtFree(joystick->device), NULL);
            }
        }
    }
    /* add handler */ {
        if(joystick->fd != -1) {
            joystick->input_id = XtAppAddInput(XtWidgetToApplicationContext(widget), joystick->fd, POINTER(XtInputReadMask | XtInputExceptMask), INPUT_CALLBACK_PROC(&JoystickHandler), POINTER(widget));
        }
    }
}

/**
 * XemEmulatorWidget::DestroyJoystick()
 */
static void DestroyJoystick(Widget widget, XemJoystick* joystick)
{
    /* clear coordinates */ {
        joystick->x = 0;
        joystick->y = 0;
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
        if(joystick->device != NULL) {
            joystick->device = (XtFree(joystick->device), NULL);
        }
    }
}

/**
 * XemEmulatorWidget::GetJoystickFromFileDescriptor()
 */
static XemJoystick* GetJoystickFromFileDescriptor(Widget widget, int fd)
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
    /* joysticks */ {
        InitializeJoystick(widget, &self->emulator.joystick0, self->emulator.joystick0.device, 0);
        InitializeJoystick(widget, &self->emulator.joystick1, self->emulator.joystick1.device, 1);
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
        DestroyJoystick(widget, &self->emulator.joystick0);
        DestroyJoystick(widget, &self->emulator.joystick1);
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
            DestroyJoystick(prev_widget, &prev->emulator.joystick0);
            self->emulator.joystick0 = prev->emulator.joystick0;
            InitializeJoystick(widget, &self->emulator.joystick0, next->emulator.joystick0.device, 0);
        }
    }
    /* joystick1 */ {
        if(prev->emulator.joystick1.device != next->emulator.joystick1.device) {
            DestroyJoystick(prev_widget, &prev->emulator.joystick1);
            self->emulator.joystick1 = prev->emulator.joystick1;
            InitializeJoystick(widget, &self->emulator.joystick1, next->emulator.joystick1.device, 1);
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
    if(event->type == KeyPress) {
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
    if(event->type == KeyRelease) {
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
    if(event->type == ButtonPress) {
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
    if(event->type == ButtonRelease) {
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
 * XemEmulatorWidget::TimeOutHandler()
 *
 * @param widget specifies the XemEmulatorWidget instance
 * @param source specifies the source
 * @param input_id specifies the XtInputId instance
 */
static void JoystickHandler(Widget widget, int* source, XtInputId* input_id)
{
    XemEmulatorWidget self     = EMULATOR_WIDGET(widget);
    XemJoystick*      joystick = GetJoystickFromFileDescriptor(widget, *source);

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
                XtWarning("an unexpected error occured while reading a joystick");
                joystick->input_id = (XtRemoveInput(joystick->input_id), INPUT_ID(0));
                return;
            }
        }
        /* decode joystick event */ {
            switch(event.type) {
                case JS_EVENT_AXIS:
                    {
                        XEvent xevent;
                        /* update joystick */ {
                            if((event.number % 2) == 0) {
                                joystick->x = event.value;
                            }
                            else {
                                joystick->y = event.value;
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
                            xevent.xmotion.time        = event.time;
                            xevent.xmotion.x           = joystick->x;
                            xevent.xmotion.y           = joystick->y;
                            xevent.xmotion.x_root      = 0;
                            xevent.xmotion.y_root      = 0;
                            xevent.xmotion.state       = AnyModifier;
                            xevent.xmotion.is_hint     = 0;
                            xevent.xmotion.same_screen = True;
                        }
                        /* adjust mask */ {
                            if(joystick->id >= 0) {
                                xevent.xmotion.state <<= 1;
                            }
                            if(joystick->id >= 1) {
                                xevent.xmotion.state <<= 1;
                            }
                        }
                        /* call input-proc */ {
                            (void) (*self->emulator.machine.input_proc)(self->emulator.machine.instance, CopyOrFillEvent(widget, &xevent));
                        }
                    }
                    break;
                case JS_EVENT_BUTTON:
                    {
                        XEvent xevent;
                        /* initialize button event */ {
                            xevent.xbutton.type        = (event.value != 0 ? ButtonPress : ButtonRelease);
                            xevent.xbutton.serial      = 0UL;
                            xevent.xbutton.send_event  = True;
                            xevent.xbutton.display     = XtDisplay(widget);
                            xevent.xbutton.window      = XtWindow(widget);
                            xevent.xbutton.root        = None;
                            xevent.xbutton.subwindow   = None;
                            xevent.xbutton.time        = event.time;
                            xevent.xbutton.x           = 0;
                            xevent.xbutton.y           = 0;
                            xevent.xbutton.x_root      = 0;
                            xevent.xbutton.y_root      = 0;
                            xevent.xbutton.state       = AnyModifier;
                            xevent.xbutton.button      = event.number;
                            xevent.xbutton.same_screen = True;
                        }
                        /* adjust mask */ {
                            if(joystick->id >= 0) {
                                xevent.xmotion.state <<= 1;
                            }
                            if(joystick->id >= 1) {
                                xevent.xmotion.state <<= 1;
                            }
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
        DestroyJoystick(widget, joystick);
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
