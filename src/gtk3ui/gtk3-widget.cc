/*
 * gtk3-widget.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "gtk3-widget.h"

// ---------------------------------------------------------------------------
// gtk3::WidgetTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct WidgetTraits
    : BasicTraits
{
    static auto create_widget() -> GtkWidget*
    {
        return nullptr;
    }

    static auto destroy(GtkWidget*& instance) -> void
    {
        if(instance != nullptr) {
            instance = (::gtk_widget_destroy(instance), nullptr);
        }
    }

    static auto show_all(Widget& widget) -> void
    {
        if(widget) {
            return ::gtk_widget_show_all(widget);
        }
    }

    static auto show(Widget& widget) -> void
    {
        if(widget) {
            return ::gtk_widget_show(widget);
        }
    }

    static auto hide(Widget& widget) -> void
    {
        if(widget) {
            return ::gtk_widget_hide(widget);
        }
    }

    static auto grab_focus(Widget& widget) -> void
    {
        if(widget) {
            return ::gtk_widget_grab_focus(widget);
        }
    }

    static auto set_can_focus(Widget& widget, bool can_focus) -> void
    {
        if(widget) {
            return ::gtk_widget_set_can_focus(widget, can_focus);
        }
    }

    static auto set_focus_on_click(Widget& widget, bool focus_on_click) -> void
    {
        if(widget) {
            return ::gtk_widget_set_focus_on_click(widget, focus_on_click);
        }
    }

    static auto set_sensitive(Widget& widget, bool sensitive) -> void
    {
        if(widget) {
            return ::gtk_widget_set_sensitive(widget, sensitive);
        }
    }

    static auto is_sensitive(Widget& widget) -> bool
    {
        if(widget) {
            return ::gtk_widget_is_sensitive(widget) != FALSE;
        }
        return false;
    }

    static auto drag_dest_set(Widget& widget, GtkDestDefaults flags, const GtkTargetEntry* targets, int num_targets, GdkDragAction actions) -> void
    {
        if(widget) {
            return ::gtk_drag_dest_set(widget, flags, targets, num_targets, actions);
        }
    }

    static auto add_events(Widget& widget, gint events) -> void
    {
        if(widget) {
            return ::gtk_widget_add_events(widget, events);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::WidgetTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Widget
// ---------------------------------------------------------------------------

namespace gtk3 {

Widget::Widget()
    : Widget(traits::create_widget())
{
}

Widget::Widget(GtkWidget* instance)
    : _instance(instance)
{
    traits::register_widget_instance(_instance);
}

Widget::~Widget()
{
    traits::destroy(_instance);
}

auto Widget::destroy() -> void
{
    return traits::destroy(_instance);
}

auto Widget::show_all() -> void
{
    return traits::show_all(*this);
}

auto Widget::show() -> void
{
    return traits::show(*this);
}

auto Widget::hide() -> void
{
    return traits::hide(*this);
}

auto Widget::grab_focus() -> void
{
    return traits::grab_focus(*this);
}

auto Widget::set_can_focus(bool can_focus) -> void
{
    return traits::set_can_focus(*this, can_focus);
}

auto Widget::set_focus_on_click(bool focus_on_click) -> void
{
    return traits::set_focus_on_click(*this, focus_on_click);
}

auto Widget::set_sensitive(bool sensitive) -> void
{
    return traits::set_sensitive(*this, sensitive);
}

auto Widget::is_sensitive() -> bool
{
    return traits::is_sensitive(*this);
}

auto Widget::drag_dest_set(GtkDestDefaults flags, const GtkTargetEntry* targets, int num_targets, GdkDragAction actions) -> void
{
    return traits::drag_dest_set(*this, flags, targets, num_targets, actions);
}

auto Widget::signal_connect(const char* signal, GCallback callback, void* data) -> void
{
    return traits::signal_connect(_instance, signal, callback, data);
}

auto Widget::add_realize_callback(GCallback callback, void* data) -> void
{
    return signal_connect(sig_realize, callback, data);
}

auto Widget::add_unrealize_callback(GCallback callback, void* data) -> void
{
    return signal_connect(sig_unrealize, callback, data);
}

auto Widget::add_key_press_event_callback(GCallback callback, void* data) -> void
{
    traits::add_events(*this, GDK_KEY_PRESS_MASK);

    return signal_connect(sig_key_press_event, callback, data);
}

auto Widget::add_key_release_event_callback(GCallback callback, void* data) -> void
{
    traits::add_events(*this, GDK_KEY_RELEASE_MASK);

    return signal_connect(sig_key_release_event, callback, data);
}

auto Widget::add_button_press_event_callback(GCallback callback, void* data) -> void
{
    traits::add_events(*this, GDK_BUTTON_PRESS_MASK);

    return signal_connect(sig_button_press_event, callback, data);
}

auto Widget::add_button_release_event_callback(GCallback callback, void* data) -> void
{
    traits::add_events(*this, GDK_BUTTON_RELEASE_MASK);

    return signal_connect(sig_button_release_event, callback, data);
}

auto Widget::add_motion_notify_event_callback(GCallback callback, void* data) -> void
{
    traits::add_events(*this, GDK_POINTER_MOTION_MASK);

    return signal_connect(sig_motion_notify_event, callback, data);
}

auto Widget::add_drag_data_received_callback(GCallback callback, void* data) -> void
{
    return signal_connect(sig_drag_data_received, callback, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
