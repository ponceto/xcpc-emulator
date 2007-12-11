/*
 * z80cpu.c - Copyright (c) 2001, 2006, 2007 Olivier Poncet
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
  z80cpu->mreq_rd = NULL;
  z80cpu->mreq_wr = NULL;
  z80cpu->iorq_rd = NULL;
  z80cpu->iorq_wr = NULL;
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
  z80cpu->IF.W     = 0x0000;
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
  GdevZ80REG WZ;
  guint8   I, J;
  guint8 opcode;

start:
  if(T_STATES <= 0) {
    return;
  }
  if(z80cpu->IF.W & (IFF_HLT | IFF_NMI | IFF_INT)) {
    if(z80cpu->IF.W & IFF_NMI) {
      z80cpu->IF.W &= ~(IFF_HLT | IFF_NMI | IFF_INT | IFF_2);
      (*z80cpu->mreq_wr)(z80cpu, --SP_W, PC_H);
      (*z80cpu->mreq_wr)(z80cpu, --SP_W, PC_L);
      PC_W = 0x0066; T_STATES -= 11;
      goto start;
    }
    if(z80cpu->IF.W & IFF_INT) {
      z80cpu->IF.W &= ~(IFF_HLT | IFF_NMI | IFF_INT | IFF_2 | IFF_1);
      (*z80cpu->mreq_wr)(z80cpu, --SP_W, PC_H);
      (*z80cpu->mreq_wr)(z80cpu, --SP_W, PC_L);
      PC_W = 0x0038; T_STATES -= 13;
      goto start;
    }
    if(z80cpu->IF.W & IFF_HLT) {
      T_STATES -= 4;
      goto start;
    }
  }

decode_op:
  IR_L = (IR_L & 0x80) | ((IR_L + 1) & 0x7f);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
#include "z80cpu_opcode.h"
    case 0x00: /* NOP          */ NOP_NULL_NULL(NULL, NULL, 1,  4); goto start;
    case 0x27: /* DAA          */ DAA_NULL_NULL(NULL, NULL, 1,  4); goto start;
    case 0x03: /* INC BC       */ INC_RG16_NULL(BC_W, NULL, 1,  6); goto start;
    case 0x13: /* INC DE       */ INC_RG16_NULL(DE_W, NULL, 1,  6); goto start;
    case 0x23: /* INC HL       */ INC_RG16_NULL(HL_W, NULL, 1,  6); goto start;
    case 0x33: /* INC SP       */ INC_RG16_NULL(SP_W, NULL, 1,  6); goto start;
    case 0x0b: /* DEC BC       */ DEC_RG16_NULL(BC_W, NULL, 1,  6); goto start;
    case 0x1b: /* DEC DE       */ DEC_RG16_NULL(DE_W, NULL, 1,  6); goto start;
    case 0x2b: /* DEC HL       */ DEC_RG16_NULL(HL_W, NULL, 1,  6); goto start;
    case 0x3b: /* DEC SP       */ DEC_RG16_NULL(SP_W, NULL, 1,  6); goto start;
    case 0x06: /* LD B,n       */ MOV_RG08_IM08(BC_H, PC_W, 2,  7); goto start;
    case 0x0e: /* LD C,n       */ MOV_RG08_IM08(BC_L, PC_W, 2,  7); goto start;
    case 0x16: /* LD D,n       */ MOV_RG08_IM08(DE_H, PC_W, 2,  7); goto start;
    case 0x1e: /* LD E,n       */ MOV_RG08_IM08(DE_L, PC_W, 2,  7); goto start;
    case 0x26: /* LD H,n       */ MOV_RG08_IM08(HL_H, PC_W, 2,  7); goto start;
    case 0x2e: /* LD L,n       */ MOV_RG08_IM08(HL_L, PC_W, 2,  7); goto start;
    case 0x3e: /* LD A,n       */ MOV_RG08_IM08(AF_H, PC_W, 2,  7); goto start;
    case 0x40: /* LD B,B       */ MOV_RG08_RG08(BC_H, BC_H, 1,  4); goto start;
    case 0x41: /* LD B,C       */ MOV_RG08_RG08(BC_H, BC_L, 1,  4); goto start;
    case 0x42: /* LD B,D       */ MOV_RG08_RG08(BC_H, DE_H, 1,  4); goto start;
    case 0x43: /* LD B,E       */ MOV_RG08_RG08(BC_H, DE_L, 1,  4); goto start;
    case 0x44: /* LD B,H       */ MOV_RG08_RG08(BC_H, HL_H, 1,  4); goto start;
    case 0x45: /* LD B,L       */ MOV_RG08_RG08(BC_H, HL_L, 1,  4); goto start;
    case 0x46: /* LD B,(HL)    */ MOV_RG08_MM08(BC_H, HL_W, 2,  7); goto start;
    case 0x47: /* LD B,A       */ MOV_RG08_RG08(BC_H, AF_H, 1,  4); goto start;
    case 0x48: /* LD C,B       */ MOV_RG08_RG08(BC_L, BC_H, 1,  4); goto start;
    case 0x49: /* LD C,C       */ MOV_RG08_RG08(BC_L, BC_L, 1,  4); goto start;
    case 0x4a: /* LD C,D       */ MOV_RG08_RG08(BC_L, DE_H, 1,  4); goto start;
    case 0x4b: /* LD C,E       */ MOV_RG08_RG08(BC_L, DE_L, 1,  4); goto start;
    case 0x4c: /* LD C,H       */ MOV_RG08_RG08(BC_L, HL_H, 1,  4); goto start;
    case 0x4d: /* LD C,L       */ MOV_RG08_RG08(BC_L, HL_L, 1,  4); goto start;
    case 0x4e: /* LD C,(HL)    */ MOV_RG08_MM08(BC_L, HL_W, 2,  7); goto start;
    case 0x4f: /* LD C,A       */ MOV_RG08_RG08(BC_L, AF_H, 1,  4); goto start;
    case 0x50: /* LD D,B       */ MOV_RG08_RG08(DE_H, BC_H, 1,  4); goto start;
    case 0x51: /* LD D,C       */ MOV_RG08_RG08(DE_H, BC_L, 1,  4); goto start;
    case 0x52: /* LD D,D       */ MOV_RG08_RG08(DE_H, DE_H, 1,  4); goto start;
    case 0x53: /* LD D,E       */ MOV_RG08_RG08(DE_H, DE_L, 1,  4); goto start;
    case 0x54: /* LD D,H       */ MOV_RG08_RG08(DE_H, HL_H, 1,  4); goto start;
    case 0x55: /* LD D,L       */ MOV_RG08_RG08(DE_H, HL_L, 1,  4); goto start;
    case 0x56: /* LD D,(HL)    */ MOV_RG08_MM08(DE_H, HL_W, 2,  7); goto start;
    case 0x57: /* LD D,A       */ MOV_RG08_RG08(DE_H, AF_H, 1,  4); goto start;
    case 0x58: /* LD E,B       */ MOV_RG08_RG08(DE_L, BC_H, 1,  4); goto start;
    case 0x59: /* LD E,C       */ MOV_RG08_RG08(DE_L, BC_L, 1,  4); goto start;
    case 0x5a: /* LD E,D       */ MOV_RG08_RG08(DE_L, DE_H, 1,  4); goto start;
    case 0x5b: /* LD E,E       */ MOV_RG08_RG08(DE_L, DE_L, 1,  4); goto start;
    case 0x5c: /* LD E,H       */ MOV_RG08_RG08(DE_L, HL_H, 1,  4); goto start;
    case 0x5d: /* LD E,L       */ MOV_RG08_RG08(DE_L, HL_L, 1,  4); goto start;
    case 0x5e: /* LD E,(HL)    */ MOV_RG08_MM08(DE_L, HL_W, 2,  7); goto start;
    case 0x5f: /* LD E,A       */ MOV_RG08_RG08(DE_L, AF_H, 1,  4); goto start;
    case 0x60: /* LD H,B       */ MOV_RG08_RG08(HL_H, BC_H, 1,  4); goto start;
    case 0x61: /* LD H,C       */ MOV_RG08_RG08(HL_H, BC_L, 1,  4); goto start;
    case 0x62: /* LD H,D       */ MOV_RG08_RG08(HL_H, DE_H, 1,  4); goto start;
    case 0x63: /* LD H,E       */ MOV_RG08_RG08(HL_H, DE_L, 1,  4); goto start;
    case 0x64: /* LD H,H       */ MOV_RG08_RG08(HL_H, HL_H, 1,  4); goto start;
    case 0x65: /* LD H,L       */ MOV_RG08_RG08(HL_H, HL_L, 1,  4); goto start;
    case 0x66: /* LD H,(HL)    */ MOV_RG08_MM08(HL_H, HL_W, 2,  7); goto start;
    case 0x67: /* LD H,A       */ MOV_RG08_RG08(HL_H, AF_H, 1,  4); goto start;
    case 0x68: /* LD L,B       */ MOV_RG08_RG08(HL_L, BC_H, 1,  4); goto start;
    case 0x69: /* LD L,C       */ MOV_RG08_RG08(HL_L, BC_L, 1,  4); goto start;
    case 0x6a: /* LD L,D       */ MOV_RG08_RG08(HL_L, DE_H, 1,  4); goto start;
    case 0x6b: /* LD L,E       */ MOV_RG08_RG08(HL_L, DE_L, 1,  4); goto start;
    case 0x6c: /* LD L,H       */ MOV_RG08_RG08(HL_L, HL_H, 1,  4); goto start;
    case 0x6d: /* LD L,L       */ MOV_RG08_RG08(HL_L, HL_L, 1,  4); goto start;
    case 0x6e: /* LD L,(HL)    */ MOV_RG08_MM08(HL_L, HL_W, 2,  7); goto start;
    case 0x6f: /* LD L,A       */ MOV_RG08_RG08(HL_L, AF_H, 1,  4); goto start;
    case 0x70: /* LD (HL),B    */ MOV_MM08_RG08(HL_W, BC_H, 2,  7); goto start;
    case 0x71: /* LD (HL),C    */ MOV_MM08_RG08(HL_W, BC_L, 2,  7); goto start;
    case 0x72: /* LD (HL),D    */ MOV_MM08_RG08(HL_W, DE_H, 2,  7); goto start;
    case 0x73: /* LD (HL),E    */ MOV_MM08_RG08(HL_W, DE_L, 2,  7); goto start;
    case 0x74: /* LD (HL),H    */ MOV_MM08_RG08(HL_W, HL_H, 2,  7); goto start;
    case 0x75: /* LD (HL),L    */ MOV_MM08_RG08(HL_W, HL_L, 2,  7); goto start;
    case 0x76: /* HALT         */ HLT_NULL_NULL(NULL, NULL, 1,  4); goto start;
    case 0x77: /* LD (HL),A    */ MOV_MM08_RG08(HL_W, AF_H, 2,  7); goto start;
    case 0x78: /* LD A,B       */ MOV_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0x79: /* LD A,C       */ MOV_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0x7a: /* LD A,D       */ MOV_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0x7b: /* LD A,E       */ MOV_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0x7c: /* LD A,H       */ MOV_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0x7d: /* LD A,L       */ MOV_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0x7e: /* LD A,(HL)    */ MOV_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0x7f: /* LD A,A       */ MOV_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0x80: /* ADD A,B      */ ADD_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0x81: /* ADD A,C      */ ADD_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0x82: /* ADD A,D      */ ADD_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0x83: /* ADD A,E      */ ADD_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0x84: /* ADD A,H      */ ADD_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0x85: /* ADD A,L      */ ADD_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0x86: /* ADD A,(HL)   */ ADD_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0x87: /* ADD A,A      */ ADD_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0x88: /* ADC A,B      */ ADC_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0x89: /* ADC A,C      */ ADC_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0x8a: /* ADC A,D      */ ADC_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0x8b: /* ADC A,E      */ ADC_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0x8c: /* ADC A,H      */ ADC_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0x8d: /* ADC A,L      */ ADC_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0x8e: /* ADC A,(HL)   */ ADC_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0x8f: /* ADC A,A      */ ADC_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0x90: /* SUB A,B      */ SUB_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0x91: /* SUB A,C      */ SUB_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0x92: /* SUB A,D      */ SUB_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0x93: /* SUB A,E      */ SUB_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0x94: /* SUB A,H      */ SUB_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0x95: /* SUB A,L      */ SUB_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0x96: /* SUB A,(HL)   */ SUB_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0x97: /* SUB A,A      */ SUB_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0x98: /* SBC A,B      */ SBC_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0x99: /* SBC A,C      */ SBC_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0x9a: /* SBC A,D      */ SBC_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0x9b: /* SBC A,E      */ SBC_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0x9c: /* SBC A,H      */ SBC_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0x9d: /* SBC A,L      */ SBC_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0x9e: /* SBC A,(HL)   */ SBC_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0x9f: /* SBC A,A      */ SBC_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0xa0: /* AND A,B      */ AND_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0xa1: /* AND A,C      */ AND_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0xa2: /* AND A,D      */ AND_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0xa3: /* AND A,E      */ AND_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0xa4: /* AND A,H      */ AND_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0xa5: /* AND A,L      */ AND_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0xa6: /* AND A,(HL)   */ AND_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0xa7: /* AND A,A      */ AND_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0xa8: /* XOR A,B      */ XOR_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0xa9: /* XOR A,C      */ XOR_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0xaa: /* XOR A,D      */ XOR_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0xab: /* XOR A,E      */ XOR_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0xac: /* XOR A,H      */ XOR_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0xad: /* XOR A,L      */ XOR_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0xae: /* XOR A,(HL)   */ XOR_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0xaf: /* XOR A,A      */ XOR_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0xb0: /* OR A,B       */ IOR_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0xb1: /* OR A,C       */ IOR_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0xb2: /* OR A,D       */ IOR_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0xb3: /* OR A,E       */ IOR_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0xb4: /* OR A,H       */ IOR_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0xb5: /* OR A,L       */ IOR_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0xb6: /* OR A,(HL)    */ IOR_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0xb7: /* OR A,A       */ IOR_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0xb8: /* CP A,B       */ CMP_RG08_RG08(AF_H, BC_H, 1,  4); goto start;
    case 0xb9: /* CP A,C       */ CMP_RG08_RG08(AF_H, BC_L, 1,  4); goto start;
    case 0xba: /* CP A,D       */ CMP_RG08_RG08(AF_H, DE_H, 1,  4); goto start;
    case 0xbb: /* CP A,E       */ CMP_RG08_RG08(AF_H, DE_L, 1,  4); goto start;
    case 0xbc: /* CP A,H       */ CMP_RG08_RG08(AF_H, HL_H, 1,  4); goto start;
    case 0xbd: /* CP A,L       */ CMP_RG08_RG08(AF_H, HL_L, 1,  4); goto start;
    case 0xbe: /* CP A,(HL)    */ CMP_RG08_MM08(AF_H, HL_W, 2,  7); goto start;
    case 0xbf: /* CP A,A       */ CMP_RG08_RG08(AF_H, AF_H, 1,  4); goto start;
    case 0xcb:
      goto decode_cb;
    case 0xdd:
      goto decode_dd;
    case 0xed:
      goto decode_ed;
    case 0xfd:
      goto decode_fd;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: %02X: illegal opcode", (PC_W - 1), opcode);
      goto start;
  }
  T_STATES -= Cycles[opcode];
  goto start;

decode_cb:
  IR_L = (IR_L & 0x80) | ((IR_L + 1) & 0x7f);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
    case 0x00: /* RLC B        */ RLC_RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x01: /* RLC C        */ RLC_RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x02: /* RLC D        */ RLC_RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x03: /* RLC E        */ RLC_RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x04: /* RLC H        */ RLC_RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x05: /* RLC L        */ RLC_RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x06: /* RLC (HL)     */ RLC_MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x07: /* RLC A        */ RLC_RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x08: /* RRC B        */ RRC_RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x09: /* RRC C        */ RRC_RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x0a: /* RRC D        */ RRC_RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x0b: /* RRC E        */ RRC_RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x0c: /* RRC H        */ RRC_RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x0d: /* RRC L        */ RRC_RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x0e: /* RRC (HL)     */ RRC_MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x0f: /* RRC A        */ RRC_RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x10: /* RL  B        */ RL__RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x11: /* RL  C        */ RL__RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x12: /* RL  D        */ RL__RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x13: /* RL  E        */ RL__RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x14: /* RL  H        */ RL__RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x15: /* RL  L        */ RL__RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x16: /* RL  (HL)     */ RL__MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x17: /* RL  A        */ RL__RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x18: /* RR  B        */ RR__RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x19: /* RR  C        */ RR__RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x1a: /* RR  D        */ RR__RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x1b: /* RR  E        */ RR__RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x1c: /* RR  H        */ RR__RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x1d: /* RR  L        */ RR__RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x1e: /* RR  (HL)     */ RR__MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x1f: /* RR  A        */ RR__RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x20: /* SLA B        */ SLA_RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x21: /* SLA C        */ SLA_RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x22: /* SLA D        */ SLA_RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x23: /* SLA E        */ SLA_RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x24: /* SLA H        */ SLA_RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x25: /* SLA L        */ SLA_RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x26: /* SLA (HL)     */ SLA_MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x27: /* SLA A        */ SLA_RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x28: /* SRA B        */ SRA_RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x29: /* SRA C        */ SRA_RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x2a: /* SRA D        */ SRA_RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x2b: /* SRA E        */ SRA_RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x2c: /* SRA H        */ SRA_RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x2d: /* SRA L        */ SRA_RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x2e: /* SRA (HL)     */ SRA_MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x2f: /* SRA A        */ SRA_RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x30: /* SLL B        */ SLL_RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x31: /* SLL C        */ SLL_RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x32: /* SLL D        */ SLL_RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x33: /* SLL E        */ SLL_RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x34: /* SLL H        */ SLL_RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x35: /* SLL L        */ SLL_RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x36: /* SLL (HL)     */ SLL_MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x37: /* SLL A        */ SLL_RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x38: /* SRL B        */ SRL_RG08_NULL(BC_H, NULL, 2,  8); goto start;
    case 0x39: /* SRL C        */ SRL_RG08_NULL(BC_L, NULL, 2,  8); goto start;
    case 0x3a: /* SRL D        */ SRL_RG08_NULL(DE_H, NULL, 2,  8); goto start;
    case 0x3b: /* SRL E        */ SRL_RG08_NULL(DE_L, NULL, 2,  8); goto start;
    case 0x3c: /* SRL H        */ SRL_RG08_NULL(HL_H, NULL, 2,  8); goto start;
    case 0x3d: /* SRL L        */ SRL_RG08_NULL(HL_L, NULL, 2,  8); goto start;
    case 0x3e: /* SRL (HL)     */ SRL_MM08_NULL(HL_W, NULL, 4, 15); goto start;
    case 0x3f: /* SRL A        */ SRL_RG08_NULL(AF_H, NULL, 2,  8); goto start;
    case 0x40: /* BIT 0,B      */ BIT_RG08_MASK(BC_H, 0x01, 2,  8); goto start;
    case 0x41: /* BIT 0,C      */ BIT_RG08_MASK(BC_L, 0x01, 2,  8); goto start;
    case 0x42: /* BIT 0,D      */ BIT_RG08_MASK(DE_H, 0x01, 2,  8); goto start;
    case 0x43: /* BIT 0,E      */ BIT_RG08_MASK(DE_L, 0x01, 2,  8); goto start;
    case 0x44: /* BIT 0,H      */ BIT_RG08_MASK(HL_H, 0x01, 2,  8); goto start;
    case 0x45: /* BIT 0,L      */ BIT_RG08_MASK(HL_L, 0x01, 2,  8); goto start;
    case 0x46: /* BIT 0,(HL)   */ BIT_MM08_MASK(HL_W, 0x01, 3, 12); goto start;
    case 0x47: /* BIT 0,A      */ BIT_RG08_MASK(AF_H, 0x01, 2,  8); goto start;
    case 0x48: /* BIT 1,B      */ BIT_RG08_MASK(BC_H, 0x02, 2,  8); goto start;
    case 0x49: /* BIT 1,C      */ BIT_RG08_MASK(BC_L, 0x02, 2,  8); goto start;
    case 0x4a: /* BIT 1,D      */ BIT_RG08_MASK(DE_H, 0x02, 2,  8); goto start;
    case 0x4b: /* BIT 1,E      */ BIT_RG08_MASK(DE_L, 0x02, 2,  8); goto start;
    case 0x4c: /* BIT 1,H      */ BIT_RG08_MASK(HL_H, 0x02, 2,  8); goto start;
    case 0x4d: /* BIT 1,L      */ BIT_RG08_MASK(HL_L, 0x02, 2,  8); goto start;
    case 0x4e: /* BIT 1,(HL)   */ BIT_MM08_MASK(HL_W, 0x02, 3, 12); goto start;
    case 0x4f: /* BIT 1,A      */ BIT_RG08_MASK(AF_H, 0x02, 2,  8); goto start;
    case 0x50: /* BIT 2,B      */ BIT_RG08_MASK(BC_H, 0x04, 2,  8); goto start;
    case 0x51: /* BIT 2,C      */ BIT_RG08_MASK(BC_L, 0x04, 2,  8); goto start;
    case 0x52: /* BIT 2,D      */ BIT_RG08_MASK(DE_H, 0x04, 2,  8); goto start;
    case 0x53: /* BIT 2,E      */ BIT_RG08_MASK(DE_L, 0x04, 2,  8); goto start;
    case 0x54: /* BIT 2,H      */ BIT_RG08_MASK(HL_H, 0x04, 2,  8); goto start;
    case 0x55: /* BIT 2,L      */ BIT_RG08_MASK(HL_L, 0x04, 2,  8); goto start;
    case 0x56: /* BIT 2,(HL)   */ BIT_MM08_MASK(HL_W, 0x04, 3, 12); goto start;
    case 0x57: /* BIT 2,A      */ BIT_RG08_MASK(AF_H, 0x04, 2,  8); goto start;
    case 0x58: /* BIT 3,B      */ BIT_RG08_MASK(BC_H, 0x08, 2,  8); goto start;
    case 0x59: /* BIT 3,C      */ BIT_RG08_MASK(BC_L, 0x08, 2,  8); goto start;
    case 0x5a: /* BIT 3,D      */ BIT_RG08_MASK(DE_H, 0x08, 2,  8); goto start;
    case 0x5b: /* BIT 3,E      */ BIT_RG08_MASK(DE_L, 0x08, 2,  8); goto start;
    case 0x5c: /* BIT 3,H      */ BIT_RG08_MASK(HL_H, 0x08, 2,  8); goto start;
    case 0x5d: /* BIT 3,L      */ BIT_RG08_MASK(HL_L, 0x08, 2,  8); goto start;
    case 0x5e: /* BIT 3,(HL)   */ BIT_MM08_MASK(HL_W, 0x08, 3, 12); goto start;
    case 0x5f: /* BIT 3,A      */ BIT_RG08_MASK(AF_H, 0x08, 2,  8); goto start;
    case 0x60: /* BIT 4,B      */ BIT_RG08_MASK(BC_H, 0x10, 2,  8); goto start;
    case 0x61: /* BIT 4,C      */ BIT_RG08_MASK(BC_L, 0x10, 2,  8); goto start;
    case 0x62: /* BIT 4,D      */ BIT_RG08_MASK(DE_H, 0x10, 2,  8); goto start;
    case 0x63: /* BIT 4,E      */ BIT_RG08_MASK(DE_L, 0x10, 2,  8); goto start;
    case 0x64: /* BIT 4,H      */ BIT_RG08_MASK(HL_H, 0x10, 2,  8); goto start;
    case 0x65: /* BIT 4,L      */ BIT_RG08_MASK(HL_L, 0x10, 2,  8); goto start;
    case 0x66: /* BIT 4,(HL)   */ BIT_MM08_MASK(HL_W, 0x10, 3, 12); goto start;
    case 0x67: /* BIT 4,A      */ BIT_RG08_MASK(AF_H, 0x10, 2,  8); goto start;
    case 0x68: /* BIT 5,B      */ BIT_RG08_MASK(BC_H, 0x20, 2,  8); goto start;
    case 0x69: /* BIT 5,C      */ BIT_RG08_MASK(BC_L, 0x20, 2,  8); goto start;
    case 0x6a: /* BIT 5,D      */ BIT_RG08_MASK(DE_H, 0x20, 2,  8); goto start;
    case 0x6b: /* BIT 5,E      */ BIT_RG08_MASK(DE_L, 0x20, 2,  8); goto start;
    case 0x6c: /* BIT 5,H      */ BIT_RG08_MASK(HL_H, 0x20, 2,  8); goto start;
    case 0x6d: /* BIT 5,L      */ BIT_RG08_MASK(HL_L, 0x20, 2,  8); goto start;
    case 0x6e: /* BIT 5,(HL)   */ BIT_MM08_MASK(HL_W, 0x20, 3, 12); goto start;
    case 0x6f: /* BIT 5,A      */ BIT_RG08_MASK(AF_H, 0x20, 2,  8); goto start;
    case 0x70: /* BIT 6,B      */ BIT_RG08_MASK(BC_H, 0x40, 2,  8); goto start;
    case 0x71: /* BIT 6,C      */ BIT_RG08_MASK(BC_L, 0x40, 2,  8); goto start;
    case 0x72: /* BIT 6,D      */ BIT_RG08_MASK(DE_H, 0x40, 2,  8); goto start;
    case 0x73: /* BIT 6,E      */ BIT_RG08_MASK(DE_L, 0x40, 2,  8); goto start;
    case 0x74: /* BIT 6,H      */ BIT_RG08_MASK(HL_H, 0x40, 2,  8); goto start;
    case 0x75: /* BIT 6,L      */ BIT_RG08_MASK(HL_L, 0x40, 2,  8); goto start;
    case 0x76: /* BIT 6,(HL)   */ BIT_MM08_MASK(HL_W, 0x40, 3, 12); goto start;
    case 0x77: /* BIT 6,A      */ BIT_RG08_MASK(AF_H, 0x40, 2,  8); goto start;
    case 0x78: /* BIT 7,B      */ BIT_RG08_MASK(BC_H, 0x80, 2,  8); goto start;
    case 0x79: /* BIT 7,C      */ BIT_RG08_MASK(BC_L, 0x80, 2,  8); goto start;
    case 0x7a: /* BIT 7,D      */ BIT_RG08_MASK(DE_H, 0x80, 2,  8); goto start;
    case 0x7b: /* BIT 7,E      */ BIT_RG08_MASK(DE_L, 0x80, 2,  8); goto start;
    case 0x7c: /* BIT 7,H      */ BIT_RG08_MASK(HL_H, 0x80, 2,  8); goto start;
    case 0x7d: /* BIT 7,L      */ BIT_RG08_MASK(HL_L, 0x80, 2,  8); goto start;
    case 0x7e: /* BIT 7,(HL)   */ BIT_MM08_MASK(HL_W, 0x80, 3, 12); goto start;
    case 0x7f: /* BIT 7,A      */ BIT_RG08_MASK(AF_H, 0x80, 2,  8); goto start;
    case 0x80: /* RES 0,B      */ RES_RG08_MASK(BC_H, 0x01, 2,  8); goto start;
    case 0x81: /* RES 0,C      */ RES_RG08_MASK(BC_L, 0x01, 2,  8); goto start;
    case 0x82: /* RES 0,D      */ RES_RG08_MASK(DE_H, 0x01, 2,  8); goto start;
    case 0x83: /* RES 0,E      */ RES_RG08_MASK(DE_L, 0x01, 2,  8); goto start;
    case 0x84: /* RES 0,H      */ RES_RG08_MASK(HL_H, 0x01, 2,  8); goto start;
    case 0x85: /* RES 0,L      */ RES_RG08_MASK(HL_L, 0x01, 2,  8); goto start;
    case 0x86: /* RES 0,(HL)   */ RES_MM08_MASK(HL_W, 0x01, 4, 15); goto start;
    case 0x87: /* RES 0,A      */ RES_RG08_MASK(AF_H, 0x01, 2,  8); goto start;
    case 0x88: /* RES 1,B      */ RES_RG08_MASK(BC_H, 0x02, 2,  8); goto start;
    case 0x89: /* RES 1,C      */ RES_RG08_MASK(BC_L, 0x02, 2,  8); goto start;
    case 0x8a: /* RES 1,D      */ RES_RG08_MASK(DE_H, 0x02, 2,  8); goto start;
    case 0x8b: /* RES 1,E      */ RES_RG08_MASK(DE_L, 0x02, 2,  8); goto start;
    case 0x8c: /* RES 1,H      */ RES_RG08_MASK(HL_H, 0x02, 2,  8); goto start;
    case 0x8d: /* RES 1,L      */ RES_RG08_MASK(HL_L, 0x02, 2,  8); goto start;
    case 0x8e: /* RES 1,(HL)   */ RES_MM08_MASK(HL_W, 0x02, 4, 15); goto start;
    case 0x8f: /* RES 1,A      */ RES_RG08_MASK(AF_H, 0x02, 2,  8); goto start;
    case 0x90: /* RES 2,B      */ RES_RG08_MASK(BC_H, 0x04, 2,  8); goto start;
    case 0x91: /* RES 2,C      */ RES_RG08_MASK(BC_L, 0x04, 2,  8); goto start;
    case 0x92: /* RES 2,D      */ RES_RG08_MASK(DE_H, 0x04, 2,  8); goto start;
    case 0x93: /* RES 2,E      */ RES_RG08_MASK(DE_L, 0x04, 2,  8); goto start;
    case 0x94: /* RES 2,H      */ RES_RG08_MASK(HL_H, 0x04, 2,  8); goto start;
    case 0x95: /* RES 2,L      */ RES_RG08_MASK(HL_L, 0x04, 2,  8); goto start;
    case 0x96: /* RES 2,(HL)   */ RES_MM08_MASK(HL_W, 0x04, 4, 15); goto start;
    case 0x97: /* RES 2,A      */ RES_RG08_MASK(AF_H, 0x04, 2,  8); goto start;
    case 0x98: /* RES 3,B      */ RES_RG08_MASK(BC_H, 0x08, 2,  8); goto start;
    case 0x99: /* RES 3,C      */ RES_RG08_MASK(BC_L, 0x08, 2,  8); goto start;
    case 0x9a: /* RES 3,D      */ RES_RG08_MASK(DE_H, 0x08, 2,  8); goto start;
    case 0x9b: /* RES 3,E      */ RES_RG08_MASK(DE_L, 0x08, 2,  8); goto start;
    case 0x9c: /* RES 3,H      */ RES_RG08_MASK(HL_H, 0x08, 2,  8); goto start;
    case 0x9d: /* RES 3,L      */ RES_RG08_MASK(HL_L, 0x08, 2,  8); goto start;
    case 0x9e: /* RES 3,(HL)   */ RES_MM08_MASK(HL_W, 0x08, 4, 15); goto start;
    case 0x9f: /* RES 3,A      */ RES_RG08_MASK(AF_H, 0x08, 2,  8); goto start;
    case 0xa0: /* RES 4,B      */ RES_RG08_MASK(BC_H, 0x10, 2,  8); goto start;
    case 0xa1: /* RES 4,C      */ RES_RG08_MASK(BC_L, 0x10, 2,  8); goto start;
    case 0xa2: /* RES 4,D      */ RES_RG08_MASK(DE_H, 0x10, 2,  8); goto start;
    case 0xa3: /* RES 4,E      */ RES_RG08_MASK(DE_L, 0x10, 2,  8); goto start;
    case 0xa4: /* RES 4,H      */ RES_RG08_MASK(HL_H, 0x10, 2,  8); goto start;
    case 0xa5: /* RES 4,L      */ RES_RG08_MASK(HL_L, 0x10, 2,  8); goto start;
    case 0xa6: /* RES 4,(HL)   */ RES_MM08_MASK(HL_W, 0x10, 4, 15); goto start;
    case 0xa7: /* RES 4,A      */ RES_RG08_MASK(AF_H, 0x10, 2,  8); goto start;
    case 0xa8: /* RES 5,B      */ RES_RG08_MASK(BC_H, 0x20, 2,  8); goto start;
    case 0xa9: /* RES 5,C      */ RES_RG08_MASK(BC_L, 0x20, 2,  8); goto start;
    case 0xaa: /* RES 5,D      */ RES_RG08_MASK(DE_H, 0x20, 2,  8); goto start;
    case 0xab: /* RES 5,E      */ RES_RG08_MASK(DE_L, 0x20, 2,  8); goto start;
    case 0xac: /* RES 5,H      */ RES_RG08_MASK(HL_H, 0x20, 2,  8); goto start;
    case 0xad: /* RES 5,L      */ RES_RG08_MASK(HL_L, 0x20, 2,  8); goto start;
    case 0xae: /* RES 5,(HL)   */ RES_MM08_MASK(HL_W, 0x20, 4, 15); goto start;
    case 0xaf: /* RES 5,A      */ RES_RG08_MASK(AF_H, 0x20, 2,  8); goto start;
    case 0xb0: /* RES 6,B      */ RES_RG08_MASK(BC_H, 0x40, 2,  8); goto start;
    case 0xb1: /* RES 6,C      */ RES_RG08_MASK(BC_L, 0x40, 2,  8); goto start;
    case 0xb2: /* RES 6,D      */ RES_RG08_MASK(DE_H, 0x40, 2,  8); goto start;
    case 0xb3: /* RES 6,E      */ RES_RG08_MASK(DE_L, 0x40, 2,  8); goto start;
    case 0xb4: /* RES 6,H      */ RES_RG08_MASK(HL_H, 0x40, 2,  8); goto start;
    case 0xb5: /* RES 6,L      */ RES_RG08_MASK(HL_L, 0x40, 2,  8); goto start;
    case 0xb6: /* RES 6,(HL)   */ RES_MM08_MASK(HL_W, 0x40, 4, 15); goto start;
    case 0xb7: /* RES 6,A      */ RES_RG08_MASK(AF_H, 0x40, 2,  8); goto start;
    case 0xb8: /* RES 7,B      */ RES_RG08_MASK(BC_H, 0x80, 2,  8); goto start;
    case 0xb9: /* RES 7,C      */ RES_RG08_MASK(BC_L, 0x80, 2,  8); goto start;
    case 0xba: /* RES 7,D      */ RES_RG08_MASK(DE_H, 0x80, 2,  8); goto start;
    case 0xbb: /* RES 7,E      */ RES_RG08_MASK(DE_L, 0x80, 2,  8); goto start;
    case 0xbc: /* RES 7,H      */ RES_RG08_MASK(HL_H, 0x80, 2,  8); goto start;
    case 0xbd: /* RES 7,L      */ RES_RG08_MASK(HL_L, 0x80, 2,  8); goto start;
    case 0xbe: /* RES 7,(HL)   */ RES_MM08_MASK(HL_W, 0x80, 4, 15); goto start;
    case 0xbf: /* RES 7,A      */ RES_RG08_MASK(AF_H, 0x80, 2,  8); goto start;
    case 0xc0: /* SET 0,B      */ SET_RG08_MASK(BC_H, 0x01, 2,  8); goto start;
    case 0xc1: /* SET 0,C      */ SET_RG08_MASK(BC_L, 0x01, 2,  8); goto start;
    case 0xc2: /* SET 0,D      */ SET_RG08_MASK(DE_H, 0x01, 2,  8); goto start;
    case 0xc3: /* SET 0,E      */ SET_RG08_MASK(DE_L, 0x01, 2,  8); goto start;
    case 0xc4: /* SET 0,H      */ SET_RG08_MASK(HL_H, 0x01, 2,  8); goto start;
    case 0xc5: /* SET 0,L      */ SET_RG08_MASK(HL_L, 0x01, 2,  8); goto start;
    case 0xc6: /* SET 0,(HL)   */ SET_MM08_MASK(HL_W, 0x01, 4, 15); goto start;
    case 0xc7: /* SET 0,A      */ SET_RG08_MASK(AF_H, 0x01, 2,  8); goto start;
    case 0xc8: /* SET 1,B      */ SET_RG08_MASK(BC_H, 0x02, 2,  8); goto start;
    case 0xc9: /* SET 1,C      */ SET_RG08_MASK(BC_L, 0x02, 2,  8); goto start;
    case 0xca: /* SET 1,D      */ SET_RG08_MASK(DE_H, 0x02, 2,  8); goto start;
    case 0xcb: /* SET 1,E      */ SET_RG08_MASK(DE_L, 0x02, 2,  8); goto start;
    case 0xcc: /* SET 1,H      */ SET_RG08_MASK(HL_H, 0x02, 2,  8); goto start;
    case 0xcd: /* SET 1,L      */ SET_RG08_MASK(HL_L, 0x02, 2,  8); goto start;
    case 0xce: /* SET 1,(HL)   */ SET_MM08_MASK(HL_W, 0x02, 4, 15); goto start;
    case 0xcf: /* SET 1,A      */ SET_RG08_MASK(AF_H, 0x02, 2,  8); goto start;
    case 0xd0: /* SET 2,B      */ SET_RG08_MASK(BC_H, 0x04, 2,  8); goto start;
    case 0xd1: /* SET 2,C      */ SET_RG08_MASK(BC_L, 0x04, 2,  8); goto start;
    case 0xd2: /* SET 2,D      */ SET_RG08_MASK(DE_H, 0x04, 2,  8); goto start;
    case 0xd3: /* SET 2,E      */ SET_RG08_MASK(DE_L, 0x04, 2,  8); goto start;
    case 0xd4: /* SET 2,H      */ SET_RG08_MASK(HL_H, 0x04, 2,  8); goto start;
    case 0xd5: /* SET 2,L      */ SET_RG08_MASK(HL_L, 0x04, 2,  8); goto start;
    case 0xd6: /* SET 2,(HL)   */ SET_MM08_MASK(HL_W, 0x04, 4, 15); goto start;
    case 0xd7: /* SET 2,A      */ SET_RG08_MASK(AF_H, 0x04, 2,  8); goto start;
    case 0xd8: /* SET 3,B      */ SET_RG08_MASK(BC_H, 0x08, 2,  8); goto start;
    case 0xd9: /* SET 3,C      */ SET_RG08_MASK(BC_L, 0x08, 2,  8); goto start;
    case 0xda: /* SET 3,D      */ SET_RG08_MASK(DE_H, 0x08, 2,  8); goto start;
    case 0xdb: /* SET 3,E      */ SET_RG08_MASK(DE_L, 0x08, 2,  8); goto start;
    case 0xdc: /* SET 3,H      */ SET_RG08_MASK(HL_H, 0x08, 2,  8); goto start;
    case 0xdd: /* SET 3,L      */ SET_RG08_MASK(HL_L, 0x08, 2,  8); goto start;
    case 0xde: /* SET 3,(HL)   */ SET_MM08_MASK(HL_W, 0x08, 4, 15); goto start;
    case 0xdf: /* SET 3,A      */ SET_RG08_MASK(AF_H, 0x08, 2,  8); goto start;
    case 0xe0: /* SET 4,B      */ SET_RG08_MASK(BC_H, 0x10, 2,  8); goto start;
    case 0xe1: /* SET 4,C      */ SET_RG08_MASK(BC_L, 0x10, 2,  8); goto start;
    case 0xe2: /* SET 4,D      */ SET_RG08_MASK(DE_H, 0x10, 2,  8); goto start;
    case 0xe3: /* SET 4,E      */ SET_RG08_MASK(DE_L, 0x10, 2,  8); goto start;
    case 0xe4: /* SET 4,H      */ SET_RG08_MASK(HL_H, 0x10, 2,  8); goto start;
    case 0xe5: /* SET 4,L      */ SET_RG08_MASK(HL_L, 0x10, 2,  8); goto start;
    case 0xe6: /* SET 4,(HL)   */ SET_MM08_MASK(HL_W, 0x10, 4, 15); goto start;
    case 0xe7: /* SET 4,A      */ SET_RG08_MASK(AF_H, 0x10, 2,  8); goto start;
    case 0xe8: /* SET 5,B      */ SET_RG08_MASK(BC_H, 0x20, 2,  8); goto start;
    case 0xe9: /* SET 5,C      */ SET_RG08_MASK(BC_L, 0x20, 2,  8); goto start;
    case 0xea: /* SET 5,D      */ SET_RG08_MASK(DE_H, 0x20, 2,  8); goto start;
    case 0xeb: /* SET 5,E      */ SET_RG08_MASK(DE_L, 0x20, 2,  8); goto start;
    case 0xec: /* SET 5,H      */ SET_RG08_MASK(HL_H, 0x20, 2,  8); goto start;
    case 0xed: /* SET 5,L      */ SET_RG08_MASK(HL_L, 0x20, 2,  8); goto start;
    case 0xee: /* SET 5,(HL)   */ SET_MM08_MASK(HL_W, 0x20, 4, 15); goto start;
    case 0xef: /* SET 5,A      */ SET_RG08_MASK(AF_H, 0x20, 2,  8); goto start;
    case 0xf0: /* SET 6,B      */ SET_RG08_MASK(BC_H, 0x40, 2,  8); goto start;
    case 0xf1: /* SET 6,C      */ SET_RG08_MASK(BC_L, 0x40, 2,  8); goto start;
    case 0xf2: /* SET 6,D      */ SET_RG08_MASK(DE_H, 0x40, 2,  8); goto start;
    case 0xf3: /* SET 6,E      */ SET_RG08_MASK(DE_L, 0x40, 2,  8); goto start;
    case 0xf4: /* SET 6,H      */ SET_RG08_MASK(HL_H, 0x40, 2,  8); goto start;
    case 0xf5: /* SET 6,L      */ SET_RG08_MASK(HL_L, 0x40, 2,  8); goto start;
    case 0xf6: /* SET 6,(HL)   */ SET_MM08_MASK(HL_W, 0x40, 4, 15); goto start;
    case 0xf7: /* SET 6,A      */ SET_RG08_MASK(AF_H, 0x40, 2,  8); goto start;
    case 0xf8: /* SET 7,B      */ SET_RG08_MASK(BC_H, 0x80, 2,  8); goto start;
    case 0xf9: /* SET 7,C      */ SET_RG08_MASK(BC_L, 0x80, 2,  8); goto start;
    case 0xfa: /* SET 7,D      */ SET_RG08_MASK(DE_H, 0x80, 2,  8); goto start;
    case 0xfb: /* SET 7,E      */ SET_RG08_MASK(DE_L, 0x80, 2,  8); goto start;
    case 0xfc: /* SET 7,H      */ SET_RG08_MASK(HL_H, 0x80, 2,  8); goto start;
    case 0xfd: /* SET 7,L      */ SET_RG08_MASK(HL_L, 0x80, 2,  8); goto start;
    case 0xfe: /* SET 7,(HL)   */ SET_MM08_MASK(HL_W, 0x80, 4, 15); goto start;
    case 0xff: /* SET 7,A      */ SET_RG08_MASK(AF_H, 0x80, 2,  8); goto start;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: CB %02X: illegal opcode", (PC_W - 2), opcode);
      goto start;
  }

decode_dd:
  IR_L = (IR_L & 0x80) | ((IR_L + 1) & 0x7f);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
#include "z80cpu_opcode_DD.h"
    case 0x00: /* NOP          */ NOP_NULL_NULL(NULL, NULL, 2,  8); goto start;
    case 0x27: /* DAA          */ DAA_NULL_NULL(NULL, NULL, 2,  8); goto start;
    case 0x03: /* INC BC       */ INC_RG16_NULL(BC_W, NULL, 2, 10); goto start;
    case 0x13: /* INC DE       */ INC_RG16_NULL(DE_W, NULL, 2, 10); goto start;
    case 0x23: /* INC IX       */ INC_RG16_NULL(IX_W, NULL, 2, 10); goto start;
    case 0x33: /* INC SP       */ INC_RG16_NULL(SP_W, NULL, 2, 10); goto start;
    case 0x0b: /* DEC BC       */ DEC_RG16_NULL(BC_W, NULL, 2, 10); goto start;
    case 0x1b: /* DEC DE       */ DEC_RG16_NULL(DE_W, NULL, 2, 10); goto start;
    case 0x2b: /* DEC IX       */ DEC_RG16_NULL(IX_W, NULL, 2, 10); goto start;
    case 0x3b: /* DEC SP       */ DEC_RG16_NULL(SP_W, NULL, 2, 10); goto start;
    case 0x06: /* LD B,n       */ MOV_RG08_IM08(BC_H, PC_W, 3, 11); goto start;
    case 0x0e: /* LD C,n       */ MOV_RG08_IM08(BC_L, PC_W, 3, 11); goto start;
    case 0x16: /* LD D,n       */ MOV_RG08_IM08(DE_H, PC_W, 3, 11); goto start;
    case 0x1e: /* LD E,n       */ MOV_RG08_IM08(DE_L, PC_W, 3, 11); goto start;
    case 0x26: /* LD IXh,n     */ MOV_RG08_IM08(IX_H, PC_W, 3, 11); goto start;
    case 0x2e: /* LD IXl,n     */ MOV_RG08_IM08(IX_L, PC_W, 3, 11); goto start;
    case 0x3e: /* LD A,n       */ MOV_RG08_IM08(AF_H, PC_W, 3, 11); goto start;
    case 0x40: /* LD B,B       */ MOV_RG08_RG08(BC_H, BC_H, 2,  8); goto start;
    case 0x41: /* LD B,C       */ MOV_RG08_RG08(BC_H, BC_L, 2,  8); goto start;
    case 0x42: /* LD B,D       */ MOV_RG08_RG08(BC_H, DE_H, 2,  8); goto start;
    case 0x43: /* LD B,E       */ MOV_RG08_RG08(BC_H, DE_L, 2,  8); goto start;
    case 0x44: /* LD B,IXh     */ MOV_RG08_RG08(BC_H, IX_H, 2,  8); goto start;
    case 0x45: /* LD B,IXl     */ MOV_RG08_RG08(BC_H, IX_L, 2,  8); goto start;
    case 0x46: /* LD B,(IX+d)  */ MOV_RG08_XY08(BC_H, IX_W, 5, 19); goto start;
    case 0x47: /* LD B,A       */ MOV_RG08_RG08(BC_H, AF_H, 2,  8); goto start;
    case 0x48: /* LD C,B       */ MOV_RG08_RG08(BC_L, BC_H, 2,  8); goto start;
    case 0x49: /* LD C,C       */ MOV_RG08_RG08(BC_L, BC_L, 2,  8); goto start;
    case 0x4a: /* LD C,D       */ MOV_RG08_RG08(BC_L, DE_H, 2,  8); goto start;
    case 0x4b: /* LD C,E       */ MOV_RG08_RG08(BC_L, DE_L, 2,  8); goto start;
    case 0x4c: /* LD C,IXh     */ MOV_RG08_RG08(BC_L, IX_H, 2,  8); goto start;
    case 0x4d: /* LD C,IXl     */ MOV_RG08_RG08(BC_L, IX_L, 2,  8); goto start;
    case 0x4e: /* LD C,(IX+d)  */ MOV_RG08_XY08(BC_L, IX_W, 5, 19); goto start;
    case 0x4f: /* LD C,A       */ MOV_RG08_RG08(BC_L, AF_H, 2,  8); goto start;
    case 0x50: /* LD D,B       */ MOV_RG08_RG08(DE_H, BC_H, 2,  8); goto start;
    case 0x51: /* LD D,C       */ MOV_RG08_RG08(DE_H, BC_L, 2,  8); goto start;
    case 0x52: /* LD D,D       */ MOV_RG08_RG08(DE_H, DE_H, 2,  8); goto start;
    case 0x53: /* LD D,E       */ MOV_RG08_RG08(DE_H, DE_L, 2,  8); goto start;
    case 0x54: /* LD D,IXh     */ MOV_RG08_RG08(DE_H, IX_H, 2,  8); goto start;
    case 0x55: /* LD D,IXl     */ MOV_RG08_RG08(DE_H, IX_L, 2,  8); goto start;
    case 0x56: /* LD D,(IX+d)  */ MOV_RG08_XY08(DE_H, IX_W, 5, 19); goto start;
    case 0x57: /* LD D,A       */ MOV_RG08_RG08(DE_H, AF_H, 2,  8); goto start;
    case 0x58: /* LD E,B       */ MOV_RG08_RG08(DE_L, BC_H, 2,  8); goto start;
    case 0x59: /* LD E,C       */ MOV_RG08_RG08(DE_L, BC_L, 2,  8); goto start;
    case 0x5a: /* LD E,D       */ MOV_RG08_RG08(DE_L, DE_H, 2,  8); goto start;
    case 0x5b: /* LD E,E       */ MOV_RG08_RG08(DE_L, DE_L, 2,  8); goto start;
    case 0x5c: /* LD E,IXh     */ MOV_RG08_RG08(DE_L, IX_H, 2,  8); goto start;
    case 0x5d: /* LD E,IXl     */ MOV_RG08_RG08(DE_L, IX_L, 2,  8); goto start;
    case 0x5e: /* LD E,(IX+d)  */ MOV_RG08_XY08(DE_L, IX_W, 5, 19); goto start;
    case 0x5f: /* LD E,A       */ MOV_RG08_RG08(DE_L, AF_H, 2,  8); goto start;
    case 0x60: /* LD IXh,B     */ MOV_RG08_RG08(IX_H, BC_H, 2,  8); goto start;
    case 0x61: /* LD IXh,C     */ MOV_RG08_RG08(IX_H, BC_L, 2,  8); goto start;
    case 0x62: /* LD IXh,D     */ MOV_RG08_RG08(IX_H, DE_H, 2,  8); goto start;
    case 0x63: /* LD IXh,E     */ MOV_RG08_RG08(IX_H, DE_L, 2,  8); goto start;
    case 0x64: /* LD IXh,IXh   */ MOV_RG08_RG08(IX_H, IX_H, 2,  8); goto start;
    case 0x65: /* LD IXh,IXl   */ MOV_RG08_RG08(IX_H, IX_L, 2,  8); goto start;
    case 0x66: /* LD H,(IX+d)  */ MOV_RG08_XY08(HL_H, IX_W, 5, 19); goto start;
    case 0x67: /* LD IXh,A     */ MOV_RG08_RG08(IX_H, AF_H, 2,  8); goto start;
    case 0x68: /* LD IXl,B     */ MOV_RG08_RG08(IX_L, BC_H, 2,  8); goto start;
    case 0x69: /* LD IXl,C     */ MOV_RG08_RG08(IX_L, BC_L, 2,  8); goto start;
    case 0x6a: /* LD IXl,D     */ MOV_RG08_RG08(IX_L, DE_H, 2,  8); goto start;
    case 0x6b: /* LD IXl,E     */ MOV_RG08_RG08(IX_L, DE_L, 2,  8); goto start;
    case 0x6c: /* LD IXl,IXh   */ MOV_RG08_RG08(IX_L, IX_H, 2,  8); goto start;
    case 0x6d: /* LD IXl,IXl   */ MOV_RG08_RG08(IX_L, IX_L, 2,  8); goto start;
    case 0x6e: /* LD L,(IX+d)  */ MOV_RG08_XY08(HL_L, IX_W, 5, 19); goto start;
    case 0x6f: /* LD IXl,A     */ MOV_RG08_RG08(IX_L, AF_H, 2,  8); goto start;
    case 0x70: /* LD (IX+d),B  */ MOV_XY08_RG08(IX_W, BC_H, 5, 19); goto start;
    case 0x71: /* LD (IX+d),C  */ MOV_XY08_RG08(IX_W, BC_L, 5, 19); goto start;
    case 0x72: /* LD (IX+d),D  */ MOV_XY08_RG08(IX_W, DE_H, 5, 19); goto start;
    case 0x73: /* LD (IX+d),E  */ MOV_XY08_RG08(IX_W, DE_L, 5, 19); goto start;
    case 0x74: /* LD (IX+d),H  */ MOV_XY08_RG08(IX_W, HL_H, 5, 19); goto start;
    case 0x75: /* LD (IX+d),L  */ MOV_XY08_RG08(IX_W, HL_L, 5, 19); goto start;
    case 0x76: /* HALT         */ HLT_NULL_NULL(NULL, NULL, 2,  8); goto start;
    case 0x77: /* LD (IX+d),A  */ MOV_XY08_RG08(IX_W, AF_H, 5, 19); goto start;
    case 0x78: /* LD A,B       */ MOV_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x79: /* LD A,C       */ MOV_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x7a: /* LD A,D       */ MOV_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x7b: /* LD A,E       */ MOV_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x7c: /* LD A,IXh     */ MOV_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0x7d: /* LD A,IXl     */ MOV_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0x7e: /* LD A,(IX+d)  */ MOV_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0x7f: /* LD A,A       */ MOV_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x80: /* ADD A,B      */ ADD_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x81: /* ADD A,C      */ ADD_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x82: /* ADD A,D      */ ADD_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x83: /* ADD A,E      */ ADD_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x84: /* ADD A,IXh    */ ADD_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0x85: /* ADD A,IXl    */ ADD_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0x86: /* ADD A,(IX+d) */ ADD_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0x87: /* ADD A,A      */ ADD_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x88: /* ADC A,B      */ ADC_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x89: /* ADC A,C      */ ADC_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x8a: /* ADC A,D      */ ADC_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x8b: /* ADC A,E      */ ADC_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x8c: /* ADC A,IXh    */ ADC_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0x8d: /* ADC A,IXl    */ ADC_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0x8e: /* ADC A,(IX+d) */ ADC_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0x8f: /* ADC A,A      */ ADC_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x90: /* SUB A,B      */ SUB_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x91: /* SUB A,C      */ SUB_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x92: /* SUB A,D      */ SUB_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x93: /* SUB A,E      */ SUB_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x94: /* SUB A,IXh    */ SUB_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0x95: /* SUB A,IXl    */ SUB_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0x96: /* SUB A,(IX+d) */ SUB_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0x97: /* SUB A,A      */ SUB_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x98: /* SBC A,B      */ SBC_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x99: /* SBC A,C      */ SBC_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x9a: /* SBC A,D      */ SBC_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x9b: /* SBC A,E      */ SBC_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x9c: /* SBC A,IXh    */ SBC_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0x9d: /* SBC A,IXl    */ SBC_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0x9e: /* SBC A,(IX+d) */ SBC_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0x9f: /* SBC A,A      */ SBC_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xa0: /* AND A,B      */ AND_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xa1: /* AND A,C      */ AND_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xa2: /* AND A,D      */ AND_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xa3: /* AND A,E      */ AND_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xa4: /* AND A,IXh    */ AND_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0xa5: /* AND A,IXl    */ AND_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0xa6: /* AND A,(IX+d) */ AND_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0xa7: /* AND A,A      */ AND_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xa8: /* XOR A,B      */ XOR_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xa9: /* XOR A,C      */ XOR_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xaa: /* XOR A,D      */ XOR_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xab: /* XOR A,E      */ XOR_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xac: /* XOR A,IXh    */ XOR_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0xad: /* XOR A,IXl    */ XOR_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0xae: /* XOR A,(IX+d) */ XOR_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0xaf: /* XOR A,A      */ XOR_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xb0: /* OR A,B       */ IOR_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xb1: /* OR A,C       */ IOR_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xb2: /* OR A,D       */ IOR_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xb3: /* OR A,E       */ IOR_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xb4: /* OR A,IXh     */ IOR_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0xb5: /* OR A,IXl     */ IOR_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0xb6: /* OR A,(IX+d)  */ IOR_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0xb7: /* OR A,A       */ IOR_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xb8: /* CP A,B       */ CMP_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xb9: /* CP A,C       */ CMP_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xba: /* CP A,D       */ CMP_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xbb: /* CP A,E       */ CMP_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xbc: /* CP A,IXh     */ CMP_RG08_RG08(AF_H, IX_H, 2,  8); goto start;
    case 0xbd: /* CP A,IXl     */ CMP_RG08_RG08(AF_H, IX_L, 2,  8); goto start;
    case 0xbe: /* CP A,(IX+d)  */ CMP_RG08_XY08(AF_H, IX_W, 5, 19); goto start;
    case 0xbf: /* CP A,A       */ CMP_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xcb:
      goto decode_dd_cb;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: DD %02X: illegal opcode", (PC_W - 2), opcode);
      goto start;
  }
  T_STATES -= CyclesXX[opcode];
  goto start;

decode_dd_cb:
  WZ_W = IX_W + (gint8) (*z80cpu->mreq_rd)(z80cpu, PC_W++);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
    case 0x06: /* RLC   (IX+d) */ RLC_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x0e: /* RRC   (IX+d) */ RRC_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x16: /* RL    (IX+d) */ RL__MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x1e: /* RR    (IX+d) */ RR__MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x26: /* SLA   (IX+d) */ SLA_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x2e: /* SRA   (IX+d) */ SRA_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x36: /* SLL   (IX+d) */ SLL_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x3e: /* SRL   (IX+d) */ SRL_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x46: /* BIT 0,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x01, 5, 20); goto start;
    case 0x4e: /* BIT 1,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x02, 5, 20); goto start;
    case 0x56: /* BIT 2,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x04, 5, 20); goto start;
    case 0x5e: /* BIT 3,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x08, 5, 20); goto start;
    case 0x66: /* BIT 4,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x10, 5, 20); goto start;
    case 0x6e: /* BIT 5,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x20, 5, 20); goto start;
    case 0x76: /* BIT 6,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x40, 5, 20); goto start;
    case 0x7e: /* BIT 7,(IX+d) */ BIT_MM08_MASK(WZ_W, 0x80, 5, 20); goto start;
    case 0x86: /* RES 0,(IX+d) */ RES_MM08_MASK(WZ_W, 0x01, 6, 23); goto start;
    case 0x8e: /* RES 1,(IX+d) */ RES_MM08_MASK(WZ_W, 0x02, 6, 23); goto start;
    case 0x96: /* RES 2,(IX+d) */ RES_MM08_MASK(WZ_W, 0x04, 6, 23); goto start;
    case 0x9e: /* RES 3,(IX+d) */ RES_MM08_MASK(WZ_W, 0x08, 6, 23); goto start;
    case 0xa6: /* RES 4,(IX+d) */ RES_MM08_MASK(WZ_W, 0x10, 6, 23); goto start;
    case 0xae: /* RES 5,(IX+d) */ RES_MM08_MASK(WZ_W, 0x20, 6, 23); goto start;
    case 0xb6: /* RES 6,(IX+d) */ RES_MM08_MASK(WZ_W, 0x40, 6, 23); goto start;
    case 0xbe: /* RES 7,(IX+d) */ RES_MM08_MASK(WZ_W, 0x80, 6, 23); goto start;
    case 0xc6: /* SET 0,(IX+d) */ SET_MM08_MASK(WZ_W, 0x01, 6, 23); goto start;
    case 0xce: /* SET 1,(IX+d) */ SET_MM08_MASK(WZ_W, 0x02, 6, 23); goto start;
    case 0xd6: /* SET 2,(IX+d) */ SET_MM08_MASK(WZ_W, 0x04, 6, 23); goto start;
    case 0xde: /* SET 3,(IX+d) */ SET_MM08_MASK(WZ_W, 0x08, 6, 23); goto start;
    case 0xe6: /* SET 4,(IX+d) */ SET_MM08_MASK(WZ_W, 0x10, 6, 23); goto start;
    case 0xee: /* SET 5,(IX+d) */ SET_MM08_MASK(WZ_W, 0x20, 6, 23); goto start;
    case 0xf6: /* SET 6,(IX+d) */ SET_MM08_MASK(WZ_W, 0x40, 6, 23); goto start;
    case 0xfe: /* SET 7,(IX+d) */ SET_MM08_MASK(WZ_W, 0x80, 6, 23); goto start;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: DD CB nn %02X: illegal opcode", (PC_W - 4), opcode);
      goto start;
  }

decode_ed:
  IR_L = (IR_L & 0x80) | ((IR_L + 1) & 0x7f);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
#include "z80cpu_opcode_ED.h"
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: ED %02X: illegal opcode", (PC_W - 2), opcode);
      goto start;
  }
  T_STATES -= CyclesED[opcode];
  goto start;

decode_fd:
  IR_L = (IR_L & 0x80) | ((IR_L + 1) & 0x7f);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
#include "z80cpu_opcode_FD.h"
    case 0x00: /* NOP          */ NOP_NULL_NULL(NULL, NULL, 2,  8); goto start;
    case 0x27: /* DAA          */ DAA_NULL_NULL(NULL, NULL, 2,  8); goto start;
    case 0x03: /* INC BC       */ INC_RG16_NULL(BC_W, NULL, 2, 10); goto start;
    case 0x13: /* INC DE       */ INC_RG16_NULL(DE_W, NULL, 2, 10); goto start;
    case 0x23: /* INC IY       */ INC_RG16_NULL(IY_W, NULL, 2, 10); goto start;
    case 0x33: /* INC SP       */ INC_RG16_NULL(SP_W, NULL, 2, 10); goto start;
    case 0x0b: /* DEC BC       */ DEC_RG16_NULL(BC_W, NULL, 2, 10); goto start;
    case 0x1b: /* DEC DE       */ DEC_RG16_NULL(DE_W, NULL, 2, 10); goto start;
    case 0x2b: /* DEC IY       */ DEC_RG16_NULL(IY_W, NULL, 2, 10); goto start;
    case 0x3b: /* DEC SP       */ DEC_RG16_NULL(SP_W, NULL, 2, 10); goto start;
    case 0x06: /* LD B,n       */ MOV_RG08_IM08(BC_H, PC_W, 3, 11); goto start;
    case 0x0e: /* LD C,n       */ MOV_RG08_IM08(BC_L, PC_W, 3, 11); goto start;
    case 0x16: /* LD D,n       */ MOV_RG08_IM08(DE_H, PC_W, 3, 11); goto start;
    case 0x1e: /* LD E,n       */ MOV_RG08_IM08(DE_L, PC_W, 3, 11); goto start;
    case 0x26: /* LD IYh,n     */ MOV_RG08_IM08(IY_H, PC_W, 3, 11); goto start;
    case 0x2e: /* LD IYl,n     */ MOV_RG08_IM08(IY_L, PC_W, 3, 11); goto start;
    case 0x3e: /* LD A,n       */ MOV_RG08_IM08(AF_H, PC_W, 3, 11); goto start;
    case 0x40: /* LD B,B       */ MOV_RG08_RG08(BC_H, BC_H, 2,  8); goto start;
    case 0x41: /* LD B,C       */ MOV_RG08_RG08(BC_H, BC_L, 2,  8); goto start;
    case 0x42: /* LD B,D       */ MOV_RG08_RG08(BC_H, DE_H, 2,  8); goto start;
    case 0x43: /* LD B,E       */ MOV_RG08_RG08(BC_H, DE_L, 2,  8); goto start;
    case 0x44: /* LD B,IYh     */ MOV_RG08_RG08(BC_H, IY_H, 2,  8); goto start;
    case 0x45: /* LD B,IYl     */ MOV_RG08_RG08(BC_H, IY_L, 2,  8); goto start;
    case 0x46: /* LD B,(IY+d)  */ MOV_RG08_XY08(BC_H, IY_W, 5, 19); goto start;
    case 0x47: /* LD B,A       */ MOV_RG08_RG08(BC_H, AF_H, 2,  8); goto start;
    case 0x48: /* LD C,B       */ MOV_RG08_RG08(BC_L, BC_H, 2,  8); goto start;
    case 0x49: /* LD C,C       */ MOV_RG08_RG08(BC_L, BC_L, 2,  8); goto start;
    case 0x4a: /* LD C,D       */ MOV_RG08_RG08(BC_L, DE_H, 2,  8); goto start;
    case 0x4b: /* LD C,E       */ MOV_RG08_RG08(BC_L, DE_L, 2,  8); goto start;
    case 0x4c: /* LD C,IYh     */ MOV_RG08_RG08(BC_L, IY_H, 2,  8); goto start;
    case 0x4d: /* LD C,IYl     */ MOV_RG08_RG08(BC_L, IY_L, 2,  8); goto start;
    case 0x4e: /* LD C,(IY+d)  */ MOV_RG08_XY08(BC_L, IY_W, 5, 19); goto start;
    case 0x4f: /* LD C,A       */ MOV_RG08_RG08(BC_L, AF_H, 2,  8); goto start;
    case 0x50: /* LD D,B       */ MOV_RG08_RG08(DE_H, BC_H, 2,  8); goto start;
    case 0x51: /* LD D,C       */ MOV_RG08_RG08(DE_H, BC_L, 2,  8); goto start;
    case 0x52: /* LD D,D       */ MOV_RG08_RG08(DE_H, DE_H, 2,  8); goto start;
    case 0x53: /* LD D,E       */ MOV_RG08_RG08(DE_H, DE_L, 2,  8); goto start;
    case 0x54: /* LD D,IYh     */ MOV_RG08_RG08(DE_H, IY_H, 2,  8); goto start;
    case 0x55: /* LD D,IYl     */ MOV_RG08_RG08(DE_H, IY_L, 2,  8); goto start;
    case 0x56: /* LD D,(IY+d)  */ MOV_RG08_XY08(DE_H, IY_W, 5, 19); goto start;
    case 0x57: /* LD D,A       */ MOV_RG08_RG08(DE_H, AF_H, 2,  8); goto start;
    case 0x58: /* LD E,B       */ MOV_RG08_RG08(DE_L, BC_H, 2,  8); goto start;
    case 0x59: /* LD E,C       */ MOV_RG08_RG08(DE_L, BC_L, 2,  8); goto start;
    case 0x5a: /* LD E,D       */ MOV_RG08_RG08(DE_L, DE_H, 2,  8); goto start;
    case 0x5b: /* LD E,E       */ MOV_RG08_RG08(DE_L, DE_L, 2,  8); goto start;
    case 0x5c: /* LD E,IYh     */ MOV_RG08_RG08(DE_L, IY_H, 2,  8); goto start;
    case 0x5d: /* LD E,IYl     */ MOV_RG08_RG08(DE_L, IY_L, 2,  8); goto start;
    case 0x5e: /* LD E,(IY+d)  */ MOV_RG08_XY08(DE_L, IY_W, 5, 19); goto start;
    case 0x5f: /* LD E,A       */ MOV_RG08_RG08(DE_L, AF_H, 2,  8); goto start;
    case 0x60: /* LD IYh,B     */ MOV_RG08_RG08(IY_H, BC_H, 2,  8); goto start;
    case 0x61: /* LD IYh,C     */ MOV_RG08_RG08(IY_H, BC_L, 2,  8); goto start;
    case 0x62: /* LD IYh,D     */ MOV_RG08_RG08(IY_H, DE_H, 2,  8); goto start;
    case 0x63: /* LD IYh,E     */ MOV_RG08_RG08(IY_H, DE_L, 2,  8); goto start;
    case 0x64: /* LD IYh,IYh   */ MOV_RG08_RG08(IY_H, IY_H, 2,  8); goto start;
    case 0x65: /* LD IYh,IYl   */ MOV_RG08_RG08(IY_H, IY_L, 2,  8); goto start;
    case 0x66: /* LD H,(IY+d)  */ MOV_RG08_XY08(HL_H, IY_W, 5, 19); goto start;
    case 0x67: /* LD IYh,A     */ MOV_RG08_RG08(IY_H, AF_H, 2,  8); goto start;
    case 0x68: /* LD IYl,B     */ MOV_RG08_RG08(IY_L, BC_H, 2,  8); goto start;
    case 0x69: /* LD IYl,C     */ MOV_RG08_RG08(IY_L, BC_L, 2,  8); goto start;
    case 0x6a: /* LD IYl,D     */ MOV_RG08_RG08(IY_L, DE_H, 2,  8); goto start;
    case 0x6b: /* LD IYl,E     */ MOV_RG08_RG08(IY_L, DE_L, 2,  8); goto start;
    case 0x6c: /* LD IYl,IYh   */ MOV_RG08_RG08(IY_L, IY_H, 2,  8); goto start;
    case 0x6d: /* LD IYl,IYl   */ MOV_RG08_RG08(IY_L, IY_L, 2,  8); goto start;
    case 0x6e: /* LD L,(IY+d)  */ MOV_RG08_XY08(HL_L, IY_W, 5, 19); goto start;
    case 0x6f: /* LD IYl,A     */ MOV_RG08_RG08(IY_L, AF_H, 2,  8); goto start;
    case 0x70: /* LD (IY+d),B  */ MOV_XY08_RG08(IY_W, BC_H, 5, 19); goto start;
    case 0x71: /* LD (IY+d),C  */ MOV_XY08_RG08(IY_W, BC_L, 5, 19); goto start;
    case 0x72: /* LD (IY+d),D  */ MOV_XY08_RG08(IY_W, DE_H, 5, 19); goto start;
    case 0x73: /* LD (IY+d),E  */ MOV_XY08_RG08(IY_W, DE_L, 5, 19); goto start;
    case 0x74: /* LD (IY+d),H  */ MOV_XY08_RG08(IY_W, HL_H, 5, 19); goto start;
    case 0x75: /* LD (IY+d),L  */ MOV_XY08_RG08(IY_W, HL_L, 5, 19); goto start;
    case 0x76: /* HALT         */ HLT_NULL_NULL(NULL, NULL, 2,  8); goto start;
    case 0x77: /* LD (IY+d),A  */ MOV_XY08_RG08(IY_W, AF_H, 5, 19); goto start;
    case 0x78: /* LD A,B       */ MOV_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x79: /* LD A,C       */ MOV_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x7a: /* LD A,D       */ MOV_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x7b: /* LD A,E       */ MOV_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x7c: /* LD A,IYh     */ MOV_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0x7d: /* LD A,IYl     */ MOV_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0x7e: /* LD A,(IY+d)  */ MOV_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0x7f: /* LD A,A       */ MOV_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x80: /* ADD A,B      */ ADD_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x81: /* ADD A,C      */ ADD_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x82: /* ADD A,D      */ ADD_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x83: /* ADD A,E      */ ADD_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x84: /* ADD A,IYh    */ ADD_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0x85: /* ADD A,IYl    */ ADD_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0x86: /* ADD A,(IY+d) */ ADD_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0x87: /* ADD A,A      */ ADD_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x88: /* ADC A,B      */ ADC_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x89: /* ADC A,C      */ ADC_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x8a: /* ADC A,D      */ ADC_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x8b: /* ADC A,E      */ ADC_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x8c: /* ADC A,IYh    */ ADC_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0x8d: /* ADC A,IYl    */ ADC_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0x8e: /* ADC A,(IY+d) */ ADC_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0x8f: /* ADC A,A      */ ADC_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x90: /* SUB A,B      */ SUB_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x91: /* SUB A,C      */ SUB_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x92: /* SUB A,D      */ SUB_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x93: /* SUB A,E      */ SUB_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x94: /* SUB A,IYh    */ SUB_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0x95: /* SUB A,IYl    */ SUB_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0x96: /* SUB A,(IY+d) */ SUB_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0x97: /* SUB A,A      */ SUB_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0x98: /* SBC A,B      */ SBC_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0x99: /* SBC A,C      */ SBC_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0x9a: /* SBC A,D      */ SBC_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0x9b: /* SBC A,E      */ SBC_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0x9c: /* SBC A,IYh    */ SBC_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0x9d: /* SBC A,IYl    */ SBC_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0x9e: /* SBC A,(IY+d) */ SBC_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0x9f: /* SBC A,A      */ SBC_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xa0: /* AND A,B      */ AND_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xa1: /* AND A,C      */ AND_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xa2: /* AND A,D      */ AND_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xa3: /* AND A,E      */ AND_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xa4: /* AND A,IYh    */ AND_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0xa5: /* AND A,IYl    */ AND_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0xa6: /* AND A,(IY+d) */ AND_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0xa7: /* AND A,A      */ AND_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xa8: /* XOR A,B      */ XOR_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xa9: /* XOR A,C      */ XOR_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xaa: /* XOR A,D      */ XOR_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xab: /* XOR A,E      */ XOR_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xac: /* XOR A,IYh    */ XOR_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0xad: /* XOR A,IYl    */ XOR_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0xae: /* XOR A,(IY+d) */ XOR_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0xaf: /* XOR A,A      */ XOR_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xb0: /* OR A,B       */ IOR_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xb1: /* OR A,C       */ IOR_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xb2: /* OR A,D       */ IOR_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xb3: /* OR A,E       */ IOR_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xb4: /* OR A,IYh     */ IOR_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0xb5: /* OR A,IYl     */ IOR_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0xb6: /* OR A,(IY+d)  */ IOR_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0xb7: /* OR A,A       */ IOR_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xb8: /* CP A,B       */ CMP_RG08_RG08(AF_H, BC_H, 2,  8); goto start;
    case 0xb9: /* CP A,C       */ CMP_RG08_RG08(AF_H, BC_L, 2,  8); goto start;
    case 0xba: /* CP A,D       */ CMP_RG08_RG08(AF_H, DE_H, 2,  8); goto start;
    case 0xbb: /* CP A,E       */ CMP_RG08_RG08(AF_H, DE_L, 2,  8); goto start;
    case 0xbc: /* CP A,IYh     */ CMP_RG08_RG08(AF_H, IY_H, 2,  8); goto start;
    case 0xbd: /* CP A,IYl     */ CMP_RG08_RG08(AF_H, IY_L, 2,  8); goto start;
    case 0xbe: /* CP A,(IY+d)  */ CMP_RG08_XY08(AF_H, IY_W, 5, 19); goto start;
    case 0xbf: /* CP A,A       */ CMP_RG08_RG08(AF_H, AF_H, 2,  8); goto start;
    case 0xcb:
      goto decode_fd_cb;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: FD %02X: illegal opcode", (PC_W - 2), opcode);
      goto start;
  }
  T_STATES -= CyclesXX[opcode];
  goto start;

decode_fd_cb:
  WZ_W = IY_W + (gint8) (*z80cpu->mreq_rd)(z80cpu, PC_W++);
  switch(opcode = (*z80cpu->mreq_rd)(z80cpu, PC_W++)) {
    case 0x06: /* RLC   (IY+d) */ RLC_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x0e: /* RRC   (IY+d) */ RRC_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x16: /* RL    (IY+d) */ RL__MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x1e: /* RR    (IY+d) */ RR__MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x26: /* SLA   (IY+d) */ SLA_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x2e: /* SRA   (IY+d) */ SRA_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x36: /* SLL   (IY+d) */ SLL_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x3e: /* SRL   (IY+d) */ SRL_MM08_NULL(WZ_W, NULL, 6, 23); goto start;
    case 0x46: /* BIT 0,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x01, 5, 20); goto start;
    case 0x4e: /* BIT 1,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x02, 5, 20); goto start;
    case 0x56: /* BIT 2,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x04, 5, 20); goto start;
    case 0x5e: /* BIT 3,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x08, 5, 20); goto start;
    case 0x66: /* BIT 4,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x10, 5, 20); goto start;
    case 0x6e: /* BIT 5,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x20, 5, 20); goto start;
    case 0x76: /* BIT 6,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x40, 5, 20); goto start;
    case 0x7e: /* BIT 7,(IY+d) */ BIT_MM08_MASK(WZ_W, 0x80, 5, 20); goto start;
    case 0x86: /* RES 0,(IY+d) */ RES_MM08_MASK(WZ_W, 0x01, 6, 23); goto start;
    case 0x8e: /* RES 1,(IY+d) */ RES_MM08_MASK(WZ_W, 0x02, 6, 23); goto start;
    case 0x96: /* RES 2,(IY+d) */ RES_MM08_MASK(WZ_W, 0x04, 6, 23); goto start;
    case 0x9e: /* RES 3,(IY+d) */ RES_MM08_MASK(WZ_W, 0x08, 6, 23); goto start;
    case 0xa6: /* RES 4,(IY+d) */ RES_MM08_MASK(WZ_W, 0x10, 6, 23); goto start;
    case 0xae: /* RES 5,(IY+d) */ RES_MM08_MASK(WZ_W, 0x20, 6, 23); goto start;
    case 0xb6: /* RES 6,(IY+d) */ RES_MM08_MASK(WZ_W, 0x40, 6, 23); goto start;
    case 0xbe: /* RES 7,(IY+d) */ RES_MM08_MASK(WZ_W, 0x80, 6, 23); goto start;
    case 0xc6: /* SET 0,(IY+d) */ SET_MM08_MASK(WZ_W, 0x01, 6, 23); goto start;
    case 0xce: /* SET 1,(IY+d) */ SET_MM08_MASK(WZ_W, 0x02, 6, 23); goto start;
    case 0xd6: /* SET 2,(IY+d) */ SET_MM08_MASK(WZ_W, 0x04, 6, 23); goto start;
    case 0xde: /* SET 3,(IY+d) */ SET_MM08_MASK(WZ_W, 0x08, 6, 23); goto start;
    case 0xe6: /* SET 4,(IY+d) */ SET_MM08_MASK(WZ_W, 0x10, 6, 23); goto start;
    case 0xee: /* SET 5,(IY+d) */ SET_MM08_MASK(WZ_W, 0x20, 6, 23); goto start;
    case 0xf6: /* SET 6,(IY+d) */ SET_MM08_MASK(WZ_W, 0x40, 6, 23); goto start;
    case 0xfe: /* SET 7,(IY+d) */ SET_MM08_MASK(WZ_W, 0x80, 6, 23); goto start;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: FD CB nn %02X: illegal opcode", (PC_W - 4), opcode);
      goto start;
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
 * GdevZ80CPU::assert_int()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
void gdev_z80cpu_assert_int(GdevZ80CPU *z80cpu)
{
  if(IF_W & IFF_1) {
    IF_W |= IFF_INT;
  }
}

/**
 * GdevZ80CPU::assert_nmi()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
void gdev_z80cpu_assert_nmi(GdevZ80CPU *z80cpu)
{
  IF_W |= IFF_NMI;
}
