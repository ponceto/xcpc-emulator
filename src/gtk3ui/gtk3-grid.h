/*
 * gtk3-grid.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_GRID_H__
#define __GTK3_CXX_GRID_H__

#include <gtk3ui/gtk3-container.h>

// ---------------------------------------------------------------------------
// gtk3::Grid
// ---------------------------------------------------------------------------

namespace gtk3 {

class Grid
    : public Container
{
public: // public interface
    Grid();

    Grid(GtkWidget*);

    Grid(const Grid&) = delete;

    Grid& operator=(const Grid&) = delete;

    virtual ~Grid() = default;

    operator GtkGrid*() const
    {
        return GTK_GRID(_instance);
    }

    auto create_grid() -> void;

    auto attach(Widget& child, int left, int top, int width, int height) -> void;

    auto set_row_spacing(unsigned int spacing) -> void;

    auto set_row_homogeneous(bool homogeneous) -> void;

    auto set_column_spacing(unsigned int spacing) -> void;

    auto set_column_homogeneous(bool homogeneous) -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_GRID_H__ */
