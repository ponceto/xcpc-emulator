/*
 * cpu-z80a-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_CPU_Z80A_PRIV_H__
#define __XCPC_CPU_Z80A_PRIV_H__

#include <xcpc/cpu-z80a/cpu-z80a.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AVOID_UNUSED_WARNING
#define AVOID_UNUSED_WARNING(symbol) (void)(symbol)
#endif

#define _SF   0x80 /* Sign                   */
#define _ZF   0x40 /* Zero                   */
#define _5F   0x20 /* Undocumented           */
#define _HF   0x10 /* HalfCarry / HalfBorrow */
#define _3F   0x08 /* Undocumented           */
#define _PF   0x04 /* Parity                 */
#define _OF   0x04 /* Overflow               */
#define _NF   0x02 /* Add / Sub              */
#define _CF   0x01 /* Carry / Borrow         */

#define _ADD  0x00 /* ADD operation          */
#define _ADC  0x00 /* ADC operation          */
#define _SUB  0x02 /* SUB operation          */
#define _SBC  0x02 /* SBC operation          */
#define _AND  0x10 /* AND operation          */
#define _XOR  0x00 /* XOR operation          */
#define _IOR  0x00 /* IOR operation          */
#define _CMP  0x02 /* CMP operation          */

#define SIGNED_BYTE(value) ((int8_t)(value))

#define vector_00h 0x0000
#define vector_08h 0x0008
#define vector_10h 0x0010
#define vector_18h 0x0018
#define vector_20h 0x0020
#define vector_28h 0x0028
#define vector_30h 0x0030
#define vector_38h 0x0038
#define vector_66h 0x0066

#define SELF self
#define IFACE SELF->iface
#define STATE SELF->state
#define CPU_REGS STATE.regs
#define CPU_CTRS STATE.ctrs
#define AF_Q CPU_REGS.AF.q
#define AF_W CPU_REGS.AF.w.l
#define AF_H CPU_REGS.AF.b.h
#define AF_L CPU_REGS.AF.b.l
#define AF_P CPU_REGS.AF.w.h
#define BC_Q CPU_REGS.BC.q
#define BC_W CPU_REGS.BC.w.l
#define BC_H CPU_REGS.BC.b.h
#define BC_L CPU_REGS.BC.b.l
#define BC_P CPU_REGS.BC.w.h
#define DE_Q CPU_REGS.DE.q
#define DE_W CPU_REGS.DE.w.l
#define DE_H CPU_REGS.DE.b.h
#define DE_L CPU_REGS.DE.b.l
#define DE_P CPU_REGS.DE.w.h
#define HL_Q CPU_REGS.HL.q
#define HL_W CPU_REGS.HL.w.l
#define HL_H CPU_REGS.HL.b.h
#define HL_L CPU_REGS.HL.b.l
#define HL_P CPU_REGS.HL.w.h
#define IX_Q CPU_REGS.IX.q
#define IX_W CPU_REGS.IX.w.l
#define IX_H CPU_REGS.IX.b.h
#define IX_L CPU_REGS.IX.b.l
#define IY_Q CPU_REGS.IY.q
#define IY_W CPU_REGS.IY.w.l
#define IY_H CPU_REGS.IY.b.h
#define IY_L CPU_REGS.IY.b.l
#define SP_Q CPU_REGS.SP.q
#define SP_W CPU_REGS.SP.w.l
#define SP_H CPU_REGS.SP.b.h
#define SP_L CPU_REGS.SP.b.l
#define PC_Q CPU_REGS.PC.q
#define PC_W CPU_REGS.PC.w.l
#define PC_H CPU_REGS.PC.b.h
#define PC_L CPU_REGS.PC.b.l
#define IR_Q CPU_REGS.IR.q
#define IR_W CPU_REGS.IR.w.l
#define IR_H CPU_REGS.IR.b.h
#define IR_L CPU_REGS.IR.b.l
#define IF_Q CPU_REGS.IF.q
#define IF_W CPU_REGS.IF.w.l
#define IF_H CPU_REGS.IF.b.h
#define IF_L CPU_REGS.IF.b.l
#define WZ_Q WZ.q
#define WZ_W WZ.w.l
#define WZ_H WZ.b.h
#define WZ_L WZ.b.l
#define T1_Q T1.q
#define T1_W T1.w.l
#define T1_H T1.b.h
#define T1_L T1.b.l
#define T2_Q T2.q
#define T2_W T2.w.l
#define T2_H T2.b.h
#define T2_L T2.b.l
#define M_CYCLES CPU_CTRS.m_cycles
#define T_STATES CPU_CTRS.t_states
#define T_PERIOD CPU_CTRS.t_period

#define MREQ_M1(addr,data) data=(*IFACE.mreq_m1)(self,addr)
#define MREQ_RD(addr,data) data=(*IFACE.mreq_rd)(self,addr)
#define MREQ_WR(addr,data) (*IFACE.mreq_wr)(self,addr,data)

#define IORQ_M1(port,data) data=(*IFACE.iorq_m1)(self,port)
#define IORQ_RD(port,data) data=(*IFACE.iorq_rd)(self,port)
#define IORQ_WR(port,data) (*IFACE.iorq_wr)(self,port,data)

#define m_fetch_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_cb_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_dd_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_ed_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_fd_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_ddcb_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_fdcb_opcode() \
    do { \
        last_op = (*IFACE.mreq_m1)(self, PC_W++); \
    } while(0)

#define m_fetch_ddcb_offset() \
    do { \
        WZ_W = IX_W + SIGNED_BYTE((*IFACE.mreq_rd)(self, PC_W++)); \
    } while(0)

#define m_fetch_fdcb_offset() \
    do { \
        WZ_W = IY_W + SIGNED_BYTE((*IFACE.mreq_rd)(self, PC_W++)); \
    } while(0)

#define m_illegal() \
    do { \
    } while(0)

#define m_illegal_cb() \
    do { \
    } while(0)

#define m_illegal_dd() \
    do { \
    } while(0)

#define m_illegal_ed() \
    do { \
    } while(0)

#define m_illegal_fd() \
    do { \
    } while(0)

#define m_illegal_ddcb() \
    do { \
    } while(0)

#define m_illegal_fdcb() \
    do { \
    } while(0)

#define m_refresh_dram() \
    do { \
        IR_L = ((IR_L + 0) & 0x80) \
             | ((IR_L + 1) & 0x7f) \
             ; \
    } while(0)

#define m_consume(cycles, states) \
    do { \
        M_CYCLES += cycles; \
        T_STATES += states; \
        T_PERIOD -= states; \
    } while(0)

#define m_pending_nmi() \
    ((IF_W & _NMI) != 0)

#define m_pending_int() \
    ((IF_W & _INT) != 0)

#define m_halted() \
    ((IF_W & _HLT) != 0)

#define m_acknowledge_nmi() \
    do { \
        IF_W &= ~(_HLT | _NMI | _INT | _IFF2); \
    } while(0)

#define m_acknowledge_int() \
    do { \
        IF_W &= ~(_HLT | _NMI | _INT | _IFF2 | _IFF1); \
        IORQ_M1(0x0000, T1_L); \
    } while(0)

#define m_push_r16(r16) \
    do { \
        T1_W = r16; \
        MREQ_WR(--SP_W, T1_H); \
        MREQ_WR(--SP_W, T1_L); \
    } while(0)

#define m_rst_vec16(vector) \
    do { \
        PC_W = vector; \
    } while(0)

#define m_nop() \
    do { \
    } while(0)

#define m_ld_r08_r08(reg1, reg2) \
    do { \
        reg1 = reg2; \
    } while(0)

#define m_neg() \
    do { \
        T1_L = AF_H; \
        AF_H = 0; \
        M_SUB(T1_L); \
    } while(0)

#define m_reti() \
    do { \
        MREQ_RD(SP_W++, PC_L); \
        MREQ_RD(SP_W++, PC_H); \
    } while(0)

#define m_retn() \
    do { \
        if((IF_W & _IFF2) != 0) { \
            IF_W |=  _IFF1; \
        } \
        else { \
            IF_W &= ~_IFF1; \
        } \
        MREQ_RD(SP_W++, PC_L); \
        MREQ_RD(SP_W++, PC_H); \
    } while(0)

#define m_im_0() \
    do { \
        IF_W = (IF_W & ~(_IM1 | _IM2)); \
    } while(0)

#define m_im_1() \
    do { \
        IF_W = (IF_W & ~(_IM1 | _IM2)) | (_IM1); \
    } while(0)

#define m_im_2() \
    do { \
        IF_W = (IF_W & ~(_IM1 | _IM2)) | (_IM2); \
    } while(0)

#define m_im_3() \
    do { \
        IF_W = (IF_W | (_IM1 | _IM2)); \
    } while(0)

#define m_rrd() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        T2_L = (T1_L >> 4) | (AF_H << 4); \
        MREQ_WR(HL_W, T2_L); \
        AF_H = (T1_L & 0x0f) | (AF_H & 0xf0); \
        AF_L = PZSTable[AF_H] | (AF_L & _CF); \
    } while(0)

#define m_rld() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        T2_L = (T1_L << 4) | (AF_H & 0x0f); \
        MREQ_WR(HL_W, T2_L); \
        AF_H = (T1_L >> 4) | (AF_H & 0xf0); \
        AF_L = PZSTable[AF_H] | (AF_L & _CF); \
    } while(0)

#define m_ldir() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        MREQ_WR(DE_W, T1_L); \
        ++HL_W; \
        ++DE_W; \
        --BC_W; \
        AF_L &= ~(_NF | _HF | _PF); \
        if(BC_W != 0) { \
            AF_L |= _NF; \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
    } while(0)

#define m_lddr() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        MREQ_WR(DE_W, T1_L); \
        --HL_W; \
        --DE_W; \
        --BC_W; \
        AF_L &= ~(_NF | _HF | _PF); \
        if(BC_W != 0) { \
            AF_L |= _NF; \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
    } while(0)

#define m_cpir() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        T2_L = AF_H - T1_L; \
        ++HL_W; \
        --BC_W; \
        AF_L = _NF | (AF_L & _CF) | ZSTable[T2_L] | ((AF_H ^ T1_L ^ T2_L) &_HF) | (BC_W ? _PF : 0); \
        if((BC_W != 0) && (T2_L != 0)) { \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
    } while(0)

#define m_cpdr() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        T2_L = AF_H - T1_L; \
        --HL_W; \
        --BC_W; \
        AF_L = _NF | (AF_L & _CF) | ZSTable[T2_L] | ((AF_H ^ T1_L ^ T2_L) &_HF) | (BC_W ? _PF : 0); \
        if((BC_W != 0) && (T2_L != 0)) { \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
    } while(0)

#define m_inir() \
    do { \
        IORQ_RD(BC_W, T1_L); \
        MREQ_WR(HL_W, T1_L); \
        ++HL_W; \
        --BC_H; \
        if(BC_H != 0) { \
            AF_L = _NF; \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
        else { \
            AF_L = _ZF | _NF; \
        } \
    } while(0)

#define m_indr() \
    do { \
        IORQ_RD(BC_W, T1_L); \
        MREQ_WR(HL_W, T1_L); \
        --HL_W; \
        --BC_H; \
        if(BC_H != 0) { \
            AF_L = _NF; \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
        else { \
            AF_L = _ZF | _NF; \
        } \
    } while(0)

#define m_otir() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        ++HL_W; \
        --BC_H; \
        IORQ_WR(BC_W, T1_L); \
        if(BC_H != 0) { \
            AF_L = _NF | (HL_L + T1_L > 255 ? (_CF | _HF) : 0); \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
        else { \
            AF_L = _ZF | _NF | (HL_L + T1_L > 255 ? (_CF | _HF) : 0); \
        } \
    } while(0)

#define m_otdr() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        --HL_W; \
        --BC_H; \
        IORQ_WR(BC_W, T1_L); \
        if(BC_H != 0) { \
            AF_L = _NF | (HL_L + T1_L > 255 ? (_CF | _HF) : 0); \
            PC_W -= 2; \
            M_CYCLES += 1; \
            T_STATES += 5; \
            T_PERIOD -= 5; \
        } \
        else { \
            AF_L = _ZF | _NF | (HL_L + T1_L > 255 ? (_CF | _HF) : 0); \
        } \
    } while(0)

#define m_ldi() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        MREQ_WR(DE_W, T1_L); \
        ++HL_W; \
        ++DE_W; \
        --BC_W; \
        AF_L = (AF_L &~(_NF | _HF | _PF)) | (BC_W ? _PF : 0); \
    } while(0)

#define m_ldd() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        MREQ_WR(DE_W, T1_L); \
        --HL_W; \
        --DE_W; \
        --BC_W; \
        AF_L = (AF_L &~(_NF | _HF | _PF)) | (BC_W ? _PF : 0); \
    } while(0)

#define m_cpi() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        T2_L = AF_H - T1_L; \
        ++HL_W; \
        --BC_W; \
        AF_L = _NF | (AF_L & _CF) | ZSTable[T2_L] | ((AF_H ^ T1_L ^ T2_L) &_HF) | (BC_W ? _PF : 0); \
    } while(0)

#define m_cpd() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        T2_L = AF_H - T1_L; \
        --HL_W; \
        --BC_W; \
        AF_L = _NF | (AF_L & _CF) | ZSTable[T2_L] | ((AF_H ^ T1_L ^ T2_L) &_HF) | (BC_W ? _PF : 0); \
    } while(0)

#define m_ini() \
    do { \
        IORQ_RD(BC_W, T1_L); \
        MREQ_WR(HL_W, T1_L); \
        ++HL_W; \
        --BC_H; \
        AF_L = _NF | (BC_H ? 0 : _ZF); \
    } while(0)

#define m_ind() \
    do { \
        IORQ_RD(BC_W, T1_L); \
        MREQ_WR(HL_W, T1_L); \
        --HL_W; \
        --BC_H; \
        AF_L = _NF | (BC_H ? 0 : _ZF); \
    } while(0)

#define m_outi() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        ++HL_W; \
        --BC_H; \
        IORQ_WR(BC_W, T1_L); \
        AF_L = _NF | (BC_H ? 0 : _ZF) | (HL_L + T1_L > 255 ? (_CF | _HF) : 0); \
    } while(0)

#define m_outd() \
    do { \
        MREQ_RD(HL_W, T1_L); \
        --HL_W; \
        --BC_H; \
        IORQ_WR(BC_W, T1_L); \
        AF_L = _NF | (BC_H ? 0 : _ZF) | (HL_L + T1_L > 255 ? (_CF | _HF) : 0); \
    } while(0)

#define m_ld_a_i() \
    do { \
        AF_H = IR_H; \
        AF_L = (AF_L & _CF) | (IF_W &_IFF2 ? _PF : 0) | ZSTable[AF_H]; \
    } while(0)

#define m_ld_a_r() \
    do { \
        AF_H = IR_L; \
        AF_L = (AF_L & _CF) | (IF_W &_IFF2 ? _PF : 0) | ZSTable[AF_H]; \
    } while(0)

#define m_ld_i_a() \
    do { \
        IR_H = AF_H; \
    } while(0)

#define m_ld_r_a() \
    do { \
        IR_L = AF_H; \
    } while(0)

#define m_in_r08_ind_r08(data, port) \
    do { \
        T1_H = BC_H; \
        T1_L = port; \
        IORQ_RD(T1_W, data); \
        AF_L = PZSTable[data] | (AF_L & _CF); \
    } while(0)

#define m_out_ind_r08_r08(port, data) \
    do { \
        T1_H = BC_H; \
        T1_L = port; \
        IORQ_WR(T1_W, data); \
    } while(0)

#define m_ld_r16_ind_i16(data) \
    do { \
        MREQ_RD(PC_W++, T1_L); \
        MREQ_RD(PC_W++, T1_H); \
        MREQ_RD(T1_W++, T2_L); \
        MREQ_RD(T1_W++, T2_H); \
        data = T2_W; \
    } while(0)

#define m_ld_ind_i16_r16(data) \
    do { \
        T2_W = data; \
        MREQ_RD(PC_W++, T1_L); \
        MREQ_RD(PC_W++, T1_H); \
        MREQ_WR(T1_W++, T2_L); \
        MREQ_WR(T1_W++, T2_H); \
    } while(0)

#define m_adc_r16_r16(reg1, reg2) \
    do { \
        M_ADCW(reg1, reg2); \
    } while(0)

#define m_sbc_r16_r16(reg1, reg2) \
    do { \
        M_SBCW(reg1, reg2); \
    } while(0)

enum Codes
{
    NOP,LD_BC_WORD,LD_xBC_A,INC_BC,INC_B,DEC_B,LD_B_BYTE,RLCA,
    EX_AF_AF,ADD_HL_BC,LD_A_xBC,DEC_BC,INC_C,DEC_C,LD_C_BYTE,RRCA,
    DJNZ,LD_DE_WORD,LD_xDE_A,INC_DE,INC_D,DEC_D,LD_D_BYTE,RLA,
    JR,ADD_HL_DE,LD_A_xDE,DEC_DE,INC_E,DEC_E,LD_E_BYTE,RRA,
    JR_NZ,LD_HL_WORD,LD_xWORD_HL,INC_HL,INC_H,DEC_H,LD_H_BYTE,DAA,
    JR_Z,ADD_HL_HL,LD_HL_xWORD,DEC_HL,INC_L,DEC_L,LD_L_BYTE,CPL,
    JR_NC,LD_SP_WORD,LD_xWORD_A,INC_SP,INC_xHL,DEC_xHL,LD_xHL_BYTE,SCF,
    JR_C,ADD_HL_SP,LD_A_xWORD,DEC_SP,INC_A,DEC_A,LD_A_BYTE,CCF,
    LD_B_B,LD_B_C,LD_B_D,LD_B_E,LD_B_H,LD_B_L,LD_B_xHL,LD_B_A,
    LD_C_B,LD_C_C,LD_C_D,LD_C_E,LD_C_H,LD_C_L,LD_C_xHL,LD_C_A,
    LD_D_B,LD_D_C,LD_D_D,LD_D_E,LD_D_H,LD_D_L,LD_D_xHL,LD_D_A,
    LD_E_B,LD_E_C,LD_E_D,LD_E_E,LD_E_H,LD_E_L,LD_E_xHL,LD_E_A,
    LD_H_B,LD_H_C,LD_H_D,LD_H_E,LD_H_H,LD_H_L,LD_H_xHL,LD_H_A,
    LD_L_B,LD_L_C,LD_L_D,LD_L_E,LD_L_H,LD_L_L,LD_L_xHL,LD_L_A,
    LD_xHL_B,LD_xHL_C,LD_xHL_D,LD_xHL_E,LD_xHL_H,LD_xHL_L,HALT,LD_xHL_A,
    LD_A_B,LD_A_C,LD_A_D,LD_A_E,LD_A_H,LD_A_L,LD_A_xHL,LD_A_A,
    ADD_B,ADD_C,ADD_D,ADD_E,ADD_H,ADD_L,ADD_xHL,ADD_A,
    ADC_B,ADC_C,ADC_D,ADC_E,ADC_H,ADC_L,ADC_xHL,ADC_A,
    SUB_B,SUB_C,SUB_D,SUB_E,SUB_H,SUB_L,SUB_xHL,SUB_A,
    SBC_B,SBC_C,SBC_D,SBC_E,SBC_H,SBC_L,SBC_xHL,SBC_A,
    AND_B,AND_C,AND_D,AND_E,AND_H,AND_L,AND_xHL,AND_A,
    XOR_B,XOR_C,XOR_D,XOR_E,XOR_H,XOR_L,XOR_xHL,XOR_A,
    OR_B,OR_C,OR_D,OR_E,OR_H,OR_L,OR_xHL,OR_A,
    CP_B,CP_C,CP_D,CP_E,CP_H,CP_L,CP_xHL,CP_A,
    RET_NZ,POP_BC,JP_NZ,JP,CALL_NZ,PUSH_BC,ADD_BYTE,RST00,
    RET_Z,RET,JP_Z,PFX_CB,CALL_Z,CALL,ADC_BYTE,RST08,
    RET_NC,POP_DE,JP_NC,OUTA,CALL_NC,PUSH_DE,SUB_BYTE,RST10,
    RET_C,EXX,JP_C,INA,CALL_C,PFX_DD,SBC_BYTE,RST18,
    RET_PO,POP_HL,JP_PO,EX_HL_xSP,CALL_PO,PUSH_HL,AND_BYTE,RST20,
    RET_PE,LD_PC_HL,JP_PE,EX_DE_HL,CALL_PE,PFX_ED,XOR_BYTE,RST28,
    RET_P,POP_AF,JP_P,DI,CALL_P,PUSH_AF,OR_BYTE,RST30,
    RET_M,LD_SP_HL,JP_M,EI,CALL_M,PFX_FD,CP_BYTE,RST38
};

#define WrZ80(addr,value)  ((*IFACE.mreq_wr)((SELF),(addr),(value)))
#define RdZ80(addr)        ((*IFACE.mreq_rd)((SELF),(addr)))
#define OutZ80(addr,value) ((*IFACE.iorq_wr)((SELF),(addr),(value)))
#define InZ80(addr)        ((*IFACE.iorq_rd)((SELF),(addr)))

#define S(Fl)        CPU_REGS.AF.b.l|=Fl
#define R(Fl)        CPU_REGS.AF.b.l&=~(Fl)
#define FLAGS(Rg,Fl) CPU_REGS.AF.b.l=Fl|ZSTable[Rg]

#define M_POP(Rg)      \
  CPU_REGS.Rg.b.l=RdZ80(CPU_REGS.SP.w.l++);CPU_REGS.Rg.b.h=RdZ80(CPU_REGS.SP.w.l++)
#define M_PUSH(Rg)     \
  WrZ80(--CPU_REGS.SP.w.l,CPU_REGS.Rg.b.h);WrZ80(--CPU_REGS.SP.w.l,CPU_REGS.Rg.b.l)

#define M_CALL         \
  WZ.b.l=RdZ80(CPU_REGS.PC.w.l++);WZ.b.h=RdZ80(CPU_REGS.PC.w.l++);         \
  WrZ80(--CPU_REGS.SP.w.l,CPU_REGS.PC.b.h);WrZ80(--CPU_REGS.SP.w.l,CPU_REGS.PC.b.l); \
  CPU_REGS.PC.w.l=WZ.w.l

#define M_JP  WZ.b.l=RdZ80(CPU_REGS.PC.w.l++);WZ.b.h=RdZ80(CPU_REGS.PC.w.l);CPU_REGS.PC.w.l=WZ.w.l
#define M_JR  CPU_REGS.PC.w.l+=(int8_t)RdZ80(CPU_REGS.PC.w.l)+1
#define M_RET CPU_REGS.PC.b.l=RdZ80(CPU_REGS.SP.w.l++);CPU_REGS.PC.b.h=RdZ80(CPU_REGS.SP.w.l++)

#define M_RST(Ad)      \
  WrZ80(--CPU_REGS.SP.w.l,CPU_REGS.PC.b.h);WrZ80(--CPU_REGS.SP.w.l,CPU_REGS.PC.b.l);CPU_REGS.PC.w.l=Ad

#define M_LDWORD(Rg)   \
  CPU_REGS.Rg.b.l=RdZ80(CPU_REGS.PC.w.l++);CPU_REGS.Rg.b.h=RdZ80(CPU_REGS.PC.w.l++)

#define M_SUB(Rg)      \
  WZ.w.l=CPU_REGS.AF.b.h-Rg;    \
  CPU_REGS.AF.b.l=           \
    ((CPU_REGS.AF.b.h^Rg)&(CPU_REGS.AF.b.h^WZ.b.l)&0x80? _OF:0)| \
    _NF|-WZ.b.h|ZSTable[WZ.b.l]|                      \
    ((CPU_REGS.AF.b.h^Rg^WZ.b.l)&_HF);                     \
  CPU_REGS.AF.b.h=WZ.b.l

#define M_IN(Rg)        \
  Rg=InZ80(CPU_REGS.BC.w.l);  \
  CPU_REGS.AF.b.l=PZSTable[Rg]|(CPU_REGS.AF.b.l&_CF)

#define M_INC(Rg)       \
  Rg++;                 \
  CPU_REGS.AF.b.l=            \
    (CPU_REGS.AF.b.l&_CF)|ZSTable[Rg]|           \
    (Rg==0x80? _OF:0)|(Rg&0x0F? 0:_HF)

#define M_DEC(Rg)       \
  Rg--;                 \
  CPU_REGS.AF.b.l=            \
    _NF|(CPU_REGS.AF.b.l&_CF)|ZSTable[Rg]| \
    (Rg==0x7F? _OF:0)|((Rg&0x0F)==0x0F? _HF:0)

#define M_ADDW(Rg1,Rg2) \
  WZ.w.l=(CPU_REGS.Rg1.w.l+CPU_REGS.Rg2.w.l)&0xFFFF;                        \
  CPU_REGS.AF.b.l=                                             \
    (CPU_REGS.AF.b.l&~(_HF|_NF|_CF))|                 \
    ((CPU_REGS.Rg1.w.l^CPU_REGS.Rg2.w.l^WZ.w.l)&0x1000? _HF:0)|          \
    (((long)CPU_REGS.Rg1.w.l+(long)CPU_REGS.Rg2.w.l)&0x10000? _CF:0); \
  CPU_REGS.Rg1.w.l=WZ.w.l

#define M_ADCW(reg1, reg2) \
  T2_W = reg2; \
  T1_L=AF_L&_CF;WZ_W=(reg1+T2_W+T1_L)&0xFFFF; \
  AF_L= \
    (((long)reg1+(long)T2_W+(long)T1_L)&0x10000? _CF:0)| \
    (~(reg1^T2_W)&(T2_W^WZ_W)&0x8000? _OF:0)| \
    ((reg1^T2_W^WZ_W)&0x1000? _HF:0)| \
    (WZ_W? 0:_ZF)|(WZ.b.h&_SF); \
  reg1=WZ_W

#define M_SBCW(reg1, reg2) \
  T2_W = reg2; \
  T1_L=AF_L&_CF;WZ_W=(reg1-T2_W-T1_L)&0xFFFF; \
  AF_L= \
    _NF| \
    (((long)reg1-(long)T2_W-(long)T1_L)&0x10000? _CF:0)| \
    ((reg1^T2_W)&(reg1^WZ_W)&0x8000? _OF:0)| \
    ((reg1^T2_W^WZ_W)&0x1000? _HF:0)| \
    (WZ_W? 0:_ZF)|(WZ.b.h&_SF); \
  reg1=WZ_W

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_PRIV_H__ */
