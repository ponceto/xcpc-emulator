/*
 * mem-core.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_MEM_CORE_H__
#define __XCPC_MEM_CORE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace mem {

class State;
class Instance;
class Interface;

}

// ---------------------------------------------------------------------------
// mem::Type
// ---------------------------------------------------------------------------

namespace mem {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_ROM     =  0,
    TYPE_RAM     =  1,
};

}

// ---------------------------------------------------------------------------
// mem::State
// ---------------------------------------------------------------------------

namespace mem {

struct State
{
    uint8_t type;
    uint8_t data[16384];
};

}

// ---------------------------------------------------------------------------
// mem::Instance
// ---------------------------------------------------------------------------

namespace mem {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto load(const std::string& filename, size_t offset) -> void;

    auto fetch(uint8_t* data, const size_t size) -> void;

    auto store(uint8_t* data, const size_t size) -> void;

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
// mem::Interface
// ---------------------------------------------------------------------------

namespace mem {

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

#endif /* __XCPC_MEM_CORE_H__ */
