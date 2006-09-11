/*
 * ay_3_8910.h - Copyright (c) 2001, 2006 Olivier Poncet
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
#ifndef __AY_3_8910_H__
#define __AY_3_8910_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  byte current;
  byte registers[16];
} AY_3_8910;

extern AY_3_8910 ay_3_8910;

extern void ay_3_8910_init(void);
extern void ay_3_8910_reset(void);
extern void ay_3_8910_exit(void);

#ifdef __cplusplus
}
#endif

#endif
