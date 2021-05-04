/*
 * ppi-8255-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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

#define XCPC_PPI_8255_IFACE(instance) (&(instance)->iface)
#define XCPC_PPI_8255_SETUP(instance) (&(instance)->setup)
#define XCPC_PPI_8255_STATE(instance) (&(instance)->state)

#define XCPC_PPI_8255_RD_FUNC(func) ((XcpcPpi8255RdFunc)(func))
#define XCPC_PPI_8255_WR_FUNC(func) ((XcpcPpi8255WrFunc)(func))

typedef struct _XcpcPpi8255Iface XcpcPpi8255Iface;
typedef struct _XcpcPpi8255Setup XcpcPpi8255Setup;
typedef struct _XcpcPpi8255State XcpcPpi8255State;
typedef struct _XcpcPpi8255Ports XcpcPpi8255Ports;
typedef struct _XcpcPpi8255      XcpcPpi8255;

typedef uint8_t (*XcpcPpi8255RdFunc)(XcpcPpi8255* ppi_8255, uint8_t data, void* user_data);
typedef uint8_t (*XcpcPpi8255WrFunc)(XcpcPpi8255* ppi_8255, uint8_t data, void* user_data);

struct _XcpcPpi8255Iface
{
    void* user_data;
    XcpcPpi8255RdFunc rd_port_a;
    XcpcPpi8255WrFunc wr_port_a;
    XcpcPpi8255RdFunc rd_port_b;
    XcpcPpi8255WrFunc wr_port_b;
    XcpcPpi8255RdFunc rd_port_c;
    XcpcPpi8255WrFunc wr_port_c;
};

struct _XcpcPpi8255Setup
{
    int reserved;
};

struct _XcpcPpi8255State
{
    uint8_t port_a;
    uint8_t port_b;
    uint8_t port_c;
    uint8_t ctrl_p;
};

struct _XcpcPpi8255Ports
{
    uint8_t ga;
    uint8_t gb;
    uint8_t pa;
    uint8_t pb;
    uint8_t pc;
};

struct _XcpcPpi8255
{
    XcpcPpi8255Iface iface;
    XcpcPpi8255Setup setup;
    XcpcPpi8255State state;
    XcpcPpi8255Ports ports;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PPI_8255_IMPL_H__ */
