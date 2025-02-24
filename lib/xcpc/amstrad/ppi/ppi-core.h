/*
 * ppi-core.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_PPI_CORE_H__
#define __XCPC_PPI_CORE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace ppi {

class State;
class Instance;
class Interface;

}

// ---------------------------------------------------------------------------
// ppi::Type
// ---------------------------------------------------------------------------

namespace ppi {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
};

}

// ---------------------------------------------------------------------------
// ppi::State
// ---------------------------------------------------------------------------

namespace ppi {

struct State
{
    uint8_t type;
    uint8_t port_a;
    uint8_t port_b;
    uint8_t port_c;
    uint8_t ctrl_p;
    uint8_t grp_a;
    uint8_t grp_b;
    uint8_t dir_a;
    uint8_t dir_b;
    uint8_t dir_c;
};

}

// ---------------------------------------------------------------------------
// ppi::Instance
// ---------------------------------------------------------------------------

namespace ppi {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto rd_port_a(uint8_t value) -> uint8_t;

    auto wr_port_a(uint8_t value) -> uint8_t;

    auto rd_port_b(uint8_t value) -> uint8_t;

    auto wr_port_b(uint8_t value) -> uint8_t;

    auto rd_port_c(uint8_t value) -> uint8_t;

    auto wr_port_c(uint8_t value) -> uint8_t;

    auto rd_ctrl_p(uint8_t value) -> uint8_t;

    auto wr_ctrl_p(uint8_t value) -> uint8_t;

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
// ppi::Interface
// ---------------------------------------------------------------------------

namespace ppi {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;

    virtual auto ppi_port_a_rd(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto ppi_port_a_wr(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto ppi_port_b_rd(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto ppi_port_b_wr(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto ppi_port_c_rd(Instance& instance, uint8_t data) -> uint8_t = 0;

    virtual auto ppi_port_c_wr(Instance& instance, uint8_t data) -> uint8_t = 0;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_PPI_CORE_H__ */
