/*
 * gtk3-emulator-x11.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_EMULATOR_X11_H__
#define __GTK3_CXX_EMULATOR_X11_H__

#include <gtk3ui/gtk3-widget.h>

// ---------------------------------------------------------------------------
// gtk3::EmulatorX11
// ---------------------------------------------------------------------------

namespace gtk3 {

class EmulatorX11
    : public Widget
{
public: // public interface
    EmulatorX11();

    EmulatorX11(GtkWidget*);

    EmulatorX11(EmulatorX11&&) = delete;

    EmulatorX11(const EmulatorX11&) = delete;

    EmulatorX11& operator=(EmulatorX11&&) = delete;

    EmulatorX11& operator=(const EmulatorX11&) = delete;

    virtual ~EmulatorX11() = default;

    operator GtkEmulatorX11*() const
    {
        return GTK_EMULATOR_X11(_instance);
    }

    auto create_emulator_x11() -> void;

    auto shutdown() -> void;

    auto set_backend(const GemBackend* backend) -> void;

    auto set_joystick(int id, const std::string& device) -> void;

    auto get_joystick_emulation() -> bool;

    auto set_joystick_emulation(bool enabled) -> void;

    auto add_hotkey_callback(GCallback callback, void* data) -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_EMULATOR_X11_H__ */
