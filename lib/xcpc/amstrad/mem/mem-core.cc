/*
 * mem-core.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "mem-core.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = mem::Type;
    using State     = mem::State;
    using Instance  = mem::Instance;
    using Interface = mem::Interface;
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
        auto reset_rom = [&]() -> void
        {
            for(auto& byte : state.data) {
                byte |= 0;
            }
        };

        auto reset_ram = [&]() -> void
        {
            for(auto& byte : state.data) {
                byte &= 0;
            }
        };

        auto reset_data = [&]() -> void
        {
            switch(state.type) {
                case Type::TYPE_ROM:
                    reset_rom();
                    break;
                case Type::TYPE_RAM:
                    reset_ram();
                    break;
                default:
                    throw std::runtime_error("invalid memory type");
                    break;
            }
        };

        return reset_data();
    }

    static inline auto clock(State& state) -> void
    {
    }
};

}

// ---------------------------------------------------------------------------
// mem::Instance
// ---------------------------------------------------------------------------

namespace mem {

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

auto Instance::load(const std::string& filename, size_t offset) -> void
{
    FILE*            file = nullptr;
    void*            data = _state.data;
    constexpr size_t size = sizeof(_state.data);

    auto file_open = [&]() -> void
    {
        if((file = ::fopen(filename.c_str(), "r")) == nullptr) {
            throw std::runtime_error("fopen() has failed");
        }
    };

    auto file_seek = [&]() -> void
    {
        if(::fseek(file, offset, SEEK_SET) != 0) {
            throw std::runtime_error("fseek() has failed");
        }
    };

    auto file_read = [&]() -> void
    {

        if(::fread(data, 1, size, file) != size) {
            throw std::runtime_error("fread() has failed");
        }
    };

    auto file_close = [&]() -> void
    {
        if(file != nullptr) {
            file = (static_cast<void>(::fclose(file)), nullptr);
        }
    };

    try {
        file_open();
        file_seek();
        file_read();
        file_close();
    }
    catch(...) {
        file_close();
        throw;
    }
}

auto Instance::fetch(uint8_t* data, const size_t size) -> void
{
    if((data != nullptr) && (size == sizeof(_state.data))) {
        static_cast<void>(::memcpy(data, _state.data, size));
    }
    else {
        throw std::runtime_error("fetch() has failed");
    }
}

auto Instance::store(uint8_t* data, const size_t size) -> void
{
    if((data != nullptr) && (size == sizeof(_state.data))) {
        static_cast<void>(::memcpy(_state.data, data, size));
    }
    else {
        throw std::runtime_error("store() has failed");
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
