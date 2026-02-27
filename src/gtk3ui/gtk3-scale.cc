/*
 * gtk3-scale.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-scale.h"

// ---------------------------------------------------------------------------
// gtk3::ScaleTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ScaleTraits
    : BasicTraits
{
    static auto create_scale() -> GtkWidget*
    {
        return nullptr;
    }

    static auto create_scale(GtkOrientation orientation, double min, double max, double step) -> GtkWidget*
    {
        return ::gtk_scale_new_with_range(orientation, min, max, step);
    }

    static auto set_digits(Scale& scale, int digits) -> void
    {
        if(scale) {
            ::gtk_scale_set_digits(scale, digits);
        }
    }

    static auto set_value_pos(Scale& scale, GtkPositionType pos) -> void
    {
        if(scale) {
            ::gtk_scale_set_value_pos(scale, pos);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ScaleTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Scale
// ---------------------------------------------------------------------------

namespace gtk3 {

Scale::Scale()
    : Scale(traits::create_scale())
{
}

Scale::Scale(GtkWidget* instance)
    : Range(instance)
{
}

auto Scale::create_scale(GtkOrientation orientation, double min, double max, double step) -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_scale(orientation, min, max, step);
        traits::register_widget_instance(_instance);
    }
}

auto Scale::set_digits(int digits) -> void
{
    return traits::set_digits(*this, digits);
}

auto Scale::set_value_pos(GtkPositionType pos) -> void
{
    return traits::set_value_pos(*this, pos);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
