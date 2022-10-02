/*
 * cpu-z80a-priv.h - Copyright (c) 2001-2022 - Olivier Poncet
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

#define CF 0x01 /* carry / borrow         */
#define NF 0x02 /* add / sub              */
#define VF 0x04 /* overflow               */
#define PF 0x04 /* parity                 */
#define XF 0x08 /* undocumented           */
#define HF 0x10 /* halfcarry / halfborrow */
#define YF 0x20 /* undocumented           */
#define ZF 0x40 /* zero                   */
#define SF 0x80 /* sign                   */

#define ST_IFF1 0x01 /* interrupt flip-flop #1 */
#define ST_IFF2 0x02 /* interrupt flip-flop #2 */
#define ST_IFF  0x03 /* interrupt flip-flop    */
#define ST_IM1  0x04 /* interrupt mode #1      */
#define ST_IM2  0x08 /* interrupt mode #2      */
#define ST_XYZ  0x10 /* not used               */
#define ST_NMI  0x20 /* pending NMI            */
#define ST_INT  0x40 /* pending INT            */
#define ST_HLT  0x80 /* cpu is HALTed          */

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#define VECTOR_00H 0x0000
#define VECTOR_08H 0x0008
#define VECTOR_10H 0x0010
#define VECTOR_18H 0x0018
#define VECTOR_20H 0x0020
#define VECTOR_28H 0x0028
#define VECTOR_30H 0x0030
#define VECTOR_38H 0x0038
#define VECTOR_66H 0x0066

#define SELF self
#define IFACE SELF->iface
#define STATE SELF->state
#define REGS STATE.regs
#define CTRS STATE.ctrs

#define WZ_R REGS.WZ.l.r
#define WZ_P REGS.WZ.w.h
#define WZ_W REGS.WZ.w.l
#define WZ_H REGS.WZ.b.h
#define WZ_L REGS.WZ.b.l

#define AF_R REGS.AF.l.r
#define AF_P REGS.AF.w.h
#define AF_W REGS.AF.w.l
#define AF_X REGS.AF.b.x
#define AF_Y REGS.AF.b.y
#define AF_H REGS.AF.b.h
#define AF_L REGS.AF.b.l

#define BC_R REGS.BC.l.r
#define BC_P REGS.BC.w.h
#define BC_W REGS.BC.w.l
#define BC_X REGS.BC.b.x
#define BC_Y REGS.BC.b.y
#define BC_H REGS.BC.b.h
#define BC_L REGS.BC.b.l

#define DE_R REGS.DE.l.r
#define DE_P REGS.DE.w.h
#define DE_W REGS.DE.w.l
#define DE_X REGS.DE.b.x
#define DE_Y REGS.DE.b.y
#define DE_H REGS.DE.b.h
#define DE_L REGS.DE.b.l

#define HL_R REGS.HL.l.r
#define HL_P REGS.HL.w.h
#define HL_W REGS.HL.w.l
#define HL_X REGS.HL.b.x
#define HL_Y REGS.HL.b.y
#define HL_H REGS.HL.b.h
#define HL_L REGS.HL.b.l

#define IX_R REGS.IX.l.r
#define IX_P REGS.IX.w.h
#define IX_W REGS.IX.w.l
#define IX_H REGS.IX.b.h
#define IX_L REGS.IX.b.l

#define IY_R REGS.IY.l.r
#define IY_P REGS.IY.w.h
#define IY_W REGS.IY.w.l
#define IY_H REGS.IY.b.h
#define IY_L REGS.IY.b.l

#define SP_R REGS.SP.l.r
#define SP_P REGS.SP.w.h
#define SP_W REGS.SP.w.l
#define SP_H REGS.SP.b.h
#define SP_L REGS.SP.b.l

#define PC_R REGS.PC.l.r
#define PC_P REGS.PC.w.h
#define PC_W REGS.PC.w.l
#define PC_H REGS.PC.b.h
#define PC_L REGS.PC.b.l

#define IR_R REGS.IR.l.r
#define IR_P REGS.IR.w.h
#define IR_W REGS.IR.w.l
#define IR_H REGS.IR.b.h
#define IR_L REGS.IR.b.l

#define ST_R REGS.ST.l.r
#define ST_P REGS.ST.w.h
#define ST_W REGS.ST.w.l
#define ST_H REGS.ST.b.h
#define ST_L REGS.ST.b.l

#define M_CYCLES CTRS.m_cycles
#define T_STATES CTRS.t_states
#define I_PERIOD CTRS.i_period

#define OP_R OP.l.r
#define OP_P OP.w.h
#define OP_W OP.w.l
#define OP_H OP.b.h
#define OP_L OP.b.l

#define T0_R T0.l.r
#define T0_P T0.w.h
#define T0_W T0.w.l
#define T0_H T0.b.h
#define T0_L T0.b.l

#define T1_R T1.l.r
#define T1_P T1.w.h
#define T1_W T1.w.l
#define T1_H T1.b.h
#define T1_L T1.b.l

#define T2_R T2.l.r
#define T2_P T2.w.h
#define T2_W T2.w.l
#define T2_H T2.b.h
#define T2_L T2.b.l

#define T3_R T3.l.r
#define T3_P T3.w.h
#define T3_W T3.w.l
#define T3_H T3.b.h
#define T3_L T3.b.l

#define PREV_PC prev_pc

#define MREQ_M1(addr, data) (void)(data=(*IFACE.mreq_m1)((self),(addr),(0x00),(IFACE.user_data)))
#define MREQ_RD(addr, data) (void)(data=(*IFACE.mreq_rd)((self),(addr),(0x00),(IFACE.user_data)))
#define MREQ_WR(addr, data) (void)((*IFACE.mreq_wr)((self),(addr),(data),(IFACE.user_data)))

#define IORQ_M1(port, data) (void)(data=(*IFACE.iorq_m1)((self),(port),(0x00),(IFACE.user_data)))
#define IORQ_RD(port, data) (void)(data=(*IFACE.iorq_rd)((self),(port),(0x00),(IFACE.user_data)))
#define IORQ_WR(port, data) (void)((*IFACE.iorq_wr)((self),(port),(data),(IFACE.user_data)))

#define SIGNED_BYTE(value) ((int8_t)(value))
#define UNSIGNED_BYTE(value) ((uint8_t)(value))
#define SIGNED_WORD(value) ((int16_t)(value))
#define UNSIGNED_WORD(value) ((uint16_t)(value))
#define SIGNED_LONG(value) ((int32_t)(value))
#define UNSIGNED_LONG(value) ((uint32_t)(value))

#define LO_NIBBLE(value) ((value) & 0x0f)
#define HI_NIBBLE(value) ((value) & 0xf0)

#define HAS_IFF1 ((ST_L & ST_IFF1) != 0)
#define HAS_IFF2 ((ST_L & ST_IFF2) != 0)
#define HAS_IM1  ((ST_L & ST_IM1) != 0)
#define HAS_IM2  ((ST_L & ST_IM2) != 0)
#define HAS_XYZ  ((ST_L & ST_XYZ) != 0)
#define HAS_NMI  ((ST_L & ST_NMI) != 0)
#define HAS_INT  ((ST_L & ST_INT) != 0)
#define HAS_HLT  ((ST_L & ST_HLT) != 0)

#define SET_IFF1() do { ST_L |= ST_IFF1; } while(0)
#define SET_IFF2() do { ST_L |= ST_IFF2; } while(0)
#define SET_IM1()  do { ST_L |= ST_IM1; } while(0)
#define SET_IM2()  do { ST_L |= ST_IM2; } while(0)
#define SET_XYZ()  do { ST_L |= ST_XYZ; } while(0)
#define SET_NMI()  do { ST_L |= ST_NMI; } while(0)
#define SET_INT()  do { ST_L |= ST_INT; } while(0)
#define SET_HLT()  do { ST_L |= ST_HLT; } while(0)

#define CLR_IFF1() do { ST_L &= ~ST_IFF1; } while(0)
#define CLR_IFF2() do { ST_L &= ~ST_IFF2; } while(0)
#define CLR_IM1()  do { ST_L &= ~ST_IM1; } while(0)
#define CLR_IM2()  do { ST_L &= ~ST_IM2; } while(0)
#define CLR_XYZ()  do { ST_L &= ~ST_XYZ; } while(0)
#define CLR_NMI()  do { ST_L &= ~ST_NMI; } while(0)
#define CLR_INT()  do { ST_L &= ~ST_INT; } while(0)
#define CLR_HLT()  do { ST_L &= ~ST_HLT; } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_PRIV_H__ */
