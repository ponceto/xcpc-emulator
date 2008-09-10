/*
 * z80cpu.c - Copyright (c) 2001, 2006, 2007, 2008 Olivier Poncet
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

#define _SF     0x80 /* Sign                   */
#define _ZF     0x40 /* Zero                   */
#define _5F     0x20 /* Undocumented           */
#define _HF     0x10 /* HalfCarry / HalfBorrow */
#define _3F     0x08 /* Undocumented           */
#define _PF     0x04 /* Parity                 */
#define _OF     0x04 /* Overflow               */
#define _NF     0x02 /* Add / Sub              */
#define _CF     0x01 /* Carry / Borrow         */

#define _ADD    0x00 /* ADD operation          */
#define _ADC    0x00 /* ADC operation          */
#define _SUB    0x02 /* SUB operation          */
#define _SBC    0x02 /* SBC operation          */
#define _AND    0x10 /* AND operation          */
#define _XOR    0x00 /* XOR operation          */
#define _IOR    0x00 /* IOR operation          */
#define _CMP    0x02 /* CMP operation          */

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
  z80cpu->ccounter = 0;
}

/**
 * GdevZ80CPU::clock()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_clock(GdevZ80CPU *z80cpu)
{
  guint8 opcode;
  guint8 TMP1;
  guint8 TMP2;
  GdevZ80REG WZ;

next:
  if(z80cpu->ccounter <= 0) {
    return;
  }
  else if(z80cpu->IF.W & IFF_NMI) {
    z80cpu->IF.W &= ~(IFF_HLT | IFF_NMI | IFF_INT | IFF_2);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.h);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.l);
    z80cpu->PC.W = 0x0066;
    z80cpu->m_cycles += 1;
    z80cpu->t_states += 11;
    z80cpu->ccounter -= 11;
    goto next;
  }
  else if(z80cpu->IF.W & IFF_INT) {
    z80cpu->IF.W &= ~(IFF_HLT | IFF_NMI | IFF_INT | IFF_2 | IFF_1);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.h);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.l);
    z80cpu->PC.W = 0x0038;
    z80cpu->m_cycles += 1;
    z80cpu->t_states += 13;
    z80cpu->ccounter -= 13;
    goto next;
  }
  else if(z80cpu->IF.W & IFF_HLT) {
    z80cpu->m_cycles += 1;
    z80cpu->t_states += 4;
    z80cpu->ccounter -= 4;
    goto next;
  }
fetch:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->PC.W++);
execute:
  switch(opcode) {
#include "z80cpu_opcode.h"
    case 0x00: /* NOP           */
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x03: /* INC BC        */
      z80cpu->BC.W++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x06: /* LD B,n        */
      z80cpu->BC.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x0b: /* DEC BC        */
      z80cpu->BC.W--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x0e: /* LD C,n        */
      z80cpu->BC.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x13: /* INC DE        */
      z80cpu->DE.W++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x16: /* LD D,n        */
      z80cpu->DE.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x1b: /* DEC DE        */
      z80cpu->DE.W--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x1e: /* LD E,n        */
      z80cpu->DE.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x23: /* INC HL        */
      z80cpu->HL.W++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x26: /* LD H,n        */
      z80cpu->HL.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x27: /* DAA           */
      WZ.W = z80cpu->AF.B.h;
      if(z80cpu->AF.B.l & _CF) WZ.W |= 0x100;
      if(z80cpu->AF.B.l & _HF) WZ.W |= 0x200;
      if(z80cpu->AF.B.l & _NF) WZ.W |= 0x400;
      z80cpu->AF.W = DAATable[WZ.W];
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x2b: /* DEC HL        */
      z80cpu->HL.W--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x2e: /* LD L,n        */
      z80cpu->HL.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x33: /* INC SP        */
      z80cpu->SP.W++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x3b: /* DEC SP        */
      z80cpu->SP.W--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x3e: /* LD A,n        */
      z80cpu->AF.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x40: /* LD B,B        */
      z80cpu->BC.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x41: /* LD B,C        */
      z80cpu->BC.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x42: /* LD B,D        */
      z80cpu->BC.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x43: /* LD B,E        */
      z80cpu->BC.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x44: /* LD B,H        */
      z80cpu->BC.B.h = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x45: /* LD B,L        */
      z80cpu->BC.B.h = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x46: /* LD B,(HL)     */
      z80cpu->BC.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x47: /* LD B,A        */
      z80cpu->BC.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x48: /* LD C,B        */
      z80cpu->BC.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x49: /* LD C,C        */
      z80cpu->BC.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4a: /* LD C,D        */
      z80cpu->BC.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4b: /* LD C,E        */
      z80cpu->BC.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4c: /* LD C,H        */
      z80cpu->BC.B.l = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4d: /* LD C,L        */
      z80cpu->BC.B.l = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4e: /* LD C,(HL)     */
      z80cpu->BC.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x4f: /* LD C,A        */
      z80cpu->BC.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x50: /* LD D,B        */
      z80cpu->DE.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x51: /* LD D,C        */
      z80cpu->DE.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x52: /* LD D,D        */
      z80cpu->DE.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x53: /* LD D,E        */
      z80cpu->DE.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x54: /* LD D,H        */
      z80cpu->DE.B.h = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x55: /* LD D,L        */
      z80cpu->DE.B.h = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x56: /* LD D,(HL)     */
      z80cpu->DE.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x57: /* LD D,A        */
      z80cpu->DE.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x58: /* LD E,B        */
      z80cpu->DE.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x59: /* LD E,C        */
      z80cpu->DE.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5a: /* LD E,D        */
      z80cpu->DE.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5b: /* LD E,E        */
      z80cpu->DE.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5c: /* LD E,H        */
      z80cpu->DE.B.l = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5d: /* LD E,L        */
      z80cpu->DE.B.l = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5e: /* LD E,(HL)     */
      z80cpu->DE.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x5f: /* LD E,A        */
      z80cpu->DE.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x60: /* LD H,B        */
      z80cpu->HL.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x61: /* LD H,C        */
      z80cpu->HL.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x62: /* LD H,D        */
      z80cpu->HL.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x63: /* LD H,E        */
      z80cpu->HL.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x64: /* LD H,H        */
      z80cpu->HL.B.h = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x65: /* LD H,L        */
      z80cpu->HL.B.h = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x66: /* LD H,(HL)     */
      z80cpu->HL.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x67: /* LD H,A        */
      z80cpu->HL.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x68: /* LD L,B        */
      z80cpu->HL.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x69: /* LD L,C        */
      z80cpu->HL.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6a: /* LD L,D        */
      z80cpu->HL.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6b: /* LD L,E        */
      z80cpu->HL.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6c: /* LD L,H        */
      z80cpu->HL.B.l = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6d: /* LD L,L        */
      z80cpu->HL.B.l = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6e: /* LD L,(HL)     */
      z80cpu->HL.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x6f: /* LD L,A        */
      z80cpu->HL.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x70: /* LD (HL),B     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->BC.B.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x71: /* LD (HL),C     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->BC.B.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x72: /* LD (HL),D     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->DE.B.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x73: /* LD (HL),E     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->DE.B.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x74: /* LD (HL),H     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->HL.B.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x75: /* LD (HL),L     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->HL.B.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x76: /* HALT          */
      z80cpu->IF.W |= IFF_HLT;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x77: /* LD (HL),A     */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, z80cpu->AF.B.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x78: /* LD A,B        */
      z80cpu->AF.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x79: /* LD A,C        */
      z80cpu->AF.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7a: /* LD A,D        */
      z80cpu->AF.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7b: /* LD A,E        */
      z80cpu->AF.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7c: /* LD A,H        */
      z80cpu->AF.B.h = z80cpu->HL.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7d: /* LD A,L        */
      z80cpu->AF.B.h = z80cpu->HL.B.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7e: /* LD A,(HL)     */
      z80cpu->AF.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x7f: /* LD A,A        */
      z80cpu->AF.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x80: /* ADD A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x81: /* ADD A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x82: /* ADD A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x83: /* ADD A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x84: /* ADD A,H       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x85: /* ADD A,L       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x86: /* ADD A,(HL)    */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x87: /* ADD A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x88: /* ADC A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x89: /* ADC A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8a: /* ADC A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8b: /* ADC A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8c: /* ADC A,H       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8d: /* ADC A,L       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8e: /* ADC A,(HL)    */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x8f: /* ADC A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x90: /* SUB A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x91: /* SUB A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x92: /* SUB A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x93: /* SUB A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x94: /* SUB A,H       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x95: /* SUB A,L       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x96: /* SUB A,(HL)    */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x97: /* SUB A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x98: /* SBC A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x99: /* SBC A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9a: /* SBC A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9b: /* SBC A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9c: /* SBC A,H       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9d: /* SBC A,L       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9e: /* SBC A,(HL)    */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x9f: /* SBC A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa0: /* AND A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa1: /* AND A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa2: /* AND A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa3: /* AND A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa4: /* AND A,H       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa5: /* AND A,L       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa6: /* AND A,(HL)    */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xa7: /* AND A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa8: /* XOR A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa9: /* XOR A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xaa: /* XOR A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xab: /* XOR A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xac: /* XOR A,H       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xad: /* XOR A,L       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xae: /* XOR A,(HL)    */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xaf: /* XOR A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb0: /* OR A,B        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb1: /* OR A,C        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb2: /* OR A,D        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb3: /* OR A,E        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb4: /* OR A,H        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb5: /* OR A,L        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb6: /* OR A,(HL)     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xb7: /* OR A,A        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb8: /* CP A,B        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb9: /* CP A,C        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xba: /* CP A,D        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbb: /* CP A,E        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbc: /* CP A,H        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbd: /* CP A,L        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->HL.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbe: /* CP A,(HL)     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xbf: /* CP A,A        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xcb:
      goto fetch_cb;
    case 0xdd:
      goto fetch_dd;
    case 0xed:
      goto fetch_ed;
    case 0xfd:
      goto fetch_fd;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: %02X: illegal opcode", (PC_W - 1), opcode);
      goto next;
  }
  z80cpu->ccounter -= Cycles[opcode];
  goto next;
fetch_cb:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->PC.W++);
execute_cb:
  switch(opcode) {
    case 0x00: /* RLC B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x01: /* RLC C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x02: /* RLC D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x03: /* RLC E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x04: /* RLC H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x05: /* RLC L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x06: /* RLC (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x07: /* RLC A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x08: /* RRC B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x09: /* RRC C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0a: /* RRC D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0b: /* RRC E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0c: /* RRC H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0d: /* RRC L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0e: /* RRC (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x0f: /* RRC A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x10: /* RL  B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x11: /* RL  C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x12: /* RL  D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x13: /* RL  E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x14: /* RL  H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x15: /* RL  L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x16: /* RL  (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x17: /* RL  A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x18: /* RR  B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x19: /* RR  C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1a: /* RR  D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1b: /* RR  E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1c: /* RR  H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1d: /* RR  L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1e: /* RR  (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x1f: /* RR  A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x20: /* SLA B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x21: /* SLA C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x22: /* SLA D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x23: /* SLA E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x24: /* SLA H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x25: /* SLA L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x26: /* SLA (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x27: /* SLA A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x28: /* SRA B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x29: /* SRA C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2a: /* SRA D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2b: /* SRA E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2c: /* SRA H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2d: /* SRA L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2e: /* SRA (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x2f: /* SRA A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x30: /* SLL B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x31: /* SLL C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x32: /* SLL D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x33: /* SLL E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x34: /* SLL H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x35: /* SLL L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x36: /* SLL (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x37: /* SLL A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x38: /* SRL B         */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x39: /* SRL C         */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->BC.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3a: /* SRL D         */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3b: /* SRL E         */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->DE.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3c: /* SRL H         */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3d: /* SRL L         */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->HL.B.l = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3e: /* SRL (HL)      */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x3f: /* SRL A         */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      z80cpu->AF.B.h = TMP2;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x40: /* BIT 0,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x41: /* BIT 0,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x42: /* BIT 0,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x43: /* BIT 0,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x44: /* BIT 0,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x45: /* BIT 0,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x46: /* BIT 0,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x47: /* BIT 0,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x48: /* BIT 1,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x49: /* BIT 1,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4a: /* BIT 1,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4b: /* BIT 1,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4c: /* BIT 1,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4d: /* BIT 1,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4e: /* BIT 1,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x4f: /* BIT 1,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x50: /* BIT 2,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x51: /* BIT 2,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x52: /* BIT 2,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x53: /* BIT 2,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x54: /* BIT 2,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x55: /* BIT 2,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x56: /* BIT 2,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x57: /* BIT 2,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x58: /* BIT 3,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x59: /* BIT 3,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5a: /* BIT 3,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5b: /* BIT 3,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5c: /* BIT 3,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5d: /* BIT 3,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5e: /* BIT 3,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x5f: /* BIT 3,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x60: /* BIT 4,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x61: /* BIT 4,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x62: /* BIT 4,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x63: /* BIT 4,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x64: /* BIT 4,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x65: /* BIT 4,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x66: /* BIT 4,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x67: /* BIT 4,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x68: /* BIT 5,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x69: /* BIT 5,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6a: /* BIT 5,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6b: /* BIT 5,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6c: /* BIT 5,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6d: /* BIT 5,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6e: /* BIT 5,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x6f: /* BIT 5,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x70: /* BIT 6,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x71: /* BIT 6,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x72: /* BIT 6,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x73: /* BIT 6,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x74: /* BIT 6,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x75: /* BIT 6,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x76: /* BIT 6,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x77: /* BIT 6,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x78: /* BIT 7,B       */
      TMP1 = z80cpu->BC.B.h;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x79: /* BIT 7,C       */
      TMP1 = z80cpu->BC.B.l;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7a: /* BIT 7,D       */
      TMP1 = z80cpu->DE.B.h;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7b: /* BIT 7,E       */
      TMP1 = z80cpu->DE.B.l;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7c: /* BIT 7,H       */
      TMP1 = z80cpu->HL.B.h;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7d: /* BIT 7,L       */
      TMP1 = z80cpu->HL.B.l;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7e: /* BIT 7,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x7f: /* BIT 7,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x80: /* RES 0,B       */
      z80cpu->BC.B.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x81: /* RES 0,C       */
      z80cpu->BC.B.l &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x82: /* RES 0,D       */
      z80cpu->DE.B.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x83: /* RES 0,E       */
      z80cpu->DE.B.l &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x84: /* RES 0,H       */
      z80cpu->HL.B.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x85: /* RES 0,L       */
      z80cpu->HL.B.l &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x86: /* RES 0,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x87: /* RES 0,A       */
      z80cpu->AF.B.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x88: /* RES 1,B       */
      z80cpu->BC.B.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x89: /* RES 1,C       */
      z80cpu->BC.B.l &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8a: /* RES 1,D       */
      z80cpu->DE.B.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8b: /* RES 1,E       */
      z80cpu->DE.B.l &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8c: /* RES 1,H       */
      z80cpu->HL.B.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8d: /* RES 1,L       */
      z80cpu->HL.B.l &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8e: /* RES 1,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x8f: /* RES 1,A       */
      z80cpu->AF.B.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x90: /* RES 2,B       */
      z80cpu->BC.B.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x91: /* RES 2,C       */
      z80cpu->BC.B.l &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x92: /* RES 2,D       */
      z80cpu->DE.B.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x93: /* RES 2,E       */
      z80cpu->DE.B.l &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x94: /* RES 2,H       */
      z80cpu->HL.B.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x95: /* RES 2,L       */
      z80cpu->HL.B.l &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x96: /* RES 2,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x97: /* RES 2,A       */
      z80cpu->AF.B.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x98: /* RES 3,B       */
      z80cpu->BC.B.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x99: /* RES 3,C       */
      z80cpu->BC.B.l &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9a: /* RES 3,D       */
      z80cpu->DE.B.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9b: /* RES 3,E       */
      z80cpu->DE.B.l &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9c: /* RES 3,H       */
      z80cpu->HL.B.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9d: /* RES 3,L       */
      z80cpu->HL.B.l &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9e: /* RES 3,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x9f: /* RES 3,A       */
      z80cpu->AF.B.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa0: /* RES 4,B       */
      z80cpu->BC.B.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa1: /* RES 4,C       */
      z80cpu->BC.B.l &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa2: /* RES 4,D       */
      z80cpu->DE.B.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa3: /* RES 4,E       */
      z80cpu->DE.B.l &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa4: /* RES 4,H       */
      z80cpu->HL.B.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa5: /* RES 4,L       */
      z80cpu->HL.B.l &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa6: /* RES 4,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xa7: /* RES 4,A       */
      z80cpu->AF.B.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa8: /* RES 5,B       */
      z80cpu->BC.B.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa9: /* RES 5,C       */
      z80cpu->BC.B.l &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xaa: /* RES 5,D       */
      z80cpu->DE.B.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xab: /* RES 5,E       */
      z80cpu->DE.B.l &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xac: /* RES 5,H       */
      z80cpu->HL.B.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xad: /* RES 5,L       */
      z80cpu->HL.B.l &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xae: /* RES 5,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xaf: /* RES 5,A       */
      z80cpu->AF.B.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb0: /* RES 6,B       */
      z80cpu->BC.B.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb1: /* RES 6,C       */
      z80cpu->BC.B.l &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb2: /* RES 6,D       */
      z80cpu->DE.B.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb3: /* RES 6,E       */
      z80cpu->DE.B.l &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb4: /* RES 6,H       */
      z80cpu->HL.B.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb5: /* RES 6,L       */
      z80cpu->HL.B.l &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb6: /* RES 6,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xb7: /* RES 6,A       */
      z80cpu->AF.B.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb8: /* RES 7,B       */
      z80cpu->BC.B.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb9: /* RES 7,C       */
      z80cpu->BC.B.l &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xba: /* RES 7,D       */
      z80cpu->DE.B.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbb: /* RES 7,E       */
      z80cpu->DE.B.l &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbc: /* RES 7,H       */
      z80cpu->HL.B.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbd: /* RES 7,L       */
      z80cpu->HL.B.l &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbe: /* RES 7,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xbf: /* RES 7,A       */
      z80cpu->AF.B.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc0: /* SET 0,B       */
      z80cpu->BC.B.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc1: /* SET 0,C       */
      z80cpu->BC.B.l |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc2: /* SET 0,D       */
      z80cpu->DE.B.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc3: /* SET 0,E       */
      z80cpu->DE.B.l |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc4: /* SET 0,H       */
      z80cpu->HL.B.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc5: /* SET 0,L       */
      z80cpu->HL.B.l |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc6: /* SET 0,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xc7: /* SET 0,A       */
      z80cpu->AF.B.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc8: /* SET 1,B       */
      z80cpu->BC.B.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc9: /* SET 1,C       */
      z80cpu->BC.B.l |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xca: /* SET 1,D       */
      z80cpu->DE.B.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcb: /* SET 1,E       */
      z80cpu->DE.B.l |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcc: /* SET 1,H       */
      z80cpu->HL.B.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcd: /* SET 1,L       */
      z80cpu->HL.B.l |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xce: /* SET 1,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xcf: /* SET 1,A       */
      z80cpu->AF.B.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd0: /* SET 2,B       */
      z80cpu->BC.B.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd1: /* SET 2,C       */
      z80cpu->BC.B.l |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd2: /* SET 2,D       */
      z80cpu->DE.B.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd3: /* SET 2,E       */
      z80cpu->DE.B.l |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd4: /* SET 2,H       */
      z80cpu->HL.B.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd5: /* SET 2,L       */
      z80cpu->HL.B.l |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd6: /* SET 2,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xd7: /* SET 2,A       */
      z80cpu->AF.B.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd8: /* SET 3,B       */
      z80cpu->BC.B.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd9: /* SET 3,C       */
      z80cpu->BC.B.l |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xda: /* SET 3,D       */
      z80cpu->DE.B.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xdb: /* SET 3,E       */
      z80cpu->DE.B.l |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xdc: /* SET 3,H       */
      z80cpu->HL.B.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xdd: /* SET 3,L       */
      z80cpu->HL.B.l |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xde: /* SET 3,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xdf: /* SET 3,A       */
      z80cpu->AF.B.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe0: /* SET 4,B       */
      z80cpu->BC.B.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe1: /* SET 4,C       */
      z80cpu->BC.B.l |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe2: /* SET 4,D       */
      z80cpu->DE.B.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe3: /* SET 4,E       */
      z80cpu->DE.B.l |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe4: /* SET 4,H       */
      z80cpu->HL.B.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe5: /* SET 4,L       */
      z80cpu->HL.B.l |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe6: /* SET 4,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xe7: /* SET 4,A       */
      z80cpu->AF.B.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe8: /* SET 5,B       */
      z80cpu->BC.B.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe9: /* SET 5,C       */
      z80cpu->BC.B.l |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xea: /* SET 5,D       */
      z80cpu->DE.B.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xeb: /* SET 5,E       */
      z80cpu->DE.B.l |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xec: /* SET 5,H       */
      z80cpu->HL.B.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xed: /* SET 5,L       */
      z80cpu->HL.B.l |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xee: /* SET 5,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xef: /* SET 5,A       */
      z80cpu->AF.B.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf0: /* SET 6,B       */
      z80cpu->BC.B.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf1: /* SET 6,C       */
      z80cpu->BC.B.l |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf2: /* SET 6,D       */
      z80cpu->DE.B.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf3: /* SET 6,E       */
      z80cpu->DE.B.l |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf4: /* SET 6,H       */
      z80cpu->HL.B.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf5: /* SET 6,L       */
      z80cpu->HL.B.l |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf6: /* SET 6,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xf7: /* SET 6,A       */
      z80cpu->AF.B.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf8: /* SET 7,B       */
      z80cpu->BC.B.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf9: /* SET 7,C       */
      z80cpu->BC.B.l |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfa: /* SET 7,D       */
      z80cpu->DE.B.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfb: /* SET 7,E       */
      z80cpu->DE.B.l |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfc: /* SET 7,H       */
      z80cpu->HL.B.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfd: /* SET 7,L       */
      z80cpu->HL.B.l |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfe: /* SET 7,(HL)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, z80cpu->HL.W);
      TMP2 = TMP1 | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->HL.W, TMP2);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xff: /* SET 7,A       */
      z80cpu->AF.B.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
  }
fetch_dd:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->PC.W++);
execute_dd:
  switch(opcode) {
#include "z80cpu_opcode_DD.h"
    case 0x00: /* NOP           */
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x03: /* INC BC        */
      z80cpu->BC.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x06: /* LD B,n        */
      z80cpu->BC.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x0b: /* DEC BC        */
      z80cpu->BC.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x0e: /* LD C,n        */
      z80cpu->BC.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x13: /* INC DE        */
      z80cpu->DE.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x16: /* LD D,n        */
      z80cpu->DE.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x1b: /* DEC DE        */
      z80cpu->DE.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x1e: /* LD E,n        */
      z80cpu->DE.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x23: /* INC IX        */
      z80cpu->IX.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x26: /* LD IXh,n      */
      z80cpu->IX.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x27: /* DAA           */
      WZ.W = z80cpu->AF.B.h;
      if(z80cpu->AF.B.l & _CF) WZ.W |= 0x100;
      if(z80cpu->AF.B.l & _HF) WZ.W |= 0x200;
      if(z80cpu->AF.B.l & _NF) WZ.W |= 0x400;
      z80cpu->AF.W = DAATable[WZ.W];
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2b: /* DEC IX        */
      z80cpu->IX.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x2e: /* LD IXl,n      */
      z80cpu->IX.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x33: /* INC SP        */
      z80cpu->SP.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3b: /* DEC SP        */
      z80cpu->SP.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3e: /* LD A,n        */
      z80cpu->AF.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x40: /* LD B,B        */
      z80cpu->BC.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x41: /* LD B,C        */
      z80cpu->BC.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x42: /* LD B,D        */
      z80cpu->BC.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x43: /* LD B,E        */
      z80cpu->BC.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x44: /* LD B,IXh      */
      z80cpu->BC.B.h = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x45: /* LD B,IXl      */
      z80cpu->BC.B.h = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x46: /* LD B,(IX+d)   */
      z80cpu->BC.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x47: /* LD B,A        */
      z80cpu->BC.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x48: /* LD C,B        */
      z80cpu->BC.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x49: /* LD C,C        */
      z80cpu->BC.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4a: /* LD C,D        */
      z80cpu->BC.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4b: /* LD C,E        */
      z80cpu->BC.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4c: /* LD C,IXh      */
      z80cpu->BC.B.l = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4d: /* LD C,IXl      */
      z80cpu->BC.B.l = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4e: /* LD C,(IX+d)   */
      z80cpu->BC.B.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x4f: /* LD C,A        */
      z80cpu->BC.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x50: /* LD D,B        */
      z80cpu->DE.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x51: /* LD D,C        */
      z80cpu->DE.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x52: /* LD D,D        */
      z80cpu->DE.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x53: /* LD D,E        */
      z80cpu->DE.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x54: /* LD D,IXh      */
      z80cpu->DE.B.h = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x55: /* LD D,IXl      */
      z80cpu->DE.B.h = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x56: /* LD D,(IX+d)   */
      z80cpu->DE.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x57: /* LD D,A        */
      z80cpu->DE.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x58: /* LD E,B        */
      z80cpu->DE.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x59: /* LD E,C        */
      z80cpu->DE.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5a: /* LD E,D        */
      z80cpu->DE.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5b: /* LD E,E        */
      z80cpu->DE.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5c: /* LD E,IXh      */
      z80cpu->DE.B.l = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5d: /* LD E,IXl      */
      z80cpu->DE.B.l = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5e: /* LD E,(IX+d)   */
      z80cpu->DE.B.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x5f: /* LD E,A        */
      z80cpu->DE.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x60: /* LD IXh,B      */
      z80cpu->IX.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x61: /* LD IXh,C      */
      z80cpu->IX.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x62: /* LD IXh,D      */
      z80cpu->IX.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x63: /* LD IXh,E      */
      z80cpu->IX.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x64: /* LD IXh,IXh    */
      z80cpu->IX.B.h = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x65: /* LD IXh,IXl    */
      z80cpu->IX.B.h = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x66: /* LD H,(IX+d)   */
      z80cpu->HL.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x67: /* LD IXh,A      */
      z80cpu->IX.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x68: /* LD IXl,B      */
      z80cpu->IX.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x69: /* LD IXl,C      */
      z80cpu->IX.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6a: /* LD IXl,D      */
      z80cpu->IX.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6b: /* LD IXl,E      */
      z80cpu->IX.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6c: /* LD IXl,IXh    */
      z80cpu->IX.B.l = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6d: /* LD IXl,IXl    */
      z80cpu->IX.B.l = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6e: /* LD L,(IX+d)   */
      z80cpu->HL.B.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x6f: /* LD IXl,A      */
      z80cpu->IX.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x70: /* LD (IX+d),B   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->BC.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x71: /* LD (IX+d),C   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->BC.B.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x72: /* LD (IX+d),D   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->DE.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x73: /* LD (IX+d),E   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->DE.B.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x74: /* LD (IX+d),H   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->HL.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x75: /* LD (IX+d),L   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->HL.B.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x76: /* HALT          */
      z80cpu->IF.W |= IFF_HLT;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x77: /* LD (IX+d),A   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->AF.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x78: /* LD A,B        */
      z80cpu->AF.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x79: /* LD A,C        */
      z80cpu->AF.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7a: /* LD A,D        */
      z80cpu->AF.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7b: /* LD A,E        */
      z80cpu->AF.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7c: /* LD A,IXh      */
      z80cpu->AF.B.h = z80cpu->IX.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7d: /* LD A,IXl      */
      z80cpu->AF.B.h = z80cpu->IX.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7e: /* LD A,(IX+d)   */
      z80cpu->AF.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x7f: /* LD A,A        */
      z80cpu->AF.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x80: /* ADD A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x81: /* ADD A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x82: /* ADD A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x83: /* ADD A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x84: /* ADD A,IXh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x85: /* ADD A,IXl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x86: /* ADD A,(IX+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x87: /* ADD A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x88: /* ADC A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x89: /* ADC A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8a: /* ADC A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8b: /* ADC A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8c: /* ADC A,IXh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8d: /* ADC A,IXl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8e: /* ADC A,(IX+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x8f: /* ADC A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x90: /* SUB A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x91: /* SUB A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x92: /* SUB A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x93: /* SUB A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x94: /* SUB A,IXh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x95: /* SUB A,IXl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x96: /* SUB A,(IX+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x97: /* SUB A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x98: /* SBC A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x99: /* SBC A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9a: /* SBC A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9b: /* SBC A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9c: /* SBC A,IXh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9d: /* SBC A,IXl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9e: /* SBC A,(IX+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x9f: /* SBC A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa0: /* AND A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa1: /* AND A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa2: /* AND A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa3: /* AND A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa4: /* AND A,IXh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa5: /* AND A,IXl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa6: /* AND A,(IX+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xa7: /* AND A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa8: /* XOR A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa9: /* XOR A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xaa: /* XOR A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xab: /* XOR A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xac: /* XOR A,IXh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xad: /* XOR A,IXl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xae: /* XOR A,(IX+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xaf: /* XOR A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb0: /* OR A,B        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb1: /* OR A,C        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb2: /* OR A,D        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb3: /* OR A,E        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb4: /* OR A,IXh      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb5: /* OR A,IXl      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb6: /* OR A,(IX+d)   */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xb7: /* OR A,A        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb8: /* CP A,B        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb9: /* CP A,C        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xba: /* CP A,D        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbb: /* CP A,E        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbc: /* CP A,IXh      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbd: /* CP A,IXl      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IX.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbe: /* CP A,(IX+d)   */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xbf: /* CP A,A        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcb:
      goto fetch_dd_cb;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: DD %02X: illegal opcode", (PC_W - 2), opcode);
      goto next;
  }
  z80cpu->ccounter -= CyclesXX[opcode];
  goto next;
fetch_ed:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->PC.W++);
execute_ed:
  switch(opcode) {
#include "z80cpu_opcode_ED.h"
    case 0xcb:
      goto fetch_ed_cb;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: ED %02X: illegal opcode", (PC_W - 2), opcode);
      goto next;
  }
  z80cpu->ccounter -= CyclesED[opcode];
  goto next;
fetch_fd:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->PC.W++);
execute_fd:
  switch(opcode) {
#include "z80cpu_opcode_FD.h"
    case 0x00: /* NOP           */
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x03: /* INC BC        */
      z80cpu->BC.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x06: /* LD B,n        */
      z80cpu->BC.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x0b: /* DEC BC        */
      z80cpu->BC.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x0e: /* LD C,n        */
      z80cpu->BC.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x13: /* INC DE        */
      z80cpu->DE.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x16: /* LD D,n        */
      z80cpu->DE.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x1b: /* DEC DE        */
      z80cpu->DE.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x1e: /* LD E,n        */
      z80cpu->DE.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x23: /* INC IY        */
      z80cpu->IY.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x26: /* LD IYh,n      */
      z80cpu->IY.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x27: /* DAA           */
      WZ.W = z80cpu->AF.B.h;
      if(z80cpu->AF.B.l & _CF) WZ.W |= 0x100;
      if(z80cpu->AF.B.l & _HF) WZ.W |= 0x200;
      if(z80cpu->AF.B.l & _NF) WZ.W |= 0x400;
      z80cpu->AF.W = DAATable[WZ.W];
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2b: /* DEC IY        */
      z80cpu->IY.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x2e: /* LD IYl,n      */
      z80cpu->IY.B.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x33: /* INC SP        */
      z80cpu->SP.W++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3b: /* DEC SP        */
      z80cpu->SP.W--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3e: /* LD A,n        */
      z80cpu->AF.B.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x40: /* LD B,B        */
      z80cpu->BC.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x41: /* LD B,C        */
      z80cpu->BC.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x42: /* LD B,D        */
      z80cpu->BC.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x43: /* LD B,E        */
      z80cpu->BC.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x44: /* LD B,IYh      */
      z80cpu->BC.B.h = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x45: /* LD B,IYl      */
      z80cpu->BC.B.h = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x46: /* LD B,(IY+d)   */
      z80cpu->BC.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x47: /* LD B,A        */
      z80cpu->BC.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x48: /* LD C,B        */
      z80cpu->BC.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x49: /* LD C,C        */
      z80cpu->BC.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4a: /* LD C,D        */
      z80cpu->BC.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4b: /* LD C,E        */
      z80cpu->BC.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4c: /* LD C,IYh      */
      z80cpu->BC.B.l = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4d: /* LD C,IYl      */
      z80cpu->BC.B.l = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4e: /* LD C,(IY+d)   */
      z80cpu->BC.B.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x4f: /* LD C,A        */
      z80cpu->BC.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x50: /* LD D,B        */
      z80cpu->DE.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x51: /* LD D,C        */
      z80cpu->DE.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x52: /* LD D,D        */
      z80cpu->DE.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x53: /* LD D,E        */
      z80cpu->DE.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x54: /* LD D,IYh      */
      z80cpu->DE.B.h = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x55: /* LD D,IYl      */
      z80cpu->DE.B.h = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x56: /* LD D,(IY+d)   */
      z80cpu->DE.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x57: /* LD D,A        */
      z80cpu->DE.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x58: /* LD E,B        */
      z80cpu->DE.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x59: /* LD E,C        */
      z80cpu->DE.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5a: /* LD E,D        */
      z80cpu->DE.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5b: /* LD E,E        */
      z80cpu->DE.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5c: /* LD E,IYh      */
      z80cpu->DE.B.l = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5d: /* LD E,IYl      */
      z80cpu->DE.B.l = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5e: /* LD E,(IY+d)   */
      z80cpu->DE.B.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x5f: /* LD E,A        */
      z80cpu->DE.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x60: /* LD IYh,B      */
      z80cpu->IY.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x61: /* LD IYh,C      */
      z80cpu->IY.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x62: /* LD IYh,D      */
      z80cpu->IY.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x63: /* LD IYh,E      */
      z80cpu->IY.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x64: /* LD IYh,IYh    */
      z80cpu->IY.B.h = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x65: /* LD IYh,IYl    */
      z80cpu->IY.B.h = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x66: /* LD H,(IY+d)   */
      z80cpu->HL.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x67: /* LD IYh,A      */
      z80cpu->IY.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x68: /* LD IYl,B      */
      z80cpu->IY.B.l = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x69: /* LD IYl,C      */
      z80cpu->IY.B.l = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6a: /* LD IYl,D      */
      z80cpu->IY.B.l = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6b: /* LD IYl,E      */
      z80cpu->IY.B.l = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6c: /* LD IYl,IYh    */
      z80cpu->IY.B.l = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6d: /* LD IYl,IYl    */
      z80cpu->IY.B.l = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6e: /* LD L,(IY+d)   */
      z80cpu->HL.B.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x6f: /* LD IYl,A      */
      z80cpu->IY.B.l = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x70: /* LD (IY+d),B   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->BC.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x71: /* LD (IY+d),C   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->BC.B.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x72: /* LD (IY+d),D   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->DE.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x73: /* LD (IY+d),E   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->DE.B.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x74: /* LD (IY+d),H   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->HL.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x75: /* LD (IY+d),L   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->HL.B.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x76: /* HALT          */
      z80cpu->IF.W |= IFF_HLT;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x77: /* LD (IY+d),A   */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)), z80cpu->AF.B.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x78: /* LD A,B        */
      z80cpu->AF.B.h = z80cpu->BC.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x79: /* LD A,C        */
      z80cpu->AF.B.h = z80cpu->BC.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7a: /* LD A,D        */
      z80cpu->AF.B.h = z80cpu->DE.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7b: /* LD A,E        */
      z80cpu->AF.B.h = z80cpu->DE.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7c: /* LD A,IYh      */
      z80cpu->AF.B.h = z80cpu->IY.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7d: /* LD A,IYl      */
      z80cpu->AF.B.h = z80cpu->IY.B.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7e: /* LD A,(IY+d)   */
      z80cpu->AF.B.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x7f: /* LD A,A        */
      z80cpu->AF.B.h = z80cpu->AF.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x80: /* ADD A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x81: /* ADD A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x82: /* ADD A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x83: /* ADD A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x84: /* ADD A,IYh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x85: /* ADD A,IYl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x86: /* ADD A,(IY+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x87: /* ADD A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 + TMP2;
      WZ.B.h = _ADD | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x88: /* ADC A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x89: /* ADC A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8a: /* ADC A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8b: /* ADC A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8c: /* ADC A,IYh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8d: /* ADC A,IYl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8e: /* ADC A,(IY+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x8f: /* ADC A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 + TMP2 + (z80cpu->AF.B.l & _CF);
      WZ.B.h = _ADC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & TMP2 & ~WZ.B.l) | (~TMP1 & ~TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x90: /* SUB A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x91: /* SUB A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x92: /* SUB A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x93: /* SUB A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x94: /* SUB A,IYh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x95: /* SUB A,IYl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x96: /* SUB A,(IY+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x97: /* SUB A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _SUB | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x98: /* SBC A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x99: /* SBC A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9a: /* SBC A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9b: /* SBC A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9c: /* SBC A,IYh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9d: /* SBC A,IYl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9e: /* SBC A,(IY+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x9f: /* SBC A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2 - (z80cpu->AF.B.l & _CF);
      WZ.B.h = _SBC | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa0: /* AND A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa1: /* AND A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa2: /* AND A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa3: /* AND A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa4: /* AND A,IYh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa5: /* AND A,IYl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa6: /* AND A,(IY+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xa7: /* AND A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 & TMP2;
      WZ.B.h = _AND | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa8: /* XOR A,B       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa9: /* XOR A,C       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xaa: /* XOR A,D       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xab: /* XOR A,E       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xac: /* XOR A,IYh     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xad: /* XOR A,IYl     */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xae: /* XOR A,(IY+d)  */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xaf: /* XOR A,A       */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 ^ TMP2;
      WZ.B.h = _XOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb0: /* OR A,B        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb1: /* OR A,C        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb2: /* OR A,D        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb3: /* OR A,E        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb4: /* OR A,IYh      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb5: /* OR A,IYl      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb6: /* OR A,(IY+d)   */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xb7: /* OR A,A        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 | TMP2;
      WZ.B.h = _IOR | (WZ.B.l & (_SF | _5F | _3F)) | PZSTable[WZ.B.l];
      z80cpu->AF.B.h = WZ.B.l;
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb8: /* CP A,B        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb9: /* CP A,C        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->BC.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xba: /* CP A,D        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbb: /* CP A,E        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->DE.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbc: /* CP A,IYh      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbd: /* CP A,IYl      */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->IY.B.l;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbe: /* CP A,(IY+d)   */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++)));
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xbf: /* CP A,A        */
      TMP1 = z80cpu->AF.B.h;
      TMP2 = z80cpu->AF.B.h;
      WZ.W = TMP1 - TMP2;
      WZ.B.h = _CMP | (WZ.B.l & (_SF | _5F | _3F)) | ((WZ.B.l ^ TMP1 ^ TMP2) & _HF) | (WZ.B.h & _CF);
      if(WZ.B.l == 0) {
        WZ.B.h |= _ZF;
      }
      if(((TMP1 & ~TMP2 & ~WZ.B.l) | (~TMP1 & TMP2 & WZ.B.l)) & _SF) {
        WZ.B.h |= _OF;
      }
      z80cpu->AF.B.l = WZ.B.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcb:
      goto fetch_fd_cb;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: FD %02X: illegal opcode", (PC_W - 2), opcode);
      goto next;
  }
  z80cpu->ccounter -= CyclesXX[opcode];
  goto next;
fetch_dd_cb:
  WZ.W = z80cpu->IX.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
  opcode = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
execute_dd_cb:
  switch(opcode) {
    case 0x06: /* RLC (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0e: /* RRC (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x16: /* RL  (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1e: /* RR  (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x26: /* SLA (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2e: /* SRA (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x36: /* SLL (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3e: /* SRL (IX+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x46: /* BIT 0,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4e: /* BIT 1,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x56: /* BIT 2,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5e: /* BIT 3,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x66: /* BIT 4,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6e: /* BIT 5,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x76: /* BIT 6,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7e: /* BIT 7,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x86: /* RES 0,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8e: /* RES 1,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x96: /* RES 2,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9e: /* RES 3,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa6: /* RES 4,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xae: /* RES 5,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb6: /* RES 6,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbe: /* RES 7,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc6: /* SET 0,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xce: /* SET 1,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd6: /* SET 2,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xde: /* SET 3,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe6: /* SET 4,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xee: /* SET 5,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf6: /* SET 6,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfe: /* SET 7,(IX+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: DD CB nn %02X: illegal opcode", (PC_W - 4), opcode);
      goto next;
  }
fetch_ed_cb:
  opcode = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
execute_ed_cb:
  switch(opcode) {
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: ED CB nn %02X: illegal opcode", (PC_W - 4), opcode);
      goto next;
  }
fetch_fd_cb:
  WZ.W = z80cpu->IY.W + (gint8) (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
  opcode = (*z80cpu->mreq_rd)(z80cpu, z80cpu->PC.W++);
execute_fd_cb:
  switch(opcode) {
    case 0x06: /* RLC (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 << 1) | (TMP1 >> 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0e: /* RRC (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 >> 1) | (TMP1 << 7);
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x16: /* RL  (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 << 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x01;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1e: /* RR  (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 >> 1;
      if(z80cpu->AF.B.l & _CF) {
        TMP2 |= 0x80;
      }
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x26: /* SLA (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = ((gint8) TMP1) << 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2e: /* SRA (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = ((gint8) TMP1) >> 1;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x36: /* SLL (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 << 1) | 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x80) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3e: /* SRL (IY+d)    */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = (TMP1 >> 1) & 0x7f;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2];
      if(TMP1 & 0x01) {
        z80cpu->AF.B.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x46: /* BIT 0,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x01;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4e: /* BIT 1,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x02;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x56: /* BIT 2,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x04;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5e: /* BIT 3,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x08;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x66: /* BIT 4,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x10;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6e: /* BIT 5,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x20;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x76: /* BIT 6,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x40;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7e: /* BIT 7,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x80;
      z80cpu->AF.B.l = (TMP2 & (_SF | _5F | _3F)) | PZSTable[TMP2] | (z80cpu->AF.B.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x86: /* RES 0,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8e: /* RES 1,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x96: /* RES 2,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9e: /* RES 3,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa6: /* RES 4,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xae: /* RES 5,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb6: /* RES 6,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbe: /* RES 7,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc6: /* SET 0,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xce: /* SET 1,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd6: /* SET 2,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xde: /* SET 3,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe6: /* SET 4,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xee: /* SET 5,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf6: /* SET 6,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfe: /* SET 7,(IY+d)  */
      TMP1 = (*z80cpu->mreq_rd)(z80cpu, WZ.W);
      TMP2 = TMP1 | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.W, TMP2);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    default:
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%04X: FD CB nn %02X: illegal opcode", (PC_W - 4), opcode);
      goto next;
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
