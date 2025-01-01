/*
 * gtk3-button.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __GTK3_CXX_BUTTON_H__
#define __GTK3_CXX_BUTTON_H__

#include <gtk3ui/gtk3-bin.h>

// ---------------------------------------------------------------------------
// gtk3::Button
// ---------------------------------------------------------------------------

namespace gtk3 {

class Button
    : public Bin
{
public: // public interface
    Button();

    Button(GtkWidget*);

    Button(const Button&) = delete;

    Button& operator=(const Button&) = delete;

    virtual ~Button() = default;

    operator GtkButton*() const
    {
        return GTK_BUTTON(_instance);
    }

    void create_button();

    void create_button_with_label(const std::string& string);

    void add_activate_callback(GCallback callback, void* data);

    void add_clicked_callback(GCallback callback, void* data);

    void add_enter_callback(GCallback callback, void* data);

    void add_leave_callback(GCallback callback, void* data);

    void add_pressed_callback(GCallback callback, void* data);

    void add_released_callback(GCallback callback, void* data);
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_BUTTON_H__ */
