/*
 * ppi_8255.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __PPI_8255_H__
#define __PPI_8255_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PPI_8255 {
  byte control;
  byte port_a;
  byte port_b;
  byte port_c;
} PPI_8255;

extern void ppi_8255_init (PPI_8255 *ppi_8255);
extern void ppi_8255_clock(PPI_8255 *ppi_8255);
extern void ppi_8255_reset(PPI_8255 *ppi_8255);
extern void ppi_8255_exit (PPI_8255 *ppi_8255);

#ifdef __cplusplus
}
#endif

#endif
