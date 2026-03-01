/*
 * gtk3-emulator-ogl.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-emulator-ogl.h"

// ---------------------------------------------------------------------------
// gtk3::EmulatorOGLTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct EmulatorOGLTraits
    : BasicTraits
{
    static auto create_emulator_ogl() -> GtkWidget*
    {
        return ::gtk_emulator_ogl_new();
    }

    static auto shutdown(EmulatorOGL& emulator) -> void
    {
        if(emulator) {
            ::gtk_emulator_ogl_shutdown(emulator);
        }
    }

    static auto set_backend(EmulatorOGL& emulator, const GemBackend* backend) -> void
    {
        if(emulator) {
            ::gtk_emulator_ogl_set_backend(emulator, backend);
        }
    }

    static auto set_joystick(EmulatorOGL& emulator, int id, const std::string& device) -> void
    {
        if(emulator) {
            ::gtk_emulator_ogl_set_joystick(emulator, id, device.c_str());
        }
    }

    static auto get_joystick_emulation(EmulatorOGL& emulator) -> bool
    {
        if(emulator) {
            return ::gtk_emulator_ogl_get_joystick_emulation(emulator);
        }
        return false;
    }

    static auto set_joystick_emulation(EmulatorOGL& emulator, bool enabled) -> void
    {
        if(emulator) {
            ::gtk_emulator_ogl_set_joystick_emulation(emulator, enabled ? TRUE : FALSE);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::EmulatorOGLTraits;

}

// ---------------------------------------------------------------------------
// gtk3::EmulatorOGL
// ---------------------------------------------------------------------------

namespace gtk3 {

EmulatorOGL::EmulatorOGL()
    : EmulatorOGL(traits::create_emulator_ogl())
{
}

EmulatorOGL::EmulatorOGL(GtkWidget* instance)
    : GLArea(instance)
{
}

auto EmulatorOGL::create_emulator_ogl() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_emulator_ogl();
        traits::register_widget_instance(_instance);
    }
}

auto EmulatorOGL::shutdown() -> void
{
    return traits::shutdown(*this);
}

auto EmulatorOGL::set_backend(const GemBackend* backend) -> void
{
    return traits::set_backend(*this, backend);
}

auto EmulatorOGL::set_joystick(int id, const std::string& device) -> void
{
    return traits::set_joystick(*this, id, device);
}

auto EmulatorOGL::get_joystick_emulation() -> bool
{
    return traits::get_joystick_emulation(*this);
}

auto EmulatorOGL::set_joystick_emulation(bool enabled) -> void
{
    return traits::set_joystick_emulation(*this, enabled);
}

auto EmulatorOGL::add_hotkey_callback(GCallback callback, void* data) -> void
{
    return signal_connect(sig_hotkey, callback, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
