/*
 * gtk3-range.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_RANGE_H__
#define __GTK3_CXX_RANGE_H__

#include <gtk3ui/gtk3-widget.h>

// ---------------------------------------------------------------------------
// gtk3::Range
// ---------------------------------------------------------------------------

namespace gtk3 {

class Range
    : public Widget
{
public: // public interface
    Range();

    Range(GtkWidget*);

    Range(Range&&) = delete;

    Range(const Range&) = delete;

    Range& operator=(Range&&) = delete;

    Range& operator=(const Range&) = delete;

    virtual ~Range() = default;

    operator GtkRange*() const
    {
        return GTK_RANGE(_instance);
    }

    auto get_value() const -> double;

    auto set_value(double value) -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_RANGE_H__ */
