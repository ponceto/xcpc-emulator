/*
 * gtk3-emulator.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-emulator.h"

// ---------------------------------------------------------------------------
// gtk3::EmulatorTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct EmulatorTraits
    : BasicTraits
{
    static GtkWidget* create_emulator()
    {
        return ::gtk_emulator_new();
    }

    static void set_backend(Emulator& emulator, const GemBackend* backend)
    {
        if(emulator) {
            ::gtk_emulator_set_backend(emulator, backend);
        }
    }

    static void set_joystick(Emulator& emulator, int id, const std::string& device)
    {
        if(emulator) {
            ::gtk_emulator_set_joystick(emulator, id, device.c_str());
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::EmulatorTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Emulator
// ---------------------------------------------------------------------------

namespace gtk3 {

Emulator::Emulator()
    : Emulator(traits::create_emulator())
{
}

Emulator::Emulator(GtkWidget* instance)
    : Widget(instance)
{
}

void Emulator::create_emulator()
{
    if(_instance == nullptr) {
        _instance = traits::create_emulator();
        traits::register_widget_instance(_instance);
    }
}

void Emulator::set_backend(const GemBackend* backend)
{
    return traits::set_backend(*this, backend);
}

void Emulator::set_joystick(int id, const std::string& device)
{
    return traits::set_joystick(*this, id, device);
}

void Emulator::add_hotkey_callback(GCallback callback, void* data)
{
    return signal_connect(sig_hotkey, callback, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
