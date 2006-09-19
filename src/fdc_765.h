/*
 * fdc_765.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __FDC_765_H__
#define __FDC_765_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FDC_765 {
  byte status;
  byte motors;
} FDC_765;

extern void fdc_765_init (FDC_765 *fdc_765);
extern void fdc_765_clock(FDC_765 *fdc_765);
extern void fdc_765_reset(FDC_765 *fdc_765);
extern void fdc_765_exit (FDC_765 *fdc_765);

#ifdef __cplusplus
}
#endif

#endif
