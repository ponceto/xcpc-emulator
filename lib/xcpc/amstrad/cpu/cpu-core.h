/*
 * cpu-core.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_CPU_CORE_H__
#define __XCPC_CPU_CORE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace cpu {

struct State;
class  Instance;
class  Interface;

}

// ---------------------------------------------------------------------------
// cpu::Type
// ---------------------------------------------------------------------------

namespace cpu {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
};

}

// ---------------------------------------------------------------------------
// cpu::Register
// ---------------------------------------------------------------------------

namespace cpu {

union Register
{
    struct /* long */
    {
        uint32_t r;
    } l;
#if defined(MSB_FIRST) && !defined(LSB_FIRST)
    struct /* word */
    {
        uint16_t h;
        uint16_t l;
    } w;
    struct /* byte */
    {
        uint8_t  x;
        uint8_t  y;
        uint8_t  h;
        uint8_t  l;
    } b;
#endif
#if !defined(MSB_FIRST) && defined(LSB_FIRST)
    struct /* word */
    {
        uint16_t l;
        uint16_t h;
    } w;
    struct /* byte */
    {
        uint8_t  l;
        uint8_t  h;
        uint8_t  y;
        uint8_t  x;
    } b;
#endif
};

}

// ---------------------------------------------------------------------------
// cpu::State
// ---------------------------------------------------------------------------

namespace cpu {

struct State
{
    uint8_t  type;
    Register r_af; /* AF & AF'            */
    Register r_bc; /* BC & BC'            */
    Register r_de; /* DE & DE'            */
    Register r_hl; /* HL & HL'            */
    Register r_ix; /* IX Index            */
    Register r_iy; /* IY Index            */
    Register r_sp; /* Stack Pointer       */
    Register r_pc; /* Program Counter     */
    Register r_ir; /* Interrupt & Refresh */
    Register r_st; /* IFF, IM & Control   */
    uint32_t m_cycles;
    uint32_t t_states;
    uint32_t i_period;
};

}

// ---------------------------------------------------------------------------
// cpu::Instance
// ---------------------------------------------------------------------------

namespace cpu {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto pulse_nmi() -> void;

    auto pulse_int() -> void;

    auto operator->() -> State*
    {
        return &_state;
    }

    auto get_af_h () -> uint8_t;
    auto get_af_l () -> uint8_t;
    auto get_bc_h () -> uint8_t;
    auto get_bc_l () -> uint8_t;
    auto get_de_h () -> uint8_t;
    auto get_de_l () -> uint8_t;
    auto get_hl_h () -> uint8_t;
    auto get_hl_l () -> uint8_t;
    auto get_ix_h () -> uint8_t;
    auto get_ix_l () -> uint8_t;
    auto get_iy_h () -> uint8_t;
    auto get_iy_l () -> uint8_t;
    auto get_sp_h () -> uint8_t;
    auto get_sp_l () -> uint8_t;
    auto get_pc_h () -> uint8_t;
    auto get_pc_l () -> uint8_t;
    auto get_ir_h () -> uint8_t;
    auto get_ir_l () -> uint8_t;
    auto get_af_x () -> uint8_t;
    auto get_af_y () -> uint8_t;
    auto get_bc_x () -> uint8_t;
    auto get_bc_y () -> uint8_t;
    auto get_de_x () -> uint8_t;
    auto get_de_y () -> uint8_t;
    auto get_hl_x () -> uint8_t;
    auto get_hl_y () -> uint8_t;
    auto get_im_l () -> uint8_t;
    auto get_iff1 () -> uint8_t;
    auto get_iff2 () -> uint8_t;

    auto set_af_h (uint8_t data) -> void;
    auto set_af_l (uint8_t data) -> void;
    auto set_bc_h (uint8_t data) -> void;
    auto set_bc_l (uint8_t data) -> void;
    auto set_de_h (uint8_t data) -> void;
    auto set_de_l (uint8_t data) -> void;
    auto set_hl_h (uint8_t data) -> void;
    auto set_hl_l (uint8_t data) -> void;
    auto set_ix_h (uint8_t data) -> void;
    auto set_ix_l (uint8_t data) -> void;
    auto set_iy_h (uint8_t data) -> void;
    auto set_iy_l (uint8_t data) -> void;
    auto set_sp_h (uint8_t data) -> void;
    auto set_sp_l (uint8_t data) -> void;
    auto set_pc_h (uint8_t data) -> void;
    auto set_pc_l (uint8_t data) -> void;
    auto set_ir_h (uint8_t data) -> void;
    auto set_ir_l (uint8_t data) -> void;
    auto set_af_x (uint8_t data) -> void;
    auto set_af_y (uint8_t data) -> void;
    auto set_bc_x (uint8_t data) -> void;
    auto set_bc_y (uint8_t data) -> void;
    auto set_de_x (uint8_t data) -> void;
    auto set_de_y (uint8_t data) -> void;
    auto set_hl_x (uint8_t data) -> void;
    auto set_hl_y (uint8_t data) -> void;
    auto set_im_l (uint8_t data) -> void;
    auto set_iff1 (uint8_t data) -> void;
    auto set_iff2 (uint8_t data) -> void;

protected: // protected data
    Interface& _interface;
    State      _state;
};

}

// ---------------------------------------------------------------------------
// cpu::Interface
// ---------------------------------------------------------------------------

namespace cpu {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;

    virtual auto cpu_mreq_m1(Instance& instance, uint16_t addr, uint8_t data) -> uint8_t = 0;

    virtual auto cpu_mreq_rd(Instance& instance, uint16_t addr, uint8_t data) -> uint8_t = 0;

    virtual auto cpu_mreq_wr(Instance& instance, uint16_t addr, uint8_t data) -> uint8_t = 0;

    virtual auto cpu_iorq_m1(Instance& instance, uint16_t port, uint8_t data) -> uint8_t = 0;

    virtual auto cpu_iorq_rd(Instance& instance, uint16_t port, uint8_t data) -> uint8_t = 0;

    virtual auto cpu_iorq_wr(Instance& instance, uint16_t port, uint8_t data) -> uint8_t = 0;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_CPU_CORE_H__ */
