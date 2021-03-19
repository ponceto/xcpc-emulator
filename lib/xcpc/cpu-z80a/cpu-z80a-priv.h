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

#define _SF   0x80 /* Sign                   */
#define _ZF   0x40 /* Zero                   */
#define _5F   0x20 /* Undocumented           */
#define _HF   0x10 /* HalfCarry / HalfBorrow */
#define _3F   0x08 /* Undocumented           */
#define _PF   0x04 /* Parity                 */
#define _OF   0x04 /* Overflow               */
#define _NF   0x02 /* Add / Sub              */
#define _CF   0x01 /* Carry / Borrow         */

#define _HLT  0x80 /* CPU is HALTed          */
#define _NMI  0x40 /* Pending NMI            */
#define _INT  0x20 /* Pending INT            */
#define _XYZ  0x10 /* Not Used               */
#define _IM2  0x08 /* Interrupt Mode #2      */
#define _IM1  0x04 /* Interrupt Mode #1      */
#define _IFF2 0x02 /* Interrupt Flip-Flop #2 */
#define _IFF1 0x01 /* Interrupt Flip-Flop #1 */

#define THIS self
#define AF_Q self->state.regs.AF.q
#define AF_W self->state.regs.AF.w.l
#define AF_H self->state.regs.AF.b.h
#define AF_L self->state.regs.AF.b.l
#define AF_P self->state.regs.AF.w.h
#define BC_Q self->state.regs.BC.q
#define BC_W self->state.regs.BC.w.l
#define BC_H self->state.regs.BC.b.h
#define BC_L self->state.regs.BC.b.l
#define BC_P self->state.regs.BC.w.h
#define DE_Q self->state.regs.DE.q
#define DE_W self->state.regs.DE.w.l
#define DE_H self->state.regs.DE.b.h
#define DE_L self->state.regs.DE.b.l
#define DE_P self->state.regs.DE.w.h
#define HL_Q self->state.regs.HL.q
#define HL_W self->state.regs.HL.w.l
#define HL_H self->state.regs.HL.b.h
#define HL_L self->state.regs.HL.b.l
#define HL_P self->state.regs.HL.w.h
#define IX_Q self->state.regs.IX.q
#define IX_W self->state.regs.IX.w.l
#define IX_H self->state.regs.IX.b.h
#define IX_L self->state.regs.IX.b.l
#define IY_Q self->state.regs.IY.q
#define IY_W self->state.regs.IY.w.l
#define IY_H self->state.regs.IY.b.h
#define IY_L self->state.regs.IY.b.l
#define SP_Q self->state.regs.SP.q
#define SP_W self->state.regs.SP.w.l
#define SP_H self->state.regs.SP.b.h
#define SP_L self->state.regs.SP.b.l
#define PC_Q self->state.regs.PC.q
#define PC_W self->state.regs.PC.w.l
#define PC_H self->state.regs.PC.b.h
#define PC_L self->state.regs.PC.b.l
#define IR_Q self->state.regs.IR.q
#define IR_W self->state.regs.IR.w.l
#define IR_H self->state.regs.IR.b.h
#define IR_L self->state.regs.IR.b.l
#define IF_Q self->state.regs.IF.q
#define IF_W self->state.regs.IF.w.l
#define IF_H self->state.regs.IF.b.h
#define IF_L self->state.regs.IF.b.l
#define WZ_Q WZ.q
#define WZ_W WZ.w.l
#define WZ_H WZ.b.h
#define WZ_L WZ.b.l
#define M_CYCLES self->state.ctrs.m_cycles
#define T_STATES self->state.ctrs.t_states
#define CCOUNTER self->state.ctrs.ccounter
#define MREQ_RD(addr,data) data=(*self->iface.mreq_rd)(self,addr)
#define MREQ_WR(addr,data) (*self->iface.mreq_wr)(self,addr,data)
#define IORQ_RD(port,data) data=(*iface.self->iorq_rd)(self,port)
#define IORQ_WR(port,data) (*self->iface.iorq_wr)(self,port,data)

#define WrZ80(addr,value)  ((*THIS->iface.mreq_wr)((THIS),(addr),(value)))
#define RdZ80(addr)        ((*THIS->iface.mreq_rd)((THIS),(addr)))
#define OutZ80(addr,value) ((*THIS->iface.iorq_wr)((THIS),(addr),(value)))
#define InZ80(addr)        ((*THIS->iface.iorq_rd)((THIS),(addr)))

#define _ADD 0x00 /* ADD operation */
#define _ADC 0x00 /* ADC operation */
#define _SUB 0x02 /* SUB operation */
#define _SBC 0x02 /* SBC operation */
#define _AND 0x10 /* AND operation */
#define _XOR 0x00 /* XOR operation */
#define _IOR 0x00 /* IOR operation */
#define _CMP 0x02 /* CMP operation */

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

enum CodesED
{
    DB_00,DB_01,DB_02,DB_03,DB_04,DB_05,DB_06,DB_07,
    DB_08,DB_09,DB_0A,DB_0B,DB_0C,DB_0D,DB_0E,DB_0F,
    DB_10,DB_11,DB_12,DB_13,DB_14,DB_15,DB_16,DB_17,
    DB_18,DB_19,DB_1A,DB_1B,DB_1C,DB_1D,DB_1E,DB_1F,
    DB_20,DB_21,DB_22,DB_23,DB_24,DB_25,DB_26,DB_27,
    DB_28,DB_29,DB_2A,DB_2B,DB_2C,DB_2D,DB_2E,DB_2F,
    DB_30,DB_31,DB_32,DB_33,DB_34,DB_35,DB_36,DB_37,
    DB_38,DB_39,DB_3A,DB_3B,DB_3C,DB_3D,DB_3E,DB_3F,
    IN_B_xC,OUT_xC_B,SBC_HL_BC,LD_xWORDe_BC,NEG,RETN,IM_0,LD_I_A,
    IN_C_xC,OUT_xC_C,ADC_HL_BC,LD_BC_xWORDe,DB_4C,RETI,DB_,LD_R_A,
    IN_D_xC,OUT_xC_D,SBC_HL_DE,LD_xWORDe_DE,DB_54,DB_55,IM_1,LD_A_I,
    IN_E_xC,OUT_xC_E,ADC_HL_DE,LD_DE_xWORDe,DB_5C,DB_5D,IM_2,LD_A_R,
    IN_H_xC,OUT_xC_H,SBC_HL_HL,LD_xWORDe_HL,DB_64,DB_65,DB_66,RRD,
    IN_L_xC,OUT_xC_L,ADC_HL_HL,LD_HL_xWORDe,DB_6C,DB_6D,DB_6E,RLD,
    IN_F_xC,DB_71,SBC_HL_SP,LD_xWORDe_SP,DB_74,DB_75,DB_76,DB_77,
    IN_A_xC,OUT_xC_A,ADC_HL_SP,LD_SP_xWORDe,DB_7C,DB_7D,DB_7E,DB_7F,
    DB_80,DB_81,DB_82,DB_83,DB_84,DB_85,DB_86,DB_87,
    DB_88,DB_89,DB_8A,DB_8B,DB_8C,DB_8D,DB_8E,DB_8F,
    DB_90,DB_91,DB_92,DB_93,DB_94,DB_95,DB_96,DB_97,
    DB_98,DB_99,DB_9A,DB_9B,DB_9C,DB_9D,DB_9E,DB_9F,
    LDI,CPI,INI,OUTI,DB_A4,DB_A5,DB_A6,DB_A7,
    LDD,CPD,IND,OUTD,DB_AC,DB_AD,DB_AE,DB_AF,
    LDIR,CPIR,INIR,OTIR,DB_B4,DB_B5,DB_B6,DB_B7,
    LDDR,CPDR,INDR,OTDR,DB_BC,DB_BD,DB_BE,DB_BF,
    DB_C0,DB_C1,DB_C2,DB_C3,DB_C4,DB_C5,DB_C6,DB_C7,
    DB_C8,DB_C9,DB_CA,DB_CB,DB_CC,DB_CD,DB_CE,DB_CF,
    DB_D0,DB_D1,DB_D2,DB_D3,DB_D4,DB_D5,DB_D6,DB_D7,
    DB_D8,DB_D9,DB_DA,DB_DB,DB_DC,DB_DD,DB_DE,DB_DF,
    DB_E0,DB_E1,DB_E2,DB_E3,DB_E4,DB_E5,DB_E6,DB_E7,
    DB_E8,DB_E9,DB_EA,DB_EB,DB_EC,DB_ED,DB_EE,DB_EF,
    DB_F0,DB_F1,DB_F2,DB_F3,DB_F4,DB_F5,DB_F6,DB_F7,
    DB_F8,DB_F9,DB_FA,DB_FB,DB_FC,DB_FD,DB_FE,DB_FF
};

#define S(Fl)        THIS->state.regs.AF.b.l|=Fl
#define R(Fl)        THIS->state.regs.AF.b.l&=~(Fl)
#define FLAGS(Rg,Fl) THIS->state.regs.AF.b.l=Fl|ZSTable[Rg]

#define M_POP(Rg)      \
  THIS->state.regs.Rg.b.l=RdZ80(THIS->state.regs.SP.w.l++);THIS->state.regs.Rg.b.h=RdZ80(THIS->state.regs.SP.w.l++)
#define M_PUSH(Rg)     \
  WrZ80(--THIS->state.regs.SP.w.l,THIS->state.regs.Rg.b.h);WrZ80(--THIS->state.regs.SP.w.l,THIS->state.regs.Rg.b.l)

#define M_CALL         \
  WZ.b.l=RdZ80(THIS->state.regs.PC.w.l++);WZ.b.h=RdZ80(THIS->state.regs.PC.w.l++);         \
  WrZ80(--THIS->state.regs.SP.w.l,THIS->state.regs.PC.b.h);WrZ80(--THIS->state.regs.SP.w.l,THIS->state.regs.PC.b.l); \
  THIS->state.regs.PC.w.l=WZ.w.l

#define M_JP  WZ.b.l=RdZ80(THIS->state.regs.PC.w.l++);WZ.b.h=RdZ80(THIS->state.regs.PC.w.l);THIS->state.regs.PC.w.l=WZ.w.l
#define M_JR  THIS->state.regs.PC.w.l+=(gint8)RdZ80(THIS->state.regs.PC.w.l)+1
#define M_RET THIS->state.regs.PC.b.l=RdZ80(THIS->state.regs.SP.w.l++);THIS->state.regs.PC.b.h=RdZ80(THIS->state.regs.SP.w.l++)

#define M_RST(Ad)      \
  WrZ80(--THIS->state.regs.SP.w.l,THIS->state.regs.PC.b.h);WrZ80(--THIS->state.regs.SP.w.l,THIS->state.regs.PC.b.l);THIS->state.regs.PC.w.l=Ad

#define M_LDWORD(Rg)   \
  THIS->state.regs.Rg.b.l=RdZ80(THIS->state.regs.PC.w.l++);THIS->state.regs.Rg.b.h=RdZ80(THIS->state.regs.PC.w.l++)

#define M_SUB(Rg)      \
  WZ.w.l=THIS->state.regs.AF.b.h-Rg;    \
  THIS->state.regs.AF.b.l=           \
    ((THIS->state.regs.AF.b.h^Rg)&(THIS->state.regs.AF.b.h^WZ.b.l)&0x80? _OF:0)| \
    _NF|-WZ.b.h|ZSTable[WZ.b.l]|                      \
    ((THIS->state.regs.AF.b.h^Rg^WZ.b.l)&_HF);                     \
  THIS->state.regs.AF.b.h=WZ.b.l

#define M_IN(Rg)        \
  Rg=InZ80(THIS->state.regs.BC.w.l);  \
  THIS->state.regs.AF.b.l=PZSTable[Rg]|(THIS->state.regs.AF.b.l&_CF)

#define M_INC(Rg)       \
  Rg++;                 \
  THIS->state.regs.AF.b.l=            \
    (THIS->state.regs.AF.b.l&_CF)|ZSTable[Rg]|           \
    (Rg==0x80? _OF:0)|(Rg&0x0F? 0:_HF)

#define M_DEC(Rg)       \
  Rg--;                 \
  THIS->state.regs.AF.b.l=            \
    _NF|(THIS->state.regs.AF.b.l&_CF)|ZSTable[Rg]| \
    (Rg==0x7F? _OF:0)|((Rg&0x0F)==0x0F? _HF:0)

#define M_ADDW(Rg1,Rg2) \
  WZ.w.l=(THIS->state.regs.Rg1.w.l+THIS->state.regs.Rg2.w.l)&0xFFFF;                        \
  THIS->state.regs.AF.b.l=                                             \
    (THIS->state.regs.AF.b.l&~(_HF|_NF|_CF))|                 \
    ((THIS->state.regs.Rg1.w.l^THIS->state.regs.Rg2.w.l^WZ.w.l)&0x1000? _HF:0)|          \
    (((long)THIS->state.regs.Rg1.w.l+(long)THIS->state.regs.Rg2.w.l)&0x10000? _CF:0); \
  THIS->state.regs.Rg1.w.l=WZ.w.l

#define M_ADCW(Rg)      \
  TMP1=THIS->state.regs.AF.b.l&_CF;WZ.w.l=(THIS->state.regs.HL.w.l+THIS->state.regs.Rg.w.l+TMP1)&0xFFFF;           \
  THIS->state.regs.AF.b.l=                                                   \
    (((long)THIS->state.regs.HL.w.l+(long)THIS->state.regs.Rg.w.l+(long)TMP1)&0x10000? _CF:0)| \
    (~(THIS->state.regs.HL.w.l^THIS->state.regs.Rg.w.l)&(THIS->state.regs.Rg.w.l^WZ.w.l)&0x8000? _OF:0)|       \
    ((THIS->state.regs.HL.w.l^THIS->state.regs.Rg.w.l^WZ.w.l)&0x1000? _HF:0)|                  \
    (WZ.w.l? 0:_ZF)|(WZ.b.h&_SF);                            \
  THIS->state.regs.HL.w.l=WZ.w.l

#define M_SBCW(Rg)      \
  TMP1=THIS->state.regs.AF.b.l&_CF;WZ.w.l=(THIS->state.regs.HL.w.l-THIS->state.regs.Rg.w.l-TMP1)&0xFFFF;           \
  THIS->state.regs.AF.b.l=                                                   \
    _NF|                                                    \
    (((long)THIS->state.regs.HL.w.l-(long)THIS->state.regs.Rg.w.l-(long)TMP1)&0x10000? _CF:0)| \
    ((THIS->state.regs.HL.w.l^THIS->state.regs.Rg.w.l)&(THIS->state.regs.HL.w.l^WZ.w.l)&0x8000? _OF:0)|        \
    ((THIS->state.regs.HL.w.l^THIS->state.regs.Rg.w.l^WZ.w.l)&0x1000? _HF:0)|                  \
    (WZ.w.l? 0:_ZF)|(WZ.b.h&_SF);                            \
  THIS->state.regs.HL.w.l=WZ.w.l

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_CPU_Z80A_PRIV_H__ */
