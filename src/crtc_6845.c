/*
 * crtc_6845.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "crtc_6845.h"

/**
 * CRTC_6845::init()
 *
 * @param self specifies the CRTC_6845 instance
 */
void crtc_6845_init(CRTC_6845 *self)
{
  crtc_6845_reset(self);
}

/**
 * CRTC_6845::clock()
 *
 * @param self specifies the CRTC_6845 instance
 */
void crtc_6845_clock(CRTC_6845 *self)
{
}

/**
 * CRTC_6845::reset()
 *
 * @param self specifies the CRTC_6845 instance
 */
void crtc_6845_reset(CRTC_6845 *self)
{
  self->addr_reg     = 0x00; /* Address Register      [---xxxxx] */
  self->reg_file[ 0] = 0x65; /* Horiz. Total          [hhhhhhhh] */
  self->reg_file[ 1] = 0x50; /* Horiz. Displayed      [hhhhhhhh] */
  self->reg_file[ 2] = 0x56; /* Horiz. Sync Position  [hhhhhhhh] */
  self->reg_file[ 3] = 0x09; /* H // V Sync Width     [vvvvhhhh] */
  self->reg_file[ 4] = 0x18; /* Verti. Total          [-vvvvvvv] */
  self->reg_file[ 5] = 0x0a; /* Verti. Total Adjust   [---vvvvv] */
  self->reg_file[ 6] = 0x18; /* Verti. Displayed      [-vvvvvvv] */
  self->reg_file[ 7] = 0x18; /* Verti. Sync Position  [-vvvvvvv] */
  self->reg_file[ 8] = 0x00; /* Interlace Mode        [------xx] */
  self->reg_file[ 9] = 0x0b; /* Max Scan Line Address [---xxxxx] */
  self->reg_file[10] = 0x00; /* Cursor Start          [-xxxxxxx] */
  self->reg_file[11] = 0x0b; /* Cursor End            [---xxxxx] */
  self->reg_file[12] = 0x00; /* Start Address (MSB)   [--xxxxxx] */
  self->reg_file[13] = 0x80; /* Start Address (LSB)   [xxxxxxxx] */
  self->reg_file[14] = 0x00; /* Cursor (MSB)          [--xxxxxx] */
  self->reg_file[15] = 0x80; /* Cursor (LSB)          [xxxxxxxx] */
  self->reg_file[16] = 0x00; /* Light Pen (MSB)       [--xxxxxx] */
  self->reg_file[17] = 0x00; /* Light Pen (LSB)       [xxxxxxxx] */
  self->hsync = 0;
  self->vsync = 0;
}

/**
 * CRTC_6845::exit()
 *
 * @param self specifies the CRTC_6845 instance
 */
void crtc_6845_exit(CRTC_6845 *self)
{
}
