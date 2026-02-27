/*
 * gtk3-scale.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_SCALE_H__
#define __GTK3_CXX_SCALE_H__

#include <gtk3ui/gtk3-range.h>

// ---------------------------------------------------------------------------
// gtk3::Scale
// ---------------------------------------------------------------------------

namespace gtk3 {

class Scale
    : public Range
{
public: // public interface
    Scale();

    Scale(GtkWidget*);

    Scale(const Scale&) = delete;

    Scale& operator=(const Scale&) = delete;

    virtual ~Scale() = default;

    operator GtkScale*() const
    {
        return GTK_SCALE(_instance);
    }

    auto create_scale(GtkOrientation orientation, double min, double max, double step) -> void;

    auto set_digits(int digits) -> void;

    auto set_value_pos(GtkPositionType pos) -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_SCALE_H__ */
