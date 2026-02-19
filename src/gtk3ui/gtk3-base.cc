/*
 * gtk3-base.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-base.h"

// ---------------------------------------------------------------------------
// gtk3::signals
// ---------------------------------------------------------------------------

namespace gtk3 {

const char sig_destroy[]              = "destroy";
const char sig_open[]                 = "open";
const char sig_startup[]              = "startup";
const char sig_shutdown[]             = "shutdown";
const char sig_activate[]             = "activate";
const char sig_clicked[]              = "clicked";
const char sig_enter[]                = "enter";
const char sig_leave[]                = "leave";
const char sig_pressed[]              = "pressed";
const char sig_released[]             = "released";
const char sig_select[]               = "select";
const char sig_deselect[]             = "deselect";
const char sig_drag_data_received[]   = "drag-data-received";
const char sig_hotkey[]               = "hotkey";
const char sig_render[]               = "render";
const char sig_resize[]               = "resize";
const char sig_realize[]              = "realize";
const char sig_unrealize[]            = "unrealize";
const char sig_key_press_event[]      = "key-press-event";
const char sig_key_release_event[]    = "key-release-event";
const char sig_button_press_event[]   = "button-press-event";
const char sig_button_release_event[] = "button-release-event";
const char sig_motion_notify_event[]  = "motion-notify-event";

}

// ---------------------------------------------------------------------------
// <anonymous>
// ---------------------------------------------------------------------------

namespace {

auto on_destroy(GtkWidget* widget_ptr, GtkWidget** widget_ref) -> void
{
    if((widget_ref != nullptr) && (*widget_ref == widget_ptr)) {
        *widget_ref = nullptr;
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::BasicTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

auto BasicTraits::register_widget_instance(GtkWidget*& instance) -> void
{
    if(instance != nullptr) {
        static_cast<void>(signal_connect(G_OBJECT(instance), sig_destroy, G_CALLBACK(&on_destroy), &instance));
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
