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
  (void) RunZ80(self);
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
  self->I        = 0x00;
  self->R        = 0x00;
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
