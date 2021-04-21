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

static void gem_emulator_destroy(GtkWidget* widget)
{
    /* call superclass method */ {
        GTK_WIDGET_CLASS(gem_emulator_parent_class)->destroy(widget);
    }
}

static void gem_emulator_realize(GtkWidget* widget)
{
    GtkAllocation allocation;
    GdkWindowAttr attributes;
    gint          attributes_mask;

    if(gtk_widget_get_has_window(widget) == FALSE) {
        GTK_WIDGET_CLASS(gem_emulator_parent_class)->realize(widget);
    }
    else {
        gtk_widget_set_realized(widget, TRUE);
        /* get allocation attributes */ {
            gtk_widget_get_allocation(widget, &allocation);
        }
        /* set attributes */ {
            attributes.window_type = GDK_WINDOW_CHILD;
            attributes.x           = allocation.x;
            attributes.y           = allocation.y;
            attributes.width       = allocation.width;
            attributes.height      = allocation.height;
            attributes.wclass      = GDK_INPUT_OUTPUT;
            attributes.visual      = gtk_widget_get_visual(widget);
            attributes.event_mask  = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_STRUCTURE_MASK | GDK_FOCUS_CHANGE_MASK;
            attributes_mask        = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
        }
        /* create window */ {
            GdkWindow* window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
            gtk_widget_register_window(widget, window);
            gtk_widget_set_window(widget, window);
        }
    }
}

static void gem_emulator_unrealize(GtkWidget* widget)
{
    /* call superclass method */ {
        GTK_WIDGET_CLASS(gem_emulator_parent_class)->unrealize(widget);
    }
}

static gboolean gem_emulator_key_press_event(GtkWidget* widget, GdkEventKey* event)
{
    /* todo */ {
        (void) fprintf(stdout, "%s::key_press_event()\n", gtk_widget_get_name(widget));
    }
    return TRUE;
}

static gboolean gem_emulator_key_release_event(GtkWidget* widget, GdkEventKey* event)
{
    /* todo */ {
        (void) fprintf(stdout, "%s::key_release_event()\n", gtk_widget_get_name(widget));
    }
    return TRUE;
}

static gboolean gem_emulator_button_press_event(GtkWidget* widget, GdkEventButton* event)
{
    /* todo */ {
        (void) fprintf(stdout, "%s::button_press_event()\n", gtk_widget_get_name(widget));
    }
    if(gtk_widget_has_focus(widget) == FALSE) {
        gtk_widget_grab_focus(widget);
    }
    return TRUE;
}

static gboolean gem_emulator_button_release_event(GtkWidget* widget, GdkEventButton* event)
{
    /* todo */ {
        (void) fprintf(stdout, "%s::button_release_event()\n", gtk_widget_get_name(widget));
    }
    return TRUE;
}

static gboolean gem_emulator_configure_event(GtkWidget* widget, GdkEventConfigure* event)
{
    /* todo */ {
        (void) fprintf(stdout, "%s::configure_event()\n", gtk_widget_get_name(widget));
    }
    return TRUE;
}

static void gem_emulator_class_init(GemEmulatorClass* class)
{
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

    widget_class->destroy              = &gem_emulator_destroy;
    widget_class->realize              = &gem_emulator_realize;
    widget_class->unrealize            = &gem_emulator_unrealize;
    widget_class->key_press_event      = &gem_emulator_key_press_event;
    widget_class->key_release_event    = &gem_emulator_key_release_event;
    widget_class->button_press_event   = &gem_emulator_button_press_event;
    widget_class->button_release_event = &gem_emulator_button_release_event;
    widget_class->configure_event      = &gem_emulator_configure_event;
}

static void gem_emulator_init(GemEmulator* self)
{
    gtk_widget_set_can_focus(GTK_WIDGET(self), TRUE);
    gtk_widget_set_focus_on_click(GTK_WIDGET(self), TRUE);
    gtk_widget_set_receives_default(GTK_WIDGET(self), TRUE);
    gtk_widget_set_has_window(GTK_WIDGET(self), TRUE);
    gem_emulator_machine_construct(self, &self->machine);
    gem_emulator_keyboard_construct(self, &self->keyboard);
    gem_emulator_joystick_construct(self, &self->joystick0);
    gem_emulator_joystick_construct(self, &self->joystick1);
}

GtkWidget* gem_emulator_new(void)
{
    return g_object_new(GEM_TYPE_EMULATOR, NULL);
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
