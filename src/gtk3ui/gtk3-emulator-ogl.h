/*
 * gtk3-emulator-ogl.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_EMULATOR_OGL_H__
#define __GTK3_CXX_EMULATOR_OGL_H__

#include <gtk3ui/gtk3-gl-area.h>

// ---------------------------------------------------------------------------
// gtk3::EmulatorOGL
// ---------------------------------------------------------------------------

namespace gtk3 {

class EmulatorOGL
    : public GLArea
{
public: // public interface
    EmulatorOGL();

    EmulatorOGL(GtkWidget*);

    EmulatorOGL(EmulatorOGL&&) = delete;

    EmulatorOGL(const EmulatorOGL&) = delete;

    EmulatorOGL& operator=(EmulatorOGL&&) = delete;

    EmulatorOGL& operator=(const EmulatorOGL&) = delete;

    virtual ~EmulatorOGL() = default;

    operator GtkEmulatorOGL*() const
    {
        return GTK_EMULATOR_OGL(_instance);
    }

    auto create_emulator_ogl() -> void;

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

#endif /* __GTK3_CXX_EMULATOR_OGL_H__ */
