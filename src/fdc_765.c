/*
 * fdc_765.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "fdc_765.h"

/**
 * FDC_765::init()
 *
 * @param self specifies the FDC_765 instance
 */
void fdc_765_init(FDC_765 *self)
{
  fdc_765_reset(self);
}

/**
 * FDC_765::clock()
 *
 * @param self specifies the FDC_765 instance
 */
void fdc_765_clock(FDC_765 *self)
{
}

/**
 * FDC_765::reset()
 *
 * @param self specifies the FDC_765 instance
 */
void fdc_765_reset(FDC_765 *self)
{
  self->status = 0x80;
  self->motors = 0x00;
}

/**
 * FDC_765::exit()
 *
 * @param self specifies the FDC_765 instance
 */
void fdc_765_exit(FDC_765 *self)
{
}
