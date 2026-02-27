/*
 * gtk3-grid.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-grid.h"

// ---------------------------------------------------------------------------
// gtk3::GridTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct GridTraits
    : BasicTraits
{
    static auto create_grid() -> GtkWidget*
    {
        return ::gtk_grid_new();
    }

    static auto attach(Grid& grid, Widget& child, int left, int top, int width, int height) -> void
    {
        if(grid && child) {
            ::gtk_grid_attach(grid, child, left, top, width, height);
        }
    }

    static auto set_row_spacing(Grid& grid, unsigned int spacing) -> void
    {
        if(grid) {
            ::gtk_grid_set_row_spacing(grid, spacing);
        }
    }

    static auto set_row_homogeneous(Grid& grid, bool homogeneous) -> void
    {
        if(grid) {
            ::gtk_grid_set_row_homogeneous(grid, homogeneous);
        }
    }

    static auto set_column_spacing(Grid& grid, unsigned int spacing) -> void
    {
        if(grid) {
            ::gtk_grid_set_column_spacing(grid, spacing);
        }
    }

    static auto set_column_homogeneous(Grid& grid, bool homogeneous) -> void
    {
        if(grid) {
            ::gtk_grid_set_column_homogeneous(grid, homogeneous);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::GridTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Grid
// ---------------------------------------------------------------------------

namespace gtk3 {

Grid::Grid()
    : Grid(traits::create_grid())
{
}

Grid::Grid(GtkWidget* instance)
    : Container(instance)
{
}

auto Grid::create_grid() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_grid();
        traits::register_widget_instance(_instance);
    }
}

auto Grid::attach(Widget& child, int left, int top, int width, int height) -> void
{
    return traits::attach(*this, child, left, top, width, height);
}

auto Grid::set_row_spacing(unsigned int spacing) -> void
{
    return traits::set_row_spacing(*this, spacing);
}

auto Grid::set_row_homogeneous(bool homogeneous) -> void
{
    return traits::set_row_homogeneous(*this, homogeneous);
}

auto Grid::set_column_spacing(unsigned int spacing) -> void
{
    return traits::set_column_spacing(*this, spacing);
}

auto Grid::set_column_homogeneous(bool homogeneous) -> void
{
    return traits::set_column_homogeneous(*this, homogeneous);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
