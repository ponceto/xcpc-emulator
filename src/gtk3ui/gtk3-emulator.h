/*
 * gtk3-emulator.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __GTK3_CXX_EMULATOR_H__
#define __GTK3_CXX_EMULATOR_H__

#include <gtk3ui/gtk3-widget.h>

// ---------------------------------------------------------------------------
// gtk3::Emulator
// ---------------------------------------------------------------------------

namespace gtk3 {

class Emulator
    : public Widget
{
public: // public interface
    Emulator();

    Emulator(GtkWidget*);

    Emulator(const Emulator&) = delete;

    Emulator& operator=(const Emulator&) = delete;

    virtual ~Emulator() = default;

    operator GtkEmulator*() const
    {
        return GTK_EMULATOR(_instance);
    }

    void create_emulator();

    void set_backend(const GemBackend* backend);

    void set_joystick(int id, const std::string& device);

    void add_hotkey_callback(GCallback callback, void* data);
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_EMULATOR_H__ */
