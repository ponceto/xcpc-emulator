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

static void gdev_z80cpu_debug(GdevZ80CPU *z80cpu);
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

  device_class->debug = (GdevDeviceProc) gdev_z80cpu_debug;
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
  gdev_z80cpu_reset(z80cpu);
  z80cpu->mm_rd = NULL;
  z80cpu->mm_wr = NULL;
  z80cpu->io_rd = NULL;
  z80cpu->io_wr = NULL;
}

/**
 * GdevZ80CPU::debug()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_debug(GdevZ80CPU *z80cpu)
{
}

/**
 * GdevZ80CPU::reset()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_reset(GdevZ80CPU *z80cpu)
{
  z80cpu->AF.W    = 0x0000;
  z80cpu->BC.W    = 0x0000;
  z80cpu->DE.W    = 0x0000;
  z80cpu->HL.W    = 0x0000;
  z80cpu->AF1.W   = 0x0000;
  z80cpu->BC1.W   = 0x0000;
  z80cpu->DE1.W   = 0x0000;
  z80cpu->HL1.W   = 0x0000;
  z80cpu->IX.W    = 0x0000;
  z80cpu->IY.W    = 0x0000;
  z80cpu->SP.W    = 0x0000;
  z80cpu->PC.W    = 0x0000;
  z80cpu->IR.W    = 0x0000;
  z80cpu->IFF     = 0x00;
  z80cpu->TStates = 0;
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
  z80cpu->IFF &= ~IFF_EI;
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->TStates -= Cycles[I];
  switch(I) {
#include "z80cpu_opcode.h"
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
  z80cpu->TStates -= CyclesCB[I];
  switch(I) {
#include "z80cpu_opcode_CB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_dd:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->TStates -= CyclesXX[I];
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
  z80cpu->TStates -= CyclesXXCB[I];
  switch(I) {
#include "z80cpu_opcode_DDCB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_ed:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->TStates -= CyclesED[I];
  switch(I) {
#include "z80cpu_opcode_ED.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_fd:
  z80cpu->IR.B.l = (z80cpu->IR.B.l & 0x80) | ((z80cpu->IR.B.l + 1) & 0x7f);
  I = (*z80cpu->mm_rd)(z80cpu, z80cpu->PC.W++);
  z80cpu->TStates -= CyclesXX[I];
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
  z80cpu->TStates -= CyclesXXCB[I];
  switch(I) {
#include "z80cpu_opcode_FDCB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_ko:
  (void) fprintf(stderr, "CPU_Z80: illegal opcode ... \n");
  (void) fflush(stderr);

decode_ok:
  if(z80cpu->TStates > 0) {
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
  if((z80cpu->IFF & IFF_EI) != 0) {
    return;
  }
  if((z80cpu->IFF & IFF_1) || (vector == INT_NMI)) {
    /* If HALTed, take CPU off HALT instruction */
    if(z80cpu->IFF & IFF_HALT) {
      z80cpu->PC.W++;
      z80cpu->IFF &= ~IFF_HALT;
    }
    /* PUSH PC */
    (*z80cpu->mm_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.h);
    (*z80cpu->mm_wr)(z80cpu, --z80cpu->SP.W, z80cpu->PC.B.l);
    /* If it is NMI... */
    if(vector==INT_NMI) {
      if(z80cpu->IFF & IFF_1) {
        z80cpu->IFF |=  IFF_2;
      }
      else {
        z80cpu->IFF &= ~IFF_2;
      }
      z80cpu->IFF &= ~(IFF_1 | IFF_EI);
      z80cpu->PC.W = 0x0066;
      return;
    }
    /* Further interrupts off */
    z80cpu->IFF &= ~(IFF_1 | IFF_2 | IFF_EI);
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
