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

#ifndef CAST_WIDGET
#define CAST_WIDGET(widget) ((Widget)(widget))
#endif

#ifndef CAST_WIDGET_CLASS
#define CAST_WIDGET_CLASS(widget_class) ((WidgetClass)(widget_class))
#endif

#ifndef CAST_EMULATOR
#define CAST_EMULATOR(widget) ((XemEmulatorWidget)(widget))
#endif

#ifndef XT_POINTER
#define XT_POINTER(pointer) ((XtPointer)(pointer))
#endif

#ifndef XT_TIMER_CALLBACK_PROC
#define XT_TIMER_CALLBACK_PROC(proc) ((XtTimerCallbackProc)(proc))
#endif

#ifndef XT_INPUT_CALLBACK_PROC
#define XT_INPUT_CALLBACK_PROC(proc) ((XtInputCallbackProc)(proc))
#endif

#ifndef XT_INTERVAL_ID
#define XT_INTERVAL_ID(interval_id) ((XtIntervalId)(interval_id))
#endif

#ifndef XT_INPUT_ID
#define XT_INPUT_ID(input_id) ((XtInputId)(input_id))
#endif

#ifndef EMULATOR_DEFAULT_TIMEOUT
#define EMULATOR_DEFAULT_TIMEOUT 100UL
#endif

#ifndef EMULATOR_DEFAULT_WIDTH
#define EMULATOR_DEFAULT_WIDTH 768
#endif

#ifndef EMULATOR_DEFAULT_HEIGHT
#define EMULATOR_DEFAULT_HEIGHT 576
#endif

#ifndef KEY_DELAY_THRESHOLD
#define KEY_DELAY_THRESHOLD 20
#endif

#ifndef KEY_REPEAT_THRESHOLD
#define KEY_REPEAT_THRESHOLD 10
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
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineCreateFunc */ {
        XtNmachineCreateFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.create_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineDestroyFunc */ {
        XtNmachineDestroyFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.destroy_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineRealizeFunc */ {
        XtNmachineRealizeFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.realize_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineResizeFunc */ {
        XtNmachineResizeFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.resize_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineExposeFunc */ {
        XtNmachineExposeFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.expose_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineInputFunc */ {
        XtNmachineInputFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.input_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNmachineClockFunc */ {
        XtNmachineClockFunc, XtCFunction, XtRFunction,
        sizeof(XemEmulatorFunc), XtOffsetOf(XemEmulatorRec, emulator.machine.clock_func),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNjoystick0 */ {
        XtNjoystick0, XtCJoystick, XtRString,
        sizeof(String), XtOffsetOf(XemEmulatorRec, emulator.joystick0.device),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNjoystick1 */ {
        XtNjoystick1, XtCJoystick, XtRString,
        sizeof(String), XtOffsetOf(XemEmulatorRec, emulator.joystick1.device),
        XtRImmediate, XT_POINTER(NULL)
    },
    /* XtNhotkeyCallback */ {
        XtNhotkeyCallback, XtCCallback, XtRCallback,
        sizeof(XtCallbackList), XtOffsetOf(XemEmulatorRec, emulator.hotkey_callback),
        XtRImmediate, XT_POINTER(NULL)
    },
};

/*
 * XemEmulatorWidget::SuperClass
 */
static WidgetClass XemEmulatorSuperClass = CAST_WIDGET_CLASS(NULL);

/*
 * XemEmulatorWidget::Class
 */
externaldef(xememulatorclassrec) XemEmulatorClassRec xemEmulatorClassRec = {
    /* CoreClassPart */ {
        CAST_WIDGET_CLASS(&coreClassRec), /* superclass            */
        "XemEmulator",                    /* class_name            */
        sizeof(XemEmulatorRec),           /* widget_size           */
        ClassInitialize,                  /* class_initialize      */
        NULL,                             /* class_part_initialize */
        FALSE,                            /* class_inited          */
        Initialize,                       /* initialize            */
        NULL,                             /* initialize_hook       */
        Realize,                          /* realize               */
        actions,                          /* actions               */
        XtNumber(actions),                /* num_actions           */
        resources,                        /* resources             */
        XtNumber(resources),              /* num_resources         */
        NULLQUARK,                        /* xrm_class             */
        TRUE,                             /* compress_motion       */
        TRUE,                             /* compress_exposure     */
        TRUE,                             /* compress_enterleave   */
        FALSE,                            /* visible_interest      */
        Destroy,                          /* destroy               */
        Resize,                           /* resize                */
        Redraw,                           /* expose                */
        SetValues,                        /* set_values            */
        NULL,                             /* set_values_hook       */
        XtInheritSetValuesAlmost,         /* set_values_almost     */
        NULL,                             /* get_values_hook       */
        XtInheritAcceptFocus,             /* accept_focus          */
        XtVersion,                        /* version               */
        NULL,                             /* callback_private      */
        translations,                     /* tm_table              */
        XtInheritQueryGeometry,           /* query_geometry        */
        XtInheritDisplayAccelerator,      /* display_accelerator   */
        NULL                              /* extension             */
    },
    /* XemEmulatorClassPart */ {
        NULL                              /* extension             */
    }
};

externaldef(xememulatorwidgetclass) WidgetClass xemEmulatorWidgetClass = CAST_WIDGET_CLASS(&xemEmulatorClassRec);

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
 * XemEmulatorWidget::DefaultMachineFunc()
 */
static unsigned long DefaultMachineFunc(XtPointer data, XEvent* event)
{
    return EMULATOR_DEFAULT_TIMEOUT;
}

/*
 * XemEmulatorWidget::TimeOutHandler()
 */
static void TimeOutHandler(Widget widget, XtIntervalId* timer)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);
    unsigned long     timeout = 0UL;

    /* acknowledge timer */ {
        if(*timer == self->emulator.timer) {
            self->emulator.timer = XT_INTERVAL_ID(0);
        }
    }
    /* call timer-proc */ {
        if((self->core.sensitive != FALSE) && (self->core.ancestor_sensitive != FALSE)) {
            timeout = (*self->emulator.machine.clock_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, NULL), NULL);
        }
        else {
            timeout = DefaultMachineFunc(NULL, NULL);
        }
    }
    /* schedule timer */ {
        self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), (timeout != 0UL ? timeout : 1UL), XT_TIMER_CALLBACK_PROC(&TimeOutHandler), XT_POINTER(widget));
    }
    /* process throttled input event */ {
        (void) XemEventsProcess(widget, &self->emulator.events);
    }
}

/*
 * XemEmulatorWidget::Schedule()
 */
static void Schedule(Widget widget, unsigned long timeout)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(self->emulator.timer == XT_INTERVAL_ID(0)) {
        self->emulator.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(widget), (timeout != 0UL ? timeout : 1UL), XT_TIMER_CALLBACK_PROC(&TimeOutHandler), XT_POINTER(widget));
    }
}

/*
 * XemEmulatorWidget::Unschedule()
 */
static void Unschedule(Widget widget)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(self->emulator.timer != XT_INTERVAL_ID(0)) {
        XtRemoveTimeOut(self->emulator.timer);
        self->emulator.timer = XT_INTERVAL_ID(0);
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    /* initialize geometry */ {
        if(request->core.width == 0) {
            self->core.width = EMULATOR_DEFAULT_WIDTH;
        }
        if(request->core.height == 0) {
            self->core.height = EMULATOR_DEFAULT_HEIGHT;
        }
    }
    /* construct video */ {
        (void) XemVideoConstruct(widget, &self->emulator.video);
    }
    /* construct audio */ {
        (void) XemAudioConstruct(widget, &self->emulator.audio);
    }
    /* construct events */ {
        (void) XemEventsConstruct(widget, &self->emulator.events);
    }
    /* construct keyboard */ {
        (void) XemKeyboardConstruct(widget, &self->emulator.keyboard, 0);
    }
    /* construct joysticks */ {
        (void) XemJoystickConstruct(widget, &self->emulator.joystick0, self->emulator.joystick0.device, 0);
        (void) XemJoystickConstruct(widget, &self->emulator.joystick1, self->emulator.joystick1.device, 1);
    }
    /* construct machine */ {
        (void) XemMachineConstruct(widget, &self->emulator.machine);
    }
    /* initialize timer */ {
        self->emulator.timer = XT_INTERVAL_ID(0);
    }
    /* call create-proc */ {
        (void) (*self->emulator.machine.create_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, NULL), NULL);
    }
    /* schedule timer */ {
        Schedule(widget, DefaultMachineFunc(NULL, NULL));
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    /* unrealize video & audio */ {
        (void) XemVideoUnrealize(widget, &self->emulator.video);
        (void) XemAudioUnrealize(widget, &self->emulator.audio);
    }
    /* call superclass method */ {
        if(XemEmulatorSuperClass->core_class.realize != NULL) {
            (*XemEmulatorSuperClass->core_class.realize)(widget, mask, attributes);
        }
    }
    /* realize video & audio */ {
        (void) XemVideoRealize(widget, &self->emulator.video);
        (void) XemAudioRealize(widget, &self->emulator.audio);
    }
}

/**
 * XemEmulatorWidget::Destroy()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Destroy(Widget widget)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    /* unschedule timer */ {
        Unschedule(widget);
    }
    /* call destroy-proc */ {
        (void) (*self->emulator.machine.destroy_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, NULL), NULL);
    }
    /* destruct machine */ {
        (void) XemMachineDestruct(widget, &self->emulator.machine);
    }
    /* destruct joysticks */ {
        (void) XemJoystickDestruct(widget, &self->emulator.joystick0);
        (void) XemJoystickDestruct(widget, &self->emulator.joystick1);
    }
    /* destruct keyboard */ {
        (void) XemKeyboardDestruct(widget, &self->emulator.keyboard);
    }
    /* destruct events */ {
        (void) XemEventsDestruct(widget, &self->emulator.events);
    }
    /* destruct audio */ {
        (void) XemAudioDestruct(widget, &self->emulator.audio);
    }
    /* destruct video */ {
        (void) XemVideoDestruct(widget, &self->emulator.video);
    }
}

/**
 * XemEmulatorWidget::Resize()
 *
 * @param widget specifies the XemEmulatorWidget instance
 */
static void Resize(Widget widget)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    /* call resize-proc */ {
        (void) (*self->emulator.machine.resize_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, NULL), NULL);
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(event->type == Expose) {
        (void) (*self->emulator.machine.expose_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, event), NULL);
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
    XemEmulatorWidget prev = CAST_EMULATOR(prev_widget);
    XemEmulatorWidget next = CAST_EMULATOR(next_widget);
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    /* joystick0 */ {
        if(prev->emulator.joystick0.device != next->emulator.joystick0.device) {
            (void) XemJoystickDestruct(prev_widget, &prev->emulator.joystick0);
            (void) XemJoystickConstruct(widget, &self->emulator.joystick0, next->emulator.joystick0.device, 0);
        }
    }
    /* joystick1 */ {
        if(prev->emulator.joystick1.device != next->emulator.joystick1.device) {
            (void) XemJoystickDestruct(prev_widget, &prev->emulator.joystick1);
            (void) XemJoystickConstruct(widget, &self->emulator.joystick1, next->emulator.joystick1.device, 1);
        }
    }
    /* sanitize machine */ {
        (void) XemMachineSanitize(widget, &self->emulator.machine);
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(event->type == KeyPress) {
        /* preprocess keyboard event */ {
            if(XemKeyboardPreprocessEvent(widget, &self->emulator.keyboard, event) != FALSE) {
                return;
            }
        }
        /* check for same successive keypress/keyrelease */ {
            XEvent* prev = &self->emulator.events.last_key_event;
            if((prev->type == KeyPress) || (prev->type == KeyRelease)) {
                if((prev->xkey.display == event->xkey.display)
                && (prev->xkey.window  == event->xkey.window )
                && (prev->xkey.keycode == event->xkey.keycode)
                && ((event->xkey.time - prev->xkey.time) < KEY_DELAY_THRESHOLD)) {
                    (void) XemEventsThrottle(widget, &self->emulator.events, XemEventsCopyOrFill(widget, &self->emulator.events, NULL));
                }
            }
        }
        (void) XemEventsThrottle(widget, &self->emulator.events, XemEventsCopyOrFill(widget, &self->emulator.events, event));
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

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
        (void) XemEventsThrottle(widget, &self->emulator.events, XemEventsCopyOrFill(widget, &self->emulator.events, event));
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(event->type == ButtonPress) {
        (void) XemEventsThrottle(widget, &self->emulator.events, XemEventsCopyOrFill(widget, &self->emulator.events, event));
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(event->type == ButtonRelease) {
        (void) XemEventsThrottle(widget, &self->emulator.events, XemEventsCopyOrFill(widget, &self->emulator.events, event));
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
    XemEmulatorWidget self = CAST_EMULATOR(widget);

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
            (void) (*self->emulator.machine.resize_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, event), NULL);
        }
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


/*
 * Utilities
 */

XemVideo* XemVideoConstruct(Widget widget, XemVideo* video)
{
    /* initialize */ {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

XemVideo* XemVideoDestruct(Widget widget, XemVideo* video)
{
    /* finalize */ {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

XemVideo* XemVideoRealize(Widget widget, XemVideo* video)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(video->display == NULL) {
        video->display = XtDisplay(widget);
        video->window  = XtWindow(widget);
    }
    if(video->display != NULL) {
        (void) (*self->emulator.machine.realize_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, NULL), NULL);
    }
    return video;
}

XemVideo* XemVideoUnrealize(Widget widget, XemVideo* video)
{
    if(video->display != NULL) {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

XemAudio* XemAudioConstruct(Widget widget, XemAudio* audio)
{
    /* initialize */ {
        audio->reserved = NULL;
    }
    return audio;
}

XemAudio* XemAudioDestruct(Widget widget, XemAudio* audio)
{
    /* finalize */ {
        audio->reserved = NULL;
    }
    return audio;
}

XemAudio* XemAudioRealize(Widget widget, XemAudio* audio)
{
    return audio;
}

XemAudio* XemAudioUnrealize(Widget widget, XemAudio* audio)
{
    return audio;
}

XemEvents* XemEventsConstruct(Widget widget, XemEvents* events)
{
    XEvent event;

    /* initialize event */ {
        (void) memset(&event, 0, sizeof(event));
        event.xany.type       = GenericEvent;
        event.xany.serial     = 0UL;
        event.xany.send_event = True;
        event.xany.display    = NULL;
        event.xany.window     = None;
    }
    /* initialize */ {
        events->last_rcv_event = event;
        events->last_key_event = event;
        events->head = 0;
        events->tail = 0;
    }
    return events;
}

XemEvents* XemEventsDestruct(Widget widget, XemEvents* events)
{
    XEvent event;

    /* initialize event */ {
        (void) memset(&event, 0, sizeof(event));
        event.xany.type       = GenericEvent;
        event.xany.serial     = 0UL;
        event.xany.send_event = True;
        event.xany.display    = NULL;
        event.xany.window     = None;
    }
    /* finalize */ {
        events->last_rcv_event = event;
        events->last_key_event = event;
        events->head = 0;
        events->tail = 0;
    }
    return events;
}

XemEvents* XemEventsThrottle(Widget widget, XemEvents* events, XEvent* event)
{
    unsigned int head = ((events->head + 0) % countof(events->list));
    unsigned int tail = ((events->tail + 1) % countof(events->list));

    if(tail != head) {
        events->list[events->tail] = *event;
        events->head = head;
        events->tail = tail;
    }
    return events;
}

XemEvents* XemEventsProcess(Widget widget, XemEvents* events)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);
    int event_type = 0;

    while(events->head != events->tail) {
        XEvent* event = &events->list[events->head];
        if(event_type == 0) {
            event_type = event->type;
        }
        if(event->type == event_type) {
            (void) (*self->emulator.machine.input_func)(self->emulator.machine.instance, event, NULL);
            events->head = ((events->head + 1) % countof(events->list));
            events->tail = ((events->tail + 0) % countof(events->list));
        }
        else {
            break;
        }
    }
    return events;
}

XEvent* XemEventsCopyOrFill(Widget widget, XemEvents* events, XEvent* event)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(event != NULL) {
        events->last_rcv_event = *event;
        if((event->type == KeyPress)
        || (event->type == KeyRelease)) {
            events->last_key_event = *event;
        }
    }
    else {
        events->last_rcv_event.xany.type       = GenericEvent;
        events->last_rcv_event.xany.serial     = 0UL;
        events->last_rcv_event.xany.send_event = True;
        events->last_rcv_event.xany.display    = self->emulator.video.display;
        events->last_rcv_event.xany.window     = self->emulator.video.window;
    }
    return &events->last_rcv_event;
}

XemKeyboard* XemKeyboardConstruct(Widget widget, XemKeyboard* keyboard, int id)
{
    /* initialize */ {
        keyboard->js_enabled = FALSE;
        keyboard->js_id      = id;
        keyboard->js_axis_x  = 0;
        keyboard->js_axis_y  = 0;
        keyboard->js_button0 = 0;
        keyboard->js_button1 = 0;
    }
    return keyboard;
}

XemKeyboard* XemKeyboardDestruct(Widget widget, XemKeyboard* keyboard)
{
    /* finalize */ {
        keyboard->js_enabled = FALSE;
        keyboard->js_id      = -1;
        keyboard->js_axis_x  = 0;
        keyboard->js_axis_y  = 0;
        keyboard->js_button0 = 0;
        keyboard->js_button1 = 0;
    }
    return keyboard;
}

static Boolean XemKeyboardPreprocessEvent(Widget widget, XemKeyboard* keyboard, XEvent* event)
{
    XemEmulatorWidget self   = CAST_EMULATOR(widget);
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
                            xevent.xbutton.display     = self->emulator.video.display;
                            xevent.xbutton.window      = self->emulator.video.window;
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
                            (void) (*self->emulator.machine.input_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, &xevent), NULL);
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
                            xevent.xmotion.display     = self->emulator.video.display;
                            xevent.xmotion.window      = self->emulator.video.window;
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
                            (void) (*self->emulator.machine.input_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, &xevent), NULL);
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

XemJoystick* XemJoystickConstruct(Widget widget, XemJoystick* joystick, const char* device, int id)
{
    /* initialize */ {
        joystick->device     = NULL;
        joystick->identifier = NULL;
        joystick->fd         = -1;
        joystick->input_id   = XT_INPUT_ID(0);
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
                joystick->input_id = XtAppAddInput(XtWidgetToApplicationContext(widget), joystick->fd, XT_POINTER(XtInputReadMask | XtInputExceptMask), XT_INPUT_CALLBACK_PROC(&XemJoystickHandler), XT_POINTER(widget));
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
    return joystick;
}

XemJoystick* XemJoystickDestruct(Widget widget, XemJoystick* joystick)
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
        if(joystick->input_id != XT_INPUT_ID(0)) {
            joystick->input_id = (XtRemoveInput(joystick->input_id), XT_INPUT_ID(0));
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
    return joystick;
}

XemJoystick* XemJoystickLookupByFd(Widget widget, int fd)
{
    XemEmulatorWidget self = CAST_EMULATOR(widget);

    if(fd == self->emulator.joystick0.fd) {
        return &self->emulator.joystick0;
    }
    if(fd == self->emulator.joystick1.fd) {
        return &self->emulator.joystick1;
    }
    return NULL;
}

void XemJoystickHandler(Widget widget, int* source, XtInputId* input_id)
{
    XemEmulatorWidget self     = CAST_EMULATOR(widget);
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
                joystick->input_id = (XtRemoveInput(joystick->input_id), XT_INPUT_ID(0));
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
                            xevent.xbutton.display     = self->emulator.video.display;
                            xevent.xbutton.window      = self->emulator.video.window;
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
                            (void) (*self->emulator.machine.input_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, &xevent), NULL);
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
                            xevent.xmotion.display     = self->emulator.video.display;
                            xevent.xmotion.window      = self->emulator.video.window;
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
                            (void) (*self->emulator.machine.input_func)(self->emulator.machine.instance, XemEventsCopyOrFill(widget, &self->emulator.events, &xevent), NULL);
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
        (void) XemJoystickDestruct(widget, joystick);
    }
}

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

XemJoystick* XemJoystickDump(Widget widget, XemJoystick* joystick, unsigned char button)
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
    return joystick;
}

XemMachine* XemMachineConstruct(Widget widget, XemMachine* machine)
{
    return XemMachineSanitize(widget, machine);
}

XemMachine* XemMachineDestruct(Widget widget, XemMachine* machine)
{
    /* finalize */ {
        machine->instance     = NULL;
        machine->create_func  = NULL;
        machine->destroy_func = NULL;
        machine->realize_func = NULL;
        machine->resize_func  = NULL;
        machine->expose_func  = NULL;
        machine->input_func   = NULL;
        machine->clock_func   = NULL;
    }
    return XemMachineSanitize(widget, machine);
}

XemMachine* XemMachineSanitize(Widget widget, XemMachine* machine)
{
    /* sanitize */ {
        if(machine->create_func  == NULL) { machine->create_func  = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
        if(machine->destroy_func == NULL) { machine->destroy_func = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
        if(machine->realize_func == NULL) { machine->realize_func = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
        if(machine->resize_func  == NULL) { machine->resize_func  = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
        if(machine->expose_func  == NULL) { machine->expose_func  = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
        if(machine->input_func   == NULL) { machine->input_func   = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
        if(machine->clock_func   == NULL) { machine->clock_func   = XEM_EMULATOR_FUNC(&DefaultMachineFunc); }
    }
    return machine;
}
