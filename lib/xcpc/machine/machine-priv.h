/*
 * machine-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __XCPC_MACHINE_PRIV_H__
#define __XCPC_MACHINE_PRIV_H__

#include <xcpc/machine/machine.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcHorzProps XcpcHorzProps;
typedef struct _XcpcVertProps XcpcVertProps;
typedef struct _XcpcBorders   XcpcBorders;

struct _XcpcHorzProps
{
    int cw;  /* h. char width    : pixels */
    int ht;  /* h. total         : chars  */
    int hd;  /* h. displayed     : chars  */
    int hsp; /* h. sync position : chars  */
    int hsw; /* h. sync width    : pixels */
};

struct _XcpcVertProps
{
    int ch;  /* v. char height   : pixels */
    int vt;  /* v. total         : chars  */
    int vd;  /* v. displayed     : chars  */
    int vsp; /* v. sync position : chars  */
    int vsw; /* v. sync width    : pixels */
};

struct _XcpcBorders
{
    int top; /* top border    : pixels */
    int bot; /* bottom border : pixels */
    int lft; /* left border   : pixels */
    int rgt; /* right border  : pixels */
};

#define XCPC_BYTE_PTR(pointer) ((uint8_t*)(pointer))
#define XCPC_WORD_PTR(pointer) ((uint16_t*)(pointer))
#define XCPC_LONG_PTR(pointer) ((uint32_t*)(pointer))

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MACHINE_PRIV_H__ */
