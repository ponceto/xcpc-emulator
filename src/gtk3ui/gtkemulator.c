/*
 * gtkemulator.c - Copyright (c) 2001-2024 - Olivier Poncet
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
#include <glib-unix.h>
#ifdef HAVE_LINUX_JOYSTICK_H
#include <linux/joystick.h>
#include <sys/ioctl.h>
#endif
#include "gtkemulatorprivate.h"

G_DEFINE_TYPE(GtkEmulator, gtk_emulator, GTK_TYPE_WIDGET)

enum GtkEmulatorSignal
{
    SIG_HOTKEY  = 0,
    LAST_SIGNAL = 1,
};

static guint emulator_signals[LAST_SIGNAL] = {
    0, /* SIG_HOTKEY */
};

static gboolean timer_handler(GtkWidget* widget)
{
    GtkEmulator*  self    = CAST_EMULATOR(widget);
    unsigned long timeout = 0UL;

    /* acknowledge timer */ {
        self->timer = 0;
    }
    /* call clock_func */ {
        GemBackend* backend = &self->backend;
        GemEvent    closure;
        closure.u.any.x11_event = gem_events_copy_or_fill(widget, &self->events, NULL);
        timeout = (*backend->clock_func)(backend->instance, &closure);
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
    GtkEmulator* self = CAST_EMULATOR(widget);

    if(self->timer == 0) {
        self->timer = g_timeout_add(timeout, G_SOURCE_FUNC(&timer_handler), self);
    }
}

static void unschedule(GtkWidget* widget)
{
    GtkEmulator* self = CAST_EMULATOR(widget);

    if(self->timer != 0) {
        self->timer = ((void) g_source_remove(self->timer), 0U);
    }
}

static GdkFilterReturn impl_filter_func(GdkXEvent* native_event, GdkEvent* event, gpointer user_data)
{
    GtkWidget*   widget    = CAST_WIDGET(user_data);
    GtkEmulator* self      = CAST_EMULATOR(user_data);
    XEvent*      x11_event = CAST_XEVENT(native_event);

    switch(x11_event->type) {
        case ConfigureNotify:
            {
                /* process width */ {
                    if(gtk_widget_get_allocated_width(widget) != x11_event->xconfigure.width) {
                        self->minimum_width = x11_event->xconfigure.width;
                        self->natural_width = x11_event->xconfigure.width;
                    }
                }
                /* process height */ {
                    if(gtk_widget_get_allocated_height(widget) != x11_event->xconfigure.height) {
                        self->minimum_height = x11_event->xconfigure.height;
                        self->natural_height = x11_event->xconfigure.height;
                    }
                }
                /* dispatch event */ {
                    (void) gem_events_dispatch(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, x11_event));
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
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);

    /* call superclass method */ {
        (*super->parent_class.dispose)(object);
    }
}

static void impl_object_finalize(GObject* object)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);

    /* call superclass method */ {
        (*super->parent_class.finalize)(object);
    }
}

static void impl_widget_destroy(GtkWidget* widget)
{
    GtkEmulator*    self  = CAST_EMULATOR(widget);
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);

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
    /* destruct video */ {
        (void) gem_video_destruct(widget, &self->video);
    }
    /* call superclass method */ {
        (*super->destroy)(widget);
    }
}

static void impl_widget_realize(GtkWidget* widget)
{
    GtkEmulator*  self = CAST_EMULATOR(widget);
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
    /* realize video */ {
        (void) gem_video_realize(widget, &self->video);
    }
}

static void impl_widget_unrealize(GtkWidget* widget)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);
    GtkEmulator*    self  = CAST_EMULATOR(widget);

    /* unrealize video */ {
        (void) gem_video_unrealize(widget, &self->video);
    }
    /* call superclass method */ {
        (*super->unrealize)(widget);
    }
}

static gboolean impl_widget_draw(GtkWidget* widget, cairo_t* cr)
{
    GtkEmulator* self  = CAST_EMULATOR(widget);
    XEvent       x11_event;

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
        x11_event.xexpose.type       = Expose;
        x11_event.xexpose.serial     = 0UL;
        x11_event.xexpose.send_event = True;
        x11_event.xexpose.display    = self->video.display;
        x11_event.xexpose.window     = self->video.window;
        x11_event.xexpose.x          = 0;
        x11_event.xexpose.y          = 0;
        x11_event.xexpose.width      = gtk_widget_get_allocated_width(widget);
        x11_event.xexpose.height     = gtk_widget_get_allocated_height(widget);
        x11_event.xexpose.count      = 0;
    }
    /* dispatch event */ {
        (void) gem_events_dispatch(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
    }
    return FALSE;
}

static void impl_widget_size_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);

    /* call superclass method */ {
        (*super->size_allocate)(widget, allocation);
    }
}

static void impl_widget_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width)
{
    GtkEmulator* self = CAST_EMULATOR(widget);

    /* set values */ {
        *minimum_width = self->minimum_width;
        *natural_width = self->natural_width;
    }
}

static void impl_widget_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height)
{
    GtkEmulator* self = CAST_EMULATOR(widget);

    /* set values */ {
        *minimum_height = self->minimum_height;
        *natural_height = self->natural_height;
    }
}

static void impl_widget_adjust_size_request(GtkWidget* widget, GtkOrientation orientation, gint* minimum_size, gint* natural_size)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);
    GtkEmulator*    self  = CAST_EMULATOR(widget);

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
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gtk_emulator_parent_class);
    GtkEmulator*    self  = CAST_EMULATOR(widget);

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
    GtkEmulator* self = CAST_EMULATOR(widget);
    XEvent       x11_event;

    /* forge keypress event */ {
        x11_event.xkey.type        = KeyPress;
        x11_event.xkey.serial      = 0UL;
        x11_event.xkey.send_event  = True;
        x11_event.xkey.display     = self->video.display;
        x11_event.xkey.window      = self->video.window;
        x11_event.xkey.root        = None;
        x11_event.xkey.subwindow   = None;
        x11_event.xkey.time        = event->time;
        x11_event.xkey.x           = 0;
        x11_event.xkey.y           = 0;
        x11_event.xkey.x_root      = 0;
        x11_event.xkey.y_root      = 0;
        x11_event.xkey.state       = 0;
        x11_event.xkey.keycode     = event->hardware_keycode;
        x11_event.xkey.same_screen = True;
    }
    /* adjust event state */ {
        if(event->state & GDK_SHIFT_MASK   ) x11_event.xkey.state |= ShiftMask;
        if(event->state & GDK_LOCK_MASK    ) x11_event.xkey.state |= LockMask;
        if(event->state & GDK_CONTROL_MASK ) x11_event.xkey.state |= ControlMask;
        if(event->state & GDK_MOD1_MASK    ) x11_event.xkey.state |= Mod1Mask;
        if(event->state & GDK_MOD2_MASK    ) x11_event.xkey.state |= Mod2Mask;
        if(event->state & GDK_MOD3_MASK    ) x11_event.xkey.state |= Mod3Mask;
        if(event->state & GDK_MOD4_MASK    ) x11_event.xkey.state |= Mod4Mask;
        if(event->state & GDK_MOD5_MASK    ) x11_event.xkey.state |= Mod5Mask;
        if(event->state & GDK_BUTTON1_MASK ) x11_event.xkey.state |= Button1Mask;
        if(event->state & GDK_BUTTON2_MASK ) x11_event.xkey.state |= Button2Mask;
        if(event->state & GDK_BUTTON3_MASK ) x11_event.xkey.state |= Button3Mask;
        if(event->state & GDK_BUTTON4_MASK ) x11_event.xkey.state |= Button4Mask;
        if(event->state & GDK_BUTTON5_MASK ) x11_event.xkey.state |= Button5Mask;
    }
    /* detect and discard auto-repeat */ {
        XEvent* x11_prev = &self->events.last_key_event;
        if(x11_prev->type == KeyPress) {
            if((x11_prev->xkey.display == x11_event.xkey.display)
            && (x11_prev->xkey.window  == x11_event.xkey.window )
            && (x11_prev->xkey.keycode == x11_event.xkey.keycode)
            && ((x11_event.xkey.time - x11_prev->xkey.time) < KEY_REPEAT_THRESHOLD)) {
                return TRUE;
            }
        }
    }
    /* preprocess keyboard event */ {
        if(gem_keyboard_preprocess(widget, &self->keyboard, &x11_event) != FALSE) {
            return TRUE;
        }
    }
#if 0
    /* check for same successive keypress/keyrelease */ {
        XEvent* x11_prev = &self->events.last_key_event;
        if((x11_prev->type == KeyPress) || (x11_prev->type == KeyRelease)) {
            if((x11_prev->xkey.display == x11_event.xkey.display)
            && (x11_prev->xkey.window  == x11_event.xkey.window )
            && (x11_prev->xkey.keycode == x11_event.xkey.keycode)
            && ((x11_event.xkey.time - x11_prev->xkey.time) < KEY_DELAY_THRESHOLD)) {
                (void) gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, NULL));
            }
        }
    }
#endif
    /* throttle input event */ {
        (void) gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
    }
    return TRUE;
}

static gboolean impl_widget_key_release_event(GtkWidget* widget, GdkEventKey* event)
{
    GtkEmulator* self = CAST_EMULATOR(widget);
    XEvent       x11_event;

    /* forge keypress event */ {
        x11_event.xkey.type        = KeyRelease;
        x11_event.xkey.serial      = 0UL;
        x11_event.xkey.send_event  = True;
        x11_event.xkey.display     = self->video.display;
        x11_event.xkey.window      = self->video.window;
        x11_event.xkey.root        = None;
        x11_event.xkey.subwindow   = None;
        x11_event.xkey.time        = event->time;
        x11_event.xkey.x           = 0;
        x11_event.xkey.y           = 0;
        x11_event.xkey.x_root      = 0;
        x11_event.xkey.y_root      = 0;
        x11_event.xkey.state       = 0;
        x11_event.xkey.keycode     = event->hardware_keycode;
        x11_event.xkey.same_screen = True;
    }
    /* adjust event state */ {
        if(event->state & GDK_SHIFT_MASK   ) x11_event.xkey.state |= ShiftMask;
        if(event->state & GDK_LOCK_MASK    ) x11_event.xkey.state |= LockMask;
        if(event->state & GDK_CONTROL_MASK ) x11_event.xkey.state |= ControlMask;
        if(event->state & GDK_MOD1_MASK    ) x11_event.xkey.state |= Mod1Mask;
        if(event->state & GDK_MOD2_MASK    ) x11_event.xkey.state |= Mod2Mask;
        if(event->state & GDK_MOD3_MASK    ) x11_event.xkey.state |= Mod3Mask;
        if(event->state & GDK_MOD4_MASK    ) x11_event.xkey.state |= Mod4Mask;
        if(event->state & GDK_MOD5_MASK    ) x11_event.xkey.state |= Mod5Mask;
        if(event->state & GDK_BUTTON1_MASK ) x11_event.xkey.state |= Button1Mask;
        if(event->state & GDK_BUTTON2_MASK ) x11_event.xkey.state |= Button2Mask;
        if(event->state & GDK_BUTTON3_MASK ) x11_event.xkey.state |= Button3Mask;
        if(event->state & GDK_BUTTON4_MASK ) x11_event.xkey.state |= Button4Mask;
        if(event->state & GDK_BUTTON5_MASK ) x11_event.xkey.state |= Button5Mask;
    }
    /* preprocess keyboard event */ {
        if(gem_keyboard_preprocess(widget, &self->keyboard, &x11_event) != FALSE) {
            return TRUE;
        }
    }
    /* throttle input event */ {
        (void) gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
    }
    return TRUE;
}

static gboolean impl_widget_button_press_event(GtkWidget* widget, GdkEventButton* event)
{
    if(gtk_widget_has_focus(widget) == FALSE) {
        gtk_widget_grab_focus(widget);
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

static void impl_emulator_hotkey(GtkEmulator* emulator, KeySym keysym)
{
}

static void gtk_emulator_class_init(GtkEmulatorClass* emulator_class)
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
                                                    , G_STRUCT_OFFSET(GtkEmulatorClass, sig_hotkey)
                                                    , NULL
                                                    , NULL
                                                    , NULL
                                                    , G_TYPE_NONE
                                                    , 1
                                                    , G_TYPE_POINTER );

    }
}

static void gtk_emulator_init(GtkEmulator* self)
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
        schedule(widget, 1UL);
    }
}

GtkWidget* gtk_emulator_new(void)
{
    return g_object_new(GTK_TYPE_EMULATOR, NULL);
}

void gtk_emulator_set_backend(GtkWidget* widget, const GemBackend* backend)
{
    GtkEmulator* self = CAST_EMULATOR(widget);

    /* set backend */ {
        self->backend = *backend;
    }
}

void gtk_emulator_set_joystick(GtkWidget* widget, int id, const char* device)
{
    GtkEmulator* self     = CAST_EMULATOR(widget);
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
    GtkEmulator* self        = CAST_EMULATOR(widget);
    GdkWindow*   gdk_window  = gtk_widget_get_window(widget);
    GdkDisplay*  gdk_display = gdk_window_get_display(gdk_window);

    if(video->display == NULL) {
        video->display = GDK_DISPLAY_XDISPLAY(gdk_display);
        video->window  = GDK_WINDOW_XID(gdk_window);
    }
    if(video->display != NULL) {
        GemBackend* backend = &self->backend;
        GemEvent    closure;
        closure.u.any.x11_event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->create_window_func)(backend->instance, &closure);
    }
    return video;
}

GemVideo* gem_video_unrealize(GtkWidget* widget, GemVideo* video)
{
    GtkEmulator* self = CAST_EMULATOR(widget);

    if(video->display != NULL) {
        GemBackend* backend = &self->backend;
        GemEvent    closure;
        closure.u.any.x11_event = gem_events_copy_or_fill(widget, &self->events, NULL);
        (void) (*backend->delete_window_func)(backend->instance, &closure);
    }
    if(video->display != NULL) {
        video->display = NULL;
        video->window  = None;
    }
    return video;
}

GemEvents* gem_events_construct(GtkWidget* widget, GemEvents* events)
{
    XEvent x11_event;

    /* initialize x11_event */ {
        (void) memset(&x11_event, 0, sizeof(x11_event));
        x11_event.xany.type       = GenericEvent;
        x11_event.xany.serial     = 0UL;
        x11_event.xany.send_event = True;
        x11_event.xany.display    = NULL;
        x11_event.xany.window     = None;
    }
    /* initialize */ {
        events->last_rcv_event = x11_event;
        events->last_key_event = x11_event;
        events->head = 0;
        events->tail = 0;
    }
    return events;
}

GemEvents* gem_events_destruct(GtkWidget* widget, GemEvents* events)
{
    XEvent x11_event;

    /* initialize x11_event */ {
        (void) memset(&x11_event, 0, sizeof(x11_event));
        x11_event.xany.type       = GenericEvent;
        x11_event.xany.serial     = 0UL;
        x11_event.xany.send_event = True;
        x11_event.xany.display    = NULL;
        x11_event.xany.window     = None;
    }
    /* finalize */ {
        events->last_rcv_event = x11_event;
        events->last_key_event = x11_event;
        events->head = 0;
        events->tail = 0;
    }
    return events;
}

GemEvents* gem_events_dispatch(GtkWidget* widget, GemEvents* events, XEvent* x11_event)
{
    GtkEmulator* self    = CAST_EMULATOR(widget);
    GemBackend*  backend = &self->backend;
    GemEvent     closure;

    /* initialize closure */ {
        closure.u.any.x11_event = x11_event;
    }
    /* dispatch x11_event */ {
        switch(x11_event->type) {
            case ConfigureNotify:
                {
                    (void) (*backend->resize_window_func)(backend->instance, &closure);
                }
                break;
            case Expose:
                {
                    (void) (*backend->expose_window_func)(backend->instance, &closure);
                }
                break;
            case KeyPress:
                {
                    (void) (*backend->key_press_func)(backend->instance, &closure);
                }
                break;
            case KeyRelease:
                {
                    (void) (*backend->key_release_func)(backend->instance, &closure);
                }
                break;
            case ButtonPress:
                {
                    (void) (*backend->button_press_func)(backend->instance, &closure);
                }
                break;
            case ButtonRelease:
                {
                    (void) (*backend->button_release_func)(backend->instance, &closure);
                }
                break;
            case MotionNotify:
                {
                    (void) (*backend->motion_notify_func)(backend->instance, &closure);
                }
                break;
            default:
                break;
        }
    }
    return events;
}

GemEvents* gem_events_throttle(GtkWidget* widget, GemEvents* events, XEvent* x11_event)
{
    unsigned int head = ((events->head + 0) % countof(events->list));
    unsigned int tail = ((events->tail + 1) % countof(events->list));

    if(tail != head) {
        events->list[events->tail] = *x11_event;
        events->head = head;
        events->tail = tail;
    }
    return events;
}

GemEvents* gem_events_process(GtkWidget* widget, GemEvents* events)
{
    int event_type = 0;

    while(events->head != events->tail) {
        XEvent* x11_event = &events->list[events->head];
        if(event_type == 0) {
            event_type = x11_event->type;
        }
        if(x11_event->type == event_type) {
            (void) gem_events_dispatch(widget, events, x11_event);
            events->head = ((events->head + 1) % countof(events->list));
            events->tail = ((events->tail + 0) % countof(events->list));
        }
        else {
            break;
        }
    }
    return events;
}

XEvent* gem_events_copy_or_fill(GtkWidget* widget, GemEvents* events, XEvent* x11_event)
{
    GtkEmulator* self = CAST_EMULATOR(widget);

    if(x11_event != NULL) {
        events->last_rcv_event = *x11_event;
        if((x11_event->type == KeyPress) || (x11_event->type == KeyRelease)) {
            events->last_key_event = *x11_event;
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

gboolean gem_keyboard_preprocess(GtkWidget* widget, GemKeyboard* keyboard, XEvent* x11_event)
{
    GtkEmulator* self = CAST_EMULATOR(widget);
    KeySym       keysym = XLookupKeysym(&x11_event->xkey, 0);

    if(x11_event->type == KeyPress) {
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
                        event_value  = (x11_event->type == KeyPress ? -32767 : 0);
                    }
                    break;
                case XK_Down:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 1;
                        event_value  = (x11_event->type == KeyPress ? +32767 : 0);
                    }
                    break;
                case XK_Left:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 0;
                        event_value  = (x11_event->type == KeyPress ? -32767 : 0);
                    }
                    break;
                case XK_Right:
                    {
                        event_type   = JS_EVENT_AXIS;
                        event_number = 0;
                        event_value  = (x11_event->type == KeyPress ? +32767 : 0);
                    }
                    break;
                case XK_Control_L:
                    {
                        event_type   = JS_EVENT_BUTTON;
                        event_number = 0;
                        event_value  = (x11_event->type == KeyPress ? 1 : 0);
                    }
                    break;
                case XK_Alt_L:
                    {
                        event_type   = JS_EVENT_BUTTON;
                        event_number = 1;
                        event_value  = (x11_event->type == KeyPress ? 1 : 0);
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
                        XEvent x11_event;
                        /* update keyboard */ {
                            if((event_number &= 1) == 0) {
                                keyboard->js_button0 = event_value;
                            }
                            else {
                                keyboard->js_button1 = event_value;
                            }
                        }
                        /* initialize button event */ {
                            x11_event.xbutton.type        = (event_value != 0 ? ButtonPress : ButtonRelease);
                            x11_event.xbutton.serial      = 0UL;
                            x11_event.xbutton.send_event  = True;
                            x11_event.xbutton.display     = self->video.display;
                            x11_event.xbutton.window      = self->video.window;
                            x11_event.xbutton.root        = None;
                            x11_event.xbutton.subwindow   = None;
                            x11_event.xbutton.time        = 0UL;
                            x11_event.xbutton.x           = keyboard->js_axis_x;
                            x11_event.xbutton.y           = keyboard->js_axis_y;
                            x11_event.xbutton.x_root      = 0;
                            x11_event.xbutton.y_root      = 0;
                            x11_event.xbutton.state       = AnyModifier << (keyboard->js_id + 1);
                            x11_event.xbutton.button      = event_number;
                            x11_event.xbutton.same_screen = True;
                        }
                        /* dispatch event */ {
                            (void) gem_events_dispatch(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
                        }
                    }
                    return TRUE;
                case JS_EVENT_AXIS:
                    {
                        XEvent x11_event;
                        /* update keyboard */ {
                            if((event_number &= 1) == 0) {
                                keyboard->js_axis_x = event_value;
                            }
                            else {
                                keyboard->js_axis_y = event_value;
                            }
                        }
                        /* initialize motion event */ {
                            x11_event.xmotion.type        = MotionNotify;
                            x11_event.xmotion.serial      = 0UL;
                            x11_event.xmotion.send_event  = True;
                            x11_event.xmotion.display     = self->video.display;
                            x11_event.xmotion.window      = self->video.window;
                            x11_event.xmotion.root        = None;
                            x11_event.xmotion.subwindow   = None;
                            x11_event.xmotion.time        = 0UL;
                            x11_event.xmotion.x           = keyboard->js_axis_x;
                            x11_event.xmotion.y           = keyboard->js_axis_y;
                            x11_event.xmotion.x_root      = 0;
                            x11_event.xmotion.y_root      = 0;
                            x11_event.xmotion.state       = AnyModifier << (keyboard->js_id + 1);
                            x11_event.xmotion.is_hint     = 0;
                            x11_event.xmotion.same_screen = True;
                        }
                        /* dispatch event */ {
                            (void) gem_events_dispatch(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
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
    GtkEmulator* self = CAST_EMULATOR(widget);

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
    GtkEmulator* self     = CAST_EMULATOR(widget);
    GemJoystick* joystick = gem_joystick_lookup_by_fd(widget, fd);

    /* joystick was not found */ {
        if(joystick == NULL) {
            g_warning("joystick width fd <%d> was not found", fd);
            return FALSE;
        }
    }
#ifdef HAVE_LINUX_JOYSTICK_H
    /* linux joystick */ {
        struct js_event js_event;
        /* read joystick event */ {
            const ssize_t bytes = read(joystick->fd, &js_event, sizeof(js_event));
            if(bytes != sizeof(js_event)) {
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
            switch(js_event.type) {
                case JS_EVENT_BUTTON:
                    {
                        XEvent x11_event;
                        /* check for special button */ {
                            if(js_event.value != 0) {
                                unsigned short code = joystick->js_mapping[js_event.number];
                                if(code == BTN_SELECT) {
                                    return TRUE;
                                }
                                if(code == BTN_START) {
                                    return TRUE;
                                }
                                if(code == BTN_MODE) {
                                    return TRUE;
                                }
                            }
                        }
                        /* update joystick */ {
                            if((js_event.number &= 1) == 0) {
                                joystick->js_button0 = js_event.value;
                            }
                            else {
                                joystick->js_button1 = js_event.value;
                            }
                        }
                        /* initialize button event */ {
                            x11_event.xbutton.type        = (js_event.value != 0 ? ButtonPress : ButtonRelease);
                            x11_event.xbutton.serial      = 0UL;
                            x11_event.xbutton.send_event  = True;
                            x11_event.xbutton.display     = self->video.display;
                            x11_event.xbutton.window      = self->video.window;
                            x11_event.xbutton.root        = None;
                            x11_event.xbutton.subwindow   = None;
                            x11_event.xbutton.time        = 0UL;
                            x11_event.xbutton.x           = joystick->js_axis_x;
                            x11_event.xbutton.y           = joystick->js_axis_y;
                            x11_event.xbutton.x_root      = 0;
                            x11_event.xbutton.y_root      = 0;
                            x11_event.xmotion.state       = AnyModifier << (joystick->js_id + 1);
                            x11_event.xbutton.button      = js_event.number;
                            x11_event.xbutton.same_screen = True;
                        }
                        /* dispatch event */ {
                            (void) gem_events_dispatch(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
                        }
                    }
                    break;
                case JS_EVENT_AXIS:
                    {
                        XEvent x11_event;
                        /* update joystick */ {
                            if((js_event.number &= 1) == 0) {
                                joystick->js_axis_x = js_event.value;
                            }
                            else {
                                joystick->js_axis_y = js_event.value;
                            }
                        }
                        /* initialize motion event */ {
                            x11_event.xmotion.type        = MotionNotify;
                            x11_event.xmotion.serial      = 0UL;
                            x11_event.xmotion.send_event  = True;
                            x11_event.xmotion.display     = self->video.display;
                            x11_event.xmotion.window      = self->video.window;
                            x11_event.xmotion.root        = None;
                            x11_event.xmotion.subwindow   = None;
                            x11_event.xmotion.time        = 0UL;
                            x11_event.xmotion.x           = joystick->js_axis_x;
                            x11_event.xmotion.y           = joystick->js_axis_y;
                            x11_event.xmotion.x_root      = 0;
                            x11_event.xmotion.y_root      = 0;
                            x11_event.xmotion.state       = AnyModifier << (joystick->js_id + 1);
                            x11_event.xmotion.is_hint     = 0;
                            x11_event.xmotion.same_screen = True;
                        }
                        /* dispatch event */ {
                            (void) gem_events_dispatch(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &x11_event));
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

unsigned long gem_backend_default_handler(void* instance, GemEvent* closure)
{
    return 0UL;
}

GemBackend* gem_backend_construct(GtkWidget* widget, GemBackend* backend)
{
    /* construct backend */ {
        backend->instance            = NULL;
        backend->reset_func          = &gem_backend_default_handler;
        backend->clock_func          = &gem_backend_default_handler;
        backend->create_window_func  = &gem_backend_default_handler;
        backend->delete_window_func  = &gem_backend_default_handler;
        backend->resize_window_func  = &gem_backend_default_handler;
        backend->expose_window_func  = &gem_backend_default_handler;
        backend->key_press_func      = &gem_backend_default_handler;
        backend->key_release_func    = &gem_backend_default_handler;
        backend->button_press_func   = &gem_backend_default_handler;
        backend->button_release_func = &gem_backend_default_handler;
        backend->motion_notify_func  = &gem_backend_default_handler;
    }
    return backend;
}

GemBackend* gem_backend_destruct(GtkWidget* widget, GemBackend* backend)
{
    /* destruct backend */ {
        backend->instance            = NULL;
        backend->reset_func          = &gem_backend_default_handler;
        backend->clock_func          = &gem_backend_default_handler;
        backend->create_window_func  = &gem_backend_default_handler;
        backend->delete_window_func  = &gem_backend_default_handler;
        backend->resize_window_func  = &gem_backend_default_handler;
        backend->expose_window_func  = &gem_backend_default_handler;
        backend->key_press_func      = &gem_backend_default_handler;
        backend->key_release_func    = &gem_backend_default_handler;
        backend->button_press_func   = &gem_backend_default_handler;
        backend->button_release_func = &gem_backend_default_handler;
        backend->motion_notify_func  = &gem_backend_default_handler;
    }
    return backend;
}

GemBackend* gem_backend_copy(GtkWidget* widget, GemBackend* backend, GemBackend* source)
{
    /* setup backend */ {
        backend->instance            = source->instance;
        backend->reset_func          = (source->reset_func          != NULL ? source->reset_func          : &gem_backend_default_handler);
        backend->clock_func          = (source->clock_func          != NULL ? source->clock_func          : &gem_backend_default_handler);
        backend->create_window_func  = (source->create_window_func  != NULL ? source->create_window_func  : &gem_backend_default_handler);
        backend->delete_window_func  = (source->delete_window_func  != NULL ? source->delete_window_func  : &gem_backend_default_handler);
        backend->resize_window_func  = (source->resize_window_func  != NULL ? source->resize_window_func  : &gem_backend_default_handler);
        backend->expose_window_func  = (source->expose_window_func  != NULL ? source->expose_window_func  : &gem_backend_default_handler);
        backend->key_press_func      = (source->key_press_func      != NULL ? source->key_press_func      : &gem_backend_default_handler);
        backend->key_release_func    = (source->key_release_func    != NULL ? source->key_release_func    : &gem_backend_default_handler);
        backend->button_press_func   = (source->button_press_func   != NULL ? source->button_press_func   : &gem_backend_default_handler);
        backend->button_release_func = (source->button_release_func != NULL ? source->button_release_func : &gem_backend_default_handler);
        backend->motion_notify_func  = (source->motion_notify_func  != NULL ? source->motion_notify_func  : &gem_backend_default_handler);
    }
    return backend;
}
