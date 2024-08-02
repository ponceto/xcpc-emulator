/*
 * vdc-device.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include "vdc-device.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = vdc::Type;
    using State     = vdc::State;
    using Device    = vdc::Device;
    using Interface = vdc::Interface;

    static constexpr uint8_t ADDRESS_REGISTER         = -1;
    static constexpr uint8_t HORIZONTAL_TOTAL         =  0;
    static constexpr uint8_t HORIZONTAL_DISPLAYED     =  1;
    static constexpr uint8_t HORIZONTAL_SYNC_POSITION =  2;
    static constexpr uint8_t SYNC_WIDTH               =  3;
    static constexpr uint8_t VERTICAL_TOTAL           =  4;
    static constexpr uint8_t VERTICAL_TOTAL_ADJUST    =  5;
    static constexpr uint8_t VERTICAL_DISPLAYED       =  6;
    static constexpr uint8_t VERTICAL_SYNC_POSITION   =  7;
    static constexpr uint8_t INTERLACE_MODE_AND_SKEW  =  8;
    static constexpr uint8_t MAXIMUM_SCANLINE_ADDRESS =  9;
    static constexpr uint8_t CURSOR_START             = 10;
    static constexpr uint8_t CURSOR_END               = 11;
    static constexpr uint8_t START_ADDRESS_HIGH       = 12;
    static constexpr uint8_t START_ADDRESS_LOW        = 13;
    static constexpr uint8_t CURSOR_HIGH              = 14;
    static constexpr uint8_t CURSOR_LOW               = 15;
    static constexpr uint8_t LIGHT_PEN_HIGH           = 16;
    static constexpr uint8_t LIGHT_PEN_LOW            = 17;
    static constexpr uint8_t REGISTER_COUNT           = 18;
    static constexpr uint8_t NOT_READABLE             = 0x00;
    static constexpr uint8_t REG_READABLE             = 0x01;
    static constexpr uint8_t NOT_WRITABLE             = 0x00;
    static constexpr uint8_t REG_WRITABLE             = 0x02;

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
    }

    static inline auto destruct(State& state) -> void
    {
        state.type = Type::TYPE_INVALID;
    }

    static inline auto reset_caps(State& state) -> void
    {
        state.caps.addr       = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x00] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x01] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x02] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x03] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x04] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x05] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x06] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x07] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x08] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x09] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x0a] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x0b] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x0c] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x0d] = (NOT_READABLE | REG_WRITABLE);
        state.caps.data[0x0e] = (REG_READABLE | REG_WRITABLE);
        state.caps.data[0x0f] = (REG_READABLE | REG_WRITABLE);
        state.caps.data[0x10] = (REG_READABLE | NOT_WRITABLE);
        state.caps.data[0x11] = (REG_READABLE | NOT_WRITABLE);
    }

    static inline auto reset_mask(State& state) -> void
    {
        state.mask.addr       = 0xff; /* xxxxxxxx */
        state.mask.data[0x00] = 0xff; /* xxxxxxxx */
        state.mask.data[0x01] = 0xff; /* xxxxxxxx */
        state.mask.data[0x02] = 0xff; /* xxxxxxxx */
        state.mask.data[0x03] = 0x0f; /* ----xxxx */
        state.mask.data[0x04] = 0x7f; /* -xxxxxxx */
        state.mask.data[0x05] = 0x1f; /* ---xxxxx */
        state.mask.data[0x06] = 0x7f; /* -xxxxxxx */
        state.mask.data[0x07] = 0x7f; /* -xxxxxxx */
        state.mask.data[0x08] = 0x03; /* ------xx */
        state.mask.data[0x09] = 0x1f; /* ---xxxxx */
        state.mask.data[0x0a] = 0x7f; /* -xxxxxxx */
        state.mask.data[0x0b] = 0x1f; /* ---xxxxx */
        state.mask.data[0x0c] = 0x3f; /* --xxxxxx */
        state.mask.data[0x0d] = 0xff; /* xxxxxxxx */
        state.mask.data[0x0e] = 0x3f; /* --xxxxxx */
        state.mask.data[0x0f] = 0xff; /* xxxxxxxx */
        state.mask.data[0x10] = 0x3f; /* --xxxxxx */
        state.mask.data[0x11] = 0xff; /* xxxxxxxx */
    }

    static inline auto reset_regs(State& state) -> void
    {
        state.regs.named.address_register         = 0x00; /*       0 */
        state.regs.named.horizontal_total         = 0x3f; /*      63 */
        state.regs.named.horizontal_displayed     = 0x28; /*      40 */
        state.regs.named.horizontal_sync_position = 0x2e; /*      46 */
        state.regs.named.sync_width               = 0x0e; /* 16 + 14 */
        state.regs.named.vertical_total           = 0x26; /*      38 */
        state.regs.named.vertical_total_adjust    = 0x00; /*       0 */
        state.regs.named.vertical_displayed       = 0x19; /*      25 */
        state.regs.named.vertical_sync_position   = 0x1e; /*      30 */
        state.regs.named.interlace_mode_and_skew  = 0x00; /*       0 */
        state.regs.named.maximum_scanline_address = 0x07; /*       7 */
        state.regs.named.cursor_start             = 0x00; /*       0 */
        state.regs.named.cursor_end               = 0x00; /*       0 */
        state.regs.named.start_address_high       = 0x30; /*    0x30 */
        state.regs.named.start_address_low        = 0x00; /*    0x00 */
        state.regs.named.cursor_high              = 0x00; /*    0x00 */
        state.regs.named.cursor_low               = 0x00; /*    0x00 */
        state.regs.named.light_pen_high           = 0x00; /*    0x00 */
        state.regs.named.light_pen_low            = 0x00; /*    0x00 */
    }

    static inline auto reset_core(State& state) -> void
    {
        state.core.hcc          = 0;
        state.core.vcc          = 0;
        state.core.slc          = 0;
        state.core.vac          = 0;
        state.core.hsc          = 0;
        state.core.vsc          = 0;
        state.core.hsync_signal = 0;
        state.core.vsync_signal = 0;
    }
};

}

// ---------------------------------------------------------------------------
// vdc::Device
// ---------------------------------------------------------------------------

namespace vdc {

Device::Device(const Type type, Interface& interface)
    : _interface(interface)
    , _state()
{
    StateTraits::construct(_state, type);

    reset();
}

Device::~Device()
{
    StateTraits::destruct(_state);
}

auto Device::reset() -> void
{
    StateTraits::reset_caps(_state);
    StateTraits::reset_mask(_state);
    StateTraits::reset_regs(_state);
    StateTraits::reset_core(_state);
}

auto Device::clock() -> void
{
    uint8_t const horizontal_total         = (_state.regs.named.horizontal_total         + 1);
//  uint8_t const horizontal_displayed     = (_state.regs.named.horizontal_displayed     + 0);
    uint8_t const horizontal_sync_position = (_state.regs.named.horizontal_sync_position + 0);
    uint8_t const horizontal_sync_width    = (((_state.regs.named.sync_width >> 0) & 0x0f)  );
    uint8_t const horizontal_sync_signal   = (_state.core.hsync_signal != 0                 );
    uint8_t const vertical_total           = (_state.regs.named.vertical_total           + 1);
    uint8_t const vertical_total_adjust    = (_state.regs.named.vertical_total_adjust    + 0);
//  uint8_t const vertical_displayed       = (_state.regs.named.vertical_displayed       + 0);
    uint8_t const vertical_sync_position   = (_state.regs.named.vertical_sync_position   + 0);
    uint8_t const vertical_sync_width      = (((_state.regs.named.sync_width >> 4) & 0x0f)  );
    uint8_t const vertical_sync_signal     = (_state.core.vsync_signal != 0                 );
    uint8_t       scanline_total           = (_state.regs.named.maximum_scanline_address + 1);
    uint8_t       process_hcc              = 1;
    uint8_t       process_vcc              = 0;
    uint8_t       process_slc              = 0;

    if((_state.core.vcc + 1) == vertical_total) {
        scanline_total += vertical_total_adjust;
    }
    if(process_hcc != 0) {
        if(++_state.core.hcc == horizontal_total) {
            _state.core.hcc = 0;
            process_slc = 1;
        }
        if(horizontal_sync_signal != 0) {
            _state.core.hsc = ((_state.core.hsc + 1) & 0x0f);
            if(_state.core.hsc == horizontal_sync_width) {
                _state.core.hsc = 0;
                _state.core.hsync_signal  = 0;
            }
        }
        else {
            if(_state.core.hcc == horizontal_sync_position) {
                _state.core.hsc = 0;
                _state.core.hsync_signal  = 1;
            }
        }
    }
    if(process_slc != 0) {
        if(++_state.core.slc == scanline_total) {
            _state.core.slc = 0;
            process_vcc = 1;
        }
        if(vertical_sync_signal != 0) {
            _state.core.vsc = ((_state.core.vsc + 1) & 0x0f);
            if(_state.core.vsc == vertical_sync_width) {
                _state.core.vsc = 0;
                _state.core.vsync_signal  = 0;
            }
        }
        else {
            if(_state.core.vcc == vertical_sync_position) {
                _state.core.vsc = 0;
                _state.core.vsync_signal  = 1;
            }
        }
    }
    if(process_vcc != 0) {
        if(++_state.core.vcc == vertical_total) {
            _state.core.vcc = 0;
        }
    }
    /* hsync handler */ {
        if(_state.core.hsync_signal != horizontal_sync_signal) {
            static_cast<void>(_interface.vdc_hsync(*this, _state.core.hsync_signal));
        }
    }
    /* vsync handler */ {
        if(_state.core.vsync_signal != vertical_sync_signal) {
            static_cast<void>(_interface.vdc_vsync(*this, _state.core.vsync_signal));
        }
    }
}

auto Device::get_index(uint8_t index) -> uint8_t 
{
    uint8_t const is_readable   = (_state.caps.addr & StateTraits::REG_READABLE);
    uint8_t const register_mask = (_state.mask.addr);
    uint8_t&      register_data = (_state.regs.named.address_register);

    if(is_readable != 0) {
        index = (register_data &= register_mask);
    }
    return index;
}

auto Device::set_index(uint8_t index) -> uint8_t 
{
    uint8_t const is_writable   = (_state.caps.addr & StateTraits::REG_WRITABLE);
    uint8_t const register_mask = (_state.mask.addr);
    uint8_t&      register_data = (_state.regs.named.address_register);

    if(is_writable != 0) {
        register_data = (index &= register_mask);
    }
    return index;
}

auto Device::get_value(uint8_t value) -> uint8_t 
{
    const uint8_t address_register = _state.regs.named.address_register;

    if(address_register < StateTraits::REGISTER_COUNT) {
        uint8_t const is_readable   = (_state.caps.data[address_register] & StateTraits::REG_READABLE);
        uint8_t const register_mask = (_state.mask.data[address_register]);
        uint8_t&      register_data = (_state.regs.array.data[address_register]);
        if(is_readable != 0) {
            value = (register_data &= register_mask);
        }
    }
    return value;
}

auto Device::set_value(uint8_t value) -> uint8_t 
{
    const uint8_t address_register = _state.regs.named.address_register;

    if(address_register < StateTraits::REGISTER_COUNT) {
        uint8_t const is_writable   = (_state.caps.data[address_register] & StateTraits::REG_WRITABLE);
        uint8_t const register_mask = (_state.mask.data[address_register]);
        uint8_t&      register_data = (_state.regs.array.data[address_register]);
        if(is_writable != 0) {
            register_data = (value &= register_mask);
        }
    }
    return value;
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
