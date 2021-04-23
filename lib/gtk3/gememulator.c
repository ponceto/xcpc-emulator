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
    return 0UL;
}

static void sanitize_machine_procs(GtkWidget* widget)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(self->machine.create_proc  == NULL) { self->machine.create_proc  = &default_machine_proc; }
    if(self->machine.destroy_proc == NULL) { self->machine.destroy_proc = &default_machine_proc; }
    if(self->machine.realize_proc == NULL) { self->machine.realize_proc = &default_machine_proc; }
    if(self->machine.resize_proc  == NULL) { self->machine.resize_proc  = &default_machine_proc; }
    if(self->machine.expose_proc  == NULL) { self->machine.expose_proc  = &default_machine_proc; }
    if(self->machine.input_proc   == NULL) { self->machine.input_proc   = &default_machine_proc; }
    if(self->machine.timer_proc   == NULL) { self->machine.timer_proc   = &default_machine_proc; }
}

static XEvent* copy_or_fill_event(GtkWidget* widget, XEvent* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(event != NULL) {
        self->last_rcv_event = *event;
        if((event->type == KeyPress)
        || (event->type == KeyRelease)) {
            self->last_key_event = *event;
        }
    }
    else {
        self->last_rcv_event.xany.type       = GenericEvent;
        self->last_rcv_event.xany.serial     = 0UL;
        self->last_rcv_event.xany.send_event = True;
        self->last_rcv_event.xany.display    = self->x11.display;
        self->last_rcv_event.xany.window     = self->x11.window;
    }
    return &self->last_rcv_event;
}

static void throttle_input_event(GtkWidget* widget, XEvent* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    unsigned int head = ((self->throttled.head + 0) % countof(self->throttled.list));
    unsigned int tail = ((self->throttled.tail + 1) % countof(self->throttled.list));

    if(tail != head) {
        self->throttled.list[self->throttled.tail] = *event;
        self->throttled.head = head;
        self->throttled.tail = tail;
    }
}

static void process_throttled_input_event(GtkWidget* widget)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    int event_type = 0;

    while(self->throttled.head != self->throttled.tail) {
        XEvent* event = &self->throttled.list[self->throttled.head];
        if(event_type == 0) {
            event_type = event->type;
        }
        if(event->type == event_type) {
            (void) (*self->machine.input_proc)(self->machine.instance, event);
            self->throttled.head = ((self->throttled.head + 1) % countof(self->throttled.list));
            self->throttled.tail = ((self->throttled.tail + 0) % countof(self->throttled.list));
        }
        else {
            break;
        }
    }
}

static gboolean timer_handler(GtkWidget* widget)
{
    GemEmulator*  self    = CAST_EMULATOR(widget);
    unsigned long timeout = 0UL;

    /* aknowledge timer */ {
        self->timer = 0;
    }
    /* call timer-proc */ {
        timeout = (*self->machine.timer_proc)(self->machine.instance, copy_or_fill_event(widget, NULL));
    }
    /* restart timer */ {
        self->timer = g_timeout_add(timeout, G_SOURCE_FUNC(&timer_handler), self);
    }
    /* process throttled input event */ {
        process_throttled_input_event(widget);
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

static void initialize_x11_handles(GtkWidget* gtk_widget)
{
    GemEmulator* self        = CAST_EMULATOR(gtk_widget);
    GdkWindow*   gdk_window  = gtk_widget_get_window(gtk_widget);
    GdkScreen*   gdk_screen  = gdk_window_get_screen(gdk_window);
    GdkDisplay*  gdk_display = gdk_window_get_display(gdk_window);

    if(self->x11.display == NULL) {
        /* initialize X11 handles */ {
            self->x11.display = GDK_DISPLAY_XDISPLAY(gdk_display);
            self->x11.screen  = GDK_SCREEN_XSCREEN(gdk_screen);
            self->x11.window  = GDK_WINDOW_XID(gdk_window);
        }
        /* call realize-proc */ {
            if(self->x11.display != NULL) {
                (void) (*self->machine.realize_proc)(self->machine.instance, copy_or_fill_event(gtk_widget, NULL));
            }
        }
    }
}

static void finalize_x11_handles(GtkWidget* widget)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    if(self->x11.display != NULL) {
        /* finalize X11 handles */ {
            self->x11.display = NULL;
            self->x11.screen  = NULL;
            self->x11.window  = None;
        }
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
                    }
                }
                /* process height */ {
                    if(gtk_widget_get_allocated_height(widget) != xevent->xconfigure.height) {
                        self->minimum_height = xevent->xconfigure.height;
                        self->natural_height = xevent->xconfigure.height;
                    }
                    else {
                        self->minimum_height = EMULATOR_MIN_HEIGHT;
                    }
                }
                /* call resize-proc */ {
                    (void) (*self->machine.resize_proc)(self->machine.instance, copy_or_fill_event(widget, xevent));
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

    /* destroy timeout */ {
        unschedule(widget);
    }
    /* call destroy-proc */ {
        (void) (*self->machine.destroy_proc)(self->machine.instance, copy_or_fill_event(widget, NULL));
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
            finalize_x11_handles(widget);
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
            initialize_x11_handles(widget);
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
        XEvent* prev = &self->last_key_event;
        if(prev->type == KeyPress) {
            if((prev->xkey.display == xevent.xkey.display)
            && (prev->xkey.window  == xevent.xkey.window )
            && (prev->xkey.keycode == xevent.xkey.keycode)
            && ((xevent.xkey.time - prev->xkey.time) < KEY_REPEAT_THRESHOLD)) {
                return TRUE;
            }
        }
    }
    /* check for same successive keypress/keyrelease */ {
        XEvent* prev = &self->last_key_event;
        if((prev->type == KeyPress) || (prev->type == KeyRelease)) {
            if((prev->xkey.display == xevent.xkey.display)
            && (prev->xkey.window  == xevent.xkey.window )
            && (prev->xkey.keycode == xevent.xkey.keycode)
            && ((xevent.xkey.time - prev->xkey.time) < KEY_DELAY_THRESHOLD)) {
                throttle_input_event(widget, copy_or_fill_event(widget, NULL));
            }
        }
    }
    /* throttle input event */ {
        throttle_input_event(widget, copy_or_fill_event(widget, &xevent));
    }
    return TRUE;
}

static gboolean impl_widget_key_release_event(GtkWidget* widget, GdkEventKey* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);
    XEvent       xevent;

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            initialize_x11_handles(widget);
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
    /* throttle input event */ {
        throttle_input_event(widget, copy_or_fill_event(widget, &xevent));
    }
    return TRUE;
}

static gboolean impl_widget_button_press_event(GtkWidget* widget, GdkEventButton* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            initialize_x11_handles(widget);
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
            initialize_x11_handles(widget);
        }
    }
    return TRUE;
}

static gboolean impl_widget_configure_event(GtkWidget* widget, GdkEventConfigure* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            initialize_x11_handles(widget);
        }
    }
    return TRUE;
}

static gboolean impl_widget_draw(GtkWidget* widget, cairo_t* cr)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            initialize_x11_handles(widget);
        }
    }
    /* clear surface */ {
        cairo_paint(cr);
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
    /* initialize machine */ {
        sanitize_machine_procs(widget);
    }
    /* initialize keyboard */ {
        gem_emulator_keyboard_construct(self, &self->keyboard);
    }
    /* initialize joysticks */ {
        gem_emulator_joystick_construct(self, &self->joystick0);
        gem_emulator_joystick_construct(self, &self->joystick1);
    }
    /* throttled events */ {
        self->throttled.head = 0;
        self->throttled.tail = 0;
    }
    /* initialize minimum/natural dimensions */ {
        self->minimum_width  = EMULATOR_DFL_WIDTH;
        self->minimum_height = EMULATOR_DFL_HEIGHT;
        self->natural_width  = EMULATOR_DFL_WIDTH;
        self->natural_height = EMULATOR_DFL_HEIGHT;
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
        (void) (*self->machine.destroy_proc)(self->machine.instance, copy_or_fill_event(widget, NULL));
    }
    /* set machine */ {
        self->machine = *machine;
    }
    /* initialize machine */ {
        sanitize_machine_procs(widget);
    }
    /* call create-proc */ {
        (void) (*self->machine.create_proc)(self->machine.instance, copy_or_fill_event(widget, NULL));
    }
}

void gem_emulator_keyboard_construct(GemEmulator* self, GemKeyboard* keyboard)
{
}

void gem_emulator_keyboard_destruct(GemEmulator* self, GemKeyboard* keyboard)
{
}

void gem_emulator_joystick_construct(GemEmulator* self, GemJoystick* joystick)
{
}

void gem_emulator_joystick_destruct(GemEmulator* self, GemJoystick* joystick)
{
}
