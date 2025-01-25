/*
 * fdc-core.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_FDC_CORE_H__
#define __XCPC_FDC_CORE_H__

// ---------------------------------------------------------------------------
// lib765/libdsk forward declarations
// ---------------------------------------------------------------------------

struct fdc_765;
struct floppy_drive;

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace fdc {

class State;
class Instance;
class Interface;

using FdcImpl = fdc_765;
using FddImpl = floppy_drive;

}

// ---------------------------------------------------------------------------
// fdc::Type
// ---------------------------------------------------------------------------

namespace fdc {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
};

}

// ---------------------------------------------------------------------------
// fdc::Drive
// ---------------------------------------------------------------------------

namespace fdc {

enum Drive
{
    FDC_DRIVE0 = 0,
    FDC_DRIVE1 = 1,
    FDC_DRIVE2 = 2,
    FDC_DRIVE3 = 3,
};

}

// ---------------------------------------------------------------------------
// fdc::State
// ---------------------------------------------------------------------------

namespace fdc {

struct State
{
    uint8_t  type;
    FdcImpl* fdc;
    FddImpl* fd0;
    FddImpl* fd1;
    FddImpl* fd2;
    FddImpl* fd3;
};

}

// ---------------------------------------------------------------------------
// fdc::Instance
// ---------------------------------------------------------------------------

namespace fdc {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto attach_drive(const int drive) -> void;

    auto detach_drive(const int drive) -> void;

    auto create_disk(const int drive, const std::string& filename) -> void;

    auto insert_disk(const int drive, const std::string& filename) -> void;

    auto remove_disk(const int drive) -> void;

    auto get_filename(const int drive) -> std::string;

    auto set_motor(uint8_t data) -> uint8_t;

    auto rd_stat(uint8_t data) -> uint8_t;

    auto wr_stat(uint8_t data) -> uint8_t;

    auto rd_data(uint8_t data) -> uint8_t;

    auto wr_data(uint8_t data) -> uint8_t;

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
// fdc::Interface
// ---------------------------------------------------------------------------

namespace fdc {

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

#endif /* __XCPC_FDC_CORE_H__ */
