/*
 * fdc-device.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include <libdsk/libdsk.h>
#include <lib765/765.h>
#include <xcpc/libxcpc-priv.h>
#include "fdc-device.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = fdc::Type;
    using Drive     = fdc::Drive;
    using State     = fdc::State;
    using Device    = fdc::Device;
    using Interface = fdc::Interface;
    using FdcImpl   = fdc::FdcImpl;
    using FddImpl   = fdc::FddImpl;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::FdcTraits
// ---------------------------------------------------------------------------

namespace {

struct FdcTraits final
    : public BasicTraits
{
    static inline auto create() -> FdcImpl*
    {
        FdcImpl* fdc = ::fdc_new();

        if(fdc == nullptr) {
            throw std::runtime_error("fdc_new() has failed");
        }
        return fdc;
    }

    static inline auto destroy(FdcImpl* fdc) -> FdcImpl*
    {
        if(fdc != nullptr) {
            fdc = (::fdc_destroy(&fdc), nullptr);
        }
        return fdc;
    }

    static inline auto reset(FdcImpl* fdc) -> void
    {
        if(fdc != nullptr) {
            FddImpl* fd0 = ::fdc_getdrive(fdc, Drive::FDC_DRIVE0);
            FddImpl* fd1 = ::fdc_getdrive(fdc, Drive::FDC_DRIVE1);
            FddImpl* fd2 = ::fdc_getdrive(fdc, Drive::FDC_DRIVE2);
            FddImpl* fd3 = ::fdc_getdrive(fdc, Drive::FDC_DRIVE3);
            ::fdc_reset(fdc);
            ::fdc_setdrive(fdc, Drive::FDC_DRIVE0, fd0);
            ::fdc_setdrive(fdc, Drive::FDC_DRIVE1, fd1);
            ::fdc_setdrive(fdc, Drive::FDC_DRIVE2, fd2);
            ::fdc_setdrive(fdc, Drive::FDC_DRIVE3, fd3);
        }
    }

    static inline auto clock(FdcImpl* fdc) -> void
    {
        if(fdc != nullptr) {
            ::fdc_tick(fdc);
        }
    }

    static inline auto set_motor(FdcImpl* fdc, uint8_t motor) -> uint8_t
    {
        if(fdc != nullptr) {
            ::fdc_set_motor(fdc, motor);
        }
        return motor;
    }

    static inline auto rd_stat(FdcImpl* fdc, uint8_t data) -> uint8_t
    {
        if(fdc != nullptr) {
            data = ::fdc_read_ctrl(fdc);
        }
        return data;
    }

    static inline auto wr_stat(FdcImpl* fdc, uint8_t data) -> uint8_t
    {
        if(fdc != nullptr) {
            /* do nothing */
        }
        return data;
    }

    static inline auto rd_data(FdcImpl* fdc, uint8_t data) -> uint8_t
    {
        if(fdc != nullptr) {
            data = ::fdc_read_data(fdc);
        }
        return data;
    }

    static inline auto wr_data(FdcImpl* fdc, uint8_t data) -> uint8_t
    {
        if(fdc != nullptr) {
            ::fdc_write_data(fdc, data);
        }
        return data;
    }

    static inline auto set_drive(FdcImpl* fdc, FddImpl* fdd, const int drive) -> void
    {
        if(fdc != nullptr) {
            ::fdc_setdrive(fdc, drive, fdd);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::FddTraits
// ---------------------------------------------------------------------------

namespace {

struct FddTraits final
    : public BasicTraits
{
    static inline auto create() -> FddImpl*
    {
        FddImpl* fdd = ::fd_newldsk();

        if(fdd == nullptr) {
            throw std::runtime_error("fd_newldsk() has failed");
        }
        return fdd;
    }

    static inline auto destroy(FddImpl* fdd) -> FddImpl*
    {
        if(fdd != nullptr) {
            fdd = (::fd_destroy(&fdd), nullptr);
        }
        return fdd;
    }

    static inline auto reset(FddImpl* fdd) -> void
    {
        if(fdd != nullptr) {
            ::fd_eject(fdd);
            ::fd_reset(fdd);
        }
    }

    static inline auto clock(FddImpl* fdd) -> void
    {
    }

    static inline auto create_disk(FddImpl* fdd, const std::string& filename) -> void
    {
        if(fdd != nullptr) {
            if(filename.size() != 0) {
                ::fdl_create_dsk(fdd, filename.c_str());
            }
            else {
                ::fd_eject(fdd);
            }
        }
    }

    static inline auto insert_disk(FddImpl* fdd, const std::string& filename) -> void
    {
        if(fdd != nullptr) {
            if(filename.size() != 0) {
                ::fdl_setfilename(fdd, filename.c_str());
            }
            else {
                ::fd_eject(fdd);
            }
        }
    }

    static inline auto remove_disk(FddImpl* fdd) -> void
    {
        if(fdd != nullptr) {
            ::fd_eject(fdd);
        }
    }

    static inline auto get_filename(FddImpl* fdd) -> std::string
    {
        std::string filename;

        if(fdd != nullptr) {
            filename = ::fdl_getfilename(fdd);
        }
        return filename;
    }
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
        state.fdc  = FdcTraits::create();
        state.fd0  = FddTraits::create();
        state.fd1  = FddTraits::create();
        state.fd2  = FddTraits::create();
        state.fd3  = FddTraits::create();
    }

    static inline auto destruct(State& state) -> void
    {
        state.fd3  = FddTraits::destroy(state.fd3);
        state.fd2  = FddTraits::destroy(state.fd2);
        state.fd1  = FddTraits::destroy(state.fd1);
        state.fd0  = FddTraits::destroy(state.fd0);
        state.fdc  = FdcTraits::destroy(state.fdc);
        state.type = Type::TYPE_INVALID;
    }

    static inline auto reset(State& state) -> void
    {
        FdcTraits::reset(state.fdc);
        FddTraits::reset(state.fd0);
        FddTraits::reset(state.fd1);
        FddTraits::reset(state.fd2);
        FddTraits::reset(state.fd3);
    }

    static inline auto clock(State& state) -> void
    {
        FdcTraits::clock(state.fdc);
        FddTraits::clock(state.fd0);
        FddTraits::clock(state.fd1);
        FddTraits::clock(state.fd2);
        FddTraits::clock(state.fd3);
    }
};

}

// ---------------------------------------------------------------------------
// fdc::Device
// ---------------------------------------------------------------------------

namespace fdc {

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

auto Device::attach_drive(const int drive) -> void
{
    switch(drive) {
        case Drive::FDC_DRIVE0:
            FdcTraits::set_drive(_state.fdc, _state.fd0, drive);
            break;
        case Drive::FDC_DRIVE1:
            FdcTraits::set_drive(_state.fdc, _state.fd1, drive);
            break;
        case Drive::FDC_DRIVE2:
            FdcTraits::set_drive(_state.fdc, _state.fd2, drive);
            break;
        case Drive::FDC_DRIVE3:
            FdcTraits::set_drive(_state.fdc, _state.fd3, drive);
            break;
        default:
            break;
    }
}

auto Device::detach_drive(const int drive) -> void
{
    switch(drive) {
        case Drive::FDC_DRIVE0:
            FdcTraits::set_drive(_state.fdc, nullptr, drive);
            break;
        case Drive::FDC_DRIVE1:
            FdcTraits::set_drive(_state.fdc, nullptr, drive);
            break;
        case Drive::FDC_DRIVE2:
            FdcTraits::set_drive(_state.fdc, nullptr, drive);
            break;
        case Drive::FDC_DRIVE3:
            FdcTraits::set_drive(_state.fdc, nullptr, drive);
            break;
        default:
            break;
    }
}

auto Device::create_disk(const int drive, const std::string& filename) -> void
{
    switch(drive) {
        case Drive::FDC_DRIVE0:
            FddTraits::create_disk(_state.fd0, filename);
            break;
        case Drive::FDC_DRIVE1:
            FddTraits::create_disk(_state.fd1, filename);
            break;
        case Drive::FDC_DRIVE2:
            FddTraits::create_disk(_state.fd2, filename);
            break;
        case Drive::FDC_DRIVE3:
            FddTraits::create_disk(_state.fd3, filename);
            break;
        default:
            break;
    }
}

auto Device::insert_disk(const int drive, const std::string& filename) -> void
{
    switch(drive) {
        case Drive::FDC_DRIVE0:
            FddTraits::insert_disk(_state.fd0, filename);
            break;
        case Drive::FDC_DRIVE1:
            FddTraits::insert_disk(_state.fd1, filename);
            break;
        case Drive::FDC_DRIVE2:
            FddTraits::insert_disk(_state.fd2, filename);
            break;
        case Drive::FDC_DRIVE3:
            FddTraits::insert_disk(_state.fd3, filename);
            break;
        default:
            break;
    }
}

auto Device::remove_disk(const int drive) -> void
{
    switch(drive) {
        case Drive::FDC_DRIVE0:
            FddTraits::remove_disk(_state.fd0);
            break;
        case Drive::FDC_DRIVE1:
            FddTraits::remove_disk(_state.fd1);
            break;
        case Drive::FDC_DRIVE2:
            FddTraits::remove_disk(_state.fd2);
            break;
        case Drive::FDC_DRIVE3:
            FddTraits::remove_disk(_state.fd3);
            break;
        default:
            break;
    }
}

auto Device::get_filename(const int drive) -> std::string
{
    std::string filename;

    switch(drive) {
        case Drive::FDC_DRIVE0:
            filename = FddTraits::get_filename(_state.fd0);
            break;
        case Drive::FDC_DRIVE1:
            filename = FddTraits::get_filename(_state.fd1);
            break;
        case Drive::FDC_DRIVE2:
            filename = FddTraits::get_filename(_state.fd2);
            break;
        case Drive::FDC_DRIVE3:
            filename = FddTraits::get_filename(_state.fd3);
            break;
        default:
            break;
    }
    return filename;
}

auto Device::set_motor(uint8_t data) -> uint8_t
{
    return FdcTraits::set_motor(_state.fdc, data);
}

auto Device::rd_stat(uint8_t data) -> uint8_t
{
    return FdcTraits::rd_stat(_state.fdc, data);
}

auto Device::wr_stat(uint8_t data) -> uint8_t
{
    return FdcTraits::wr_stat(_state.fdc, data);
}

auto Device::rd_data(uint8_t data) -> uint8_t
{
    return FdcTraits::rd_data(_state.fdc, data);
}

auto Device::wr_data(uint8_t data) -> uint8_t
{
    return FdcTraits::wr_data(_state.fdc, data);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
