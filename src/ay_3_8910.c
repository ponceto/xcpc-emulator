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

AY_3_8910 ay_3_8910;

void ay_3_8910_init(void)
{
  ay_3_8910_reset();
}

void ay_3_8910_reset(void)
{
int ix;

  ay_3_8910.current = 0x00;
  for(ix = 0; ix < 16; ix++) {
    ay_3_8910.registers[ix] = 0x00;
  }
}

void ay_3_8910_exit(void)
{
}
