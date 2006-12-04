/*
 * z80cpu.c - Copyright (c) 2001, 2006 Olivier Poncet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "z80cpu.h"
#include "z80cpu_tables.h"

static void gdev_z80cpu_reset(GdevZ80CPU *z80cpu);
static void gdev_z80cpu_clock(GdevZ80CPU *z80cpu);

G_DEFINE_TYPE(GdevZ80CPU, gdev_z80cpu, GDEV_TYPE_DEVICE)

/**
 * GdevZ80CPU::class_init()
 *
 * @param z80cpu_class specifies the GdevZ80CPU class
 */
static void gdev_z80cpu_class_init(GdevZ80CPUClass *z80cpu_class)
{
  GdevDeviceClass *device_class = GDEV_DEVICE_CLASS(z80cpu_class);

  device_class->reset = (GdevDeviceProc) gdev_z80cpu_reset;
  device_class->clock = (GdevDeviceProc) gdev_z80cpu_clock;
}

/**
 * GdevZ80CPU::init()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_init(GdevZ80CPU *z80cpu)
{
  z80cpu->mm_rd = NULL;
  z80cpu->mm_wr = NULL;
  z80cpu->io_rd = NULL;
  z80cpu->io_wr = NULL;
  gdev_z80cpu_reset(z80cpu);
}

/**
 * GdevZ80CPU::reset()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_reset(GdevZ80CPU *z80cpu)
{
  z80cpu->AF.W     = 0x0000;
  z80cpu->BC.W     = 0x0000;
  z80cpu->DE.W     = 0x0000;
  z80cpu->HL.W     = 0x0000;
  z80cpu->AF1.W    = 0x0000;
  z80cpu->BC1.W    = 0x0000;
  z80cpu->DE1.W    = 0x0000;
  z80cpu->HL1.W    = 0x0000;
  z80cpu->IX.W     = 0x0000;
  z80cpu->IY.W     = 0x0000;
  z80cpu->SP.W     = 0x0000;
  z80cpu->PC.W     = 0x0000;
  z80cpu->IR.W     = 0x0000;
  z80cpu->IFF      = 0x00;
  z80cpu->m_cycles = 0;
  z80cpu->t_states = 0;
}

/**
 * GdevZ80CPU::clock()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_clock(GdevZ80CPU *z80cpu)
{
  GdevZ80REG J;
  guint8     I;

decode_op:
  if((z80cpu->IFF & IFF_HALT) != 0) {
    z80cpu->t_states -= 4;
    goto decode_ok;
  }
  z80cpu->IFF &= ~IFF_3;
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= Cycles[I];
  switch(I) {
#include "z80cpu_opcode.h"
    case 0x00: /* NOP        */ OP_NOP(NULL, NULL, 1, 4); break;
    case 0x40: /* LD B,B     */ OP_MOV(BC_H, BC_H, 1, 4); break;
    case 0x41: /* LD B,C     */ OP_MOV(BC_H, BC_L, 1, 4); break;
    case 0x42: /* LD B,D     */ OP_MOV(BC_H, DE_H, 1, 4); break;
    case 0x43: /* LD B,E     */ OP_MOV(BC_H, DE_L, 1, 4); break;
    case 0x44: /* LD B,H     */ OP_MOV(BC_H, HL_H, 1, 4); break;
    case 0x45: /* LD B,L     */ OP_MOV(BC_H, HL_L, 1, 4); break;
    case 0x46: /* LD B,(HL)  */ OP_MRD(HL_W, BC_H, 2, 7); break;
    case 0x47: /* LD B,A     */ OP_MOV(BC_H, AF_H, 1, 4); break;
    case 0x48: /* LD C,B     */ OP_MOV(BC_L, BC_H, 1, 4); break;
    case 0x49: /* LD C,C     */ OP_MOV(BC_L, BC_L, 1, 4); break;
    case 0x4a: /* LD C,D     */ OP_MOV(BC_L, DE_H, 1, 4); break;
    case 0x4b: /* LD C,E     */ OP_MOV(BC_L, DE_L, 1, 4); break;
    case 0x4c: /* LD C,H     */ OP_MOV(BC_L, HL_H, 1, 4); break;
    case 0x4d: /* LD C,L     */ OP_MOV(BC_L, HL_L, 1, 4); break;
    case 0x4e: /* LD C,(HL)  */ OP_MRD(HL_W, BC_L, 2, 7); break;
    case 0x4f: /* LD C,A     */ OP_MOV(BC_L, AF_H, 1, 4); break;
    case 0x50: /* LD D,B     */ OP_MOV(DE_H, BC_H, 1, 4); break;
    case 0x51: /* LD D,C     */ OP_MOV(DE_H, BC_L, 1, 4); break;
    case 0x52: /* LD D,D     */ OP_MOV(DE_H, DE_H, 1, 4); break;
    case 0x53: /* LD D,E     */ OP_MOV(DE_H, DE_L, 1, 4); break;
    case 0x54: /* LD D,H     */ OP_MOV(DE_H, HL_H, 1, 4); break;
    case 0x55: /* LD D,L     */ OP_MOV(DE_H, HL_L, 1, 4); break;
    case 0x56: /* LD D,(HL)  */ OP_MRD(HL_W, DE_H, 2, 7); break;
    case 0x57: /* LD D,A     */ OP_MOV(DE_H, AF_H, 1, 4); break;
    case 0x58: /* LD E,B     */ OP_MOV(DE_L, BC_H, 1, 4); break;
    case 0x59: /* LD E,C     */ OP_MOV(DE_L, BC_L, 1, 4); break;
    case 0x5a: /* LD E,D     */ OP_MOV(DE_L, DE_H, 1, 4); break;
    case 0x5b: /* LD E,E     */ OP_MOV(DE_L, DE_L, 1, 4); break;
    case 0x5c: /* LD E,H     */ OP_MOV(DE_L, HL_H, 1, 4); break;
    case 0x5d: /* LD E,L     */ OP_MOV(DE_L, HL_L, 1, 4); break;
    case 0x5e: /* LD E,(HL)  */ OP_MRD(HL_W, DE_L, 2, 7); break;
    case 0x5f: /* LD E,A     */ OP_MOV(DE_L, AF_H, 1, 4); break;
    case 0x60: /* LD H,B     */ OP_MOV(HL_H, BC_H, 1, 4); break;
    case 0x61: /* LD H,C     */ OP_MOV(HL_H, BC_L, 1, 4); break;
    case 0x62: /* LD H,D     */ OP_MOV(HL_H, DE_H, 1, 4); break;
    case 0x63: /* LD H,E     */ OP_MOV(HL_H, DE_L, 1, 4); break;
    case 0x64: /* LD H,H     */ OP_MOV(HL_H, HL_H, 1, 4); break;
    case 0x65: /* LD H,L     */ OP_MOV(HL_H, HL_L, 1, 4); break;
    case 0x66: /* LD H,(HL)  */ OP_MRD(HL_W, HL_H, 2, 7); break;
    case 0x67: /* LD H,A     */ OP_MOV(HL_H, AF_H, 1, 4); break;
    case 0x68: /* LD L,B     */ OP_MOV(HL_L, BC_H, 1, 4); break;
    case 0x69: /* LD L,C     */ OP_MOV(HL_L, BC_L, 1, 4); break;
    case 0x6a: /* LD L,D     */ OP_MOV(HL_L, DE_H, 1, 4); break;
    case 0x6b: /* LD L,E     */ OP_MOV(HL_L, DE_L, 1, 4); break;
    case 0x6c: /* LD L,H     */ OP_MOV(HL_L, HL_H, 1, 4); break;
    case 0x6d: /* LD L,L     */ OP_MOV(HL_L, HL_L, 1, 4); break;
    case 0x6e: /* LD L,(HL)  */ OP_MRD(HL_W, HL_L, 2, 7); break;
    case 0x6f: /* LD L,A     */ OP_MOV(HL_L, AF_H, 1, 4); break;
    case 0x70: /* LD (HL),B  */ OP_MWR(HL_W, BC_H, 2, 7); break;
    case 0x71: /* LD (HL),C  */ OP_MWR(HL_W, BC_L, 2, 7); break;
    case 0x72: /* LD (HL),D  */ OP_MWR(HL_W, DE_H, 2, 7); break;
    case 0x73: /* LD (HL),E  */ OP_MWR(HL_W, DE_L, 2, 7); break;
    case 0x74: /* LD (HL),H  */ OP_MWR(HL_W, HL_H, 2, 7); break;
    case 0x75: /* LD (HL),L  */ OP_MWR(HL_W, HL_L, 2, 7); break;
    case 0x77: /* LD (HL),A  */ OP_MWR(HL_W, AF_H, 2, 7); break;
    case 0x78: /* LD A,B     */ OP_MOV(AF_H, BC_H, 1, 4); break;
    case 0x79: /* LD A,C     */ OP_MOV(AF_H, BC_L, 1, 4); break;
    case 0x7a: /* LD A,D     */ OP_MOV(AF_H, DE_H, 1, 4); break;
    case 0x7b: /* LD A,E     */ OP_MOV(AF_H, DE_L, 1, 4); break;
    case 0x7c: /* LD A,H     */ OP_MOV(AF_H, HL_H, 1, 4); break;
    case 0x7d: /* LD A,L     */ OP_MOV(AF_H, HL_L, 1, 4); break;
    case 0x7e: /* LD A,(HL)  */ OP_MRD(HL_W, AF_H, 2, 7); break;
    case 0x7f: /* LD A,A     */ OP_MOV(AF_H, AF_H, 1, 4); break;
#ifdef MYIMPL
    case 0x80: /* ADD A,B    */ OP_ADD(AF_H, BC_H, 1, 4); break;
    case 0x81: /* ADD A,C    */ OP_ADD(AF_H, BC_L, 1, 4); break;
    case 0x82: /* ADD A,D    */ OP_ADD(AF_H, DE_H, 1, 4); break;
    case 0x83: /* ADD A,E    */ OP_ADD(AF_H, DE_L, 1, 4); break;
    case 0x84: /* ADD A,H    */ OP_ADD(AF_H, HL_H, 1, 4); break;
    case 0x85: /* ADD A,L    */ OP_ADD(AF_H, HL_L, 1, 4); break;
    case 0x86: /* ADD A,(HL) */ OP_ADD(AF_H, XX_X, 2, 7); break;
    case 0x87: /* ADD A,A    */ OP_ADD(AF_H, AF_H, 1, 4); break;
    case 0x88: /* ADC A,B    */ OP_ADC(AF_H, BC_H, 1, 4); break;
    case 0x89: /* ADC A,C    */ OP_ADC(AF_H, BC_L, 1, 4); break;
    case 0x8a: /* ADC A,D    */ OP_ADC(AF_H, DE_H, 1, 4); break;
    case 0x8b: /* ADC A,E    */ OP_ADC(AF_H, DE_L, 1, 4); break;
    case 0x8c: /* ADC A,H    */ OP_ADC(AF_H, HL_H, 1, 4); break;
    case 0x8d: /* ADC A,L    */ OP_ADC(AF_H, HL_L, 1, 4); break;
    case 0x8e: /* ADC A,(HL) */ OP_ADC(AF_H, XX_X, 2, 7); break;
    case 0x8f: /* ADC A,A    */ OP_ADC(AF_H, AF_H, 1, 4); break;
    case 0x90: /* SUB A,B    */ OP_SUB(AF_H, BC_H, 1, 4); break;
    case 0x91: /* SUB A,C    */ OP_SUB(AF_H, BC_L, 1, 4); break;
    case 0x92: /* SUB A,D    */ OP_SUB(AF_H, DE_H, 1, 4); break;
    case 0x93: /* SUB A,E    */ OP_SUB(AF_H, DE_L, 1, 4); break;
    case 0x94: /* SUB A,H    */ OP_SUB(AF_H, HL_H, 1, 4); break;
    case 0x95: /* SUB A,L    */ OP_SUB(AF_H, HL_L, 1, 4); break;
    case 0x96: /* SUB A,(HL) */ OP_SUB(AF_H, XX_X, 2, 7); break;
    case 0x97: /* SUB A,A    */ OP_SUB(AF_H, AF_H, 1, 4); break;
    case 0x98: /* SBC A,B    */ OP_SBC(AF_H, BC_H, 1, 4); break;
    case 0x99: /* SBC A,C    */ OP_SBC(AF_H, BC_L, 1, 4); break;
    case 0x9a: /* SBC A,D    */ OP_SBC(AF_H, DE_H, 1, 4); break;
    case 0x9b: /* SBC A,E    */ OP_SBC(AF_H, DE_L, 1, 4); break;
    case 0x9c: /* SBC A,H    */ OP_SBC(AF_H, HL_H, 1, 4); break;
    case 0x9d: /* SBC A,L    */ OP_SBC(AF_H, HL_L, 1, 4); break;
    case 0x9e: /* SBC A,(HL) */ OP_SBC(AF_H, XX_X, 2, 7); break;
    case 0x9f: /* SBC A,A    */ OP_SBC(AF_H, AF_H, 1, 4); break;
    case 0xa0: /* AND A,B    */ OP_AND(AF_H, BC_H, 1, 4); break;
    case 0xa1: /* AND A,C    */ OP_AND(AF_H, BC_L, 1, 4); break;
    case 0xa2: /* AND A,D    */ OP_AND(AF_H, DE_H, 1, 4); break;
    case 0xa3: /* AND A,E    */ OP_AND(AF_H, DE_L, 1, 4); break;
    case 0xa4: /* AND A,H    */ OP_AND(AF_H, HL_H, 1, 4); break;
    case 0xa5: /* AND A,L    */ OP_AND(AF_H, HL_L, 1, 4); break;
    case 0xa6: /* AND A,(HL) */ OP_AND(AF_H, XX_X, 2, 7); break;
    case 0xa7: /* AND A,A    */ OP_AND(AF_H, AF_H, 1, 4); break;
    case 0xa8: /* XOR A,B    */ OP_XOR(AF_H, BC_H, 1, 4); break;
    case 0xa9: /* XOR A,C    */ OP_XOR(AF_H, BC_L, 1, 4); break;
    case 0xaa: /* XOR A,D    */ OP_XOR(AF_H, DE_H, 1, 4); break;
    case 0xab: /* XOR A,E    */ OP_XOR(AF_H, DE_L, 1, 4); break;
    case 0xac: /* XOR A,H    */ OP_XOR(AF_H, HL_H, 1, 4); break;
    case 0xad: /* XOR A,L    */ OP_XOR(AF_H, HL_L, 1, 4); break;
    case 0xae: /* XOR A,(HL) */ OP_XOR(AF_H, XX_X, 2, 7); break;
    case 0xaf: /* XOR A,A    */ OP_XOR(AF_H, AF_H, 1, 4); break;
    case 0xb0: /* OR A,B     */ OP_IOR(AF_H, BC_H, 1, 4); break;
    case 0xb1: /* OR A,C     */ OP_IOR(AF_H, BC_L, 1, 4); break;
    case 0xb2: /* OR A,D     */ OP_IOR(AF_H, DE_H, 1, 4); break;
    case 0xb3: /* OR A,E     */ OP_IOR(AF_H, DE_L, 1, 4); break;
    case 0xb4: /* OR A,H     */ OP_IOR(AF_H, HL_H, 1, 4); break;
    case 0xb5: /* OR A,L     */ OP_IOR(AF_H, HL_L, 1, 4); break;
    case 0xb6: /* OR A,(HL)  */ OP_IOR(AF_H, XX_X, 2, 7); break;
    case 0xb7: /* OR A,A     */ OP_IOR(AF_H, AF_H, 1, 4); break;
    case 0xb8: /* CP A,B     */ OP_CMP(AF_H, BC_H, 1, 4); break;
    case 0xb9: /* CP A,C     */ OP_CMP(AF_H, BC_L, 1, 4); break;
    case 0xba: /* CP A,D     */ OP_CMP(AF_H, DE_H, 1, 4); break;
    case 0xbb: /* CP A,E     */ OP_CMP(AF_H, DE_L, 1, 4); break;
    case 0xbc: /* CP A,H     */ OP_CMP(AF_H, HL_H, 1, 4); break;
    case 0xbd: /* CP A,L     */ OP_CMP(AF_H, HL_L, 1, 4); break;
    case 0xbe: /* CP A,(HL)  */ OP_CMP(AF_H, XX_X, 2, 7); break;
    case 0xbf: /* CP A,A     */ OP_CMP(AF_H, AF_H, 1, 4); break;
#endif
    case 0xcb:
      goto decode_cb;
    case 0xdd:
      goto decode_dd;
    case 0xed:
      goto decode_ed;
    case 0xfd:
      goto decode_fd;
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_cb:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= CyclesCB[I];
  switch(I) {
#include "z80cpu_opcode_CB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_dd:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= CyclesXX[I];
  switch(I) {
#include "z80cpu_opcode_DD.h"
    case 0xcb:
      goto decode_dd_cb;
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_dd_cb:
  J.W = z80cpu->IX.W + (gint8) (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= CyclesXXCB[I];
  switch(I) {
#include "z80cpu_opcode_DDCB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_ed:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= CyclesED[I];
  switch(I) {
#include "z80cpu_opcode_ED.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_fd:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= CyclesXX[I];
  switch(I) {
#include "z80cpu_opcode_FD.h"
    case 0xcb:
      goto decode_fd_cb;
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_fd_cb:
  J.W = z80cpu->IY.W + (gint8) (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->t_states -= CyclesXXCB[I];
  switch(I) {
#include "z80cpu_opcode_FDCB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_ko:
  g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "illegal opcode");

decode_ok:
  if(z80cpu->t_states > 0) {
    goto decode_op;
  }
}

/**
 * GdevZ80CPU::new()
 *
 * @return the GdevZ80CPU instance
 */
GdevZ80CPU *gdev_z80cpu_new(void)
{
  return(g_object_new(GDEV_TYPE_Z80CPU, NULL));
}

/**
 * GdevZ80CPU::intr()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 * @param vector specifies the Interrupt Vector
 */
void gdev_z80cpu_intr(GdevZ80CPU *z80cpu, guint16 vector)
{
  if((z80cpu->IFF & IFF_3) != 0) {
    return;
  }
  if((z80cpu->IFF & IFF_1) || (vector == INT_NMI)) {
    /* If HALTed, take CPU off HALT instruction */
    if((z80cpu->IFF & IFF_HALT) != 0) {
      z80cpu->IFF &= ~IFF_HALT;
      z80cpu->PC.W++;
    }
    /* PUSH PC */
    (*z80cpu->mm_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.h);
    (*z80cpu->mm_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.l);
    /* If it is NMI... */
    if(vector == INT_NMI) {
      if(z80cpu->IFF & IFF_1) {
        z80cpu->IFF |=  IFF_2;
      }
      else {
        z80cpu->IFF &= ~IFF_2;
      }
      z80cpu->IFF &= ~(IFF_1 | IFF_3);
      z80cpu->PC.W = 0x0066;
      return;
    }
    /* Further interrupts off */
    z80cpu->IFF &= ~(IFF_1 | IFF_2 | IFF_3);
    /* If in IM2 mode ... */
    if(z80cpu->IFF & IFF_IM2) {
      vector = (z80cpu->IR.W & 0xff00) | (vector & 0x00ff);
      z80cpu->PC.B.l = (*z80cpu->mm_rd)(z80cpu, vector++);
      z80cpu->PC.B.h = (*z80cpu->mm_rd)(z80cpu, vector++);
      return;
    }
    /* If in IM1 mode ... */
    if(z80cpu->IFF & IFF_IM1) {
      z80cpu->PC.W = 0x0038;
      return;
    }
    /* If in IM0 mode ... */
    switch(vector) {
      case INT_RST00: z80cpu->PC.W = 0x0000; break;
      case INT_RST08: z80cpu->PC.W = 0x0008; break;
      case INT_RST10: z80cpu->PC.W = 0x0010; break;
      case INT_RST18: z80cpu->PC.W = 0x0018; break;
      case INT_RST20: z80cpu->PC.W = 0x0020; break;
      case INT_RST28: z80cpu->PC.W = 0x0028; break;
      case INT_RST30: z80cpu->PC.W = 0x0030; break;
      case INT_RST38: z80cpu->PC.W = 0x0038; break;
    }
  }
}

/**
 * GdevZ80CPU::assert_irq()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
void gdev_z80cpu_assert_irq(GdevZ80CPU *z80cpu)
{
  /* if cpu is halted, wake up */
  if((z80cpu->IFF & IFF_HALT) != 0) {
    z80cpu->IFF &= ~IFF_HALT;
    z80cpu->PC.W++;
  }
  z80cpu->IFF |= IFF_IRQ;
}

/**
 * GdevZ80CPU::assert_nmi()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
void gdev_z80cpu_assert_nmi(GdevZ80CPU *z80cpu)
{
  /* if cpu is halted, wake up */
  if((z80cpu->IFF & IFF_HALT) != 0) {
    z80cpu->IFF &= ~IFF_HALT;
    z80cpu->PC.W++;
  }
  z80cpu->IFF |= IFF_NMI;
}
