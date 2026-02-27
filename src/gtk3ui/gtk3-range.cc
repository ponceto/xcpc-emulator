/*
 * gtk3-range.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-range.h"

// ---------------------------------------------------------------------------
// gtk3::RangeTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct RangeTraits
    : BasicTraits
{
    static auto create_range() -> GtkWidget*
    {
        return nullptr;
    }

    static auto get_value(const Range& range) -> double
    {
        if(range) {
            return ::gtk_range_get_value(range);
        }
        return 0.0;
    }

    static auto set_value(Range& range, double value) -> void
    {
        if(range) {
            ::gtk_range_set_value(range, value);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::RangeTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Range
// ---------------------------------------------------------------------------

namespace gtk3 {

Range::Range()
    : Range(traits::create_range())
{
}

Range::Range(GtkWidget* instance)
    : Widget(instance)
{
}

auto Range::get_value() const -> double
{
    return traits::get_value(*this);
}

auto Range::set_value(double value) -> void
{
    return traits::set_value(*this, value);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
