/*
 * vga-core.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <xcpc/libxcpc-priv.h>
#include "vga-core.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = vga::Type;
    using State     = vga::State;
    using Colormap  = vga::Colormap;
    using Scanline  = vga::Scanline;
    using Instance  = vga::Instance;
    using Interface = vga::Interface;

    static constexpr uint8_t BIT0 = 0x01;
    static constexpr uint8_t BIT1 = 0x02;
    static constexpr uint8_t BIT2 = 0x04;
    static constexpr uint8_t BIT3 = 0x08;
    static constexpr uint8_t BIT4 = 0x10;
    static constexpr uint8_t BIT5 = 0x20;
    static constexpr uint8_t BIT6 = 0x40;
    static constexpr uint8_t BIT7 = 0x80;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::StateTraits
// ---------------------------------------------------------------------------

namespace {

struct StateTraits final
    : public BasicTraits
{
    static inline auto construct(State& state, const Type type) -> void
    {
        state.type = type;
        setup_mode0(state);
        setup_mode1(state);
        setup_mode2(state);
        setup_mode3(state);
    }

    static inline auto destruct(State& state) -> void
    {
        state.type = Type::TYPE_INVALID;
    }

    static inline auto setup_mode0(State& state) -> void
    {
        uint32_t index = 0;
        for(auto& value : state.mode0) {
            value = ((index & BIT7) >> 7) | ((index & BIT3) >> 2)
                  | ((index & BIT5) >> 3) | ((index & BIT1) << 2)
                  | ((index & BIT6) >> 2) | ((index & BIT2) << 3)
                  | ((index & BIT4) << 2) | ((index & BIT0) << 7)
                  ;
            ++index;
        }
    }

    static inline auto setup_mode1(State& state) -> void
    {
        uint32_t index = 0;
        for(auto& value : state.mode1) {
            value = ((index & BIT7) >> 7) | ((index & BIT3) >> 2)
                  | ((index & BIT6) >> 4) | ((index & BIT2) << 1)
                  | ((index & BIT5) >> 1) | ((index & BIT1) << 4)
                  | ((index & BIT4) << 2) | ((index & BIT0) << 7)
                  ;
            ++index;
        }
    }

    static inline auto setup_mode2(State& state) -> void
    {
        uint32_t index = 0;
        for(auto& value : state.mode2) {
            value = ((index & BIT7) >> 7) | ((index & BIT6) >> 5)
                  | ((index & BIT5) >> 3) | ((index & BIT4) >> 1)
                  | ((index & BIT3) << 1) | ((index & BIT2) << 3)
                  | ((index & BIT1) << 5) | ((index & BIT0) << 7)
                  ;
            ++index;
        }
    }

    static inline auto setup_mode3(State& state) -> void
    {
        uint32_t index = 0;
        for(auto& value : state.mode3) {
            value = ((index & BIT7) >> 7) | ((index & BIT3) >> 2)
                  | ((index & BIT6) >> 4) | ((index & BIT2) << 1)
                  | ((index & BIT5) >> 1) | ((index & BIT1) << 4)
                  | ((index & BIT4) << 2) | ((index & BIT0) << 7)
                  ;
            ++index;
        }
    }

    static inline auto reset(State& state) -> void
    {
        state.pen &= 0;
        for(auto& ink : state.ink) {
            ink &= 0;
        }
        state.rmr &= 0;
        state.r52 &= 0;
        state.r02 &= 0;
        state.frame_x &= 0;
        state.frame_y &= 0;
        for(auto& scanline : state.scanline) {
            scanline = Scanline();
        }
    }

    static inline auto clock(State& state) -> void
    {
    }
};

}

// ---------------------------------------------------------------------------
// vga::Instance
// ---------------------------------------------------------------------------

namespace vga {

Instance::Instance(const Type type, Interface& interface)
    : _interface(interface)
    , _state()
{
    StateTraits::construct(_state, type);

    reset();
}

Instance::~Instance()
{
    StateTraits::destruct(_state);
}

auto Instance::reset() -> void
{
    StateTraits::reset(_state);
}

auto Instance::clock() -> void
{
    StateTraits::clock(_state);
}

auto Instance::get_value(uint8_t value) -> uint8_t
{
    return 0xff;
}

auto Instance::set_value(uint8_t value) -> uint8_t
{
    const uint8_t function = ((value >> 6) & 0x03);

    switch(function) {
        case 0: /* pen */
            _state.pen = ((value & 0x10) != 0 ? (value & 0x10) : (value & 0x0f));
            break;
        case 1: /* ink */
            _state.ink[_state.pen] = (value & 0x1f);
            break;
        case 2: /* interrupt control, rom configuration and screen mode */
            _state.rmr = (value & 0x1f);
            if((value & 0x10) != 0) {
                _state.r52 = 0;
            }
            _interface.vga_setup_rmr(*this, value);
            break;
        case 3: /* ram memory management */
            _interface.vga_setup_ram(*this, value);
            break;
    }
    return value;
}

auto Instance::ack_interrupt() -> void
{
    _state.r52 &= 0b11011111;
}

auto Instance::assert_hsync(uint8_t hsync) -> void
{
    auto on_rising_edge = [&]() -> void
    {
        if(++_state.frame_y < 576) {
            Scanline& scanline(_state.scanline[_state.frame_y]);
            scanline.mode = (_state.rmr & 0x03);
            int index = 0;
            for(const auto ink : _state.ink) {
                auto& entry(scanline.color[index]);
                entry.ink    = ink;
                entry.pixel0 = _state.colormap.pixel0[ink];
                entry.pixel1 = _state.colormap.pixel1[ink];
                ++index;
            }
        }
        else {
            _state.frame_y = 575;
        }
    };

    auto on_falling_edge = [&]() -> void
    {
        if(++_state.r52 == 52) {
            _state.r52 = 0;
            _interface.vga_raise_int(*this, 0);
        }
        if(_state.r02 != 0) {
            if(--_state.r02 == 0) {
                if(_state.r52 >= 32) {
                    _interface.vga_raise_int(*this, 0);
                }
                _state.r52 = 0;
            }
        }
    };

    if(hsync != 0) {
        on_rising_edge();
    }
    else {
        on_falling_edge();
    }
}

auto Instance::assert_vsync(uint8_t vsync) -> void
{
    auto on_rising_edge = [&]() -> void
    {
        _state.frame_y = 0;
        _state.r02 = 2;
    };

    auto on_falling_edge = [&]() -> void
    {
    };

    if(vsync != 0) {
        on_rising_edge();
    }
    else {
        on_falling_edge();
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
