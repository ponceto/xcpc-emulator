/*
 * vga-core.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_VGA_CORE_H__
#define __XCPC_VGA_CORE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace vga {

struct State;
class  Instance;
class  Interface;

}

// ---------------------------------------------------------------------------
// vga::Type
// ---------------------------------------------------------------------------

namespace vga {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
};

}

// ---------------------------------------------------------------------------
// vga::Colormap
// ---------------------------------------------------------------------------

namespace vga {

struct Colormap
{
    uint32_t pixel0[32];
    uint32_t pixel1[32];
};

}

// ---------------------------------------------------------------------------
// vga::Scanline
// ---------------------------------------------------------------------------

namespace vga {

struct Scanline
{
    uint8_t mode;
    struct {
        uint8_t  ink;
        uint32_t pixel0;
        uint32_t pixel1;
    } color[17];
};

}

// ---------------------------------------------------------------------------
// vga::State
// ---------------------------------------------------------------------------

namespace vga {

struct State
{
    uint8_t  type;
    uint8_t  pen;
    uint8_t  ink[17];
    uint8_t  rmr;
    uint8_t  r52;
    uint8_t  r02;
    uint16_t frame_x;
    uint16_t frame_y;
    uint8_t  mode0[256];
    uint8_t  mode1[256];
    uint8_t  mode2[256];
    uint8_t  mode3[256];
    Colormap colormap;
    Scanline scanline[576];
};

}

// ---------------------------------------------------------------------------
// vga::Instance
// ---------------------------------------------------------------------------

namespace vga {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto get_value(uint8_t value) -> uint8_t;

    auto set_value(uint8_t value) -> uint8_t;

    auto ack_interrupt() -> void;

    auto assert_hsync(uint8_t hsync) -> void;

    auto assert_vsync(uint8_t hsync) -> void;

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
// vga::Interface
// ---------------------------------------------------------------------------

namespace vga {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;

    virtual auto vga_raise_nmi(Instance& instance, uint8_t value) -> uint8_t = 0;

    virtual auto vga_raise_int(Instance& instance, uint8_t value) -> uint8_t = 0;

    virtual auto vga_setup_ram(Instance& instance, uint8_t value) -> uint8_t = 0;

    virtual auto vga_setup_rom(Instance& instance, uint8_t value) -> uint8_t = 0;

    virtual auto vga_setup_rmr(Instance& instance, uint8_t value) -> uint8_t = 0;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_VGA_CORE_H__ */
