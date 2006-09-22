/*
 * cpu_z80.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "common.h"
#include "cpu_z80.h"
#include "Z80Core.h"

/**
 * CPU_Z80::init()
 *
 * @param self specifies the CPU_Z80 instance
 */
void cpu_z80_init(CPU_Z80 *self)
{
  cpu_z80_reset(self);
}

/**
 * CPU_Z80::clock()
 *
 * @param self specifies the CPU_Z80 instance
 */
void cpu_z80_clock(CPU_Z80 *self)
{
  CPU_Z80 *R = self;
  byte I;
  pair J;

decode_op:
  self->IR.B.l = (self->IR.B.l & 0x80) | ((self->IR.B.l + 1) & 0x7f);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= Cycles[I];
  switch(I) {
#include "Z80Opcodes.h"
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
  self->IR.B.l = (self->IR.B.l & 0x80) | ((self->IR.B.l + 1) & 0x7f);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= CyclesCB[I];
  switch(I) {
#include "Z80OpcodesCB.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_dd:
  self->IR.B.l = (self->IR.B.l & 0x80) | ((self->IR.B.l + 1) & 0x7f);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= CyclesXX[I];
  switch(I) {
#define XX IX
#include "Z80OpcodesXX.h"
#undef XX
    case 0xcb:
      goto decode_dd_cb;
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_dd_cb:
  J.W = self->IX.W + (offset) cpu_z80_mm_rd(self, self->PC.W++);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= CyclesXXCB[I];
  switch(I) {
#define XX IX
#include "Z80OpcodesXXCB.h"
#undef XX
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_ed:
  self->IR.B.l = (self->IR.B.l & 0x80) | ((self->IR.B.l + 1) & 0x7f);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= CyclesED[I];
  switch(I) {
#include "Z80OpcodesED.h"
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_fd:
  self->IR.B.l = (self->IR.B.l & 0x80) | ((self->IR.B.l + 1) & 0x7f);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= CyclesXX[I];
  switch(I) {
#define XX IY
#include "Z80OpcodesXX.h"
#undef XX
    case 0xcb:
      goto decode_fd_cb;
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_fd_cb:
  J.W = self->IY.W + (offset) cpu_z80_mm_rd(self, self->PC.W++);
  I = cpu_z80_mm_rd(self, self->PC.W++);
  self->TStates -= CyclesXXCB[I];
  switch(I) {
#define XX IY
#include "Z80OpcodesXXCB.h"
#undef XX
    default:
      goto decode_ko;
  }
  goto decode_ok;

decode_ko:
  (void) fprintf(stderr, "CPU_Z80: Bad opcode ... \n");
  (void) fflush(stderr);

decode_ok:
  if(self->TStates <= 0) {
    J.W = cpu_z80_timer(self);
    if((J.W != INT_NONE) && (self->IFF & IFF_EI) == 0) {
      IntZ80(self, J.W);
    }
    self->TStates += self->IPeriod;
  }
  self->IFF &= ~IFF_EI;
  goto decode_op;
}

/**
 * CPU_Z80::reset()
 *
 * @param self specifies the CPU_Z80 instance
 */
void cpu_z80_reset(CPU_Z80 *self)
{
  self->AF.W     = 0x0000;
  self->BC.W     = 0x0000;
  self->DE.W     = 0x0000;
  self->HL.W     = 0x0000;
  self->AF1.W    = 0x0000;
  self->BC1.W    = 0x0000;
  self->DE1.W    = 0x0000;
  self->HL1.W    = 0x0000;
  self->IX.W     = 0x0000;
  self->IY.W     = 0x0000;
  self->SP.W     = 0x0000;
  self->PC.W     = 0x0000;
  self->IR.W     = 0x0000;
  self->IFF      = 0x00;
  self->IPeriod  = self->IPeriod;
  self->TStates  = self->IPeriod;
}

/**
 * CPU_Z80::exit()
 *
 * @param self specifies the CPU_Z80 instance
 */
void cpu_z80_exit(CPU_Z80 *self)
{
}
