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
}

/**
 * GdevZ80CPU::clock()
 *
 * @param z80cpu specifies the GdevZ80CPU instance
 */
static void gdev_z80cpu_clock(GdevZ80CPU *z80cpu)
{
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
