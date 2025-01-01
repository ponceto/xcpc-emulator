/*
 * gtk3-window.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "gtk3-window.h"

// ---------------------------------------------------------------------------
// gtk3::WindowTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct WindowTraits
    : BasicTraits
{
    static GtkWidget* create_window()
    {
        return nullptr;
    }

    static GtkWidget* create_toplevel_window()
    {
        return ::gtk_window_new(GTK_WINDOW_TOPLEVEL);
    }

    static GtkWidget* create_popup_window()
    {
        return ::gtk_window_new(GTK_WINDOW_POPUP);
    }

    static void set_title(Window& window, const std::string& title)
    {
        if(window) {
            ::gtk_window_set_title(window, title.c_str());
        }
    }

    static void set_icon(Window& window, GdkPixbuf* icon)
    {
        if(window) {
            ::gtk_window_set_icon(window, icon);
        }
    }

    static void set_skip_taskbar_hint(Window& window, bool taskbar_hint)
    {
        if(window) {
            ::gtk_window_set_skip_taskbar_hint(window, (taskbar_hint != false ? TRUE : FALSE));
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::WindowTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Window
// ---------------------------------------------------------------------------

namespace gtk3 {

Window::Window()
    : Window(traits::create_window())
{
}

Window::Window(GtkWidget* instance)
    : Bin(instance)
{
}

void Window::set_title(const std::string& title)
{
    return traits::set_title(*this, title);
}

void Window::set_icon(GdkPixbuf* icon)
{
    return traits::set_icon(*this, icon);
}

void Window::set_skip_taskbar_hint(bool taskbar_hint)
{
    return traits::set_skip_taskbar_hint(*this, taskbar_hint);
}

}

// ---------------------------------------------------------------------------
// gtk3::ToplevelWindow
// ---------------------------------------------------------------------------

namespace gtk3 {

ToplevelWindow::ToplevelWindow()
    : ToplevelWindow(traits::create_toplevel_window())
{
}

ToplevelWindow::ToplevelWindow(GtkWidget* instance)
    : Window(instance)
{
}

void ToplevelWindow::create_toplevel_window()
{
    if(_instance == nullptr) {
        _instance = traits::create_toplevel_window();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::PopupWindow
// ---------------------------------------------------------------------------

namespace gtk3 {

PopupWindow::PopupWindow()
    : PopupWindow(traits::create_popup_window())
{
}

PopupWindow::PopupWindow(GtkWidget* instance)
    : Window(instance)
{
}

void PopupWindow::create_popup_window()
{
    if(_instance == nullptr) {
        _instance = traits::create_popup_window();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
