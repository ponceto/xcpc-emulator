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
  z80cpu->mreq_m1 = NULL;
  z80cpu->mreq_rd = NULL;
  z80cpu->mreq_wr = NULL;
  z80cpu->iorq_m1 = NULL;
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
  z80cpu->reg.AF.q = 0;
  z80cpu->reg.BC.q = 0;
  z80cpu->reg.DE.q = 0;
  z80cpu->reg.HL.q = 0;
  z80cpu->reg.IX.q = 0;
  z80cpu->reg.IY.q = 0;
  z80cpu->reg.SP.q = 0;
  z80cpu->reg.PC.q = 0;
  z80cpu->reg.IR.q = 0;
  z80cpu->reg.IF.q = 0;
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
  u_int08_t opcode;
  GdevZ80REG T1;
  GdevZ80REG T2;
  GdevZ80REG WZ;

#define TMP1 T1.b.l
#define TMP2 T2.b.l

next:
  if(z80cpu->ccounter <= 0) {
    return;
  }
  else if(z80cpu->reg.IF.w.l & _NMI) {
    z80cpu->reg.IF.w.l &= ~(_HLT | _NMI | _INT | _IFF2);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->reg.SP.w.l, z80cpu->reg.PC.b.h);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->reg.SP.w.l, z80cpu->reg.PC.b.l);
    z80cpu->reg.PC.w.l = 0x0066;
    z80cpu->m_cycles += 3;
    z80cpu->t_states += 11;
    z80cpu->ccounter -= 11;
    goto next;
  }
  else if(z80cpu->reg.IF.w.l & _INT) {
    z80cpu->reg.IF.w.l &= ~(_HLT | _NMI | _INT | _IFF2 | _IFF1);
    (void) (*z80cpu->iorq_m1)(z80cpu, 0x0000);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->reg.SP.w.l, z80cpu->reg.PC.b.h);
    (*z80cpu->mreq_wr)(z80cpu, --z80cpu->reg.SP.w.l, z80cpu->reg.PC.b.l);
    z80cpu->reg.PC.w.l = 0x0038;
    z80cpu->m_cycles += 3;
    z80cpu->t_states += 13;
    z80cpu->ccounter -= 13;
    goto next;
  }
  else if(z80cpu->reg.IF.w.l & _HLT) {
    z80cpu->m_cycles += 1;
    z80cpu->t_states += 4;
    z80cpu->ccounter -= 4;
    goto next;
  }

fetch:
  z80cpu->reg.IR.b.l = (z80cpu->reg.IR.b.l & 0x80) | ((z80cpu->reg.IR.b.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute:
  switch(opcode) {
#include "z80cpu_opcode.h"
    case 0x00: /* NOP               */
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x03: /* INC BC            */
      z80cpu->reg.BC.w.l++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x06: /* LD B,n            */
      z80cpu->reg.BC.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x07: /* RLCA              */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x0b: /* DEC BC            */
      z80cpu->reg.BC.w.l--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x0e: /* LD C,n            */
      z80cpu->reg.BC.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x0f: /* RRCA              */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x13: /* INC DE            */
      z80cpu->reg.DE.w.l++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x16: /* LD D,n            */
      z80cpu->reg.DE.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x17: /* RLA               */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x1b: /* DEC DE            */
      z80cpu->reg.DE.w.l--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x1e: /* LD E,n            */
      z80cpu->reg.DE.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x1f: /* RRA               */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x23: /* INC HL            */
      z80cpu->reg.HL.w.l++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x26: /* LD H,n            */
      z80cpu->reg.HL.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x27: /* DAA               */
      WZ.w.l = z80cpu->reg.AF.b.h;
      if(z80cpu->reg.AF.b.l & _CF) WZ.w.l |= 0x100;
      if(z80cpu->reg.AF.b.l & _HF) WZ.w.l |= 0x200;
      if(z80cpu->reg.AF.b.l & _NF) WZ.w.l |= 0x400;
      z80cpu->reg.AF.w.l = DAATable[WZ.w.l];
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x2b: /* DEC HL            */
      z80cpu->reg.HL.w.l--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x2e: /* LD L,n            */
      z80cpu->reg.HL.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x33: /* INC SP            */
      z80cpu->reg.SP.w.l++;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x3b: /* DEC SP            */
      z80cpu->reg.SP.w.l--;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 6;
      z80cpu->ccounter -= 6;
      goto next;
    case 0x3e: /* LD A,n            */
      z80cpu->reg.AF.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x40: /* LD B,B            */
      z80cpu->reg.BC.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x41: /* LD B,C            */
      z80cpu->reg.BC.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x42: /* LD B,D            */
      z80cpu->reg.BC.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x43: /* LD B,E            */
      z80cpu->reg.BC.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x44: /* LD B,H            */
      z80cpu->reg.BC.b.h = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x45: /* LD B,L            */
      z80cpu->reg.BC.b.h = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x46: /* LD B,(HL)         */
      z80cpu->reg.BC.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x47: /* LD B,A            */
      z80cpu->reg.BC.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x48: /* LD C,B            */
      z80cpu->reg.BC.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x49: /* LD C,C            */
      z80cpu->reg.BC.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4a: /* LD C,D            */
      z80cpu->reg.BC.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4b: /* LD C,E            */
      z80cpu->reg.BC.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4c: /* LD C,H            */
      z80cpu->reg.BC.b.l = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4d: /* LD C,L            */
      z80cpu->reg.BC.b.l = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x4e: /* LD C,(HL)         */
      z80cpu->reg.BC.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x4f: /* LD C,A            */
      z80cpu->reg.BC.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x50: /* LD D,B            */
      z80cpu->reg.DE.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x51: /* LD D,C            */
      z80cpu->reg.DE.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x52: /* LD D,D            */
      z80cpu->reg.DE.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x53: /* LD D,E            */
      z80cpu->reg.DE.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x54: /* LD D,H            */
      z80cpu->reg.DE.b.h = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x55: /* LD D,L            */
      z80cpu->reg.DE.b.h = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x56: /* LD D,(HL)         */
      z80cpu->reg.DE.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x57: /* LD D,A            */
      z80cpu->reg.DE.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x58: /* LD E,B            */
      z80cpu->reg.DE.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x59: /* LD E,C            */
      z80cpu->reg.DE.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5a: /* LD E,D            */
      z80cpu->reg.DE.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5b: /* LD E,E            */
      z80cpu->reg.DE.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5c: /* LD E,H            */
      z80cpu->reg.DE.b.l = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5d: /* LD E,L            */
      z80cpu->reg.DE.b.l = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x5e: /* LD E,(HL)         */
      z80cpu->reg.DE.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x5f: /* LD E,A            */
      z80cpu->reg.DE.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x60: /* LD H,B            */
      z80cpu->reg.HL.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x61: /* LD H,C            */
      z80cpu->reg.HL.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x62: /* LD H,D            */
      z80cpu->reg.HL.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x63: /* LD H,E            */
      z80cpu->reg.HL.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x64: /* LD H,H            */
      z80cpu->reg.HL.b.h = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x65: /* LD H,L            */
      z80cpu->reg.HL.b.h = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x66: /* LD H,(HL)         */
      z80cpu->reg.HL.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x67: /* LD H,A            */
      z80cpu->reg.HL.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x68: /* LD L,B            */
      z80cpu->reg.HL.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x69: /* LD L,C            */
      z80cpu->reg.HL.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6a: /* LD L,D            */
      z80cpu->reg.HL.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6b: /* LD L,E            */
      z80cpu->reg.HL.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6c: /* LD L,H            */
      z80cpu->reg.HL.b.l = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6d: /* LD L,L            */
      z80cpu->reg.HL.b.l = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x6e: /* LD L,(HL)         */
      z80cpu->reg.HL.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x6f: /* LD L,A            */
      z80cpu->reg.HL.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x70: /* LD (HL),B         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.BC.b.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x71: /* LD (HL),C         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.BC.b.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x72: /* LD (HL),D         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.DE.b.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x73: /* LD (HL),E         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.DE.b.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x74: /* LD (HL),H         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.HL.b.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x75: /* LD (HL),L         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.HL.b.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x76: /* HALT              */
      z80cpu->reg.IF.w.l |= _HLT;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x77: /* LD (HL),A         */
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, z80cpu->reg.AF.b.h);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x78: /* LD A,B            */
      z80cpu->reg.AF.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x79: /* LD A,C            */
      z80cpu->reg.AF.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7a: /* LD A,D            */
      z80cpu->reg.AF.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7b: /* LD A,E            */
      z80cpu->reg.AF.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7c: /* LD A,H            */
      z80cpu->reg.AF.b.h = z80cpu->reg.HL.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7d: /* LD A,L            */
      z80cpu->reg.AF.b.h = z80cpu->reg.HL.b.l;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x7e: /* LD A,(HL)         */
      z80cpu->reg.AF.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x7f: /* LD A,A            */
      z80cpu->reg.AF.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x80: /* ADD A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x81: /* ADD A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x82: /* ADD A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x83: /* ADD A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x84: /* ADD A,H           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x85: /* ADD A,L           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x86: /* ADD A,(HL)        */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x87: /* ADD A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x88: /* ADC A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x89: /* ADC A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8a: /* ADC A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8b: /* ADC A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8c: /* ADC A,H           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8d: /* ADC A,L           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x8e: /* ADC A,(HL)        */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x8f: /* ADC A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x90: /* SUB A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x91: /* SUB A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x92: /* SUB A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x93: /* SUB A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x94: /* SUB A,H           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x95: /* SUB A,L           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x96: /* SUB A,(HL)        */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x97: /* SUB A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x98: /* SBC A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x99: /* SBC A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9a: /* SBC A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9b: /* SBC A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9c: /* SBC A,H           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9d: /* SBC A,L           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0x9e: /* SBC A,(HL)        */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0x9f: /* SBC A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa0: /* AND A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa1: /* AND A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa2: /* AND A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa3: /* AND A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa4: /* AND A,H           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa5: /* AND A,L           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa6: /* AND A,(HL)        */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xa7: /* AND A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa8: /* XOR A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xa9: /* XOR A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xaa: /* XOR A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xab: /* XOR A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xac: /* XOR A,H           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xad: /* XOR A,L           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xae: /* XOR A,(HL)        */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xaf: /* XOR A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb0: /* OR A,B            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb1: /* OR A,C            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb2: /* OR A,D            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb3: /* OR A,E            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb4: /* OR A,H            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb5: /* OR A,L            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb6: /* OR A,(HL)         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xb7: /* OR A,A            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb8: /* CP A,B            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xb9: /* CP A,C            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xba: /* CP A,D            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbb: /* CP A,E            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbc: /* CP A,H            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbd: /* CP A,L            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xbe: /* CP A,(HL)         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xbf: /* CP A,A            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 1;
      z80cpu->t_states += 4;
      z80cpu->ccounter -= 4;
      goto next;
    case 0xc6: /* ADD A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xcb:
      goto fetch_cb;
    case 0xce: /* ADC A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xd6: /* SUB A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xdd:
      goto fetch_dd;
    case 0xde: /* SBC A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xe6: /* AND A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xed:
      goto fetch_ed;
    case 0xee: /* XOR A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xf6: /* OR A,n            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
    case 0xfd:
      goto fetch_fd;
    case 0xfe: /* CP A,n            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 7;
      z80cpu->ccounter -= 7;
      goto next;
  }
  z80cpu->ccounter -= Cycles[opcode];
  goto next;

fetch_cb:
  z80cpu->reg.IR.b.l = (z80cpu->reg.IR.b.l & 0x80) | ((z80cpu->reg.IR.b.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute_cb:
  switch(opcode) {
    case 0x00: /* RLC B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x01: /* RLC C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x02: /* RLC D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x03: /* RLC E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x04: /* RLC H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x05: /* RLC L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x06: /* RLC (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x07: /* RLC A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x08: /* RRC B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x09: /* RRC C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0a: /* RRC D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0b: /* RRC E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0c: /* RRC H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0d: /* RRC L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0e: /* RRC (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x0f: /* RRC A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x10: /* RL  B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x11: /* RL  C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x12: /* RL  D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x13: /* RL  E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x14: /* RL  H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x15: /* RL  L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x16: /* RL  (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x17: /* RL  A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x18: /* RR  B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x19: /* RR  C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1a: /* RR  D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1b: /* RR  E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1c: /* RR  H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1d: /* RR  L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1e: /* RR  (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x1f: /* RR  A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x20: /* SLA B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x21: /* SLA C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x22: /* SLA D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x23: /* SLA E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x24: /* SLA H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x25: /* SLA L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x26: /* SLA (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x27: /* SLA A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x28: /* SRA B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x29: /* SRA C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2a: /* SRA D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2b: /* SRA E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2c: /* SRA H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2d: /* SRA L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2e: /* SRA (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x2f: /* SRA A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x30: /* SLL B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x31: /* SLL C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x32: /* SLL D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x33: /* SLL E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x34: /* SLL H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x35: /* SLL L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x36: /* SLL (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x37: /* SLL A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x38: /* SRL B             */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x39: /* SRL C             */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3a: /* SRL D             */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3b: /* SRL E             */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3c: /* SRL H             */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3d: /* SRL L             */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x3e: /* SRL (HL)          */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x3f: /* SRL A             */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x40: /* BIT 0,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x41: /* BIT 0,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x42: /* BIT 0,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x43: /* BIT 0,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x44: /* BIT 0,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x45: /* BIT 0,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x46: /* BIT 0,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x47: /* BIT 0,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x48: /* BIT 1,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x49: /* BIT 1,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4a: /* BIT 1,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4b: /* BIT 1,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4c: /* BIT 1,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4d: /* BIT 1,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4e: /* BIT 1,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x4f: /* BIT 1,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x50: /* BIT 2,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x51: /* BIT 2,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x52: /* BIT 2,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x53: /* BIT 2,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x54: /* BIT 2,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x55: /* BIT 2,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x56: /* BIT 2,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x57: /* BIT 2,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x58: /* BIT 3,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x59: /* BIT 3,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5a: /* BIT 3,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5b: /* BIT 3,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5c: /* BIT 3,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5d: /* BIT 3,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5e: /* BIT 3,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x5f: /* BIT 3,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x60: /* BIT 4,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x61: /* BIT 4,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x62: /* BIT 4,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x63: /* BIT 4,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x64: /* BIT 4,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x65: /* BIT 4,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x66: /* BIT 4,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x67: /* BIT 4,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x68: /* BIT 5,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x69: /* BIT 5,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6a: /* BIT 5,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6b: /* BIT 5,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6c: /* BIT 5,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6d: /* BIT 5,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6e: /* BIT 5,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x6f: /* BIT 5,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x70: /* BIT 6,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x71: /* BIT 6,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x72: /* BIT 6,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x73: /* BIT 6,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x74: /* BIT 6,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x75: /* BIT 6,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x76: /* BIT 6,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x77: /* BIT 6,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x78: /* BIT 7,B           */
      T1.b.l = z80cpu->reg.BC.b.h;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x79: /* BIT 7,C           */
      T1.b.l = z80cpu->reg.BC.b.l;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7a: /* BIT 7,D           */
      T1.b.l = z80cpu->reg.DE.b.h;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7b: /* BIT 7,E           */
      T1.b.l = z80cpu->reg.DE.b.l;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7c: /* BIT 7,H           */
      T1.b.l = z80cpu->reg.HL.b.h;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7d: /* BIT 7,L           */
      T1.b.l = z80cpu->reg.HL.b.l;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7e: /* BIT 7,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 12;
      z80cpu->ccounter -= 12;
      goto next;
    case 0x7f: /* BIT 7,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x80: /* RES 0,B           */
      z80cpu->reg.BC.b.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x81: /* RES 0,C           */
      z80cpu->reg.BC.b.l &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x82: /* RES 0,D           */
      z80cpu->reg.DE.b.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x83: /* RES 0,E           */
      z80cpu->reg.DE.b.l &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x84: /* RES 0,H           */
      z80cpu->reg.HL.b.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x85: /* RES 0,L           */
      z80cpu->reg.HL.b.l &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x86: /* RES 0,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x87: /* RES 0,A           */
      z80cpu->reg.AF.b.h &= 0xfe;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x88: /* RES 1,B           */
      z80cpu->reg.BC.b.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x89: /* RES 1,C           */
      z80cpu->reg.BC.b.l &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8a: /* RES 1,D           */
      z80cpu->reg.DE.b.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8b: /* RES 1,E           */
      z80cpu->reg.DE.b.l &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8c: /* RES 1,H           */
      z80cpu->reg.HL.b.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8d: /* RES 1,L           */
      z80cpu->reg.HL.b.l &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8e: /* RES 1,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x8f: /* RES 1,A           */
      z80cpu->reg.AF.b.h &= 0xfd;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x90: /* RES 2,B           */
      z80cpu->reg.BC.b.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x91: /* RES 2,C           */
      z80cpu->reg.BC.b.l &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x92: /* RES 2,D           */
      z80cpu->reg.DE.b.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x93: /* RES 2,E           */
      z80cpu->reg.DE.b.l &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x94: /* RES 2,H           */
      z80cpu->reg.HL.b.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x95: /* RES 2,L           */
      z80cpu->reg.HL.b.l &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x96: /* RES 2,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x97: /* RES 2,A           */
      z80cpu->reg.AF.b.h &= 0xfb;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x98: /* RES 3,B           */
      z80cpu->reg.BC.b.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x99: /* RES 3,C           */
      z80cpu->reg.BC.b.l &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9a: /* RES 3,D           */
      z80cpu->reg.DE.b.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9b: /* RES 3,E           */
      z80cpu->reg.DE.b.l &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9c: /* RES 3,H           */
      z80cpu->reg.HL.b.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9d: /* RES 3,L           */
      z80cpu->reg.HL.b.l &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9e: /* RES 3,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0x9f: /* RES 3,A           */
      z80cpu->reg.AF.b.h &= 0xf7;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa0: /* RES 4,B           */
      z80cpu->reg.BC.b.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa1: /* RES 4,C           */
      z80cpu->reg.BC.b.l &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa2: /* RES 4,D           */
      z80cpu->reg.DE.b.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa3: /* RES 4,E           */
      z80cpu->reg.DE.b.l &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa4: /* RES 4,H           */
      z80cpu->reg.HL.b.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa5: /* RES 4,L           */
      z80cpu->reg.HL.b.l &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa6: /* RES 4,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xa7: /* RES 4,A           */
      z80cpu->reg.AF.b.h &= 0xef;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa8: /* RES 5,B           */
      z80cpu->reg.BC.b.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa9: /* RES 5,C           */
      z80cpu->reg.BC.b.l &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xaa: /* RES 5,D           */
      z80cpu->reg.DE.b.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xab: /* RES 5,E           */
      z80cpu->reg.DE.b.l &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xac: /* RES 5,H           */
      z80cpu->reg.HL.b.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xad: /* RES 5,L           */
      z80cpu->reg.HL.b.l &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xae: /* RES 5,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xaf: /* RES 5,A           */
      z80cpu->reg.AF.b.h &= 0xdf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb0: /* RES 6,B           */
      z80cpu->reg.BC.b.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb1: /* RES 6,C           */
      z80cpu->reg.BC.b.l &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb2: /* RES 6,D           */
      z80cpu->reg.DE.b.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb3: /* RES 6,E           */
      z80cpu->reg.DE.b.l &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb4: /* RES 6,H           */
      z80cpu->reg.HL.b.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb5: /* RES 6,L           */
      z80cpu->reg.HL.b.l &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb6: /* RES 6,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xb7: /* RES 6,A           */
      z80cpu->reg.AF.b.h &= 0xbf;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb8: /* RES 7,B           */
      z80cpu->reg.BC.b.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb9: /* RES 7,C           */
      z80cpu->reg.BC.b.l &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xba: /* RES 7,D           */
      z80cpu->reg.DE.b.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbb: /* RES 7,E           */
      z80cpu->reg.DE.b.l &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbc: /* RES 7,H           */
      z80cpu->reg.HL.b.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbd: /* RES 7,L           */
      z80cpu->reg.HL.b.l &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbe: /* RES 7,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xbf: /* RES 7,A           */
      z80cpu->reg.AF.b.h &= 0x7f;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc0: /* SET 0,B           */
      z80cpu->reg.BC.b.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc1: /* SET 0,C           */
      z80cpu->reg.BC.b.l |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc2: /* SET 0,D           */
      z80cpu->reg.DE.b.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc3: /* SET 0,E           */
      z80cpu->reg.DE.b.l |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc4: /* SET 0,H           */
      z80cpu->reg.HL.b.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc5: /* SET 0,L           */
      z80cpu->reg.HL.b.l |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc6: /* SET 0,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xc7: /* SET 0,A           */
      z80cpu->reg.AF.b.h |= 0x01;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc8: /* SET 1,B           */
      z80cpu->reg.BC.b.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc9: /* SET 1,C           */
      z80cpu->reg.BC.b.l |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xca: /* SET 1,D           */
      z80cpu->reg.DE.b.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcb: /* SET 1,E           */
      z80cpu->reg.DE.b.l |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcc: /* SET 1,H           */
      z80cpu->reg.HL.b.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xcd: /* SET 1,L           */
      z80cpu->reg.HL.b.l |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xce: /* SET 1,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xcf: /* SET 1,A           */
      z80cpu->reg.AF.b.h |= 0x02;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd0: /* SET 2,B           */
      z80cpu->reg.BC.b.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd1: /* SET 2,C           */
      z80cpu->reg.BC.b.l |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd2: /* SET 2,D           */
      z80cpu->reg.DE.b.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd3: /* SET 2,E           */
      z80cpu->reg.DE.b.l |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd4: /* SET 2,H           */
      z80cpu->reg.HL.b.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd5: /* SET 2,L           */
      z80cpu->reg.HL.b.l |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd6: /* SET 2,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xd7: /* SET 2,A           */
      z80cpu->reg.AF.b.h |= 0x04;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd8: /* SET 3,B           */
      z80cpu->reg.BC.b.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xd9: /* SET 3,C           */
      z80cpu->reg.BC.b.l |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xda: /* SET 3,D           */
      z80cpu->reg.DE.b.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xdb: /* SET 3,E           */
      z80cpu->reg.DE.b.l |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xdc: /* SET 3,H           */
      z80cpu->reg.HL.b.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xdd: /* SET 3,L           */
      z80cpu->reg.HL.b.l |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xde: /* SET 3,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xdf: /* SET 3,A           */
      z80cpu->reg.AF.b.h |= 0x08;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe0: /* SET 4,B           */
      z80cpu->reg.BC.b.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe1: /* SET 4,C           */
      z80cpu->reg.BC.b.l |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe2: /* SET 4,D           */
      z80cpu->reg.DE.b.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe3: /* SET 4,E           */
      z80cpu->reg.DE.b.l |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe4: /* SET 4,H           */
      z80cpu->reg.HL.b.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe5: /* SET 4,L           */
      z80cpu->reg.HL.b.l |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe6: /* SET 4,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xe7: /* SET 4,A           */
      z80cpu->reg.AF.b.h |= 0x10;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe8: /* SET 5,B           */
      z80cpu->reg.BC.b.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xe9: /* SET 5,C           */
      z80cpu->reg.BC.b.l |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xea: /* SET 5,D           */
      z80cpu->reg.DE.b.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xeb: /* SET 5,E           */
      z80cpu->reg.DE.b.l |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xec: /* SET 5,H           */
      z80cpu->reg.HL.b.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xed: /* SET 5,L           */
      z80cpu->reg.HL.b.l |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xee: /* SET 5,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xef: /* SET 5,A           */
      z80cpu->reg.AF.b.h |= 0x20;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf0: /* SET 6,B           */
      z80cpu->reg.BC.b.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf1: /* SET 6,C           */
      z80cpu->reg.BC.b.l |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf2: /* SET 6,D           */
      z80cpu->reg.DE.b.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf3: /* SET 6,E           */
      z80cpu->reg.DE.b.l |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf4: /* SET 6,H           */
      z80cpu->reg.HL.b.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf5: /* SET 6,L           */
      z80cpu->reg.HL.b.l |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf6: /* SET 6,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xf7: /* SET 6,A           */
      z80cpu->reg.AF.b.h |= 0x40;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf8: /* SET 7,B           */
      z80cpu->reg.BC.b.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xf9: /* SET 7,C           */
      z80cpu->reg.BC.b.l |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfa: /* SET 7,D           */
      z80cpu->reg.DE.b.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfb: /* SET 7,E           */
      z80cpu->reg.DE.b.l |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfc: /* SET 7,H           */
      z80cpu->reg.HL.b.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfd: /* SET 7,L           */
      z80cpu->reg.HL.b.l |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xfe: /* SET 7,(HL)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.HL.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, z80cpu->reg.HL.w.l, T2.b.l);
      z80cpu->m_cycles += 4;
      z80cpu->t_states += 15;
      z80cpu->ccounter -= 15;
      goto next;
    case 0xff: /* SET 7,A           */
      z80cpu->reg.AF.b.h |= 0x80;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
  }

fetch_dd:
  z80cpu->reg.IR.b.l = (z80cpu->reg.IR.b.l & 0x80) | ((z80cpu->reg.IR.b.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute_dd:
  switch(opcode) {
#include "z80cpu_opcode_DD.h"
    case 0x00: /* NOP               */
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x03: /* INC BC            */
      z80cpu->reg.BC.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x06: /* LD B,n            */
      z80cpu->reg.BC.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x07: /* RLCA              */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0b: /* DEC BC            */
      z80cpu->reg.BC.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x0e: /* LD C,n            */
      z80cpu->reg.BC.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x0f: /* RRCA              */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x13: /* INC DE            */
      z80cpu->reg.DE.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x16: /* LD D,n            */
      z80cpu->reg.DE.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x17: /* RLA               */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1b: /* DEC DE            */
      z80cpu->reg.DE.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x1e: /* LD E,n            */
      z80cpu->reg.DE.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x1f: /* RRA               */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x23: /* INC IX            */
      z80cpu->reg.IX.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x26: /* LD IXh,n          */
      z80cpu->reg.IX.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x27: /* DAA               */
      WZ.w.l = z80cpu->reg.AF.b.h;
      if(z80cpu->reg.AF.b.l & _CF) WZ.w.l |= 0x100;
      if(z80cpu->reg.AF.b.l & _HF) WZ.w.l |= 0x200;
      if(z80cpu->reg.AF.b.l & _NF) WZ.w.l |= 0x400;
      z80cpu->reg.AF.w.l = DAATable[WZ.w.l];
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2b: /* DEC IX            */
      z80cpu->reg.IX.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x2e: /* LD IXl,n          */
      z80cpu->reg.IX.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x33: /* INC SP            */
      z80cpu->reg.SP.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3b: /* DEC SP            */
      z80cpu->reg.SP.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3e: /* LD A,n            */
      z80cpu->reg.AF.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x40: /* LD B,B            */
      z80cpu->reg.BC.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x41: /* LD B,C            */
      z80cpu->reg.BC.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x42: /* LD B,D            */
      z80cpu->reg.BC.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x43: /* LD B,E            */
      z80cpu->reg.BC.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x44: /* LD B,IXh          */
      z80cpu->reg.BC.b.h = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x45: /* LD B,IXl          */
      z80cpu->reg.BC.b.h = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x46: /* LD B,(IX+d)       */
      z80cpu->reg.BC.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x47: /* LD B,A            */
      z80cpu->reg.BC.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x48: /* LD C,B            */
      z80cpu->reg.BC.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x49: /* LD C,C            */
      z80cpu->reg.BC.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4a: /* LD C,D            */
      z80cpu->reg.BC.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4b: /* LD C,E            */
      z80cpu->reg.BC.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4c: /* LD C,IXh          */
      z80cpu->reg.BC.b.l = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4d: /* LD C,IXl          */
      z80cpu->reg.BC.b.l = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4e: /* LD C,(IX+d)       */
      z80cpu->reg.BC.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x4f: /* LD C,A            */
      z80cpu->reg.BC.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x50: /* LD D,B            */
      z80cpu->reg.DE.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x51: /* LD D,C            */
      z80cpu->reg.DE.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x52: /* LD D,D            */
      z80cpu->reg.DE.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x53: /* LD D,E            */
      z80cpu->reg.DE.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x54: /* LD D,IXh          */
      z80cpu->reg.DE.b.h = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x55: /* LD D,IXl          */
      z80cpu->reg.DE.b.h = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x56: /* LD D,(IX+d)       */
      z80cpu->reg.DE.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x57: /* LD D,A            */
      z80cpu->reg.DE.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x58: /* LD E,B            */
      z80cpu->reg.DE.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x59: /* LD E,C            */
      z80cpu->reg.DE.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5a: /* LD E,D            */
      z80cpu->reg.DE.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5b: /* LD E,E            */
      z80cpu->reg.DE.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5c: /* LD E,IXh          */
      z80cpu->reg.DE.b.l = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5d: /* LD E,IXl          */
      z80cpu->reg.DE.b.l = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5e: /* LD E,(IX+d)       */
      z80cpu->reg.DE.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x5f: /* LD E,A            */
      z80cpu->reg.DE.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x60: /* LD IXh,B          */
      z80cpu->reg.IX.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x61: /* LD IXh,C          */
      z80cpu->reg.IX.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x62: /* LD IXh,D          */
      z80cpu->reg.IX.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x63: /* LD IXh,E          */
      z80cpu->reg.IX.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x64: /* LD IXh,IXh        */
      z80cpu->reg.IX.b.h = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x65: /* LD IXh,IXl        */
      z80cpu->reg.IX.b.h = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x66: /* LD H,(IX+d)       */
      z80cpu->reg.HL.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x67: /* LD IXh,A          */
      z80cpu->reg.IX.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x68: /* LD IXl,B          */
      z80cpu->reg.IX.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x69: /* LD IXl,C          */
      z80cpu->reg.IX.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6a: /* LD IXl,D          */
      z80cpu->reg.IX.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6b: /* LD IXl,E          */
      z80cpu->reg.IX.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6c: /* LD IXl,IXh        */
      z80cpu->reg.IX.b.l = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6d: /* LD IXl,IXl        */
      z80cpu->reg.IX.b.l = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6e: /* LD L,(IX+d)       */
      z80cpu->reg.HL.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x6f: /* LD IXl,A          */
      z80cpu->reg.IX.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x70: /* LD (IX+d),B       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.BC.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x71: /* LD (IX+d),C       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.BC.b.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x72: /* LD (IX+d),D       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.DE.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x73: /* LD (IX+d),E       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.DE.b.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x74: /* LD (IX+d),H       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.HL.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x75: /* LD (IX+d),L       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.HL.b.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x76: /* HALT              */
      z80cpu->reg.IF.w.l |= _HLT;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x77: /* LD (IX+d),A       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.AF.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x78: /* LD A,B            */
      z80cpu->reg.AF.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x79: /* LD A,C            */
      z80cpu->reg.AF.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7a: /* LD A,D            */
      z80cpu->reg.AF.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7b: /* LD A,E            */
      z80cpu->reg.AF.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7c: /* LD A,IXh          */
      z80cpu->reg.AF.b.h = z80cpu->reg.IX.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7d: /* LD A,IXl          */
      z80cpu->reg.AF.b.h = z80cpu->reg.IX.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7e: /* LD A,(IX+d)       */
      z80cpu->reg.AF.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x7f: /* LD A,A            */
      z80cpu->reg.AF.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x80: /* ADD A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x81: /* ADD A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x82: /* ADD A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x83: /* ADD A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x84: /* ADD A,IXh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x85: /* ADD A,IXl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x86: /* ADD A,(IX+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x87: /* ADD A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x88: /* ADC A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x89: /* ADC A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8a: /* ADC A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8b: /* ADC A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8c: /* ADC A,IXh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8d: /* ADC A,IXl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8e: /* ADC A,(IX+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x8f: /* ADC A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x90: /* SUB A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x91: /* SUB A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x92: /* SUB A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x93: /* SUB A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x94: /* SUB A,IXh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x95: /* SUB A,IXl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x96: /* SUB A,(IX+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x97: /* SUB A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x98: /* SBC A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x99: /* SBC A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9a: /* SBC A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9b: /* SBC A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9c: /* SBC A,IXh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9d: /* SBC A,IXl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9e: /* SBC A,(IX+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x9f: /* SBC A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa0: /* AND A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa1: /* AND A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa2: /* AND A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa3: /* AND A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa4: /* AND A,IXh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa5: /* AND A,IXl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa6: /* AND A,(IX+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xa7: /* AND A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa8: /* XOR A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa9: /* XOR A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xaa: /* XOR A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xab: /* XOR A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xac: /* XOR A,IXh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xad: /* XOR A,IXl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xae: /* XOR A,(IX+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xaf: /* XOR A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb0: /* OR A,B            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb1: /* OR A,C            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb2: /* OR A,D            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb3: /* OR A,E            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb4: /* OR A,IXh          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb5: /* OR A,IXl          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb6: /* OR A,(IX+d)       */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xb7: /* OR A,A            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb8: /* CP A,B            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb9: /* CP A,C            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xba: /* CP A,D            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbb: /* CP A,E            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbc: /* CP A,IXh          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbd: /* CP A,IXl          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IX.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbe: /* CP A,(IX+d)       */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xbf: /* CP A,A            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc6: /* ADD A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xcb:
      goto fetch_dd_cb;
    case 0xce: /* ADC A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xd6: /* SUB A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xde: /* SBC A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xe6: /* AND A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xee: /* XOR A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xf6: /* OR A,n            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xfe: /* CP A,n            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
  }
  z80cpu->ccounter -= CyclesXX[opcode];
  goto next;

fetch_ed:
  z80cpu->reg.IR.b.l = (z80cpu->reg.IR.b.l & 0x80) | ((z80cpu->reg.IR.b.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute_ed:
  switch(opcode) {
#include "z80cpu_opcode_ED.h"
    default:
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
  }
  z80cpu->ccounter -= CyclesED[opcode];
  goto next;

fetch_fd:
  z80cpu->reg.IR.b.l = (z80cpu->reg.IR.b.l & 0x80) | ((z80cpu->reg.IR.b.l + 1) & 0x7f);
  opcode = (*z80cpu->mreq_m1)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute_fd:
  switch(opcode) {
#include "z80cpu_opcode_FD.h"
    case 0x00: /* NOP               */
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x03: /* INC BC            */
      z80cpu->reg.BC.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x06: /* LD B,n            */
      z80cpu->reg.BC.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x07: /* RLCA              */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x0b: /* DEC BC            */
      z80cpu->reg.BC.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x0e: /* LD C,n            */
      z80cpu->reg.BC.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x0f: /* RRCA              */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x13: /* INC DE            */
      z80cpu->reg.DE.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x16: /* LD D,n            */
      z80cpu->reg.DE.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x17: /* RLA               */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x1b: /* DEC DE            */
      z80cpu->reg.DE.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x1e: /* LD E,n            */
      z80cpu->reg.DE.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x1f: /* RRA               */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l &= (_SF | _ZF | _5F | _3F | _PF);
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x23: /* INC IY            */
      z80cpu->reg.IY.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x26: /* LD IYh,n          */
      z80cpu->reg.IY.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x27: /* DAA               */
      WZ.w.l = z80cpu->reg.AF.b.h;
      if(z80cpu->reg.AF.b.l & _CF) WZ.w.l |= 0x100;
      if(z80cpu->reg.AF.b.l & _HF) WZ.w.l |= 0x200;
      if(z80cpu->reg.AF.b.l & _NF) WZ.w.l |= 0x400;
      z80cpu->reg.AF.w.l = DAATable[WZ.w.l];
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x2b: /* DEC IY            */
      z80cpu->reg.IY.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x2e: /* LD IYl,n          */
      z80cpu->reg.IY.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x33: /* INC SP            */
      z80cpu->reg.SP.w.l++;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3b: /* DEC SP            */
      z80cpu->reg.SP.w.l--;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 10;
      z80cpu->ccounter -= 10;
      goto next;
    case 0x3e: /* LD A,n            */
      z80cpu->reg.AF.b.h = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0x40: /* LD B,B            */
      z80cpu->reg.BC.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x41: /* LD B,C            */
      z80cpu->reg.BC.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x42: /* LD B,D            */
      z80cpu->reg.BC.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x43: /* LD B,E            */
      z80cpu->reg.BC.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x44: /* LD B,IYh          */
      z80cpu->reg.BC.b.h = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x45: /* LD B,IYl          */
      z80cpu->reg.BC.b.h = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x46: /* LD B,(IY+d)       */
      z80cpu->reg.BC.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x47: /* LD B,A            */
      z80cpu->reg.BC.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x48: /* LD C,B            */
      z80cpu->reg.BC.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x49: /* LD C,C            */
      z80cpu->reg.BC.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4a: /* LD C,D            */
      z80cpu->reg.BC.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4b: /* LD C,E            */
      z80cpu->reg.BC.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4c: /* LD C,IYh          */
      z80cpu->reg.BC.b.l = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4d: /* LD C,IYl          */
      z80cpu->reg.BC.b.l = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x4e: /* LD C,(IY+d)       */
      z80cpu->reg.BC.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x4f: /* LD C,A            */
      z80cpu->reg.BC.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x50: /* LD D,B            */
      z80cpu->reg.DE.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x51: /* LD D,C            */
      z80cpu->reg.DE.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x52: /* LD D,D            */
      z80cpu->reg.DE.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x53: /* LD D,E            */
      z80cpu->reg.DE.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x54: /* LD D,IYh          */
      z80cpu->reg.DE.b.h = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x55: /* LD D,IYl          */
      z80cpu->reg.DE.b.h = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x56: /* LD D,(IY+d)       */
      z80cpu->reg.DE.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x57: /* LD D,A            */
      z80cpu->reg.DE.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x58: /* LD E,B            */
      z80cpu->reg.DE.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x59: /* LD E,C            */
      z80cpu->reg.DE.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5a: /* LD E,D            */
      z80cpu->reg.DE.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5b: /* LD E,E            */
      z80cpu->reg.DE.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5c: /* LD E,IYh          */
      z80cpu->reg.DE.b.l = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5d: /* LD E,IYl          */
      z80cpu->reg.DE.b.l = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x5e: /* LD E,(IY+d)       */
      z80cpu->reg.DE.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x5f: /* LD E,A            */
      z80cpu->reg.DE.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x60: /* LD IYh,B          */
      z80cpu->reg.IY.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x61: /* LD IYh,C          */
      z80cpu->reg.IY.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x62: /* LD IYh,D          */
      z80cpu->reg.IY.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x63: /* LD IYh,E          */
      z80cpu->reg.IY.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x64: /* LD IYh,IYh        */
      z80cpu->reg.IY.b.h = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x65: /* LD IYh,IYl        */
      z80cpu->reg.IY.b.h = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x66: /* LD H,(IY+d)       */
      z80cpu->reg.HL.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x67: /* LD IYh,A          */
      z80cpu->reg.IY.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x68: /* LD IYl,B          */
      z80cpu->reg.IY.b.l = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x69: /* LD IYl,C          */
      z80cpu->reg.IY.b.l = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6a: /* LD IYl,D          */
      z80cpu->reg.IY.b.l = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6b: /* LD IYl,E          */
      z80cpu->reg.IY.b.l = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6c: /* LD IYl,IYh        */
      z80cpu->reg.IY.b.l = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6d: /* LD IYl,IYl        */
      z80cpu->reg.IY.b.l = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x6e: /* LD L,(IY+d)       */
      z80cpu->reg.HL.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x6f: /* LD IYl,A          */
      z80cpu->reg.IY.b.l = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x70: /* LD (IY+d),B       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.BC.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x71: /* LD (IY+d),C       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.BC.b.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x72: /* LD (IY+d),D       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.DE.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x73: /* LD (IY+d),E       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.DE.b.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x74: /* LD (IY+d),H       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.HL.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x75: /* LD (IY+d),L       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.HL.b.l);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x76: /* HALT              */
      z80cpu->reg.IF.w.l |= _HLT;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x77: /* LD (IY+d),A       */
      (*z80cpu->mreq_wr)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)), z80cpu->reg.AF.b.h);
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x78: /* LD A,B            */
      z80cpu->reg.AF.b.h = z80cpu->reg.BC.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x79: /* LD A,C            */
      z80cpu->reg.AF.b.h = z80cpu->reg.BC.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7a: /* LD A,D            */
      z80cpu->reg.AF.b.h = z80cpu->reg.DE.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7b: /* LD A,E            */
      z80cpu->reg.AF.b.h = z80cpu->reg.DE.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7c: /* LD A,IYh          */
      z80cpu->reg.AF.b.h = z80cpu->reg.IY.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7d: /* LD A,IYl          */
      z80cpu->reg.AF.b.h = z80cpu->reg.IY.b.l;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x7e: /* LD A,(IY+d)       */
      z80cpu->reg.AF.b.h = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x7f: /* LD A,A            */
      z80cpu->reg.AF.b.h = z80cpu->reg.AF.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x80: /* ADD A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x81: /* ADD A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x82: /* ADD A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x83: /* ADD A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x84: /* ADD A,IYh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x85: /* ADD A,IYl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x86: /* ADD A,(IY+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x87: /* ADD A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x88: /* ADC A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x89: /* ADC A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8a: /* ADC A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8b: /* ADC A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8c: /* ADC A,IYh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8d: /* ADC A,IYl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x8e: /* ADC A,(IY+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x8f: /* ADC A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x90: /* SUB A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x91: /* SUB A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x92: /* SUB A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x93: /* SUB A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x94: /* SUB A,IYh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x95: /* SUB A,IYl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x96: /* SUB A,(IY+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x97: /* SUB A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x98: /* SBC A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x99: /* SBC A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9a: /* SBC A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9b: /* SBC A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9c: /* SBC A,IYh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9d: /* SBC A,IYl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0x9e: /* SBC A,(IY+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0x9f: /* SBC A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa0: /* AND A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa1: /* AND A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa2: /* AND A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa3: /* AND A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa4: /* AND A,IYh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa5: /* AND A,IYl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa6: /* AND A,(IY+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xa7: /* AND A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa8: /* XOR A,B           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xa9: /* XOR A,C           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xaa: /* XOR A,D           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xab: /* XOR A,E           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xac: /* XOR A,IYh         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xad: /* XOR A,IYl         */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xae: /* XOR A,(IY+d)      */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xaf: /* XOR A,A           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb0: /* OR A,B            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb1: /* OR A,C            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb2: /* OR A,D            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb3: /* OR A,E            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb4: /* OR A,IYh          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb5: /* OR A,IYl          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb6: /* OR A,(IY+d)       */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xb7: /* OR A,A            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb8: /* CP A,B            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xb9: /* CP A,C            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xba: /* CP A,D            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbb: /* CP A,E            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbc: /* CP A,IYh          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbd: /* CP A,IYl          */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.IY.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xbe: /* CP A,(IY+d)       */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, (z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++)));
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 19;
      z80cpu->ccounter -= 19;
      goto next;
    case 0xbf: /* CP A,A            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = z80cpu->reg.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 2;
      z80cpu->t_states += 8;
      z80cpu->ccounter -= 8;
      goto next;
    case 0xc6: /* ADD A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = _ADD | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xcb:
      goto fetch_fd_cb;
    case 0xce: /* ADC A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l + T2.b.l + (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _ADC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xd6: /* SUB A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _SUB | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xde: /* SBC A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l - (z80cpu->reg.AF.b.l & _CF);
      WZ.b.h = _SBC | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xe6: /* AND A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = _AND | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xee: /* XOR A,n           */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = _XOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xf6: /* OR A,n            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = _IOR | (WZ.b.l & (_SF | _5F | _3F)) | PZSTable[WZ.b.l];
      z80cpu->reg.AF.b.h = WZ.b.l;
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
    case 0xfe: /* CP A,n            */
      T1.b.l = z80cpu->reg.AF.b.h;
      T2.b.l = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = _CMP | (WZ.b.l & (_SF | _5F | _3F)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & _HF) | (WZ.b.h & _CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= _ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & _SF) {
        WZ.b.h |= _OF;
      }
      z80cpu->reg.AF.b.l = WZ.b.h;
      z80cpu->m_cycles += 3;
      z80cpu->t_states += 11;
      z80cpu->ccounter -= 11;
      goto next;
  }
  z80cpu->ccounter -= CyclesED[opcode];
  goto next;

fetch_dd_cb:
  WZ.w.l = z80cpu->reg.IX.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
  opcode = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute_dd_cb:
  switch(opcode) {
    case 0x00: /* LD B,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x01: /* LD C,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x02: /* LD D,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x03: /* LD E,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x04: /* LD H,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x05: /* LD L,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x06: /* RLC (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x07: /* LD A,RLC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x08: /* LD B,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x09: /* LD C,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0a: /* LD D,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0b: /* LD E,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0c: /* LD H,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0d: /* LD L,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0e: /* RRC (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0f: /* LD A,RRC (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x10: /* LD B,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x11: /* LD C,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x12: /* LD D,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x13: /* LD E,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x14: /* LD H,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x15: /* LD L,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x16: /* RL  (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x17: /* LD A,RL  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x18: /* LD B,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x19: /* LD C,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1a: /* LD D,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1b: /* LD E,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1c: /* LD H,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1d: /* LD L,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1e: /* RR  (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1f: /* LD A,RR  (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x20: /* LD B,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x21: /* LD C,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x22: /* LD D,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x23: /* LD E,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x24: /* LD H,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x25: /* LD L,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x26: /* SLA (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x27: /* LD A,SLA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x28: /* LD B,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x29: /* LD C,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2a: /* LD D,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2b: /* LD E,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2c: /* LD H,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2d: /* LD L,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2e: /* SRA (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2f: /* LD A,SRA (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x30: /* LD B,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x31: /* LD C,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x32: /* LD D,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x33: /* LD E,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x34: /* LD H,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x35: /* LD L,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x36: /* SLL (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x37: /* LD A,SLL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x38: /* LD B,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x39: /* LD C,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3a: /* LD D,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3b: /* LD E,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3c: /* LD H,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3d: /* LD L,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3e: /* SRL (IX+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3f: /* LD A,SRL (IX+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x40: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x41: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x42: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x43: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x44: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x45: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x46: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x47: /* BIT 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x48: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x49: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4a: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4b: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4c: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4d: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4e: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4f: /* BIT 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x50: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x51: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x52: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x53: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x54: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x55: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x56: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x57: /* BIT 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x58: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x59: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5a: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5b: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5c: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5d: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5e: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5f: /* BIT 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x60: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x61: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x62: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x63: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x64: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x65: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x66: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x67: /* BIT 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x68: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x69: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6a: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6b: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6c: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6d: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6e: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6f: /* BIT 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x70: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x71: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x72: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x73: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x74: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x75: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x76: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x77: /* BIT 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x78: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x79: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7a: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7b: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7c: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7d: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7e: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7f: /* BIT 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x80: /* LD B,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x81: /* LD C,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x82: /* LD D,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x83: /* LD E,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x84: /* LD H,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x85: /* LD L,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x86: /* RES 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x87: /* LD A,RES 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x88: /* LD B,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x89: /* LD C,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8a: /* LD D,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8b: /* LD E,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8c: /* LD H,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8d: /* LD L,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8e: /* RES 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8f: /* LD A,RES 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x90: /* LD B,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x91: /* LD C,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x92: /* LD D,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x93: /* LD E,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x94: /* LD H,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x95: /* LD L,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x96: /* RES 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x97: /* LD A,RES 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x98: /* LD B,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x99: /* LD C,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9a: /* LD D,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9b: /* LD E,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9c: /* LD H,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9d: /* LD L,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9e: /* RES 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9f: /* LD A,RES 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa0: /* LD B,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa1: /* LD C,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa2: /* LD D,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa3: /* LD E,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa4: /* LD H,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa5: /* LD L,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa6: /* RES 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa7: /* LD A,RES 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa8: /* LD B,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa9: /* LD C,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xaa: /* LD D,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xab: /* LD E,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xac: /* LD H,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xad: /* LD L,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xae: /* RES 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xaf: /* LD A,RES 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb0: /* LD B,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb1: /* LD C,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb2: /* LD D,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb3: /* LD E,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb4: /* LD H,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb5: /* LD L,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb6: /* RES 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb7: /* LD A,RES 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb8: /* LD B,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb9: /* LD C,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xba: /* LD D,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbb: /* LD E,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbc: /* LD H,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbd: /* LD L,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbe: /* RES 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbf: /* LD A,RES 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc0: /* LD B,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc1: /* LD C,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc2: /* LD D,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc3: /* LD E,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc4: /* LD H,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc5: /* LD L,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc6: /* SET 0,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc7: /* LD A,SET 0,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc8: /* LD B,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc9: /* LD C,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xca: /* LD D,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcb: /* LD E,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcc: /* LD H,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcd: /* LD L,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xce: /* SET 1,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcf: /* LD A,SET 1,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd0: /* LD B,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd1: /* LD C,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd2: /* LD D,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd3: /* LD E,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd4: /* LD H,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd5: /* LD L,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd6: /* SET 2,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd7: /* LD A,SET 2,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd8: /* LD B,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd9: /* LD C,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xda: /* LD D,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdb: /* LD E,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdc: /* LD H,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdd: /* LD L,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xde: /* SET 3,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdf: /* LD A,SET 3,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe0: /* LD B,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe1: /* LD C,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe2: /* LD D,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe3: /* LD E,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe4: /* LD H,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe5: /* LD L,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe6: /* SET 4,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe7: /* LD A,SET 4,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe8: /* LD B,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe9: /* LD C,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xea: /* LD D,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xeb: /* LD E,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xec: /* LD H,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xed: /* LD L,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xee: /* SET 5,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xef: /* LD A,SET 5,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf0: /* LD B,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf1: /* LD C,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf2: /* LD D,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf3: /* LD E,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf4: /* LD H,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf5: /* LD L,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf6: /* SET 6,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf7: /* LD A,SET 6,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf8: /* LD B,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf9: /* LD C,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfa: /* LD D,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfb: /* LD E,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfc: /* LD H,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfd: /* LD L,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfe: /* SET 7,(IX+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xff: /* LD A,SET 7,(IX+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
  }

fetch_fd_cb:
  WZ.w.l = z80cpu->reg.IY.w.l + (s_int08_t) (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
  opcode = (*z80cpu->mreq_rd)(z80cpu, z80cpu->reg.PC.w.l++);
decode_and_execute_fd_cb:
  switch(opcode) {
    case 0x00: /* LD B,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x01: /* LD C,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x02: /* LD D,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x03: /* LD E,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x04: /* LD H,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x05: /* LD L,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x06: /* RLC (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x07: /* LD A,RLC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x08: /* LD B,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x09: /* LD C,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0a: /* LD D,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0b: /* LD E,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0c: /* LD H,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0d: /* LD L,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0e: /* RRC (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x0f: /* LD A,RRC (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x10: /* LD B,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x11: /* LD C,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x12: /* LD D,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x13: /* LD E,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x14: /* LD H,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x15: /* LD L,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x16: /* RL  (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x17: /* LD A,RL  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l << 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x01;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x18: /* LD B,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x19: /* LD C,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1a: /* LD D,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1b: /* LD E,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1c: /* LD H,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1d: /* LD L,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1e: /* RR  (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x1f: /* LD A,RR  (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l >> 1;
      if(z80cpu->reg.AF.b.l & _CF) {
        T2.b.l |= 0x80;
      }
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x20: /* LD B,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x21: /* LD C,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x22: /* LD D,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x23: /* LD E,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x24: /* LD H,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x25: /* LD L,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x26: /* SLA (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x27: /* LD A,SLA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) << 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x28: /* LD B,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x29: /* LD C,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2a: /* LD D,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2b: /* LD E,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2c: /* LD H,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2d: /* LD L,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2e: /* SRA (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x2f: /* LD A,SRA (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = ((s_int08_t) T1.b.l) >> 1;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x30: /* LD B,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x31: /* LD C,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x32: /* LD D,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x33: /* LD E,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x34: /* LD H,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x35: /* LD L,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x36: /* SLL (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x37: /* LD A,SLL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l << 1) | 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x80) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x38: /* LD B,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x39: /* LD C,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3a: /* LD D,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3b: /* LD E,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3c: /* LD H,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3d: /* LD L,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3e: /* SRL (IY+d)        */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x3f: /* LD A,SRL (IY+d)   */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = (T1.b.l >> 1) & 0x7f;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
      if(T1.b.l & 0x01) {
        z80cpu->reg.AF.b.l |= _CF;
      }
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x40: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x41: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x42: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x43: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x44: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x45: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x46: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x47: /* BIT 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x01;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x48: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x49: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4a: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4b: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4c: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4d: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4e: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x4f: /* BIT 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x02;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x50: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x51: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x52: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x53: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x54: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x55: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x56: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x57: /* BIT 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x04;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x58: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x59: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5a: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5b: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5c: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5d: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5e: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x5f: /* BIT 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x08;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x60: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x61: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x62: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x63: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x64: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x65: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x66: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x67: /* BIT 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x10;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x68: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x69: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6a: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6b: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6c: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6d: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6e: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x6f: /* BIT 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x20;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x70: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x71: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x72: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x73: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x74: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x75: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x76: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x77: /* BIT 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x40;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x78: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x79: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7a: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7b: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7c: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7d: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7e: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x7f: /* BIT 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x80;
      z80cpu->reg.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (z80cpu->reg.AF.b.l & _CF) | _HF;
      z80cpu->m_cycles += 5;
      z80cpu->t_states += 20;
      z80cpu->ccounter -= 20;
      goto next;
    case 0x80: /* LD B,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x81: /* LD C,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x82: /* LD D,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x83: /* LD E,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x84: /* LD H,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x85: /* LD L,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x86: /* RES 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x87: /* LD A,RES 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfe;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x88: /* LD B,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x89: /* LD C,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8a: /* LD D,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8b: /* LD E,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8c: /* LD H,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8d: /* LD L,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8e: /* RES 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x8f: /* LD A,RES 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfd;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x90: /* LD B,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x91: /* LD C,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x92: /* LD D,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x93: /* LD E,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x94: /* LD H,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x95: /* LD L,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x96: /* RES 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x97: /* LD A,RES 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xfb;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x98: /* LD B,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x99: /* LD C,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9a: /* LD D,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9b: /* LD E,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9c: /* LD H,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9d: /* LD L,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9e: /* RES 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0x9f: /* LD A,RES 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xf7;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa0: /* LD B,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa1: /* LD C,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa2: /* LD D,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa3: /* LD E,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa4: /* LD H,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa5: /* LD L,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa6: /* RES 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa7: /* LD A,RES 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xef;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa8: /* LD B,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xa9: /* LD C,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xaa: /* LD D,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xab: /* LD E,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xac: /* LD H,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xad: /* LD L,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xae: /* RES 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xaf: /* LD A,RES 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xdf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb0: /* LD B,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb1: /* LD C,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb2: /* LD D,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb3: /* LD E,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb4: /* LD H,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb5: /* LD L,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb6: /* RES 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb7: /* LD A,RES 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0xbf;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb8: /* LD B,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xb9: /* LD C,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xba: /* LD D,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbb: /* LD E,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbc: /* LD H,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbd: /* LD L,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbe: /* RES 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xbf: /* LD A,RES 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l & 0x7f;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc0: /* LD B,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc1: /* LD C,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc2: /* LD D,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc3: /* LD E,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc4: /* LD H,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc5: /* LD L,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc6: /* SET 0,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc7: /* LD A,SET 0,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x01;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc8: /* LD B,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xc9: /* LD C,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xca: /* LD D,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcb: /* LD E,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcc: /* LD H,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcd: /* LD L,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xce: /* SET 1,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xcf: /* LD A,SET 1,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x02;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd0: /* LD B,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd1: /* LD C,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd2: /* LD D,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd3: /* LD E,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd4: /* LD H,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd5: /* LD L,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd6: /* SET 2,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd7: /* LD A,SET 2,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x04;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd8: /* LD B,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xd9: /* LD C,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xda: /* LD D,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdb: /* LD E,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdc: /* LD H,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdd: /* LD L,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xde: /* SET 3,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xdf: /* LD A,SET 3,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x08;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe0: /* LD B,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe1: /* LD C,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe2: /* LD D,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe3: /* LD E,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe4: /* LD H,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe5: /* LD L,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe6: /* SET 4,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe7: /* LD A,SET 4,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x10;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe8: /* LD B,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xe9: /* LD C,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xea: /* LD D,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xeb: /* LD E,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xec: /* LD H,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xed: /* LD L,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xee: /* SET 5,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xef: /* LD A,SET 5,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x20;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf0: /* LD B,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf1: /* LD C,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf2: /* LD D,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf3: /* LD E,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf4: /* LD H,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf5: /* LD L,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf6: /* SET 6,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf7: /* LD A,SET 6,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x40;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf8: /* LD B,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xf9: /* LD C,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.BC.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfa: /* LD D,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfb: /* LD E,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.DE.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfc: /* LD H,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfd: /* LD L,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.HL.b.l = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xfe: /* SET 7,(IY+d)      */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
    case 0xff: /* LD A,SET 7,(IY+d) */
      T1.b.l = (*z80cpu->mreq_rd)(z80cpu, WZ.w.l);
      T2.b.l = T1.b.l | 0x80;
      (*z80cpu->mreq_wr)(z80cpu, WZ.w.l, T2.b.l);
      z80cpu->reg.AF.b.h = T2.b.l;
      z80cpu->m_cycles += 6;
      z80cpu->t_states += 23;
      z80cpu->ccounter -= 23;
      goto next;
  }

#undef TMP1
#undef TMP2

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
  if(IF_W & _IFF1) {
    IF_W |= _INT;
  }
}

/**
 * GdevZ80CPU::assert_nmi()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
void gdev_z80cpu_assert_nmi(GdevZ80CPU *z80cpu)
{
  IF_W |= _NMI;
}
