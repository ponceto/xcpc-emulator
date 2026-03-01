/*
 * gtk3-viewport.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-viewport.h"

// ---------------------------------------------------------------------------
// gtk3::ViewportTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ViewportTraits
    : BasicTraits
{
    static auto create_viewport() -> GtkWidget*
    {
        return ::gtk_viewport_new(nullptr, nullptr);
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ViewportTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Viewport
// ---------------------------------------------------------------------------

namespace gtk3 {

Viewport::Viewport()
    : Viewport(traits::create_viewport())
{
}

Viewport::Viewport(GtkWidget* instance)
    : Bin(instance)
{
}

auto Viewport::create_viewport() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_viewport();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
