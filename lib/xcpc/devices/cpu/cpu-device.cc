/*
 * cpu-device.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "cpu-device.h"

// ---------------------------------------------------------------------------
// some useful macros
// ---------------------------------------------------------------------------

#define SELF (*this)
#define IFACE _interface
#define STATE _state
#define STACK stack

#define MREQ_M1 IFACE.cpu_mreq_m1
#define MREQ_RD IFACE.cpu_mreq_rd
#define MREQ_WR IFACE.cpu_mreq_wr

#define IORQ_M1 IFACE.cpu_iorq_m1
#define IORQ_RD IFACE.cpu_iorq_rd
#define IORQ_WR IFACE.cpu_iorq_wr

#define AF_R STATE.r_af.l.r
#define AF_P STATE.r_af.w.h
#define AF_W STATE.r_af.w.l
#define AF_X STATE.r_af.b.x
#define AF_Y STATE.r_af.b.y
#define AF_H STATE.r_af.b.h
#define AF_L STATE.r_af.b.l

#define BC_R STATE.r_bc.l.r
#define BC_P STATE.r_bc.w.h
#define BC_W STATE.r_bc.w.l
#define BC_X STATE.r_bc.b.x
#define BC_Y STATE.r_bc.b.y
#define BC_H STATE.r_bc.b.h
#define BC_L STATE.r_bc.b.l

#define DE_R STATE.r_de.l.r
#define DE_P STATE.r_de.w.h
#define DE_W STATE.r_de.w.l
#define DE_X STATE.r_de.b.x
#define DE_Y STATE.r_de.b.y
#define DE_H STATE.r_de.b.h
#define DE_L STATE.r_de.b.l

#define HL_R STATE.r_hl.l.r
#define HL_P STATE.r_hl.w.h
#define HL_W STATE.r_hl.w.l
#define HL_X STATE.r_hl.b.x
#define HL_Y STATE.r_hl.b.y
#define HL_H STATE.r_hl.b.h
#define HL_L STATE.r_hl.b.l

#define IX_R STATE.r_ix.l.r
#define IX_P STATE.r_ix.w.h
#define IX_W STATE.r_ix.w.l
#define IX_X STATE.r_ix.b.x
#define IX_Y STATE.r_ix.b.y
#define IX_H STATE.r_ix.b.h
#define IX_L STATE.r_ix.b.l

#define IY_R STATE.r_iy.l.r
#define IY_P STATE.r_iy.w.h
#define IY_W STATE.r_iy.w.l
#define IY_X STATE.r_iy.b.x
#define IY_Y STATE.r_iy.b.y
#define IY_H STATE.r_iy.b.h
#define IY_L STATE.r_iy.b.l

#define SP_R STATE.r_sp.l.r
#define SP_P STATE.r_sp.w.h
#define SP_W STATE.r_sp.w.l
#define SP_X STATE.r_sp.b.x
#define SP_Y STATE.r_sp.b.y
#define SP_H STATE.r_sp.b.h
#define SP_L STATE.r_sp.b.l

#define PC_R STATE.r_pc.l.r
#define PC_P STATE.r_pc.w.h
#define PC_W STATE.r_pc.w.l
#define PC_X STATE.r_pc.b.x
#define PC_Y STATE.r_pc.b.y
#define PC_H STATE.r_pc.b.h
#define PC_L STATE.r_pc.b.l

#define IR_R STATE.r_ir.l.r
#define IR_P STATE.r_ir.w.h
#define IR_W STATE.r_ir.w.l
#define IR_X STATE.r_ir.b.x
#define IR_Y STATE.r_ir.b.y
#define IR_H STATE.r_ir.b.h
#define IR_L STATE.r_ir.b.l

#define ST_R STATE.r_st.l.r
#define ST_P STATE.r_st.w.h
#define ST_W STATE.r_st.w.l
#define ST_X STATE.r_st.b.x
#define ST_Y STATE.r_st.b.y
#define ST_H STATE.r_st.b.h
#define ST_L STATE.r_st.b.l

#define OP_R STACK.r_op.l.r
#define OP_P STACK.r_op.w.h
#define OP_W STACK.r_op.w.l
#define OP_X STACK.r_op.b.x
#define OP_Y STACK.r_op.b.y
#define OP_H STACK.r_op.b.h
#define OP_L STACK.r_op.b.l

#define WZ_R STACK.r_wz.l.r
#define WZ_P STACK.r_wz.w.h
#define WZ_W STACK.r_wz.w.l
#define WZ_X STACK.r_wz.b.x
#define WZ_Y STACK.r_wz.b.y
#define WZ_H STACK.r_wz.b.h
#define WZ_L STACK.r_wz.b.l

#define R0_R STACK.r_r0.l.r
#define R0_P STACK.r_r0.w.h
#define R0_W STACK.r_r0.w.l
#define R0_X STACK.r_r0.b.x
#define R0_Y STACK.r_r0.b.y
#define R0_H STACK.r_r0.b.h
#define R0_L STACK.r_r0.b.l

#define R1_R STACK.r_r1.l.r
#define R1_P STACK.r_r1.w.h
#define R1_W STACK.r_r1.w.l
#define R1_X STACK.r_r1.b.x
#define R1_Y STACK.r_r1.b.y
#define R1_H STACK.r_r1.b.h
#define R1_L STACK.r_r1.b.l

#define R2_R STACK.r_r2.l.r
#define R2_P STACK.r_r2.w.h
#define R2_W STACK.r_r2.w.l
#define R2_X STACK.r_r2.b.x
#define R2_Y STACK.r_r2.b.y
#define R2_H STACK.r_r2.b.h
#define R2_L STACK.r_r2.b.l

#define R3_R STACK.r_r3.l.r
#define R3_P STACK.r_r3.w.h
#define R3_W STACK.r_r3.w.l
#define R3_X STACK.r_r3.b.x
#define R3_Y STACK.r_r3.b.y
#define R3_H STACK.r_r3.b.h
#define R3_L STACK.r_r3.b.l

#define M_CYCLES STATE.m_cycles
#define T_STATES STATE.t_states
#define I_PERIOD STATE.i_period

#define SBYTE(value) static_cast<int8_t>(value)
#define UBYTE(value) static_cast<uint8_t>(value)
#define SWORD(value) static_cast<int16_t>(value)
#define UWORD(value) static_cast<uint16_t>(value)
#define SLONG(value) static_cast<int32_t>(value)
#define ULONG(value) static_cast<uint32_t>(value)

#define LO_NIBBLE(value) ((value) & 0x0f)
#define HI_NIBBLE(value) ((value) & 0xf0)

#define HAS_IFF1 ((ST_L & ST_IFF1) != 0)
#define HAS_IFF2 ((ST_L & ST_IFF2) != 0)
#define HAS_IM1  ((ST_L & ST_IM1) != 0)
#define HAS_IM2  ((ST_L & ST_IM2) != 0)
#define HAS_AEI  ((ST_L & ST_AEI) != 0)
#define HAS_NMI  ((ST_L & ST_NMI) != 0)
#define HAS_INT  ((ST_L & ST_INT) != 0)
#define HAS_HLT  ((ST_L & ST_HLT) != 0)

#define SET_IFF1() do { ST_L |= ST_IFF1; } while(0)
#define SET_IFF2() do { ST_L |= ST_IFF2; } while(0)
#define SET_IM1()  do { ST_L |= ST_IM1; } while(0)
#define SET_IM2()  do { ST_L |= ST_IM2; } while(0)
#define SET_AEI()  do { ST_L |= ST_AEI; } while(0)
#define SET_NMI()  do { ST_L |= ST_NMI; } while(0)
#define SET_INT()  do { ST_L |= ST_INT; } while(0)
#define SET_HLT()  do { ST_L |= ST_HLT; } while(0)

#define CLR_IFF1() do { ST_L &= ~ST_IFF1; } while(0)
#define CLR_IFF2() do { ST_L &= ~ST_IFF2; } while(0)
#define CLR_IM1()  do { ST_L &= ~ST_IM1; } while(0)
#define CLR_IM2()  do { ST_L &= ~ST_IM2; } while(0)
#define CLR_AEI()  do { ST_L &= ~ST_AEI; } while(0)
#define CLR_NMI()  do { ST_L &= ~ST_NMI; } while(0)
#define CLR_INT()  do { ST_L &= ~ST_INT; } while(0)
#define CLR_HLT()  do { ST_L &= ~ST_HLT; } while(0)

// ---------------------------------------------------------------------------
// <anonymous>::bits
// ---------------------------------------------------------------------------

namespace {

constexpr uint8_t BIT0 = 0x01;
constexpr uint8_t BIT1 = 0x02;
constexpr uint8_t BIT2 = 0x04;
constexpr uint8_t BIT3 = 0x08;
constexpr uint8_t BIT4 = 0x10;
constexpr uint8_t BIT5 = 0x20;
constexpr uint8_t BIT6 = 0x40;
constexpr uint8_t BIT7 = 0x80;

}

// ---------------------------------------------------------------------------
// <anonymous>::alu_flags
// ---------------------------------------------------------------------------

namespace {

constexpr uint8_t CF = 0x01; /* carry / borrow         */
constexpr uint8_t NF = 0x02; /* add / sub              */
constexpr uint8_t VF = 0x04; /* overflow               */
constexpr uint8_t PF = 0x04; /* parity                 */
constexpr uint8_t XF = 0x08; /* undocumented           */
constexpr uint8_t HF = 0x10; /* halfcarry / halfborrow */
constexpr uint8_t YF = 0x20; /* undocumented           */
constexpr uint8_t ZF = 0x40; /* zero                   */
constexpr uint8_t SF = 0x80; /* sign                   */

}

// ---------------------------------------------------------------------------
// <anonymous>::internal_flags
// ---------------------------------------------------------------------------

namespace {

constexpr uint8_t ST_IFF1 = 0x01; /* interrupt flip-flop #1 */
constexpr uint8_t ST_IFF2 = 0x02; /* interrupt flip-flop #2 */
constexpr uint8_t ST_IM1  = 0x04; /* interrupt mode #1      */
constexpr uint8_t ST_IM2  = 0x08; /* interrupt mode #2      */
constexpr uint8_t ST_AEI  = 0x10; /* after EI               */
constexpr uint8_t ST_NMI  = 0x20; /* pending NMI            */
constexpr uint8_t ST_INT  = 0x40; /* pending INT            */
constexpr uint8_t ST_HLT  = 0x80; /* cpu is HALTed          */

}

// ---------------------------------------------------------------------------
// <anonymous>::restart_vectors
// ---------------------------------------------------------------------------

namespace {

constexpr uint16_t VECTOR_00H = 0x0000;
constexpr uint16_t VECTOR_08H = 0x0008;
constexpr uint16_t VECTOR_10H = 0x0010;
constexpr uint16_t VECTOR_18H = 0x0018;
constexpr uint16_t VECTOR_20H = 0x0020;
constexpr uint16_t VECTOR_28H = 0x0028;
constexpr uint16_t VECTOR_30H = 0x0030;
constexpr uint16_t VECTOR_38H = 0x0038;
constexpr uint16_t VECTOR_66H = 0x0066;

}

// ---------------------------------------------------------------------------
// <anonymous>::PZS - Parity / Zero / Sign
// ---------------------------------------------------------------------------

namespace {

const uint8_t PZS[256] = {
    0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x08, 0x0c, 0x0c, 0x08, 0x0c, 0x08, 0x08, 0x0c,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x0c, 0x08, 0x08, 0x0c, 0x08, 0x0c, 0x0c, 0x08,
    0x20, 0x24, 0x24, 0x20, 0x24, 0x20, 0x20, 0x24, 0x2c, 0x28, 0x28, 0x2c, 0x28, 0x2c, 0x2c, 0x28,
    0x24, 0x20, 0x20, 0x24, 0x20, 0x24, 0x24, 0x20, 0x28, 0x2c, 0x2c, 0x28, 0x2c, 0x28, 0x28, 0x2c,
    0x00, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x0c, 0x08, 0x08, 0x0c, 0x08, 0x0c, 0x0c, 0x08,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x08, 0x0c, 0x0c, 0x08, 0x0c, 0x08, 0x08, 0x0c,
    0x24, 0x20, 0x20, 0x24, 0x20, 0x24, 0x24, 0x20, 0x28, 0x2c, 0x2c, 0x28, 0x2c, 0x28, 0x28, 0x2c,
    0x20, 0x24, 0x24, 0x20, 0x24, 0x20, 0x20, 0x24, 0x2c, 0x28, 0x28, 0x2c, 0x28, 0x2c, 0x2c, 0x28,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x8c, 0x88, 0x88, 0x8c, 0x88, 0x8c, 0x8c, 0x88,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x88, 0x8c, 0x8c, 0x88, 0x8c, 0x88, 0x88, 0x8c,
    0xa4, 0xa0, 0xa0, 0xa4, 0xa0, 0xa4, 0xa4, 0xa0, 0xa8, 0xac, 0xac, 0xa8, 0xac, 0xa8, 0xa8, 0xac,
    0xa0, 0xa4, 0xa4, 0xa0, 0xa4, 0xa0, 0xa0, 0xa4, 0xac, 0xa8, 0xa8, 0xac, 0xa8, 0xac, 0xac, 0xa8,
    0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80, 0x88, 0x8c, 0x8c, 0x88, 0x8c, 0x88, 0x88, 0x8c,
    0x80, 0x84, 0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x8c, 0x88, 0x88, 0x8c, 0x88, 0x8c, 0x8c, 0x88,
    0xa0, 0xa4, 0xa4, 0xa0, 0xa4, 0xa0, 0xa0, 0xa4, 0xac, 0xa8, 0xa8, 0xac, 0xa8, 0xac, 0xac, 0xa8,
    0xa4, 0xa0, 0xa0, 0xa4, 0xa0, 0xa4, 0xa4, 0xa0, 0xa8, 0xac, 0xac, 0xa8, 0xac, 0xa8, 0xa8, 0xac,
};

}

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = cpu::Type;
    using State     = cpu::State;
    using Device    = cpu::Device;
    using Interface = cpu::Interface;
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
        state.r_af.l.r &= 0;
        state.r_bc.l.r &= 0;
        state.r_de.l.r &= 0;
        state.r_hl.l.r &= 0;
        state.r_ix.l.r &= 0;
        state.r_iy.l.r &= 0;
        state.r_sp.l.r &= 0;
        state.r_pc.l.r &= 0;
        state.r_ir.l.r &= 0;
        state.r_st.l.r &= 0;
        state.m_cycles &= 0;
        state.t_states &= 0;
        state.i_period &= 0;
    }
};

}

// ---------------------------------------------------------------------------
// cpu::Device
// ---------------------------------------------------------------------------

namespace cpu {

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
    struct Stack {
        Register r_op;
        Register r_wz;
        Register r_r0;
        Register r_r1;
        Register r_r2;
        Register r_r3;
    } stack;

#include "cpu-microcode.inc"

    if(I_PERIOD == 0) {
        goto prolog;
    }
    else {
        goto epilog;
    }

prolog:
    m_backup_pc();
    if(m_after_ei()) {
        goto check_hlt;
    }
    goto check_nmi;

check_nmi:
    if(m_take_nmi()) {
        m_rst_vec16(VECTOR_66H);
        m_consume(3, 11);
        goto epilog;
    }
    goto check_int;

check_int:
    if(m_take_int()) {
        switch(m_interrupt_mode()) {
            case 0:
                m_iorq_m1(0x0000, OP_L);
                goto execute_opcode;
            case 1:
                m_iorq_m1(0x0000, R1_L);
                m_rst_vec16(VECTOR_38H);
                m_consume(3, 13);
                break;
            case 2:
                m_mreq_wr(--SP_W, PC_H);
                m_mreq_wr(--SP_W, PC_L);
                m_iorq_m1(0x0000, R1_L);
                R1_H = IR_H;
                R1_L = R1_L;
                m_mreq_rd(R1_W++, PC_L);
                m_mreq_rd(R1_W++, PC_H);
                m_consume(5, 19);
                break;
            default:
                break;
        }
        goto epilog;
    }
    goto check_hlt;

check_hlt:
    if(m_halted()) {
        m_refresh_dram();
        m_consume(1, 4);
        goto epilog;
    }
    goto fetch_opcode;

fetch_opcode:
  m_fetch_opcode();
  m_refresh_dram();
  goto execute_opcode;

execute_opcode:
    switch(OP_L) {
#include "cpu-opcodes.inc"
        default:
            {
                constexpr uint32_t m_cycles = (1 - 0);
                constexpr uint32_t t_states = (4 - 0);
                m_illegal();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_cb_opcode:
    m_fetch_cb_opcode();
    m_refresh_dram();
    goto execute_cb_opcode;

execute_cb_opcode:
    switch(OP_L) {
#include "cpu-opcodes-cb.inc"
        default:
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_illegal_cb();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_ed_opcode:
    m_fetch_ed_opcode();
    m_refresh_dram();
    goto execute_ed_opcode;

execute_ed_opcode:
    switch(OP_L) {
#include "cpu-opcodes-ed.inc"
        default:
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_illegal_ed();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_dd_opcode:
    m_fetch_dd_opcode();
    m_refresh_dram();
    goto execute_dd_opcode;

execute_dd_opcode:
    switch(OP_L) {
#include "cpu-opcodes-dd.inc"
        default:
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_illegal_dd();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_fd_opcode:
    m_fetch_fd_opcode();
    m_refresh_dram();
    goto execute_fd_opcode;

execute_fd_opcode:
    switch(OP_L) {
#include "cpu-opcodes-fd.inc"
        default:
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_illegal_fd();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_ddcb_opcode:
    m_fetch_ddcb_opcode();
    goto execute_ddcb_opcode;

execute_ddcb_opcode:
    switch(OP_L) {
#include "cpu-opcodes-ddcb.inc"
        default:
            {
                constexpr uint32_t m_cycles = (4 - 2);
                constexpr uint32_t t_states = (16 - 8);
                m_illegal_ddcb();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

fetch_fdcb_opcode:
    m_fetch_fdcb_opcode();
    goto execute_fdcb_opcode;

execute_fdcb_opcode:
    switch(OP_L) {
#include "cpu-opcodes-fdcb.inc"
        default:
            {
                constexpr uint32_t m_cycles = (4 - 2);
                constexpr uint32_t t_states = (16 - 8);
                m_illegal_fdcb();
                m_consume(m_cycles, t_states);
            }
            break;
    }
    goto epilog;

epilog:
    --I_PERIOD;
}

auto Device::pulse_nmi() -> void
{
    ST_L |= ST_NMI;
}

auto Device::pulse_int() -> void
{
    ST_L |= ST_INT;
}

auto Device::get_af_h() -> uint8_t
{
    return AF_H;
}

auto Device::get_af_l() -> uint8_t
{
    return AF_L;
}

auto Device::get_bc_h() -> uint8_t
{
    return BC_H;
}

auto Device::get_bc_l() -> uint8_t
{
    return BC_L;
}

auto Device::get_de_h() -> uint8_t
{
    return DE_H;
}

auto Device::get_de_l() -> uint8_t
{
    return DE_L;
}

auto Device::get_hl_h() -> uint8_t
{
    return HL_H;
}

auto Device::get_hl_l() -> uint8_t
{
    return HL_L;
}

auto Device::get_ix_h() -> uint8_t
{
    return IX_H;
}

auto Device::get_ix_l() -> uint8_t
{
    return IX_L;
}

auto Device::get_iy_h() -> uint8_t
{
    return IY_H;
}

auto Device::get_iy_l() -> uint8_t
{
    return IY_L;
}

auto Device::get_sp_h() -> uint8_t
{
    return SP_H;
}

auto Device::get_sp_l() -> uint8_t
{
    return SP_L;
}

auto Device::get_pc_h() -> uint8_t
{
    return PC_H;
}

auto Device::get_pc_l() -> uint8_t
{
    return PC_L;
}

auto Device::get_ir_h() -> uint8_t
{
    return IR_H;
}

auto Device::get_ir_l() -> uint8_t
{
    return IR_L;
}

auto Device::get_af_x() -> uint8_t
{
    return AF_X;
}

auto Device::get_af_y() -> uint8_t
{
    return AF_Y;
}

auto Device::get_bc_x() -> uint8_t
{
    return BC_X;
}

auto Device::get_bc_y() -> uint8_t
{
    return BC_Y;
}

auto Device::get_de_x() -> uint8_t
{
    return DE_X;
}

auto Device::get_de_y() -> uint8_t
{
    return DE_Y;
}

auto Device::get_hl_x() -> uint8_t
{
    return HL_X;
}

auto Device::get_hl_y() -> uint8_t
{
    return HL_Y;
}

auto Device::get_im_l() -> uint8_t
{
    const uint8_t im = (HAS_IM1 ? 1 : 0)
                     | (HAS_IM2 ? 2 : 0)
                     ;
    return im;
}

auto Device::get_iff1() -> uint8_t
{
    const uint8_t iff1 = (HAS_IFF1 ? 1 : 0);

    return iff1;
}

auto Device::get_iff2() -> uint8_t
{
    const uint8_t iff2 = (HAS_IFF2 ? 1 : 0);

    return iff2;
}

auto Device::set_af_h(uint8_t data) -> void
{
    AF_H = data;
}

auto Device::set_af_l(uint8_t data) -> void
{
    AF_L = data;
}

auto Device::set_bc_h(uint8_t data) -> void
{
    BC_H = data;
}

auto Device::set_bc_l(uint8_t data) -> void
{
    BC_L = data;
}

auto Device::set_de_h(uint8_t data) -> void
{
    DE_H = data;
}

auto Device::set_de_l(uint8_t data) -> void
{
    DE_L = data;
}

auto Device::set_hl_h(uint8_t data) -> void
{
    HL_H = data;
}

auto Device::set_hl_l(uint8_t data) -> void
{
    HL_L = data;
}

auto Device::set_ix_h(uint8_t data) -> void
{
    IX_H = data;
}

auto Device::set_ix_l(uint8_t data) -> void
{
    IX_L = data;
}

auto Device::set_iy_h(uint8_t data) -> void
{
    IY_H = data;
}

auto Device::set_iy_l(uint8_t data) -> void
{
    IY_L = data;
}

auto Device::set_sp_h(uint8_t data) -> void
{
    SP_H = data;
}

auto Device::set_sp_l(uint8_t data) -> void
{
    SP_L = data;
}

auto Device::set_pc_h(uint8_t data) -> void
{
    PC_H = data;
}

auto Device::set_pc_l(uint8_t data) -> void
{
    PC_L = data;
}

auto Device::set_ir_h(uint8_t data) -> void
{
    IR_H = data;
}

auto Device::set_ir_l(uint8_t data) -> void
{
    IR_L = data;
}

auto Device::set_af_x(uint8_t data) -> void
{
    AF_X = data;
}

auto Device::set_af_y(uint8_t data) -> void
{
    AF_Y = data;
}

auto Device::set_bc_x(uint8_t data) -> void
{
    BC_X = data;
}

auto Device::set_bc_y(uint8_t data) -> void
{
    BC_Y = data;
}

auto Device::set_de_x(uint8_t data) -> void
{
    DE_X = data;
}

auto Device::set_de_y(uint8_t data) -> void
{
    DE_Y = data;
}

auto Device::set_hl_x(uint8_t data) -> void
{
    HL_X = data;
}

auto Device::set_hl_y(uint8_t data) -> void
{
    HL_Y = data;
}

auto Device::set_im_l(uint8_t data) -> void
{
    switch(data) {
        case 1:
            SET_IM1();
            CLR_IM2();
            break;
        case 2:
            CLR_IM1();
            SET_IM2();
            break;
        case 3:
            SET_IM1();
            SET_IM2();
            break;
        default:
            CLR_IM1();
            CLR_IM2();
            break;
    }
}

auto Device::set_iff1(uint8_t iff1) -> void
{
    if(iff1 != 0) {
        SET_IFF1();
    }
    else {
        CLR_IFF1();
    }
}

auto Device::set_iff2(uint8_t iff2) -> void
{
    if(iff2 != 0) {
        SET_IFF2();
    }
    else {
        CLR_IFF2();
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
