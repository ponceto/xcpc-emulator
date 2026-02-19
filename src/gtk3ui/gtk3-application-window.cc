/*
 * gtk3-application-window.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-application-window.h"

// ---------------------------------------------------------------------------
// gtk3::ApplicationWindowTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ApplicationWindowTraits
    : BasicTraits
{
    static GtkWidget* create_application_window(Application& application)
    {
        return ::gtk_application_window_new(application);
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ApplicationWindowTraits;

}

// ---------------------------------------------------------------------------
// gtk3::ApplicationWindow
// ---------------------------------------------------------------------------

namespace gtk3 {

ApplicationWindow::ApplicationWindow(Application& application)
    : ApplicationWindow(traits::create_application_window(application))
{
}

ApplicationWindow::ApplicationWindow(GtkWidget* instance)
    : Window(instance)
{
}

void ApplicationWindow::create_application_window(Application& application)
{
    if(_instance == nullptr) {
        _instance = traits::create_application_window(application);
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
