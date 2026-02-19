/*
 * gtk3-button.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-button.h"

// ---------------------------------------------------------------------------
// gtk3::ButtonTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ButtonTraits
    : BasicTraits
{
    static GtkWidget* create_button()
    {
        return ::gtk_button_new();
    }

    static GtkWidget* create_button_with_label(const std::string& string = "button")
    {
        return ::gtk_button_new_with_label(string.c_str());
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ButtonTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Button
// ---------------------------------------------------------------------------

namespace gtk3 {

Button::Button()
    : Button(traits::create_button())
{
}

Button::Button(GtkWidget* instance)
    : Bin(instance)
{
}

void Button::create_button()
{
    if(_instance == nullptr) {
        _instance = traits::create_button();
        traits::register_widget_instance(_instance);
    }
}

void Button::create_button_with_label(const std::string& string)
{
    if(_instance == nullptr) {
        _instance = traits::create_button_with_label(string);
        traits::register_widget_instance(_instance);
    }
}

void Button::add_activate_callback(GCallback callback, void* data)
{
    return signal_connect(sig_activate, callback, data);
}

void Button::add_clicked_callback(GCallback callback, void* data)
{
    return signal_connect(sig_clicked, callback, data);
}

void Button::add_enter_callback(GCallback callback, void* data)
{
    return signal_connect(sig_enter, callback, data);
}

void Button::add_leave_callback(GCallback callback, void* data)
{
    return signal_connect(sig_leave, callback, data);
}

void Button::add_pressed_callback(GCallback callback, void* data)
{
    return signal_connect(sig_pressed, callback, data);
}

void Button::add_released_callback(GCallback callback, void* data)
{
    return signal_connect(sig_released, callback, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
