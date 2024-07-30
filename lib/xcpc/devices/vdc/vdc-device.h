/*
 * vdc-device.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_VDC_DEVICE_H__
#define __XCPC_VDC_DEVICE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace vdc {

class Device;
class Interface;

}

// ---------------------------------------------------------------------------
// vdc::Type
// ---------------------------------------------------------------------------

namespace vdc {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
};

}

// ---------------------------------------------------------------------------
// vdc::State
// ---------------------------------------------------------------------------

namespace vdc {

struct State
{
    uint8_t type;
    struct
    {
        uint8_t addr;
        uint8_t data[18];
    } caps;
    struct
    {
        uint8_t addr;
        uint8_t data[18];
    } mask;
    union
    {
        struct
        {
            uint8_t addr;
            uint8_t data[18];
        } array;
        struct
        {
            uint8_t address_register;
            uint8_t horizontal_total;
            uint8_t horizontal_displayed;
            uint8_t horizontal_sync_position;
            uint8_t sync_width;
            uint8_t vertical_total;
            uint8_t vertical_total_adjust;
            uint8_t vertical_displayed;
            uint8_t vertical_sync_position;
            uint8_t interlace_mode_and_skew;
            uint8_t maximum_scanline_address;
            uint8_t cursor_start;
            uint8_t cursor_end;
            uint8_t start_address_high;
            uint8_t start_address_low;
            uint8_t cursor_high;
            uint8_t cursor_low;
            uint8_t light_pen_high;
            uint8_t light_pen_low;
        } named;
    } regs;
    struct
    {
        uint8_t hcc; /* horizontal char counter */
        uint8_t vcc; /* vertical char counter   */
        uint8_t slc; /* scanline counter        */
        uint8_t vac; /* vertical adjust counter */
        uint8_t hsc; /* horizontal sync counter */
        uint8_t vsc; /* vertical sync counter   */
        uint8_t hsync_signal;
        uint8_t vsync_signal;
    } core;
};

}

// ---------------------------------------------------------------------------
// vdc::Device
// ---------------------------------------------------------------------------

namespace vdc {

class Device
{
public: // public interface
    Device(const Type type, Interface& interface);

    Device(const Device&) = delete;

    Device& operator=(const Device&) = delete;

    virtual ~Device();

    auto reset() -> void;

    auto clock() -> void;

    auto get_index(uint8_t index) -> uint8_t;

    auto set_index(uint8_t index) -> uint8_t;

    auto get_value(uint8_t value) -> uint8_t;

    auto set_value(uint8_t value) -> uint8_t;

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
// vdc::Interface
// ---------------------------------------------------------------------------

namespace vdc {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;

    virtual auto vdc_hsync(Device& device, uint8_t hsync) -> uint8_t = 0;

    virtual auto vdc_vsync(Device& device, uint8_t vsync) -> uint8_t = 0;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_VDC_DEVICE_H__ */
