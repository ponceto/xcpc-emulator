/*
 * ppi-8255-impl.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_PPI_8255_IMPL_H__
#define __XCPC_PPI_8255_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcPpi8255 XcpcPpi8255;

struct _XcpcPpi8255
{
    uint8_t control;
    uint8_t port_a;
    uint8_t port_b;
    uint8_t port_c;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PPI_8255_IMPL_H__ */
