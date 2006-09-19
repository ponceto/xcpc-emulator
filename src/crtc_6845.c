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
 * CRTC-6845::init()
 *
 * @param self specifies the CRTC-6845 instance
 */
void crtc_6845_init(CRTC_6845 *self)
{
  crtc_6845_reset(self);
}

/**
 * CRTC-6845::clock()
 *
 * @param self specifies the CRTC-6845 instance
 */
void crtc_6845_clock(CRTC_6845 *self)
{
}

/**
 * CRTC-6845::reset()
 *
 * @param self specifies the CRTC-6845 instance
 */
void crtc_6845_reset(CRTC_6845 *self)
{
  int ix;

  self->current = 0x00;
  for(ix = 0; ix < 18; ix++) {
    self->registers[ix] = 0x00;
  }
}

/**
 * CRTC-6845::exit()
 *
 * @param self specifies the CRTC-6845 instance
 */
void crtc_6845_exit(CRTC_6845 *self)
{
}
