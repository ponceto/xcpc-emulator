/*
 * gtk3-emulator-x11.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-emulator-x11.h"

// ---------------------------------------------------------------------------
// gtk3::EmulatorX11Traits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct EmulatorX11Traits
    : BasicTraits
{
    static auto create_emulator_x11() -> GtkWidget*
    {
        return ::gtk_emulator_x11_new();
    }

    static auto shutdown(EmulatorX11& emulator) -> void
    {
        if(emulator) {
            ::gtk_emulator_x11_shutdown(emulator);
        }
    }

    static auto set_backend(EmulatorX11& emulator, const GemBackend* backend) -> void
    {
        if(emulator) {
            ::gtk_emulator_x11_set_backend(emulator, backend);
        }
    }

    static auto set_joystick(EmulatorX11& emulator, int id, const std::string& device) -> void
    {
        if(emulator) {
            ::gtk_emulator_x11_set_joystick(emulator, id, device.c_str());
        }
    }

    static auto set_joystick_emulation(EmulatorX11& emulator, bool enabled) -> void
    {
        if(emulator) {
            ::gtk_emulator_x11_set_joystick_emulation(emulator, enabled ? TRUE : FALSE);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::EmulatorX11Traits;

}

// ---------------------------------------------------------------------------
// gtk3::EmulatorX11
// ---------------------------------------------------------------------------

namespace gtk3 {

EmulatorX11::EmulatorX11()
    : EmulatorX11(traits::create_emulator_x11())
{
}

EmulatorX11::EmulatorX11(GtkWidget* instance)
    : Widget(instance)
{
}

auto EmulatorX11::create_emulator_x11() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_emulator_x11();
        traits::register_widget_instance(_instance);
    }
}

auto EmulatorX11::shutdown() -> void
{
    return traits::shutdown(*this);
}

auto EmulatorX11::set_backend(const GemBackend* backend) -> void
{
    return traits::set_backend(*this, backend);
}

auto EmulatorX11::set_joystick(int id, const std::string& device) -> void
{
    return traits::set_joystick(*this, id, device);
}

auto EmulatorX11::set_joystick_emulation(bool enabled) -> void
{
    return traits::set_joystick_emulation(*this, enabled);
}

auto EmulatorX11::add_hotkey_callback(GCallback callback, void* data) -> void
{
    return signal_connect(sig_hotkey, callback, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
