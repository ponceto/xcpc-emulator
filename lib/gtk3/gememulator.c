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
#include <gtk3/gememulator-priv.h>

G_DEFINE_TYPE(GemEmulator, gem_emulator, GTK_TYPE_WIDGET)

static unsigned long default_machine_proc(GemEmulatorData data, XEvent* event)
{
    return EMULATOR_DEFAULT_TIMEOUT;
}

static gboolean timer_handler(GtkWidget* widget)
{
    GemEmulator*  self    = CAST_EMULATOR(widget);
    unsigned long timeout = 0UL;

    /* aknowledge timer */ {
        self->timer = 0;
    }
    /* call timer-proc */ {
        if(gtk_widget_is_sensitive(widget) != FALSE) {
            timeout = (*self->machine.timer_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, NULL));
        }
        else {
            timeout = default_machine_proc(NULL, NULL);
        }
    }
    /* restart timer */ {
        self->timer = g_timeout_add(timeout, G_SOURCE_FUNC(&timer_handler), self);
    }
    /* process throttled input event */ {
        gem_events_process(widget, &self->events);
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

static GdkFilterReturn impl_filter_func(GdkXEvent* native_event, GdkEvent* event, gpointer data)
{
    GtkWidget*   widget = CAST_WIDGET(data);
    GemEmulator* self   = CAST_EMULATOR(data);
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
                /* call resize-proc */ {
                    (void) (*self->machine.resize_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, xevent));
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

    /* call destroy-proc */ {
        (void) (*self->machine.destroy_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, NULL));
    }
    /* unschedule timer */ {
        unschedule(widget);
    }
    /* destruct joysticks */ {
        gem_joystick_destruct(widget, &self->joystick0);
        gem_joystick_destruct(widget, &self->joystick1);
    }
    /* destruct keyboard */ {
        gem_keyboard_destruct(widget, &self->keyboard);
    }
    /* destruct machine */ {
        gem_machine_destruct(widget, &self->machine);
    }
    /* destruct events */ {
        gem_events_destruct(widget, &self->events);
    }
    /* destruct x11 */ {
        gem_x11_destruct(widget, &self->x11);
    }
    /* call superclass method */ {
        (*super->destroy)(widget);
    }
}

static void impl_widget_realize(GtkWidget* widget)
{
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
}

static void impl_widget_unrealize(GtkWidget* widget)
{
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);
    GemEmulator*    self  = CAST_EMULATOR(widget);

    /* finalize X11 handles if needed */ {
        if(self->x11.display != NULL) {
            gem_x11_unrealize(widget, &self->x11);
        }
    }
    /* call superclass method */ {
        (*super->unrealize)(widget);
    }
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

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            gem_x11_realize(widget, &self->x11);
        }
    }
    /* forge keypress event */ {
        xevent.xkey.type        = KeyPress;
        xevent.xkey.serial      = 0UL;
        xevent.xkey.send_event  = True;
        xevent.xkey.display     = self->x11.display;
        xevent.xkey.window      = self->x11.window;
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
                gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, NULL));
            }
        }
    }
    /* throttle input event */ {
        gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &xevent));
    }
    return TRUE;
}

static gboolean impl_widget_key_release_event(GtkWidget* widget, GdkEventKey* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    XEvent       xevent;

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            gem_x11_realize(widget, &self->x11);
        }
    }
    /* forge keypress event */ {
        xevent.xkey.type        = KeyRelease;
        xevent.xkey.serial      = 0UL;
        xevent.xkey.send_event  = True;
        xevent.xkey.display     = self->x11.display;
        xevent.xkey.window      = self->x11.window;
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
        gem_events_throttle(widget, &self->events, gem_events_copy_or_fill(widget, &self->events, &xevent));
    }
    return TRUE;
}

static gboolean impl_widget_button_press_event(GtkWidget* widget, GdkEventButton* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            gem_x11_realize(widget, &self->x11);
        }
    }
    /* grab the focus if needed */ {
        if(gtk_widget_has_focus(widget) == FALSE) {
            gtk_widget_grab_focus(widget);
        }
    }
    return TRUE;
}

static gboolean impl_widget_button_release_event(GtkWidget* widget, GdkEventButton* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            gem_x11_realize(widget, &self->x11);
        }
    }
    return TRUE;
}

static gboolean impl_widget_configure_event(GtkWidget* widget, GdkEventConfigure* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            gem_x11_realize(widget, &self->x11);
        }
    }
    return TRUE;
}

static gboolean impl_widget_draw(GtkWidget* widget, cairo_t* cr)
{
    GemEmulator* self  = CAST_EMULATOR(widget);
    XEvent       xevent;

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            gem_x11_realize(widget, &self->x11);
        }
    }
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
        xevent.xexpose.display    = self->x11.display;
        xevent.xexpose.window     = self->x11.window;
        xevent.xexpose.x          = 0;
        xevent.xexpose.y          = 0;
        xevent.xexpose.width      = gtk_widget_get_allocated_width(widget);
        xevent.xexpose.height     = gtk_widget_get_allocated_height(widget);
        xevent.xexpose.count      = 0;
    }
    /* call expose-proc */ {
        (void) (*self->machine.expose_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, &xevent));
    }
    return FALSE;
}

static void gem_emulator_class_init(GemEmulatorClass* class)
{
    GObjectClass*   object_class = G_OBJECT_CLASS(class);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

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
    /* construct x11 */ {
        gem_x11_construct(widget, &self->x11);
    }
    /* construct events */ {
        gem_events_construct(widget, &self->events);
    }
    /* construct machine */ {
        gem_machine_construct(widget, &self->machine);
    }
    /* construct keyboard */ {
        gem_keyboard_construct(widget, &self->keyboard, 0);
    }
    /* construct joysticks */ {
        gem_joystick_construct(widget, &self->joystick0, NULL, 0);
        gem_joystick_construct(widget, &self->joystick1, NULL, 1);
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
        schedule(widget, 100UL);
    }
}

GtkWidget* gem_emulator_new(void)
{
    return g_object_new(GEM_TYPE_EMULATOR, NULL);
}

void gem_emulator_set_machine(GtkWidget* widget, const GemMachine* machine)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* call destoy-proc */ {
        (void) (*self->machine.destroy_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, NULL));
    }
    /* set machine */ {
        self->machine = *machine;
    }
    /* initialize machine */ {
        gem_machine_sanitize(widget, &self->machine);
    }
    /* call create-proc */ {
        (void) (*self->machine.create_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, NULL));
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
        gem_joystick_destruct(widget, joystick);
        gem_joystick_construct(widget, joystick, device, id);
    }
}

void gem_x11_construct(GtkWidget* widget, GemX11* x11)
{
    /* initialize */ {
        x11->display = NULL;
        x11->screen  = NULL;
        x11->window  = None;
    }
}

void gem_x11_destruct(GtkWidget* widget, GemX11* x11)
{
    /* finalize */ {
        x11->display = NULL;
        x11->screen  = NULL;
        x11->window  = None;
    }
}

void gem_x11_realize(GtkWidget* gtk_widget, GemX11* x11)
{
    GemEmulator* self        = CAST_EMULATOR(gtk_widget);
    GdkWindow*   gdk_window  = gtk_widget_get_window(gtk_widget);
    GdkScreen*   gdk_screen  = gdk_window_get_screen(gdk_window);
    GdkDisplay*  gdk_display = gdk_window_get_display(gdk_window);

    /* realize X11 handles */ {
        if(x11->display == NULL) {
            x11->display = GDK_DISPLAY_XDISPLAY(gdk_display);
            x11->screen  = GDK_SCREEN_XSCREEN(gdk_screen);
            x11->window  = GDK_WINDOW_XID(gdk_window);
        }
        if(x11->display != NULL) {
            (void) (*self->machine.realize_proc)(self->machine.instance, gem_events_copy_or_fill(gtk_widget, &self->events, NULL));
        }
    }
}

void gem_x11_unrealize(GtkWidget* widget, GemX11* x11)
{
    /* unrealize X11 handles */ {
        if(x11->display != NULL) {
            x11->display = NULL;
            x11->screen  = NULL;
            x11->window  = None;
        }
    }
}

void gem_events_construct(GtkWidget* widget, GemEvents* events)
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
}

void gem_events_destruct(GtkWidget* widget, GemEvents* events)
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
}

void gem_events_throttle(GtkWidget* widget, GemEvents* events, XEvent* event)
{
    unsigned int head = ((events->head + 0) % countof(events->list));
    unsigned int tail = ((events->tail + 1) % countof(events->list));

    if(tail != head) {
        events->list[events->tail] = *event;
        events->head = head;
        events->tail = tail;
    }
}

void gem_events_process(GtkWidget* widget, GemEvents* events)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    int event_type = 0;

    while(events->head != events->tail) {
        XEvent* event = &events->list[events->head];
        if(event_type == 0) {
            event_type = event->type;
        }
        if(event->type == event_type) {
            (void) (*self->machine.input_proc)(self->machine.instance, event);
            events->head = ((events->head + 1) % countof(events->list));
            events->tail = ((events->tail + 0) % countof(events->list));
        }
        else {
            break;
        }
    }
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
        events->last_rcv_event.xany.display    = self->x11.display;
        events->last_rcv_event.xany.window     = self->x11.window;
    }
    return &events->last_rcv_event;
}

void gem_machine_construct(GtkWidget* widget, GemMachine* machine)
{
    /* initialize */ {
        machine->instance     = NULL;
        machine->create_proc  = &default_machine_proc;
        machine->destroy_proc = &default_machine_proc;
        machine->realize_proc = &default_machine_proc;
        machine->resize_proc  = &default_machine_proc;
        machine->expose_proc  = &default_machine_proc;
        machine->input_proc   = &default_machine_proc;
        machine->timer_proc   = &default_machine_proc;
    }
}

void gem_machine_destruct(GtkWidget* widget, GemMachine* machine)
{
    /* finalize */ {
        machine->instance     = NULL;
        machine->create_proc  = &default_machine_proc;
        machine->destroy_proc = &default_machine_proc;
        machine->realize_proc = &default_machine_proc;
        machine->resize_proc  = &default_machine_proc;
        machine->expose_proc  = &default_machine_proc;
        machine->input_proc   = &default_machine_proc;
        machine->timer_proc   = &default_machine_proc;
    }
}

void gem_machine_sanitize(GtkWidget* widget, GemMachine* machine)
{
    /* sanitize */ {
        if(machine->create_proc  == NULL) { machine->create_proc  = &default_machine_proc; }
        if(machine->destroy_proc == NULL) { machine->destroy_proc = &default_machine_proc; }
        if(machine->realize_proc == NULL) { machine->realize_proc = &default_machine_proc; }
        if(machine->resize_proc  == NULL) { machine->resize_proc  = &default_machine_proc; }
        if(machine->expose_proc  == NULL) { machine->expose_proc  = &default_machine_proc; }
        if(machine->input_proc   == NULL) { machine->input_proc   = &default_machine_proc; }
        if(machine->timer_proc   == NULL) { machine->timer_proc   = &default_machine_proc; }
    }
}

void gem_keyboard_construct(GtkWidget* widget, GemKeyboard* keyboard, int id)
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

void gem_keyboard_destruct(GtkWidget* widget, GemKeyboard* keyboard)
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
#if 0
            XtCallCallbackList(widget, self->hotkey_callback, &keysym);
#endif
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
                            xevent.xbutton.display     = self->x11.display;
                            xevent.xbutton.window      = self->x11.window;
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
                            (void) (*self->machine.input_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, &xevent));
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
                            xevent.xmotion.display     = self->x11.display;
                            xevent.xmotion.window      = self->x11.window;
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
                            (void) (*self->machine.input_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, &xevent));
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
void gem_joystick_construct(GtkWidget* widget, GemJoystick* joystick, const char* device, int id)
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

void gem_joystick_destruct(GtkWidget* widget, GemJoystick* joystick)
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
#if 0
                                    KeySym keysym = XK_Pause;
                                    XtCallCallbackList(widget, self->hotkey_callback, &keysym);
#endif
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
                            xevent.xbutton.display     = self->x11.display;
                            xevent.xbutton.window      = self->x11.window;
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
                            (void) (*self->machine.input_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, &xevent));
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
                            xevent.xmotion.display     = self->x11.display;
                            xevent.xmotion.window      = self->x11.window;
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
                            (void) (*self->machine.input_proc)(self->machine.instance, gem_events_copy_or_fill(widget, &self->events, &xevent));
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
