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
#include <Gem/gememulator-priv.h>

G_DEFINE_TYPE(GemEmulator, gem_emulator, GTK_TYPE_WIDGET)

static unsigned long default_machine_proc(GemEmulatorData data, XEvent* event)
{
    return 0UL;
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

static gboolean timeout_handler(GtkWidget* widget)
{
    GemEmulator*  self    = CAST_EMULATOR(widget);
    unsigned long timeout = 0UL;

    /* aknowledge timeout */ {
        self->timeout = 0;
    }
    /* call timer-proc */ {
        timeout = (*self->machine.timer_proc)(self->machine.instance, copy_or_fill_event(widget, NULL));
    }
    /* restart timeout */ {
        self->timeout = g_timeout_add(timeout, G_SOURCE_FUNC(&timeout_handler), self);
    }
    return FALSE;
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
        if(self->timeout != 0) {
            self->timeout = ((void) g_source_remove(self->timeout), 0U);
        }
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
    GtkWidgetClass* super = GTK_WIDGET_CLASS(gem_emulator_parent_class);
    GtkAllocation   allocation;
    GdkWindowAttr   attributes;
    gint            attributes_mask;

    if(gtk_widget_get_has_window(widget) == FALSE) {
        (*super->realize)(widget);
    }
    else {
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
                                   | GDK_WA_VISUAL;
        }
        /* create window */ {
            GdkWindow* window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
            gtk_widget_register_window(widget, window);
            gtk_widget_set_window(widget, window);
        }
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

static gboolean impl_widget_key_press_event(GtkWidget* widget, GdkEventKey* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            initialize_x11_handles(widget);
        }
    }
    return TRUE;
}

static gboolean impl_widget_key_release_event(GtkWidget* widget, GdkEventKey* event)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* initialize X11 handles if needed */ {
        if(self->x11.display == NULL) {
            initialize_x11_handles(widget);
        }
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

static void impl_widget_adjust_size_request(GtkWidget* widget, GtkOrientation orientation, gint* minimum_size, gint* natural_size)
{
    if(orientation == GTK_ORIENTATION_HORIZONTAL) {
        *minimum_size = 640;
        *natural_size = 768;
    }
    else {
        *minimum_size = 480;
        *natural_size = 576;
    }
}

static void impl_widget_adjust_size_allocation(GtkWidget* widget, GtkOrientation orientation, gint* minimum_size, gint* natural_size, gint* allocated_pos, gint* allocated_size)
{
}

static void gem_emulator_class_init(GemEmulatorClass* class)
{
    GObjectClass*   object_class = G_OBJECT_CLASS(class);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

    /* initialize object_class */ {
        object_class->dispose                 = &impl_object_dispose;
        object_class->finalize                = &impl_object_finalize;
    }
    /* initialize widget_class */ {
        widget_class->destroy                 = &impl_widget_destroy;
        widget_class->realize                 = &impl_widget_realize;
        widget_class->unrealize               = &impl_widget_unrealize;
        widget_class->key_press_event         = &impl_widget_key_press_event;
        widget_class->key_release_event       = &impl_widget_key_release_event;
        widget_class->button_press_event      = &impl_widget_button_press_event;
        widget_class->button_release_event    = &impl_widget_button_release_event;
        widget_class->configure_event         = &impl_widget_configure_event;
        widget_class->draw                    = &impl_widget_draw;
        widget_class->adjust_size_request     = &impl_widget_adjust_size_request;
        widget_class->adjust_size_allocation  = &impl_widget_adjust_size_allocation;
    }
}

static void gem_emulator_init(GemEmulator* self)
{
    /* initialize widget */ {
        gtk_widget_set_has_window(GTK_WIDGET(self), TRUE);
        gtk_widget_set_can_focus(GTK_WIDGET(self), TRUE);
        gtk_widget_set_focus_on_click(GTK_WIDGET(self), TRUE);
        gtk_widget_set_receives_default(GTK_WIDGET(self), TRUE);
    }
    /* initialize other stuff */ {
        gem_emulator_machine_construct(self, &self->machine);
        gem_emulator_keyboard_construct(self, &self->keyboard);
        gem_emulator_joystick_construct(self, &self->joystick0);
        gem_emulator_joystick_construct(self, &self->joystick1);
    }
    /* initialize timeout */ {
        self->timeout = g_timeout_add(100U, G_SOURCE_FUNC(&timeout_handler), self);
    }
}

GtkWidget* gem_emulator_new(void)
{
    return g_object_new(GEM_TYPE_EMULATOR, NULL);
}

void gem_emulator_set_machine(GtkWidget* widget, const GemMachine* machine)
{
    GemEmulator* self = CAST_EMULATOR(widget);

    /* set interface */ {
        self->machine = *machine;
    }
    /* adjust procs */ {
        if(self->machine.create_proc  == NULL) { self->machine.create_proc  = &default_machine_proc; }
        if(self->machine.destroy_proc == NULL) { self->machine.destroy_proc = &default_machine_proc; }
        if(self->machine.realize_proc == NULL) { self->machine.realize_proc = &default_machine_proc; }
        if(self->machine.resize_proc  == NULL) { self->machine.resize_proc  = &default_machine_proc; }
        if(self->machine.expose_proc  == NULL) { self->machine.expose_proc  = &default_machine_proc; }
        if(self->machine.input_proc   == NULL) { self->machine.input_proc   = &default_machine_proc; }
        if(self->machine.timer_proc   == NULL) { self->machine.timer_proc   = &default_machine_proc; }
    }
    /* call create-proc */ {
        (void) (*self->machine.create_proc)(self->machine.instance, copy_or_fill_event(widget, NULL));
    }
}

void gem_emulator_machine_construct(GemEmulator* self, GemMachine* machine)
{
}

void gem_emulator_machine_destruct(GemEmulator* self, GemMachine* machine)
{
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
