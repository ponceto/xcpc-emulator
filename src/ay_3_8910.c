/*
 * ay_3_8910.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "ay_3_8910.h"

/**
 * AY_3_8910::init()
 *
 * @param self specifies the AY_3_8910 instance
 */
void ay_3_8910_init(AY_3_8910 *self)
{
  ay_3_8910_reset(self);
}

/**
 * AY_3_8910::clock()
 *
 * @param self specifies the AY_3_8910 instance
 */
void ay_3_8910_clock(AY_3_8910 *self)
{
}

/**
 * AY_3_8910::reset()
 *
 * @param self specifies the AY_3_8910 instance
 */
void ay_3_8910_reset(AY_3_8910 *self)
{
  int ix;

  self->current = 0x00;
  for(ix = 0; ix < 16; ix++) {
    self->registers[ix] = 0x00;
  }
}

/**
 * AY_3_8910::exit()
 *
 * @param self specifies the AY_3_8910 instance
 */
void ay_3_8910_exit(AY_3_8910 *self)
{
}
