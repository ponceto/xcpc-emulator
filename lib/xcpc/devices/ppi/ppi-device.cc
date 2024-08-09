/*
 * ppi-device.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include "ppi-device.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = ppi::Type;
    using State     = ppi::State;
    using Device    = ppi::Device;
    using Interface = ppi::Interface;
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

    static inline auto reset(State& state) -> void
    {
        state.port_a = 0x00;
        state.port_b = 0x00;
        state.port_c = 0x00;
        state.ctrl_p = 0x00;
        state.grp_a  = 0x00;
        state.grp_b  = 0x00;
        state.dir_a  = 0x00;
        state.dir_b  = 0x00;
        state.dir_c  = 0x00;
    }

    static inline auto clock(State& state) -> void
    {
    }
};

}

// ---------------------------------------------------------------------------
// ppi::Device
// ---------------------------------------------------------------------------

namespace ppi {

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
    StateTraits::reset(_state);
}

auto Device::clock() -> void
{
    StateTraits::clock(_state);
}

auto Device::rd_port_a(uint8_t value) -> uint8_t
{
    if(_state.dir_a != 0) {
        _state.port_a = _interface.ppi_port_a_rd(*this, value);
    }
    return _state.port_a;
}

auto Device::wr_port_a(uint8_t value) -> uint8_t
{
    _state.port_a = value;

    if(_state.dir_a == 0) {
        value = _interface.ppi_port_a_wr(*this, value);
    }
    return _state.port_a;
}

auto Device::rd_port_b(uint8_t value) -> uint8_t
{
    if(_state.dir_b != 0) {
        _state.port_b = _interface.ppi_port_b_rd(*this, value);
    }
    return _state.port_b;
}

auto Device::wr_port_b(uint8_t value) -> uint8_t
{
    _state.port_b = value;

    if(_state.dir_b == 0) {
        value = _interface.ppi_port_b_wr(*this, value);
    }
    return _state.port_b;
}

auto Device::rd_port_c(uint8_t value) -> uint8_t
{
    if(_state.dir_c != 0) {
        _state.port_c = _interface.ppi_port_c_rd(*this, value);
    }
    return _state.port_c;
}

auto Device::wr_port_c(uint8_t value) -> uint8_t
{
    _state.port_c = value;

    if(_state.dir_c == 0) {
        value = _interface.ppi_port_c_wr(*this, value);
    }
    return _state.port_c;
}

auto Device::rd_ctrl_p(uint8_t value) -> uint8_t
{
    return _state.ctrl_p;
}

auto Device::wr_ctrl_p(uint8_t value) -> uint8_t
{
    if(((_state.ctrl_p = value) & 0x80) != 0) {
        /* I/O mode */ {
            const uint8_t grp_a = ((_state.ctrl_p >> 5) & 0x03);
            const uint8_t grp_b = ((_state.ctrl_p >> 2) & 0x01);
            const uint8_t dir_a = ((_state.ctrl_p >> 4) & 0x01);
            const uint8_t dir_b = ((_state.ctrl_p >> 1) & 0x01);
            const uint8_t dir_c = ((_state.ctrl_p >> 2) & 0x02)
                                | ((_state.ctrl_p >> 0) & 0x01);
            /* process group a */ {
                if(grp_a != _state.grp_a) {
                    if((_state.grp_a = grp_a) != 0) {
                        /* ppi-8255: mode is not supported for group a */
                    }
                }
            }
            /* process group b */ {
                if(grp_b != _state.grp_b) {
                    if((_state.grp_b = grp_b) != 0) {
                        /* ppi-8255: mode is not supported for group b */
                    }
                }
            }
            /* process port a */ {
                if(dir_a != _state.dir_a) {
                    if((_state.dir_a = dir_a) != 0) {
                        value = _interface.ppi_port_a_rd(*this, _state.port_a);
                        _state.port_a = value;
                    }
                    else {
                        value = _interface.ppi_port_a_wr(*this, _state.port_a);
                    }
                }
            }
            /* process port b */ {
                if(dir_b != _state.dir_b) {
                    if((_state.dir_b = dir_b) != 0) {
                        value = _interface.ppi_port_b_rd(*this, _state.port_b);
                        _state.port_b = value;
                    }
                    else {
                        value = _interface.ppi_port_b_wr(*this, _state.port_b);
                    }
                }
            }
            /* process port c */ {
                if(dir_c != _state.dir_c) {
                    if((_state.dir_c = dir_c) != 0) {
                        value = _interface.ppi_port_c_rd(*this, _state.port_c);
                        _state.port_c = value;
                    }
                    else {
                        value = _interface.ppi_port_c_wr(*this, _state.port_c);
                    }
                }
            }
        }
    }
    else {
        /* BSR mode */ {
            const uint8_t bit = ((_state.ctrl_p >> 1) & 0x07);
            const uint8_t val = ((_state.ctrl_p >> 0) & 0x01);
            /* process port c */ {
                if(_state.dir_c == 0) {
                    _state.port_c = ((_state.port_c & ~(0x1 << bit)) | (val << bit));
                    value = _interface.ppi_port_c_wr(*this, _state.port_c);
                }
            }
        }
    }
    return value;
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
