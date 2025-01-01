/*
 * gtk3-gl-area.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "gtk3-gl-area.h"

// ---------------------------------------------------------------------------
// gtk3::GLAreaTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct GLAreaTraits
    : BasicTraits
{
    static GtkWidget* create_gl_area()
    {
        return ::gtk_gl_area_new();
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::GLAreaTraits;

}

// ---------------------------------------------------------------------------
// gtk3::GLArea
// ---------------------------------------------------------------------------

namespace gtk3 {

GLArea::GLArea()
    : GLArea(traits::create_gl_area())
{
}

GLArea::GLArea(GtkWidget* instance)
    : Widget(instance)
{
}

void GLArea::create_gl_area()
{
    if(_instance == nullptr) {
        _instance = traits::create_gl_area();
        traits::register_widget_instance(_instance);
    }
}

void GLArea::add_render_callback(GCallback callback, void* data)
{
    return signal_connect(sig_render, callback, data);
}

void GLArea::add_resize_callback(GCallback callback, void* data)
{
    return signal_connect(sig_resize, callback, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
