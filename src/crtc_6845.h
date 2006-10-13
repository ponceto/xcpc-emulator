/*
 * crtc_6845.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __CRTC_6845_H__
#define __CRTC_6845_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CRTC_6845 {
  byte addr_reg;
  byte reg_file[18];
  int hsync;
  int vsync;
} CRTC_6845;

extern void crtc_6845_init (CRTC_6845 *crtc_6845);
extern void crtc_6845_clock(CRTC_6845 *crtc_6845);
extern void crtc_6845_reset(CRTC_6845 *crtc_6845);
extern void crtc_6845_exit (CRTC_6845 *crtc_6845);

extern void crtc_6845_rs(CRTC_6845 *crtc_6845, byte data);
extern void crtc_6845_wr(CRTC_6845 *crtc_6845, byte data);
extern byte crtc_6845_rd(CRTC_6845 *crtc_6845);

#ifdef __cplusplus
}
#endif

#endif
