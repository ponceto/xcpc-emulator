/*
 * kbd-device.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_KBD_DEVICE_H__
#define __XCPC_KBD_DEVICE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace kbd {

class Device;
class Interface;

}

// ---------------------------------------------------------------------------
// kbd::Type
// ---------------------------------------------------------------------------

namespace kbd {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
    TYPE_ENGLISH =  1,
    TYPE_FRENCH  =  2,
    TYPE_GERMAN  =  3,
    TYPE_SPANISH =  4,
    TYPE_DANISH  =  5,
};

}

// ---------------------------------------------------------------------------
// kbd::State
// ---------------------------------------------------------------------------

namespace kbd {

struct State
{
    uint8_t type;
    uint8_t mode;
    uint8_t line;
    uint8_t keys[16];
};

}

// ---------------------------------------------------------------------------
// kbd::Device
// ---------------------------------------------------------------------------

namespace kbd {

class Device
{
public: // public interface
    Device(const Type type, Interface& interface);

    Device(const Device&) = delete;

    Device& operator=(const Device&) = delete;

    virtual ~Device();

    auto reset() -> void;

    auto clock() -> void;

    auto set_type(const Type type) -> void;

    auto set_line(uint8_t line = 0xff) -> uint8_t;

    auto get_data(uint8_t data = 0xff) -> uint8_t;

    auto key_press(const XKeyEvent& event) -> void;

    auto key_release(const XKeyEvent& event) -> void;

    auto button_press(const XButtonEvent& event) -> void;

    auto button_release(const XButtonEvent& event) -> void;

    auto motion_notify(const XMotionEvent& event) -> void;

    auto operator->() -> State*
    {
        return &_state;
    }

protected: // protected data
    Interface& _interface;
    State      _state;
};

}

// ---------------------------------------------------------------------------
// kbd::Interface
// ---------------------------------------------------------------------------

namespace kbd {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_KBD_DEVICE_H__ */
