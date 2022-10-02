/*
 * gtk3-window.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_FRAME_H__
#define __GTK3_CXX_FRAME_H__

#include <gtk3ui/gtk3-bin.h>

// ---------------------------------------------------------------------------
// gtk3::Frame
// ---------------------------------------------------------------------------

namespace gtk3 {

class Frame
    : public Bin
{
public: // public interface
    Frame();

    Frame(GtkWidget*);

    virtual ~Frame() = default;

    operator GtkFrame*() const
    {
        return GTK_FRAME(_instance);
    }

    void create_frame(const std::string& string);
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_FRAME_H__ */
