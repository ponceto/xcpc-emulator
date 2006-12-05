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
  guint8  I, K;

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
    case 0x00: /* NOP        */ OP_NOP_NULL_NULL(NULL, NULL, 1, 4); break;
    case 0x40: /* LD B,B     */ OP_MOV_RG08_RG08(BC_H, BC_H, 1, 4); break;
    case 0x41: /* LD B,C     */ OP_MOV_RG08_RG08(BC_H, BC_L, 1, 4); break;
    case 0x42: /* LD B,D     */ OP_MOV_RG08_RG08(BC_H, DE_H, 1, 4); break;
    case 0x43: /* LD B,E     */ OP_MOV_RG08_RG08(BC_H, DE_L, 1, 4); break;
    case 0x44: /* LD B,H     */ OP_MOV_RG08_RG08(BC_H, HL_H, 1, 4); break;
    case 0x45: /* LD B,L     */ OP_MOV_RG08_RG08(BC_H, HL_L, 1, 4); break;
    case 0x46: /* LD B,(HL)  */ OP_MOV_RG08_MM08(BC_H, HL_W, 2, 7); break;
    case 0x47: /* LD B,A     */ OP_MOV_RG08_RG08(BC_H, AF_H, 1, 4); break;
    case 0x48: /* LD C,B     */ OP_MOV_RG08_RG08(BC_L, BC_H, 1, 4); break;
    case 0x49: /* LD C,C     */ OP_MOV_RG08_RG08(BC_L, BC_L, 1, 4); break;
    case 0x4a: /* LD C,D     */ OP_MOV_RG08_RG08(BC_L, DE_H, 1, 4); break;
    case 0x4b: /* LD C,E     */ OP_MOV_RG08_RG08(BC_L, DE_L, 1, 4); break;
    case 0x4c: /* LD C,H     */ OP_MOV_RG08_RG08(BC_L, HL_H, 1, 4); break;
    case 0x4d: /* LD C,L     */ OP_MOV_RG08_RG08(BC_L, HL_L, 1, 4); break;
    case 0x4e: /* LD C,(HL)  */ OP_MOV_RG08_MM08(BC_L, HL_W, 2, 7); break;
    case 0x4f: /* LD C,A     */ OP_MOV_RG08_RG08(BC_L, AF_H, 1, 4); break;
    case 0x50: /* LD D,B     */ OP_MOV_RG08_RG08(DE_H, BC_H, 1, 4); break;
    case 0x51: /* LD D,C     */ OP_MOV_RG08_RG08(DE_H, BC_L, 1, 4); break;
    case 0x52: /* LD D,D     */ OP_MOV_RG08_RG08(DE_H, DE_H, 1, 4); break;
    case 0x53: /* LD D,E     */ OP_MOV_RG08_RG08(DE_H, DE_L, 1, 4); break;
    case 0x54: /* LD D,H     */ OP_MOV_RG08_RG08(DE_H, HL_H, 1, 4); break;
    case 0x55: /* LD D,L     */ OP_MOV_RG08_RG08(DE_H, HL_L, 1, 4); break;
    case 0x56: /* LD D,(HL)  */ OP_MOV_RG08_MM08(DE_H, HL_W, 2, 7); break;
    case 0x57: /* LD D,A     */ OP_MOV_RG08_RG08(DE_H, AF_H, 1, 4); break;
    case 0x58: /* LD E,B     */ OP_MOV_RG08_RG08(DE_L, BC_H, 1, 4); break;
    case 0x59: /* LD E,C     */ OP_MOV_RG08_RG08(DE_L, BC_L, 1, 4); break;
    case 0x5a: /* LD E,D     */ OP_MOV_RG08_RG08(DE_L, DE_H, 1, 4); break;
    case 0x5b: /* LD E,E     */ OP_MOV_RG08_RG08(DE_L, DE_L, 1, 4); break;
    case 0x5c: /* LD E,H     */ OP_MOV_RG08_RG08(DE_L, HL_H, 1, 4); break;
    case 0x5d: /* LD E,L     */ OP_MOV_RG08_RG08(DE_L, HL_L, 1, 4); break;
    case 0x5e: /* LD E,(HL)  */ OP_MOV_RG08_MM08(DE_L, HL_W, 2, 7); break;
    case 0x5f: /* LD E,A     */ OP_MOV_RG08_RG08(DE_L, AF_H, 1, 4); break;
    case 0x60: /* LD H,B     */ OP_MOV_RG08_RG08(HL_H, BC_H, 1, 4); break;
    case 0x61: /* LD H,C     */ OP_MOV_RG08_RG08(HL_H, BC_L, 1, 4); break;
    case 0x62: /* LD H,D     */ OP_MOV_RG08_RG08(HL_H, DE_H, 1, 4); break;
    case 0x63: /* LD H,E     */ OP_MOV_RG08_RG08(HL_H, DE_L, 1, 4); break;
    case 0x64: /* LD H,H     */ OP_MOV_RG08_RG08(HL_H, HL_H, 1, 4); break;
    case 0x65: /* LD H,L     */ OP_MOV_RG08_RG08(HL_H, HL_L, 1, 4); break;
    case 0x66: /* LD H,(HL)  */ OP_MOV_RG08_MM08(HL_H, HL_W, 2, 7); break;
    case 0x67: /* LD H,A     */ OP_MOV_RG08_RG08(HL_H, AF_H, 1, 4); break;
    case 0x68: /* LD L,B     */ OP_MOV_RG08_RG08(HL_L, BC_H, 1, 4); break;
    case 0x69: /* LD L,C     */ OP_MOV_RG08_RG08(HL_L, BC_L, 1, 4); break;
    case 0x6a: /* LD L,D     */ OP_MOV_RG08_RG08(HL_L, DE_H, 1, 4); break;
    case 0x6b: /* LD L,E     */ OP_MOV_RG08_RG08(HL_L, DE_L, 1, 4); break;
    case 0x6c: /* LD L,H     */ OP_MOV_RG08_RG08(HL_L, HL_H, 1, 4); break;
    case 0x6d: /* LD L,L     */ OP_MOV_RG08_RG08(HL_L, HL_L, 1, 4); break;
    case 0x6e: /* LD L,(HL)  */ OP_MOV_RG08_MM08(HL_L, HL_W, 2, 7); break;
    case 0x6f: /* LD L,A     */ OP_MOV_RG08_RG08(HL_L, AF_H, 1, 4); break;
    case 0x70: /* LD (HL),B  */ OP_MOV_MM08_RG08(HL_W, BC_H, 2, 7); break;
    case 0x71: /* LD (HL),C  */ OP_MOV_MM08_RG08(HL_W, BC_L, 2, 7); break;
    case 0x72: /* LD (HL),D  */ OP_MOV_MM08_RG08(HL_W, DE_H, 2, 7); break;
    case 0x73: /* LD (HL),E  */ OP_MOV_MM08_RG08(HL_W, DE_L, 2, 7); break;
    case 0x74: /* LD (HL),H  */ OP_MOV_MM08_RG08(HL_W, HL_H, 2, 7); break;
    case 0x75: /* LD (HL),L  */ OP_MOV_MM08_RG08(HL_W, HL_L, 2, 7); break;
    case 0x76: /* HALT       */ OP_HLT_NULL_NULL(NULL, NULL, 1, 4); break;
    case 0x77: /* LD (HL),A  */ OP_MOV_MM08_RG08(HL_W, AF_H, 2, 7); break;
    case 0x78: /* LD A,B     */ OP_MOV_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0x79: /* LD A,C     */ OP_MOV_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0x7a: /* LD A,D     */ OP_MOV_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0x7b: /* LD A,E     */ OP_MOV_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0x7c: /* LD A,H     */ OP_MOV_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0x7d: /* LD A,L     */ OP_MOV_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0x7e: /* LD A,(HL)  */ OP_MOV_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0x7f: /* LD A,A     */ OP_MOV_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0x80: /* ADD A,B    */ OP_ADD_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0x81: /* ADD A,C    */ OP_ADD_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0x82: /* ADD A,D    */ OP_ADD_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0x83: /* ADD A,E    */ OP_ADD_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0x84: /* ADD A,H    */ OP_ADD_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0x85: /* ADD A,L    */ OP_ADD_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0x86: /* ADD A,(HL) */ OP_ADD_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0x87: /* ADD A,A    */ OP_ADD_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0x88: /* ADC A,B    */ OP_ADC_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0x89: /* ADC A,C    */ OP_ADC_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0x8a: /* ADC A,D    */ OP_ADC_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0x8b: /* ADC A,E    */ OP_ADC_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0x8c: /* ADC A,H    */ OP_ADC_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0x8d: /* ADC A,L    */ OP_ADC_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0x8e: /* ADC A,(HL) */ OP_ADC_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0x8f: /* ADC A,A    */ OP_ADC_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0x90: /* SUB A,B    */ OP_SUB_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0x91: /* SUB A,C    */ OP_SUB_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0x92: /* SUB A,D    */ OP_SUB_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0x93: /* SUB A,E    */ OP_SUB_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0x94: /* SUB A,H    */ OP_SUB_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0x95: /* SUB A,L    */ OP_SUB_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0x96: /* SUB A,(HL) */ OP_SUB_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0x97: /* SUB A,A    */ OP_SUB_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0x98: /* SBC A,B    */ OP_SBC_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0x99: /* SBC A,C    */ OP_SBC_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0x9a: /* SBC A,D    */ OP_SBC_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0x9b: /* SBC A,E    */ OP_SBC_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0x9c: /* SBC A,H    */ OP_SBC_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0x9d: /* SBC A,L    */ OP_SBC_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0x9e: /* SBC A,(HL) */ OP_SBC_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0x9f: /* SBC A,A    */ OP_SBC_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0xa0: /* AND A,B    */ OP_AND_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0xa1: /* AND A,C    */ OP_AND_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0xa2: /* AND A,D    */ OP_AND_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0xa3: /* AND A,E    */ OP_AND_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0xa4: /* AND A,H    */ OP_AND_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0xa5: /* AND A,L    */ OP_AND_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0xa6: /* AND A,(HL) */ OP_AND_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0xa7: /* AND A,A    */ OP_AND_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0xa8: /* XOR A,B    */ OP_XOR_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0xa9: /* XOR A,C    */ OP_XOR_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0xaa: /* XOR A,D    */ OP_XOR_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0xab: /* XOR A,E    */ OP_XOR_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0xac: /* XOR A,H    */ OP_XOR_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0xad: /* XOR A,L    */ OP_XOR_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0xae: /* XOR A,(HL) */ OP_XOR_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0xaf: /* XOR A,A    */ OP_XOR_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0xb0: /* OR A,B     */ OP_IOR_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0xb1: /* OR A,C     */ OP_IOR_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0xb2: /* OR A,D     */ OP_IOR_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0xb3: /* OR A,E     */ OP_IOR_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0xb4: /* OR A,H     */ OP_IOR_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0xb5: /* OR A,L     */ OP_IOR_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0xb6: /* OR A,(HL)  */ OP_IOR_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0xb7: /* OR A,A     */ OP_IOR_RG08_RG08(AF_H, AF_H, 1, 4); break;
    case 0xb8: /* CP A,B     */ OP_CMP_RG08_RG08(AF_H, BC_H, 1, 4); break;
    case 0xb9: /* CP A,C     */ OP_CMP_RG08_RG08(AF_H, BC_L, 1, 4); break;
    case 0xba: /* CP A,D     */ OP_CMP_RG08_RG08(AF_H, DE_H, 1, 4); break;
    case 0xbb: /* CP A,E     */ OP_CMP_RG08_RG08(AF_H, DE_L, 1, 4); break;
    case 0xbc: /* CP A,H     */ OP_CMP_RG08_RG08(AF_H, HL_H, 1, 4); break;
    case 0xbd: /* CP A,L     */ OP_CMP_RG08_RG08(AF_H, HL_L, 1, 4); break;
    case 0xbe: /* CP A,(HL)  */ OP_CMP_RG08_MM08(AF_H, HL_W, 2, 7); break;
    case 0xbf: /* CP A,A     */ OP_CMP_RG08_RG08(AF_H, AF_H, 1, 4); break;
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
#ifdef BLORK_CB
    case 0x00: /* RLC B      */ OP_RLC_RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x01: /* RLC C      */ OP_RLC_RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x02: /* RLC D      */ OP_RLC_RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x03: /* RLC E      */ OP_RLC_RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x04: /* RLC H      */ OP_RLC_RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x05: /* RLC L      */ OP_RLC_RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x06: /* RLC (HL)   */ OP_RLC_MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x07: /* RLC A      */ OP_RLC_RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x08: /* RRC B      */ OP_RRC_RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x09: /* RRC C      */ OP_RRC_RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x0a: /* RRC D      */ OP_RRC_RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x0b: /* RRC E      */ OP_RRC_RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x0c: /* RRC H      */ OP_RRC_RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x0d: /* RRC L      */ OP_RRC_RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x0e: /* RRC (HL)   */ OP_RRC_MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x0f: /* RRC A      */ OP_RRC_RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x10: /* RL B       */ OP_RL__RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x11: /* RL C       */ OP_RL__RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x12: /* RL D       */ OP_RL__RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x13: /* RL E       */ OP_RL__RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x14: /* RL H       */ OP_RL__RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x15: /* RL L       */ OP_RL__RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x16: /* RL (HL)    */ OP_RL__MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x17: /* RL A       */ OP_RL__RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x18: /* RR B       */ OP_RR__RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x19: /* RR C       */ OP_RR__RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x1a: /* RR D       */ OP_RR__RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x1b: /* RR E       */ OP_RR__RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x1c: /* RR H       */ OP_RR__RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x1d: /* RR L       */ OP_RR__RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x1e: /* RR (HL)    */ OP_RR__MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x1f: /* RR A       */ OP_RR__RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x20: /* SLA B      */ OP_SLA_RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x21: /* SLA C      */ OP_SLA_RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x22: /* SLA D      */ OP_SLA_RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x23: /* SLA E      */ OP_SLA_RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x24: /* SLA H      */ OP_SLA_RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x25: /* SLA L      */ OP_SLA_RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x26: /* SLA (HL)   */ OP_SLA_MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x27: /* SLA A      */ OP_SLA_RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x28: /* SRA B      */ OP_SRA_RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x29: /* SRA C      */ OP_SRA_RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x2a: /* SRA D      */ OP_SRA_RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x2b: /* SRA E      */ OP_SRA_RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x2c: /* SRA H      */ OP_SRA_RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x2d: /* SRA L      */ OP_SRA_RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x2e: /* SRA (HL)   */ OP_SRA_MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x2f: /* SRA A      */ OP_SRA_RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x30: /* SLL B      */ OP_SLL_RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x31: /* SLL C      */ OP_SLL_RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x32: /* SLL D      */ OP_SLL_RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x33: /* SLL E      */ OP_SLL_RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x34: /* SLL H      */ OP_SLL_RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x35: /* SLL L      */ OP_SLL_RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x36: /* SLL (HL)   */ OP_SLL_MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x37: /* SLL A      */ OP_SLL_RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x38: /* SRL B      */ OP_SRL_RG08_NULL(BC_H, NULL, 2,  8); break;
    case 0x39: /* SRL C      */ OP_SRL_RG08_NULL(BC_L, NULL, 2,  8); break;
    case 0x3a: /* SRL D      */ OP_SRL_RG08_NULL(DE_H, NULL, 2,  8); break;
    case 0x3b: /* SRL E      */ OP_SRL_RG08_NULL(DE_L, NULL, 2,  8); break;
    case 0x3c: /* SRL H      */ OP_SRL_RG08_NULL(HL_H, NULL, 2,  8); break;
    case 0x3d: /* SRL L      */ OP_SRL_RG08_NULL(HL_L, NULL, 2,  8); break;
    case 0x3e: /* SRL (HL)   */ OP_SRL_MM08_NULL(HL_W, NULL, 4, 15); break;
    case 0x3f: /* SRL A      */ OP_SRL_RG08_NULL(AF_H, NULL, 2,  8); break;
    case 0x40: /* BIT 0,B    */ OP_BIT_RG08_MASK(BC_H, 0x01, 2,  8); break;
    case 0x41: /* BIT 0,C    */ OP_BIT_RG08_MASK(BC_L, 0x01, 2,  8); break;
    case 0x42: /* BIT 0,D    */ OP_BIT_RG08_MASK(DE_H, 0x01, 2,  8); break;
    case 0x43: /* BIT 0,E    */ OP_BIT_RG08_MASK(DE_L, 0x01, 2,  8); break;
    case 0x44: /* BIT 0,H    */ OP_BIT_RG08_MASK(HL_H, 0x01, 2,  8); break;
    case 0x45: /* BIT 0,L    */ OP_BIT_RG08_MASK(HL_L, 0x01, 2,  8); break;
    case 0x46: /* BIT 0,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x01, 3, 12); break;
    case 0x47: /* BIT 0,A    */ OP_BIT_RG08_MASK(AF_H, 0x01, 2,  8); break;
    case 0x48: /* BIT 1,B    */ OP_BIT_RG08_MASK(BC_H, 0x02, 2,  8); break;
    case 0x49: /* BIT 1,C    */ OP_BIT_RG08_MASK(BC_L, 0x02, 2,  8); break;
    case 0x4a: /* BIT 1,D    */ OP_BIT_RG08_MASK(DE_H, 0x02, 2,  8); break;
    case 0x4b: /* BIT 1,E    */ OP_BIT_RG08_MASK(DE_L, 0x02, 2,  8); break;
    case 0x4c: /* BIT 1,H    */ OP_BIT_RG08_MASK(HL_H, 0x02, 2,  8); break;
    case 0x4d: /* BIT 1,L    */ OP_BIT_RG08_MASK(HL_L, 0x02, 2,  8); break;
    case 0x4e: /* BIT 1,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x02, 3, 12); break;
    case 0x4f: /* BIT 1,A    */ OP_BIT_RG08_MASK(AF_H, 0x02, 2,  8); break;
    case 0x50: /* BIT 2,B    */ OP_BIT_RG08_MASK(BC_H, 0x04, 2,  8); break;
    case 0x51: /* BIT 2,C    */ OP_BIT_RG08_MASK(BC_L, 0x04, 2,  8); break;
    case 0x52: /* BIT 2,D    */ OP_BIT_RG08_MASK(DE_H, 0x04, 2,  8); break;
    case 0x53: /* BIT 2,E    */ OP_BIT_RG08_MASK(DE_L, 0x04, 2,  8); break;
    case 0x54: /* BIT 2,H    */ OP_BIT_RG08_MASK(HL_H, 0x04, 2,  8); break;
    case 0x55: /* BIT 2,L    */ OP_BIT_RG08_MASK(HL_L, 0x04, 2,  8); break;
    case 0x56: /* BIT 2,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x04, 3, 12); break;
    case 0x57: /* BIT 2,A    */ OP_BIT_RG08_MASK(AF_H, 0x04, 2,  8); break;
    case 0x58: /* BIT 3,B    */ OP_BIT_RG08_MASK(BC_H, 0x08, 2,  8); break;
    case 0x59: /* BIT 3,C    */ OP_BIT_RG08_MASK(BC_L, 0x08, 2,  8); break;
    case 0x5a: /* BIT 3,D    */ OP_BIT_RG08_MASK(DE_H, 0x08, 2,  8); break;
    case 0x5b: /* BIT 3,E    */ OP_BIT_RG08_MASK(DE_L, 0x08, 2,  8); break;
    case 0x5c: /* BIT 3,H    */ OP_BIT_RG08_MASK(HL_H, 0x08, 2,  8); break;
    case 0x5d: /* BIT 3,L    */ OP_BIT_RG08_MASK(HL_L, 0x08, 2,  8); break;
    case 0x5e: /* BIT 3,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x08, 3, 12); break;
    case 0x5f: /* BIT 3,A    */ OP_BIT_RG08_MASK(AF_H, 0x08, 2,  8); break;
    case 0x60: /* BIT 4,B    */ OP_BIT_RG08_MASK(BC_H, 0x10, 2,  8); break;
    case 0x61: /* BIT 4,C    */ OP_BIT_RG08_MASK(BC_L, 0x10, 2,  8); break;
    case 0x62: /* BIT 4,D    */ OP_BIT_RG08_MASK(DE_H, 0x10, 2,  8); break;
    case 0x63: /* BIT 4,E    */ OP_BIT_RG08_MASK(DE_L, 0x10, 2,  8); break;
    case 0x64: /* BIT 4,H    */ OP_BIT_RG08_MASK(HL_H, 0x10, 2,  8); break;
    case 0x65: /* BIT 4,L    */ OP_BIT_RG08_MASK(HL_L, 0x10, 2,  8); break;
    case 0x66: /* BIT 4,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x10, 3, 12); break;
    case 0x67: /* BIT 4,A    */ OP_BIT_RG08_MASK(AF_H, 0x10, 2,  8); break;
    case 0x68: /* BIT 5,B    */ OP_BIT_RG08_MASK(BC_H, 0x20, 2,  8); break;
    case 0x69: /* BIT 5,C    */ OP_BIT_RG08_MASK(BC_L, 0x20, 2,  8); break;
    case 0x6a: /* BIT 5,D    */ OP_BIT_RG08_MASK(DE_H, 0x20, 2,  8); break;
    case 0x6b: /* BIT 5,E    */ OP_BIT_RG08_MASK(DE_L, 0x20, 2,  8); break;
    case 0x6c: /* BIT 5,H    */ OP_BIT_RG08_MASK(HL_H, 0x20, 2,  8); break;
    case 0x6d: /* BIT 5,L    */ OP_BIT_RG08_MASK(HL_L, 0x20, 2,  8); break;
    case 0x6e: /* BIT 5,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x20, 3, 12); break;
    case 0x6f: /* BIT 5,A    */ OP_BIT_RG08_MASK(AF_H, 0x20, 2,  8); break;
    case 0x70: /* BIT 6,B    */ OP_BIT_RG08_MASK(BC_H, 0x40, 2,  8); break;
    case 0x71: /* BIT 6,C    */ OP_BIT_RG08_MASK(BC_L, 0x40, 2,  8); break;
    case 0x72: /* BIT 6,D    */ OP_BIT_RG08_MASK(DE_H, 0x40, 2,  8); break;
    case 0x73: /* BIT 6,E    */ OP_BIT_RG08_MASK(DE_L, 0x40, 2,  8); break;
    case 0x74: /* BIT 6,H    */ OP_BIT_RG08_MASK(HL_H, 0x40, 2,  8); break;
    case 0x75: /* BIT 6,L    */ OP_BIT_RG08_MASK(HL_L, 0x40, 2,  8); break;
    case 0x76: /* BIT 6,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x40, 3, 12); break;
    case 0x77: /* BIT 6,A    */ OP_BIT_RG08_MASK(AF_H, 0x40, 2,  8); break;
    case 0x78: /* BIT 7,B    */ OP_BIT_RG08_MASK(BC_H, 0x80, 2,  8); break;
    case 0x79: /* BIT 7,C    */ OP_BIT_RG08_MASK(BC_L, 0x80, 2,  8); break;
    case 0x7a: /* BIT 7,D    */ OP_BIT_RG08_MASK(DE_H, 0x80, 2,  8); break;
    case 0x7b: /* BIT 7,E    */ OP_BIT_RG08_MASK(DE_L, 0x80, 2,  8); break;
    case 0x7c: /* BIT 7,H    */ OP_BIT_RG08_MASK(HL_H, 0x80, 2,  8); break;
    case 0x7d: /* BIT 7,L    */ OP_BIT_RG08_MASK(HL_L, 0x80, 2,  8); break;
    case 0x7e: /* BIT 7,(HL) */ OP_BIT_MM08_MASK(HL_W, 0x80, 3, 12); break;
    case 0x7f: /* BIT 7,A    */ OP_BIT_RG08_MASK(AF_H, 0x80, 2,  8); break;
    case 0x80: /* RES 0,B    */ OP_RES_RG08_MASK(BC_H, 0x01, 2,  8); break;
    case 0x81: /* RES 0,C    */ OP_RES_RG08_MASK(BC_L, 0x01, 2,  8); break;
    case 0x82: /* RES 0,D    */ OP_RES_RG08_MASK(DE_H, 0x01, 2,  8); break;
    case 0x83: /* RES 0,E    */ OP_RES_RG08_MASK(DE_L, 0x01, 2,  8); break;
    case 0x84: /* RES 0,H    */ OP_RES_RG08_MASK(HL_H, 0x01, 2,  8); break;
    case 0x85: /* RES 0,L    */ OP_RES_RG08_MASK(HL_L, 0x01, 2,  8); break;
    case 0x86: /* RES 0,(HL) */ OP_RES_MM08_MASK(HL_W, 0x01, 4, 15); break;
    case 0x87: /* RES 0,A    */ OP_RES_RG08_MASK(AF_H, 0x01, 2,  8); break;
    case 0x88: /* RES 1,B    */ OP_RES_RG08_MASK(BC_H, 0x02, 2,  8); break;
    case 0x89: /* RES 1,C    */ OP_RES_RG08_MASK(BC_L, 0x02, 2,  8); break;
    case 0x8a: /* RES 1,D    */ OP_RES_RG08_MASK(DE_H, 0x02, 2,  8); break;
    case 0x8b: /* RES 1,E    */ OP_RES_RG08_MASK(DE_L, 0x02, 2,  8); break;
    case 0x8c: /* RES 1,H    */ OP_RES_RG08_MASK(HL_H, 0x02, 2,  8); break;
    case 0x8d: /* RES 1,L    */ OP_RES_RG08_MASK(HL_L, 0x02, 2,  8); break;
    case 0x8e: /* RES 1,(HL) */ OP_RES_MM08_MASK(HL_W, 0x02, 4, 15); break;
    case 0x8f: /* RES 1,A    */ OP_RES_RG08_MASK(AF_H, 0x02, 2,  8); break;
    case 0x90: /* RES 2,B    */ OP_RES_RG08_MASK(BC_H, 0x04, 2,  8); break;
    case 0x91: /* RES 2,C    */ OP_RES_RG08_MASK(BC_L, 0x04, 2,  8); break;
    case 0x92: /* RES 2,D    */ OP_RES_RG08_MASK(DE_H, 0x04, 2,  8); break;
    case 0x93: /* RES 2,E    */ OP_RES_RG08_MASK(DE_L, 0x04, 2,  8); break;
    case 0x94: /* RES 2,H    */ OP_RES_RG08_MASK(HL_H, 0x04, 2,  8); break;
    case 0x95: /* RES 2,L    */ OP_RES_RG08_MASK(HL_L, 0x04, 2,  8); break;
    case 0x96: /* RES 2,(HL) */ OP_RES_MM08_MASK(HL_W, 0x04, 4, 15); break;
    case 0x97: /* RES 2,A    */ OP_RES_RG08_MASK(AF_H, 0x04, 2,  8); break;
    case 0x98: /* RES 3,B    */ OP_RES_RG08_MASK(BC_H, 0x08, 2,  8); break;
    case 0x99: /* RES 3,C    */ OP_RES_RG08_MASK(BC_L, 0x08, 2,  8); break;
    case 0x9a: /* RES 3,D    */ OP_RES_RG08_MASK(DE_H, 0x08, 2,  8); break;
    case 0x9b: /* RES 3,E    */ OP_RES_RG08_MASK(DE_L, 0x08, 2,  8); break;
    case 0x9c: /* RES 3,H    */ OP_RES_RG08_MASK(HL_H, 0x08, 2,  8); break;
    case 0x9d: /* RES 3,L    */ OP_RES_RG08_MASK(HL_L, 0x08, 2,  8); break;
    case 0x9e: /* RES 3,(HL) */ OP_RES_MM08_MASK(HL_W, 0x08, 4, 15); break;
    case 0x9f: /* RES 3,A    */ OP_RES_RG08_MASK(AF_H, 0x08, 2,  8); break;
    case 0xa0: /* RES 4,B    */ OP_RES_RG08_MASK(BC_H, 0x10, 2,  8); break;
    case 0xa1: /* RES 4,C    */ OP_RES_RG08_MASK(BC_L, 0x10, 2,  8); break;
    case 0xa2: /* RES 4,D    */ OP_RES_RG08_MASK(DE_H, 0x10, 2,  8); break;
    case 0xa3: /* RES 4,E    */ OP_RES_RG08_MASK(DE_L, 0x10, 2,  8); break;
    case 0xa4: /* RES 4,H    */ OP_RES_RG08_MASK(HL_H, 0x10, 2,  8); break;
    case 0xa5: /* RES 4,L    */ OP_RES_RG08_MASK(HL_L, 0x10, 2,  8); break;
    case 0xa6: /* RES 4,(HL) */ OP_RES_MM08_MASK(HL_W, 0x10, 4, 15); break;
    case 0xa7: /* RES 4,A    */ OP_RES_RG08_MASK(AF_H, 0x10, 2,  8); break;
    case 0xa8: /* RES 5,B    */ OP_RES_RG08_MASK(BC_H, 0x20, 2,  8); break;
    case 0xa9: /* RES 5,C    */ OP_RES_RG08_MASK(BC_L, 0x20, 2,  8); break;
    case 0xaa: /* RES 5,D    */ OP_RES_RG08_MASK(DE_H, 0x20, 2,  8); break;
    case 0xab: /* RES 5,E    */ OP_RES_RG08_MASK(DE_L, 0x20, 2,  8); break;
    case 0xac: /* RES 5,H    */ OP_RES_RG08_MASK(HL_H, 0x20, 2,  8); break;
    case 0xad: /* RES 5,L    */ OP_RES_RG08_MASK(HL_L, 0x20, 2,  8); break;
    case 0xae: /* RES 5,(HL) */ OP_RES_MM08_MASK(HL_W, 0x20, 4, 15); break;
    case 0xaf: /* RES 5,A    */ OP_RES_RG08_MASK(AF_H, 0x20, 2,  8); break;
    case 0xb0: /* RES 6,B    */ OP_RES_RG08_MASK(BC_H, 0x40, 2,  8); break;
    case 0xb1: /* RES 6,C    */ OP_RES_RG08_MASK(BC_L, 0x40, 2,  8); break;
    case 0xb2: /* RES 6,D    */ OP_RES_RG08_MASK(DE_H, 0x40, 2,  8); break;
    case 0xb3: /* RES 6,E    */ OP_RES_RG08_MASK(DE_L, 0x40, 2,  8); break;
    case 0xb4: /* RES 6,H    */ OP_RES_RG08_MASK(HL_H, 0x40, 2,  8); break;
    case 0xb5: /* RES 6,L    */ OP_RES_RG08_MASK(HL_L, 0x40, 2,  8); break;
    case 0xb6: /* RES 6,(HL) */ OP_RES_MM08_MASK(HL_W, 0x40, 4, 15); break;
    case 0xb7: /* RES 6,A    */ OP_RES_RG08_MASK(AF_H, 0x40, 2,  8); break;
    case 0xb8: /* RES 7,B    */ OP_RES_RG08_MASK(BC_H, 0x80, 2,  8); break;
    case 0xb9: /* RES 7,C    */ OP_RES_RG08_MASK(BC_L, 0x80, 2,  8); break;
    case 0xba: /* RES 7,D    */ OP_RES_RG08_MASK(DE_H, 0x80, 2,  8); break;
    case 0xbb: /* RES 7,E    */ OP_RES_RG08_MASK(DE_L, 0x80, 2,  8); break;
    case 0xbc: /* RES 7,H    */ OP_RES_RG08_MASK(HL_H, 0x80, 2,  8); break;
    case 0xbd: /* RES 7,L    */ OP_RES_RG08_MASK(HL_L, 0x80, 2,  8); break;
    case 0xbe: /* RES 7,(HL) */ OP_RES_MM08_MASK(HL_W, 0x80, 4, 15); break;
    case 0xbf: /* RES 7,A    */ OP_RES_RG08_MASK(AF_H, 0x80, 2,  8); break;
    case 0xc0: /* SET 0,B    */ OP_SET_RG08_MASK(BC_H, 0x01, 2,  8); break;
    case 0xc1: /* SET 0,C    */ OP_SET_RG08_MASK(BC_L, 0x01, 2,  8); break;
    case 0xc2: /* SET 0,D    */ OP_SET_RG08_MASK(DE_H, 0x01, 2,  8); break;
    case 0xc3: /* SET 0,E    */ OP_SET_RG08_MASK(DE_L, 0x01, 2,  8); break;
    case 0xc4: /* SET 0,H    */ OP_SET_RG08_MASK(HL_H, 0x01, 2,  8); break;
    case 0xc5: /* SET 0,L    */ OP_SET_RG08_MASK(HL_L, 0x01, 2,  8); break;
    case 0xc6: /* SET 0,(HL) */ OP_SET_MM08_MASK(HL_W, 0x01, 4, 15); break;
    case 0xc7: /* SET 0,A    */ OP_SET_RG08_MASK(AF_H, 0x01, 2,  8); break;
    case 0xc8: /* SET 1,B    */ OP_SET_RG08_MASK(BC_H, 0x02, 2,  8); break;
    case 0xc9: /* SET 1,C    */ OP_SET_RG08_MASK(BC_L, 0x02, 2,  8); break;
    case 0xca: /* SET 1,D    */ OP_SET_RG08_MASK(DE_H, 0x02, 2,  8); break;
    case 0xcb: /* SET 1,E    */ OP_SET_RG08_MASK(DE_L, 0x02, 2,  8); break;
    case 0xcc: /* SET 1,H    */ OP_SET_RG08_MASK(HL_H, 0x02, 2,  8); break;
    case 0xcd: /* SET 1,L    */ OP_SET_RG08_MASK(HL_L, 0x02, 2,  8); break;
    case 0xce: /* SET 1,(HL) */ OP_SET_MM08_MASK(HL_W, 0x02, 4, 15); break;
    case 0xcf: /* SET 1,A    */ OP_SET_RG08_MASK(AF_H, 0x02, 2,  8); break;
    case 0xd0: /* SET 2,B    */ OP_SET_RG08_MASK(BC_H, 0x04, 2,  8); break;
    case 0xd1: /* SET 2,C    */ OP_SET_RG08_MASK(BC_L, 0x04, 2,  8); break;
    case 0xd2: /* SET 2,D    */ OP_SET_RG08_MASK(DE_H, 0x04, 2,  8); break;
    case 0xd3: /* SET 2,E    */ OP_SET_RG08_MASK(DE_L, 0x04, 2,  8); break;
    case 0xd4: /* SET 2,H    */ OP_SET_RG08_MASK(HL_H, 0x04, 2,  8); break;
    case 0xd5: /* SET 2,L    */ OP_SET_RG08_MASK(HL_L, 0x04, 2,  8); break;
    case 0xd6: /* SET 2,(HL) */ OP_SET_MM08_MASK(HL_W, 0x04, 4, 15); break;
    case 0xd7: /* SET 2,A    */ OP_SET_RG08_MASK(AF_H, 0x04, 2,  8); break;
    case 0xd8: /* SET 3,B    */ OP_SET_RG08_MASK(BC_H, 0x08, 2,  8); break;
    case 0xd9: /* SET 3,C    */ OP_SET_RG08_MASK(BC_L, 0x08, 2,  8); break;
    case 0xda: /* SET 3,D    */ OP_SET_RG08_MASK(DE_H, 0x08, 2,  8); break;
    case 0xdb: /* SET 3,E    */ OP_SET_RG08_MASK(DE_L, 0x08, 2,  8); break;
    case 0xdc: /* SET 3,H    */ OP_SET_RG08_MASK(HL_H, 0x08, 2,  8); break;
    case 0xdd: /* SET 3,L    */ OP_SET_RG08_MASK(HL_L, 0x08, 2,  8); break;
    case 0xde: /* SET 3,(HL) */ OP_SET_MM08_MASK(HL_W, 0x08, 4, 15); break;
    case 0xdf: /* SET 3,A    */ OP_SET_RG08_MASK(AF_H, 0x08, 2,  8); break;
    case 0xe0: /* SET 4,B    */ OP_SET_RG08_MASK(BC_H, 0x10, 2,  8); break;
    case 0xe1: /* SET 4,C    */ OP_SET_RG08_MASK(BC_L, 0x10, 2,  8); break;
    case 0xe2: /* SET 4,D    */ OP_SET_RG08_MASK(DE_H, 0x10, 2,  8); break;
    case 0xe3: /* SET 4,E    */ OP_SET_RG08_MASK(DE_L, 0x10, 2,  8); break;
    case 0xe4: /* SET 4,H    */ OP_SET_RG08_MASK(HL_H, 0x10, 2,  8); break;
    case 0xe5: /* SET 4,L    */ OP_SET_RG08_MASK(HL_L, 0x10, 2,  8); break;
    case 0xe6: /* SET 4,(HL) */ OP_SET_MM08_MASK(HL_W, 0x10, 4, 15); break;
    case 0xe7: /* SET 4,A    */ OP_SET_RG08_MASK(AF_H, 0x10, 2,  8); break;
    case 0xe8: /* SET 5,B    */ OP_SET_RG08_MASK(BC_H, 0x20, 2,  8); break;
    case 0xe9: /* SET 5,C    */ OP_SET_RG08_MASK(BC_L, 0x20, 2,  8); break;
    case 0xea: /* SET 5,D    */ OP_SET_RG08_MASK(DE_H, 0x20, 2,  8); break;
    case 0xeb: /* SET 5,E    */ OP_SET_RG08_MASK(DE_L, 0x20, 2,  8); break;
    case 0xec: /* SET 5,H    */ OP_SET_RG08_MASK(HL_H, 0x20, 2,  8); break;
    case 0xed: /* SET 5,L    */ OP_SET_RG08_MASK(HL_L, 0x20, 2,  8); break;
    case 0xee: /* SET 5,(HL) */ OP_SET_MM08_MASK(HL_W, 0x20, 4, 15); break;
    case 0xef: /* SET 5,A    */ OP_SET_RG08_MASK(AF_H, 0x20, 2,  8); break;
    case 0xf0: /* SET 6,B    */ OP_SET_RG08_MASK(BC_H, 0x40, 2,  8); break;
    case 0xf1: /* SET 6,C    */ OP_SET_RG08_MASK(BC_L, 0x40, 2,  8); break;
    case 0xf2: /* SET 6,D    */ OP_SET_RG08_MASK(DE_H, 0x40, 2,  8); break;
    case 0xf3: /* SET 6,E    */ OP_SET_RG08_MASK(DE_L, 0x40, 2,  8); break;
    case 0xf4: /* SET 6,H    */ OP_SET_RG08_MASK(HL_H, 0x40, 2,  8); break;
    case 0xf5: /* SET 6,L    */ OP_SET_RG08_MASK(HL_L, 0x40, 2,  8); break;
    case 0xf6: /* SET 6,(HL) */ OP_SET_MM08_MASK(HL_W, 0x40, 4, 15); break;
    case 0xf7: /* SET 6,A    */ OP_SET_RG08_MASK(AF_H, 0x40, 2,  8); break;
    case 0xf8: /* SET 7,B    */ OP_SET_RG08_MASK(BC_H, 0x80, 2,  8); break;
    case 0xf9: /* SET 7,C    */ OP_SET_RG08_MASK(BC_L, 0x80, 2,  8); break;
    case 0xfa: /* SET 7,D    */ OP_SET_RG08_MASK(DE_H, 0x80, 2,  8); break;
    case 0xfb: /* SET 7,E    */ OP_SET_RG08_MASK(DE_L, 0x80, 2,  8); break;
    case 0xfc: /* SET 7,H    */ OP_SET_RG08_MASK(HL_H, 0x80, 2,  8); break;
    case 0xfd: /* SET 7,L    */ OP_SET_RG08_MASK(HL_L, 0x80, 2,  8); break;
    case 0xfe: /* SET 7,(HL) */ OP_SET_MM08_MASK(HL_W, 0x80, 4, 15); break;
    case 0xff: /* SET 7,A    */ OP_SET_RG08_MASK(AF_H, 0x80, 2,  8); break;
#endif
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
