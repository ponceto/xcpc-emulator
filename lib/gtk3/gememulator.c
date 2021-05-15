/*
 * gememulator.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifdef HAVE_PORTAUDIO
#include <portaudio.h>
#endif
#include <gtk3/gememulator-priv.h>

G_DEFINE_TYPE(GemEmulator, gem_emulator, GTK_TYPE_WIDGET)

enum GemEmulatorSignal
{
    SIG_HOTKEY  = 0,
    LAST_SIGNAL = 1,
};

static guint emulator_signals[LAST_SIGNAL] = {
    0, /* SIG_HOTKEY */
};

static gboolean timer_handler(GtkWidget* widget)
{
    GemEmulator*  self    = CAST_EMULATOR(widget);
    unsigned long timeout = 0UL;

    /* aknowledge timer */ {
        self->timer = 0;
    }
    /* call clock_func */ {
        if(gtk_widget_is_sensitive(widget) != FALSE) {
            XcpcBackend*       backend = &self->backend;
            XcpcBackendClosure closure;
            closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
            timeout = (*backend->clock_func)(backend->instance, &closure);
        }
        else {
            XcpcBackend*       backend = &self->backend;
            XcpcBackendClosure closure;
            closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
            timeout = (*backend->idle_func)(backend->instance, &closure);
        }
    }
    /* restart timer */ {
        self->timer = g_timeout_add(timeout, G_SOURCE_FUNC(&timer_handler), self);
    }
    /* process throttled input event */ {
        (void) gem_events_process(widget, &self->events);
    }
    return FALSE;
}

static void schedule(GtkWidget* widget, unsigned long timeout)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(self->timer == 0) {
        self->timer = g_timeout_add(timeout, G_SOURCE_FUNC(&timer_handler), self);
    }
}

static void unschedule(GtkWidget* widget)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(self->timer != 0) {
        self->timer = ((void) g_source_remove(self->timer), 0U);
    }
}

static GdkFilterReturn impl_filter_func(GdkXEvent* native_event, GdkEvent* event, gpointer user_data)
{
    GtkWidget*   widget = CAST_WIDGET(user_data);
    GemEmulator* self   = CAST_EMULATOR(user_data);
    XEvent*      xevent = CAST_XEVENT(native_event);

    switch(xevent->type) {
        case ConfigureNotify:
            {
                /* process width */ {
                    if(gtk_widget_get_allocated_width(widget) != xevent->xconfigure.width) {
                        self->minimum_width = xevent->xconfigure.width;
                        self->natural_width = xevent->xconfigure.width;
                    }
                    else {
                        self->minimum_width = EMULATOR_MIN_WIDTH;
                        self->natural_width = xevent->xconfigure.width;
                    }
                }
                /* process height */ {
                    if(gtk_widget_get_allocated_height(widget) != xevent->xconfigure.height) {
                        self->minimum_height = xevent->xconfigure.height;
                    }
                    else {
                        self->minimum_height = EMULATOR_MIN_HEIGHT;
                    }
                }
                /* call resize_func */ {
                    XcpcBackend*       backend = &self->backend;
                    XcpcBackendClosure closure;
                    closure.event = gem_events_copy_or_fill(widget, &self->events, xevent);
                    (void) (*backend->resize_func)(backend->instance, &closure);
                }
                gtk_widget_queue_resize(widget);
            }
            return GDK_FILTER_REMOVE;
        default:
            break;
    }
    return GDK_FILTER_CONTINUE;
}

static void impl_object_dispose(GObject* object)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);

    /* call superclass method */ {
        (*super->parent_class.dispose)(object);
    }
}

static void impl_object_finalize(GObject* object)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);

    /* call superclass method */ {
        (*super->parent_class.finalize)(object);
    }
}

static void impl_widget_destroy(GtkWidget* widget)
{
    GemEmulator*    self  = CAST_EMULATOR(widget);
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);

    /* call detach_func */ {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->detach_func)(backend->instance, &closure);
    }
    /* unschedule timer */ {
        unschedule(widget);
    }
    /* destruct backend */ {
        (void) gem_backend_destruct(widget, &self->backend);
    }
    /* destruct joysticks */ {
        (void) gem_joystick_destruct(widget, &self->joystick0);
        (void) gem_joystick_destruct(widget, &self->joystick1);
    }
    /* destruct keyboard */ {
        (void) gem_keyboard_destruct(widget, &self->keyboard);
    }
    /* destruct events */ {
        (void) gem_events_destruct(widget, &self->events);
    }
    /* destruct audio */ {
        (void) gem_audio_destruct(widget, &self->audio);
    }
    /* destruct video */ {
        (void) gem_video_destruct(widget, &self->video);
    }
    /* call superclass method */ {
        (*super->destroy)(widget);
    }
}

static void impl_widget_realize(GtkWidget* widget)
{
    GemEmulator*  self = CAST_EMULATOR(widget);
    GtkAllocation allocation;
    GdkWindowAttr attributes;
    gint          attributes_mask;

    /* set realized */ {
        gtk_widget_set_realized(widget, TRUE);
    }
    /* get allocation attributes */ {
        gtk_widget_get_allocation(widget, &allocation);
    }
    /* set window attributes */ {
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.x           = allocation.x;
        attributes.y           = allocation.y;
        attributes.width       = allocation.width;
        attributes.height      = allocation.height;
        attributes.wclass      = GDK_INPUT_OUTPUT;
        attributes.visual      = gtk_widget_get_visual(widget);
        attributes.event_mask  = gtk_widget_get_events(widget)
                               | GDK_STRUCTURE_MASK
                               | GDK_EXPOSURE_MASK
                               | GDK_KEY_PRESS_MASK
                               | GDK_KEY_RELEASE_MASK
                               | GDK_BUTTON_PRESS_MASK
                               | GDK_BUTTON_RELEASE_MASK
                               | GDK_FOCUS_CHANGE_MASK
                               ;
        attributes_mask        = GDK_WA_X
                               | GDK_WA_Y
                               | GDK_WA_VISUAL
                               ;
    }
    /* create window */ {
        GdkWindow* window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
        gtk_widget_register_window(widget, window);
        gtk_widget_set_window(widget, window);
        gdk_window_add_filter(window, &impl_filter_func, widget);
    }
    /* realize video & audio */ {
        (void) gem_video_realize(widget, &self->video);
        (void) gem_audio_realize(widget, &self->audio);
    }
}

static void impl_widget_unrealize(GtkWidget* widget)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);
    GemEmulator*    self  = CAST_EMULATOR(widget);

    /* unrealize video & audio */ {
        (void) gem_video_unrealize(widget, &self->video);
        (void) gem_audio_unrealize(widget, &self->audio);
    }
    /* call superclass method */ {
        (*super->unrealize)(widget);
    }
}

static gboolean impl_widget_draw(GtkWidget* widget, cairo_t* cr)
{
    GemEmulator* self  = CAST_EMULATOR(widget);
    XEvent       xevent;

    /* clear surface */ {
        GdkRGBA color = {
            0.20, /* red   */
            0.20, /* green */
            0.20, /* blue  */
            1.00, /* alpha */
        };
        gdk_cairo_set_source_rgba(cr, &color);
        cairo_paint(cr);
    }
    /* forge expose event */ {
        xevent.xexpose.type       = Expose;
        xevent.xexpose.serial     = 0UL;
        xevent.xexpose.send_event = True;
        xevent.xexpose.display    = self->video.display;
        xevent.xexpose.window     = self->video.window;
        xevent.xexpose.x          = 0;
        xevent.xexpose.y          = 0;
        xevent.xexpose.width      = gtk_widget_get_allocated_width(widget);
        xevent.xexpose.height     = gtk_widget_get_allocated_height(widget);
        xevent.xexpose.count      = 0;
    }
    /* call expose_func */ {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, &xevent);
        (void) (*backend->expose_func)(backend->instance, &closure);
    }
    return FALSE;
}

static void impl_widget_size_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);

    /* call superclass method */ {
        (*super->size_allocate)(widget, allocation);
    }
}

static void impl_widget_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* set values */ {
        *minimum_width = self->minimum_width;
        *natural_width = self->natural_width;
    }
}

static void impl_widget_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* set values */ {
        *minimum_height = self->minimum_height;
        *natural_height = self->natural_height;
    }
}

static void impl_widget_adjust_size_request(GtkWidget* widget, GtkOrientation orientation, gint* minimum_size, gint* natural_size)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);
    GemEmulator*    self  = CAST_EMULATOR(widget);

    /* set values */ {
        if(orientation == GTK_ORIENTATION_HORIZONTAL) {
            *minimum_size = self->minimum_width;
            *natural_size = self->natural_width;
        }
        else {
            *minimum_size = self->minimum_height;
            *natural_size = self->natural_height;
        }
    }
    /* call superclass method */ {
        (*super->adjust_size_request)(widget, orientation, minimum_size, natural_size);
    }
}

static void impl_widget_adjust_size_allocation(GtkWidget* widget, GtkOrientation orientation, gint* minimum_size, gint* natural_size, gint* allocated_pos, gint* allocated_size)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);
    GemEmulator*    self  = CAST_EMULATOR(widget);

    /* set values */ {
        if(orientation == GTK_ORIENTATION_HORIZONTAL) {
            *minimum_size = self->minimum_width;
            *natural_size = self->natural_width;
        }
        else {
            *minimum_size = self->minimum_height;
            *natural_size = self->natural_height;
        }
    }
    /* call superclass method */ {
        (*super->adjust_size_allocation)(widget, orientation, minimum_size, natural_size, allocated_pos, allocated_size);
    }
}

static gboolean impl_widget_key_press_event(GtkWidget* widget, GdkEventKey* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    XEvent       xevent;

    /* forge keypress event */ {
        xevent.xkey.type        = KeyPress;
        xevent.xkey.serial      = 0UL;
        xevent.xkey.send_event  = True;
        xevent.xkey.display     = self->video.display;
        xevent.xkey.window      = self->video.window;
        xevent.xkey.root        = None;
        xevent.xkey.subwindow   = None;
        xevent.xkey.time        = event->time;
        xevent.xkey.x           = 0;
        xevent.xkey.y           = 0;
        xevent.xkey.x_root      = 0;
        xevent.xkey.y_root      = 0;
        xevent.xkey.state       = 0;
        xevent.xkey.keycode     = event->hardware_keycode;
        xevent.xkey.same_screen = True;
    }
    /* adjust event state */ {
        if(event->state & GDK_SHIFT_MASK   ) xevent.xkey.state |= ShiftMask;
        if(event->state & GDK_LOCK_MASK    ) xevent.xkey.state |= LockMask;
        if(event->state & GDK_CONTROL_MASK ) xevent.xkey.state |= ControlMask;
        if(event->state & GDK_MOD1_MASK    ) xevent.xkey.state |= Mod1Mask;
        if(event->state & GDK_MOD2_MASK    ) xevent.xkey.state |= Mod2Mask;
        if(event->state & GDK_MOD3_MASK    ) xevent.xkey.state |= Mod3Mask;
        if(event->state & GDK_MOD4_MASK    ) xevent.xkey.state |= Mod4Mask;
        if(event->state & GDK_MOD5_MASK    ) xevent.xkey.state |= Mod5Mask;
        if(event->state & GDK_BUTTON1_MASK ) xevent.xkey.state |= Button1Mask;
        if(event->state & GDK_BUTTON2_MASK ) xevent.xkey.state |= Button2Mask;
        if(event->state & GDK_BUTTON3_MASK ) xevent.xkey.state |= Button3Mask;
        if(event->state & GDK_BUTTON4_MASK ) xevent.xkey.state |= Button4Mask;
        if(event->state & GDK_BUTTON5_MASK ) xevent.xkey.state |= Button5Mask;
    }
    /* check for auto-repeat */ {
        XEvent* prev = &self->events.last_key_event;
        if(prev->type == KeyPress) {
            if((prev->xkey.display == xevent.xkey.display)
            && (prev->xkey.window  == xevent.xkey.window )
            && (prev->xkey.keycode == xevent.xkey.keycode)
            && ((xevent.xkey.time - prev->xkey.time) < KEY_REPEAT_THRESHOLD)) {
                return TRUE;
            }
        }
    }
    /* preprocess keyboard event */ {
        if(gem_keyboard_preprocess(widget, &self->keyboard, &xevent) != FALSE) {
            return TRUE;
        }
    }
    /* check for same successive keypress/keyrelease */ {
        XEvent* prev = &self->events.last_key_event;
        if((prev->type == KeyPress) || (prev->type == KeyRelease)) {
            if((prev->xkey.display == xevent.xkey.display)
            && (prev->xkey.window  == xevent.xkey.window )
            && (prev->xkey.keycode == xevent.xkey.keycode)
            && ((xevent.xkey.time - prev->xkey.time) < KEY_DELAY_THRESHOLD)) {
                (void) gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, NULL));
            }
        }
    }
    /* throttle input event */ {
        (void) gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &xevent));
    }
    return TRUE;
}

static gboolean impl_widget_key_release_event(GtkWidget* widget, GdkEventKey* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    XEvent       xevent;

    /* forge keypress event */ {
        xevent.xkey.type        = KeyRelease;
        xevent.xkey.serial      = 0UL;
        xevent.xkey.send_event  = True;
        xevent.xkey.display     = self->video.display;
        xevent.xkey.window      = self->video.window;
        xevent.xkey.root        = None;
        xevent.xkey.subwindow   = None;
        xevent.xkey.time        = event->time;
        xevent.xkey.x           = 0;
        xevent.xkey.y           = 0;
        xevent.xkey.x_root      = 0;
        xevent.xkey.y_root      = 0;
        xevent.xkey.state       = 0;
        xevent.xkey.keycode     = event->hardware_keycode;
        xevent.xkey.same_screen = True;
    }
    /* adjust event state */ {
        if(event->state & GDK_SHIFT_MASK   ) xevent.xkey.state |= ShiftMask;
        if(event->state & GDK_LOCK_MASK    ) xevent.xkey.state |= LockMask;
        if(event->state & GDK_CONTROL_MASK ) xevent.xkey.state |= ControlMask;
        if(event->state & GDK_MOD1_MASK    ) xevent.xkey.state |= Mod1Mask;
        if(event->state & GDK_MOD2_MASK    ) xevent.xkey.state |= Mod2Mask;
        if(event->state & GDK_MOD3_MASK    ) xevent.xkey.state |= Mod3Mask;
        if(event->state & GDK_MOD4_MASK    ) xevent.xkey.state |= Mod4Mask;
        if(event->state & GDK_MOD5_MASK    ) xevent.xkey.state |= Mod5Mask;
        if(event->state & GDK_BUTTON1_MASK ) xevent.xkey.state |= Button1Mask;
        if(event->state & GDK_BUTTON2_MASK ) xevent.xkey.state |= Button2Mask;
        if(event->state & GDK_BUTTON3_MASK ) xevent.xkey.state |= Button3Mask;
        if(event->state & GDK_BUTTON4_MASK ) xevent.xkey.state |= Button4Mask;
        if(event->state & GDK_BUTTON5_MASK ) xevent.xkey.state |= Button5Mask;
    }
    /* preprocess keyboard event */ {
        if(gem_keyboard_preprocess(widget, &self->keyboard, &xevent) != FALSE) {
            return TRUE;
        }
    }
    /* throttle input event */ {
        (void) gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &xevent));
    }
    return TRUE;
}

static gboolean impl_widget_button_press_event(GtkWidget* widget, GdkEventButton* event)
{
    /* grab the focus if needed */ {
        if(gtk_widget_has_focus(widget) == FALSE) {
            gtk_widget_grab_focus(widget);
        }
    }
    return TRUE;
}

static gboolean impl_widget_button_release_event(GtkWidget* widget, GdkEventButton* event)
{
    return TRUE;
}

static gboolean impl_widget_configure_event(GtkWidget* widget, GdkEventConfigure* event)
{
    return TRUE;
}

static void impl_emulator_hotkey(GemEmulator* emulator, KeySym keysym)
{
}

static void gem_emulator_class_init(GemEmulatorClass* emulator_class)
{
    GObjectClass*   object_class = G_OBJECT_CLASS(emulator_class);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(emulator_class);

    /* initialize object_class */ {
        object_class->dispose                = &impl_object_dispose;
        object_class->finalize               = &impl_object_finalize;
    }
    /* initialize widget_class */ {
        widget_class->destroy                = &impl_widget_destroy;
        widget_class->realize                = &impl_widget_realize;
        widget_class->unrealize              = &impl_widget_unrealize;
        widget_class->draw                   = &impl_widget_draw;
        widget_class->size_allocate          = &impl_widget_size_allocate;
        widget_class->get_preferred_width    = &impl_widget_get_preferred_width;
        widget_class->get_preferred_height   = &impl_widget_get_preferred_height;
        widget_class->adjust_size_request    = &impl_widget_adjust_size_request;
        widget_class->adjust_size_allocation = &impl_widget_adjust_size_allocation;
        widget_class->key_press_event        = &impl_widget_key_press_event;
        widget_class->key_release_event      = &impl_widget_key_release_event;
        widget_class->button_press_event     = &impl_widget_button_press_event;
        widget_class->button_release_event   = &impl_widget_button_release_event;
        widget_class->configure_event        = &impl_widget_configure_event;
    }
    /* initialize emulator_class */ {
        emulator_class->sig_hotkey           = &impl_emulator_hotkey;
    }
    /* initialize hotkey signal */ {
        emulator_signals[SIG_HOTKEY] = g_signal_new ( "hotkey"
                                                    , G_TYPE_FROM_CLASS(object_class)
                                                    , G_SIGNAL_RUN_FIRST
                                                    , G_STRUCT_OFFSET(GemEmulatorClass, sig_hotkey)
                                                    , NULL
                                                    , NULL
                                                    , NULL
                                                    , G_TYPE_NONE
                                                    , 1
                                                    , G_TYPE_POINTER );

    }
}

static void gem_emulator_init(GemEmulator* self)
{
    GtkWidget* widget = GTK_WIDGET(self);

    /* initialize widget */ {
        gtk_widget_set_has_window(widget, TRUE);
        gtk_widget_set_can_focus(widget, TRUE);
        gtk_widget_set_focus_on_click(widget, TRUE);
        gtk_widget_set_receives_default(widget, TRUE);
    }
    /* construct video */ {
        (void) gem_video_construct(widget, &self->video);
    }
    /* construct audio */ {
        (void) gem_audio_construct(widget, &self->audio);
    }
    /* construct events */ {
        (void) gem_events_construct(widget, &self->events);
    }
    /* construct keyboard */ {
        (void) gem_keyboard_construct(widget, &self->keyboard, 0);
    }
    /* construct joysticks */ {
        (void) gem_joystick_construct(widget, &self->joystick0, NULL, 0);
        (void) gem_joystick_construct(widget, &self->joystick1, NULL, 1);
    }
    /* construct backend */ {
        (void) gem_backend_construct(widget, &self->backend);
    }
    /* initialize minimum/natural dimensions */ {
        self->minimum_width  = EMULATOR_DEFAULT_WIDTH;
        self->minimum_height = EMULATOR_DEFAULT_HEIGHT;
        self->natural_width  = EMULATOR_DEFAULT_WIDTH;
        self->natural_height = EMULATOR_DEFAULT_HEIGHT;
    }
    /* initialize timer */ {
        self->timer = 0;
    }
    /* schedule timer */ {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
        schedule(widget, (*backend->idle_func)(backend->instance, &closure));
    }
}

GtkWidget* gem_emulator_new(void)
{
    return g_object_new(GEM_TYPE_EMULATOR, NULL);
}

void gem_emulator_set_backend(GtkWidget* widget, const GemBackend* backend)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* call detach_func */ {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->detach_func)(backend->instance, &closure);
    }
    /* set backend */ {
        self->backend = *backend;
    }
    /* call attach_func */ {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->attach_func)(backend->instance, &closure);
    }
}

void gem_emulator_set_joystick(GtkWidget* widget, int id, const char* device)
{
    GemEmulator* self     = CAST_EMULATOR(widget);
    GemJoystick* joystick = NULL;

    switch(id) {
        case 0:
            joystick = &self->joystick0;
            break;
        case 1:
            joystick = &self->joystick1;
            break;
        default:
            break;
    }
    if(joystick != NULL) {
        (void) gem_joystick_destruct(widget, joystick);
        (void) gem_joystick_construct(widget, joystick, device, id);
    }
}

GemVideo* gem_video_construct(GtkWidget* widget, GemVideo* video)
{
    /* initialize */ {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

GemVideo* gem_video_destruct(GtkWidget* widget, GemVideo* video)
{
    /* finalize */ {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

GemVideo* gem_video_realize(GtkWidget* widget, GemVideo* video)
{
    GemEmulator* self        = CAST_EMULATOR(widget);
    GdkWindow*   gdk_window  = gtk_widget_get_window(widget);
    GdkDisplay*  gdk_display = gdk_window_get_display(gdk_window);

    if(video->display == NULL) {
        video->display = GDK_DISPLAY_XDISPLAY(gdk_display);
        video->window  = GDK_WINDOW_XID(gdk_window);
    }
    if(video->display != NULL) {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->realize_func)(backend->instance, &closure);
    }
    return video;
}

GemVideo* gem_video_unrealize(GtkWidget* widget, GemVideo* video)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(video->display != NULL) {
        XcpcBackend*       backend = &self->backend;
        XcpcBackendClosure closure;
        closure.event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->unrealize_func)(backend->instance, &closure);
    }
    if(video->display != NULL) {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

#ifdef HAVE_PORTAUDIO
typedef struct _GemAudioFrame GemAudioFrame;

struct _GemAudioFrame
{
    int8_t lft;
    int8_t rgt;
};
#endif

#ifdef HAVE_PORTAUDIO
int gem_audio_callback(const int8_t* input, int8_t* output, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* info, PaStreamCallbackFlags flags, void* user_data)
{
    unsigned long frame_index = 0;
    unsigned long frame_count = frames_per_buffer;

    for(frame_index = 0; frame_index < frame_count; ++frame_index) {
        GemAudioFrame* audio_frame = ((GemAudioFrame*)(output));
        audio_frame->lft = 0;
        audio_frame->rgt = 0;
        output += 2;
    }
    return 0;
}
#endif

GemAudio* gem_audio_construct(GtkWidget* widget, GemAudio* audio)
{
#ifdef HAVE_PORTAUDIO
    PaDeviceIndex       device_index      = Pa_GetDefaultOutputDevice();
    PaStreamParameters  stream_parameters = { 0 };
    const double        suggested_latency = 20.0 / 1000.0;
    const double        sample_rate       = 44100.0;
    const unsigned long frames_per_buffer = 256;

    if(device_index != paNoDevice) {
        stream_parameters.device                    = device_index;
        stream_parameters.channelCount              = 2;
        stream_parameters.sampleFormat              = paInt8;
        stream_parameters.suggestedLatency          = suggested_latency;
        stream_parameters.hostApiSpecificStreamInfo = NULL;
    }
    if(device_index != paNoDevice) {
        PaStream* stream = NULL;
        PaError pa_error = Pa_OpenStream(&stream, NULL, &stream_parameters, sample_rate, frames_per_buffer, paNoFlag, ((PaStreamCallback*)(&gem_audio_callback)), widget);
        if(pa_error != paNoError) {
            g_warning("unable to open audio stream (%s)", Pa_GetErrorText(pa_error));
        }
        else {
            audio->stream = stream;
        }
    }
#endif
    return audio;
}

GemAudio* gem_audio_destruct(GtkWidget* widget, GemAudio* audio)
{
#ifdef HAVE_PORTAUDIO
    if(audio->stream != NULL) {
        PaError pa_error = Pa_CloseStream(audio->stream);
        if(pa_error != paNoError) {
            g_warning("unable to close audio stream (%s)", Pa_GetErrorText(pa_error));
        }
        else {
            audio->stream = NULL;
        }
    }
#endif
    return audio;
}

GemAudio* gem_audio_realize(GtkWidget* widget, GemAudio* audio)
{
#ifdef HAVE_PORTAUDIO
    if((audio->stream != NULL) && (Pa_IsStreamActive(audio->stream) == 0)) {
        PaError pa_error = Pa_StartStream(audio->stream);
        if(pa_error != paNoError) {
            g_warning("unable to start audio stream (%s)", Pa_GetErrorText(pa_error));
        }
    }
#endif
    return audio;
}

GemAudio* gem_audio_unrealize(GtkWidget* widget, GemAudio* audio)
{
#ifdef HAVE_PORTAUDIO
    if((audio->stream != NULL) && (Pa_IsStreamActive(audio->stream) != 0)) {
        PaError pa_error = Pa_StopStream(audio->stream);
        if(pa_error != paNoError) {
            g_warning("unable to stop audio stream (%s)", Pa_GetErrorText(pa_error));
        }
    }
#endif
    return audio;
}

GemEvents* gem_events_construct(GtkWidget* widget, GemEvents* events)
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

GemEvents* gem_events_destruct(GtkWidget* widget, GemEvents* events)
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

GemEvents* gem_events_throttle(GtkWidget* widget, GemEvents* events, XEvent* event)
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

GemEvents* gem_events_process(GtkWidget* widget, GemEvents* events)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    int event_type = 0;

    while(events->head != events->tail) {
        XEvent* event = &events->list[events->head];
        if(event_type == 0) {
            event_type = event->type;
        }
        if(event->type == event_type) {
            XcpcBackend*       backend = &self->backend;
            XcpcBackendClosure closure;
            closure.event = event;
            (void) (*backend->input_func)(backend->instance, &closure);
            events->head = ((events->head + 1) % countof(events->list));
            events->tail = ((events->tail + 0) % countof(events->list));
        }
        else {
            break;
        }
    }
    return events;
}

XEvent* gem_events_copy_or_fill(GtkWidget* widget, GemEvents* events, XEvent* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

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
        events->last_rcv_event.xany.display    = self->video.display;
        events->last_rcv_event.xany.window     = self->video.window;
    }
    return &events->last_rcv_event;
}

GemKeyboard* gem_keyboard_construct(GtkWidget* widget, GemKeyboard* keyboard, int id)
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

GemKeyboard* gem_keyboard_destruct(GtkWidget* widget, GemKeyboard* keyboard)
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

gboolean gem_keyboard_preprocess(GtkWidget* widget, GemKeyboard* keyboard, XEvent* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    KeySym       keysym = XLookupKeysym(&event->xkey, 0);

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
            g_signal_emit(G_OBJECT(widget), emulator_signals[SIG_HOTKEY], 0, &keysym);
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
                            xevent.xbutton.display     = self->video.display;
                            xevent.xbutton.window      = self->video.window;
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
                        /* call input_func */ {
                            XcpcBackend*       backend = &self->backend;
                            XcpcBackendClosure closure;
                            closure.event = gem_events_copy_or_fill(widget, &self->events, &xevent);
                            (void) (*backend->input_func)(backend->instance, &closure);
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
                            xevent.xmotion.display     = self->video.display;
                            xevent.xmotion.window      = self->video.window;
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
                        /* call input_func */ {
                            XcpcBackend*       backend = &self->backend;
                            XcpcBackendClosure closure;
                            closure.event = gem_events_copy_or_fill(widget, &self->events, &xevent);
                            (void) (*backend->input_func)(backend->instance, &closure);
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

GemJoystick* gem_joystick_construct(GtkWidget* widget, GemJoystick* joystick, const char* device, int id)
{
    /* initialize */ {
        joystick->device     = NULL;
        joystick->identifier = NULL;
        joystick->fd         = -1;
        joystick->input_id   = 0;
        joystick->js_id      = id;
        joystick->js_axis_x  = 0;
        joystick->js_axis_y  = 0;
        joystick->js_button0 = 0;
        joystick->js_button1 = 0;
        joystick->js_buttons = 0;
    }
    /* check device name */ {
        if((device != NULL) && (*device != '\0')) {
            joystick->device = g_strdup(device);
        }
    }
    /* open joystick */ {
        if(joystick->device != NULL) {
            joystick->fd = open(joystick->device, O_RDONLY);
            if(joystick->fd != -1) {
                joystick->input_id = g_unix_fd_add(joystick->fd, (G_IO_IN | G_IO_ERR), ((GUnixFDSourceFunc)(&gem_joystick_handler)), widget);
            }
            else {
                joystick->device = (g_free(joystick->device), NULL);
            }
        }
    }
#ifdef HAVE_LINUX_JOYSTICK_H
    /* get the joystick name */ {
        if(joystick->fd != -1) {
            char buffer[256];
            if(ioctl(joystick->fd, JSIOCGNAME(sizeof(buffer)), buffer) != -1) {
                joystick->identifier = g_strdup(buffer);
            }
        }
    }
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

GemJoystick* gem_joystick_destruct(GtkWidget* widget, GemJoystick* joystick)
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
        if(joystick->input_id != 0) {
            joystick->input_id = ((void) g_source_remove(joystick->input_id), 0);
        }
    }
    /* close joystick */ {
        if(joystick->fd != -1) {
            joystick->fd = ((void) close(joystick->fd), -1);
        }
    }
    /* finalize structure */ {
        if(joystick->identifier != NULL) {
            joystick->identifier = (g_free(joystick->identifier), NULL);
        }
        if(joystick->device != NULL) {
            joystick->device = (g_free(joystick->device), NULL);
        }
    }
    return joystick;
}

GemJoystick* gem_joystick_lookup_by_fd(GtkWidget* widget, int fd)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(fd == self->joystick0.fd) {
        return &self->joystick0;
    }
    if(fd == self->joystick1.fd) {
        return &self->joystick1;
    }
    return NULL;
}

gboolean gem_joystick_handler(gint fd, GIOCondition condition, GtkWidget* widget)
{
    GemEmulator* self     = CAST_EMULATOR(widget);
    GemJoystick* joystick = gem_joystick_lookup_by_fd(widget, fd);

    /* joystick was not found */ {
        if(joystick == NULL) {
            g_warning("joystick width fd <%d> was not found", fd);
            return FALSE;
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
                g_warning("%s", buffer);
                joystick->input_id = 0;
                return FALSE;
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
                                    return TRUE;
                                }
                                else if((code == BTN_SELECT) || (code == BTN_START)) {
                                    KeySym keysym = XK_Pause;
                                    g_signal_emit(G_OBJECT(widget), emulator_signals[SIG_HOTKEY], 0, &keysym);
                                    return TRUE;
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
                            xevent.xbutton.display     = self->video.display;
                            xevent.xbutton.window      = self->video.window;
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
                        /* call input_func */ {
                            XcpcBackend*       backend = &self->backend;
                            XcpcBackendClosure closure;
                            closure.event = gem_events_copy_or_fill(widget, &self->events, &xevent);
                            (void) (*backend->input_func)(backend->instance, &closure);
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
                            xevent.xmotion.display     = self->video.display;
                            xevent.xmotion.window      = self->video.window;
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
                        /* call input_func */ {
                            XcpcBackend*       backend = &self->backend;
                            XcpcBackendClosure closure;
                            closure.event = gem_events_copy_or_fill(widget, &self->events, &xevent);
                            (void) (*backend->input_func)(backend->instance, &closure);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        return TRUE;
    }
#endif
    /* unknown joystick support */ {
        gem_joystick_destruct(widget, joystick);
    }
    return FALSE;
}

GemBackend* gem_backend_construct(GtkWidget* widget, GemBackend* backend)
{
    return xcpc_backend_init(backend);
}

GemBackend* gem_backend_destruct(GtkWidget* widget, GemBackend* backend)
{
    return xcpc_backend_fini(backend);
}
