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
 * CPU-Z80::init()
 *
 * @param self specifies the CPU-Z80 instance
 */
void cpu_z80_init(CPU_Z80 *self)
{
  cpu_z80_reset(self);
}

/**
 * CPU-Z80::clock()
 *
 * @param self specifies the CPU-Z80 instance
 */
void cpu_z80_clock(CPU_Z80 *self)
{
  CPU_Z80 *R = self;
  byte I;
  pair J;

  for(;;) {
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= Cycles[I];
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
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= CyclesCB[I];
    switch(I) {
#include "Z80OpcodesCB.h"
      default:
        goto decode_ko;
    }
    goto decode_ok;

decode_dd:
#define XX IX
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= CyclesXX[I];
    switch(I) {
#include "Z80OpcodesXX.h"
      case 0xcb:
        goto decode_dd_cb;
      default:
        goto decode_ko;
    }
    goto decode_ok;
#undef XX

decode_dd_cb:
#define XX IX
    J.W = self->XX.W + (offset) cpu_z80_mm_rd(self, self->PC.W++);
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= CyclesXXCB[I];
    switch(I) {
#include "Z80OpcodesXCB.h"
      default:
        goto decode_ko;
    }
    goto decode_ok;
#undef XX

decode_ed:
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= CyclesED[I];
    switch(I) {
#include "Z80OpcodesED.h"
      default:
        goto decode_ko;
    }
    goto decode_ok;

decode_fd:
#define XX IY
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= CyclesXX[I];
    switch(I) {
#include "Z80OpcodesXX.h"
      case 0xcb:
        goto decode_fd_cb;
      default:
        goto decode_ko;
    }
    goto decode_ok;
#undef XX

decode_fd_cb:
#define XX IY
    J.W = self->XX.W + (offset) cpu_z80_mm_rd(self, self->PC.W++);
    I = cpu_z80_mm_rd(self, self->PC.W++);
    self->ICount -= CyclesXXCB[I];
    switch(I) {
#include "Z80OpcodesXCB.h"
      default:
        goto decode_ko;
    }
    goto decode_ok;
#undef XX

decode_ko:
    (void) fprintf(stderr, "CPU-Z80: Bad opcode ... \n");
    (void) fflush(stderr);

decode_ok:
    if(self->ICount <= 0) {
      if(self->IFF & IFF_EI) {
        self->IFF = (self->IFF & ~IFF_EI) | IFF_1; /* Done with AfterEI state */
        self->ICount += self->IBackup - 1;         /* Restore the ICount      */
        if(self->ICount > 0) {
          J.W = self->IRequest;
        }
        else {
          J.W = cpu_z80_timer(self);                /* Call periodic handler   */
          self->ICount += self->IPeriod;            /* Reset the cycle counter */
          if(J.W == INT_NONE) J.W = self->IRequest; /* Pending IRQ             */
        }
      }
      else {
        J.W = cpu_z80_timer(self);                /* Call periodic handler   */
        self->ICount += self->IPeriod;            /* Reset the cycle counter */
        if(J.W == INT_NONE) J.W = self->IRequest; /* Pending IRQ             */
      }
      if(J.W == INT_QUIT) return;            /* Exit if INT_QUIT */
      if(J.W != INT_NONE) IntZ80(self, J.W); /* Int-pt if needed */
    }
  }
}

/**
 * CPU-Z80::reset()
 *
 * @param self specifies the CPU-Z80 instance
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
  self->ICount   = self->IPeriod;
  self->IRequest = INT_NONE;
}

/**
 * CPU-Z80::exit()
 *
 * @param self specifies the CPU-Z80 instance
 */
void cpu_z80_exit(CPU_Z80 *self)
{
}
