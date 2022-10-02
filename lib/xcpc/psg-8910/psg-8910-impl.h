/*
 * psg-8910-impl.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __XCPC_PSG_8910_IMPL_H__
#define __XCPC_PSG_8910_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_PSG_8910_RD_FUNC(func) ((XcpcPsg8910RdFunc)(func))
#define XCPC_PSG_8910_WR_FUNC(func) ((XcpcPsg8910WrFunc)(func))

typedef struct _XcpcPsg8910Iface XcpcPsg8910Iface;
typedef struct _XcpcPsg8910Setup XcpcPsg8910Setup;
typedef struct _XcpcPsg8910State XcpcPsg8910State;
typedef struct _XcpcPsg8910      XcpcPsg8910;

typedef union  _XcpcPsg8910Registers XcpcPsg8910Registers;
typedef struct _XcpcPsg8910Clock     XcpcPsg8910Clock;
typedef struct _XcpcPsg8910Tone      XcpcPsg8910Tone;
typedef struct _XcpcPsg8910Noise     XcpcPsg8910Noise;
typedef struct _XcpcPsg8910Envelope  XcpcPsg8910Envelope;
typedef struct _XcpcPsg8910Channel   XcpcPsg8910Channel;
typedef uint8_t (*XcpcPsg8910RdFunc)(XcpcPsg8910* psg_8910, uint8_t data, void* user_data);
typedef uint8_t (*XcpcPsg8910WrFunc)(XcpcPsg8910* psg_8910, uint8_t data, void* user_data);

union _XcpcPsg8910Registers
{
    struct
    {
        uint8_t addr;
        uint8_t data[16];
    } array;
    struct
    {
        uint8_t address_register;
        uint8_t channel_a_fine_tune;
        uint8_t channel_a_coarse_tune;
        uint8_t channel_b_fine_tune;
        uint8_t channel_b_coarse_tune;
        uint8_t channel_c_fine_tune;
        uint8_t channel_c_coarse_tune;
        uint8_t noise_generator;
        uint8_t mixer_and_io_control;
        uint8_t channel_a_amplitude;
        uint8_t channel_b_amplitude;
        uint8_t channel_c_amplitude;
        uint8_t envelope_fine_tune;
        uint8_t envelope_coarse_tune;
        uint8_t envelope_shape;
        uint8_t io_port_a;
        uint8_t io_port_b;
    } named;
};

struct _XcpcPsg8910Tone
{
    uint32_t period;
    uint32_t counter;
    uint32_t amplitude;
    uint32_t signal;
};

struct _XcpcPsg8910Noise
{
    uint32_t period;
    uint32_t counter;
};

struct _XcpcPsg8910Envelope
{
    uint32_t period;
    uint32_t counter;
    uint32_t shape;
};

struct _XcpcPsg8910Channel
{
    uint8_t  buffer[65536];
    int      rd_index;
    int      wr_index;
};

struct _XcpcPsg8910Clock
{
    uint32_t counter;
};

struct _XcpcPsg8910Iface
{
    void* user_data;
    XcpcPsg8910RdFunc rd_port_a;
    XcpcPsg8910WrFunc wr_port_a;
    XcpcPsg8910RdFunc rd_port_b;
    XcpcPsg8910WrFunc wr_port_b;
};

struct _XcpcPsg8910Setup
{
    struct
    {
        uint8_t addr;
        uint8_t data[16];
    } caps_of;
    struct
    {
        uint8_t addr;
        uint8_t data[16];
    } mask_of;
    struct
    {
        uint16_t level[16];
    } volume;
};

struct _XcpcPsg8910State
{
    XcpcPsg8910Registers regs;
    XcpcPsg8910Clock     clock;
    XcpcPsg8910Tone      tone[3];
    XcpcPsg8910Noise     noise;
    XcpcPsg8910Envelope  envelope;
    XcpcPsg8910Channel   channel[3];
};

struct _XcpcPsg8910
{
    XcpcPsg8910Iface iface;
    XcpcPsg8910Setup setup;
    XcpcPsg8910State state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_PSG_8910_IMPL_H__ */
