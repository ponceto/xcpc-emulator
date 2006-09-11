/*
 * ppi_8255.c - Copyright (c) 2001, 2006 Olivier Poncet
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
#include "ppi_8255.h"

PPI_8255 ppi_8255;

void ppi_8255_init(void)
{
  ppi_8255_reset();
}

void ppi_8255_reset(void)
{
  ppi_8255.control = 0x00;
  ppi_8255.port_a = 0x00;
  ppi_8255.port_b = 0x00;
  ppi_8255.port_c = 0x00;
}

void ppi_8255_exit(void)
{
}
