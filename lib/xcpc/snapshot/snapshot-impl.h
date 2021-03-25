/*
 * snapshot-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_SNAPSHOT_IMPL_H__
#define __XCPC_SNAPSHOT_IMPL_H__

#include <xcpc/libxcpc.h>
#include <xcpc/cpu-z80a/cpu-z80a.h>
#include <xcpc/vga-core/vga-core.h>
#include <xcpc/vdc-6845/vdc-6845.h>
#include <xcpc/ppi-8255/ppi-8255.h>
#include <xcpc/psg-8910/psg-8910.h>
#include <xcpc/fdc-765a/fdc-765a.h>
#include <xcpc/ram-bank/ram-bank.h>
#include <xcpc/rom-bank/rom-bank.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum   _XcpcSnapshotStatus  XcpcSnapshotStatus;
typedef enum   _XcpcSnapshotVersion XcpcSnapshotVersion;
typedef struct _XcpcSnapshotHeader  XcpcSnapshotHeader;
typedef struct _XcpcSnapshotMemory  XcpcSnapshotMemory;
typedef struct _XcpcSnapshot        XcpcSnapshot;

enum _XcpcSnapshotStatus
{
    XCPC_SNAPSHOT_STATUS_FAILURE            = -1,
    XCPC_SNAPSHOT_STATUS_SUCCESS            =  0,
    XCPC_SNAPSHOT_STATUS_SANITY_CHECK_ERROR =  1,
    XCPC_SNAPSHOT_STATUS_BAD_SIGNATURE      =  2,
    XCPC_SNAPSHOT_STATUS_BAD_VERSION        =  3,
    XCPC_SNAPSHOT_STATUS_BAD_FILENAME       =  4,
    XCPC_SNAPSHOT_STATUS_FILE_ERROR         =  5,
    XCPC_SNAPSHOT_STATUS_HEADER_ERROR       =  6,
    XCPC_SNAPSHOT_STATUS_MEMORY_ERROR       =  7,
};

enum _XcpcSnapshotVersion
{
    XCPC_SNAPSHOT_VERSION_1 = 1,
    XCPC_SNAPSHOT_VERSION_2 = 2,
    XCPC_SNAPSHOT_VERSION_3 = 3,
};

struct _XcpcSnapshotHeader
{
    uint8_t signature[8];
    uint8_t reserved[8];
    uint8_t version;
    uint8_t cpu_p_af_l;
    uint8_t cpu_p_af_h;
    uint8_t cpu_p_bc_l;
    uint8_t cpu_p_bc_h;
    uint8_t cpu_p_de_l;
    uint8_t cpu_p_de_h;
    uint8_t cpu_p_hl_l;
    uint8_t cpu_p_hl_h;
    uint8_t cpu_p_ir_l;
    uint8_t cpu_p_ir_h;
    uint8_t cpu_p_iff1;
    uint8_t cpu_p_iff2;
    uint8_t cpu_p_ix_l;
    uint8_t cpu_p_ix_h;
    uint8_t cpu_p_iy_l;
    uint8_t cpu_p_iy_h;
    uint8_t cpu_p_sp_l;
    uint8_t cpu_p_sp_h;
    uint8_t cpu_p_pc_l;
    uint8_t cpu_p_pc_h;
    uint8_t cpu_p_im_l;
    uint8_t cpu_a_af_l;
    uint8_t cpu_a_af_h;
    uint8_t cpu_a_bc_l;
    uint8_t cpu_a_bc_h;
    uint8_t cpu_a_de_l;
    uint8_t cpu_a_de_h;
    uint8_t cpu_a_hl_l;
    uint8_t cpu_a_hl_h;
    uint8_t vga_ink_ix;
    uint8_t vga_ink_00;
    uint8_t vga_ink_01;
    uint8_t vga_ink_02;
    uint8_t vga_ink_03;
    uint8_t vga_ink_04;
    uint8_t vga_ink_05;
    uint8_t vga_ink_06;
    uint8_t vga_ink_07;
    uint8_t vga_ink_08;
    uint8_t vga_ink_09;
    uint8_t vga_ink_10;
    uint8_t vga_ink_11;
    uint8_t vga_ink_12;
    uint8_t vga_ink_13;
    uint8_t vga_ink_14;
    uint8_t vga_ink_15;
    uint8_t vga_ink_16;
    uint8_t vga_config;
    uint8_t ram_select;
    uint8_t vdc_reg_ix;
    uint8_t vdc_reg_00;
    uint8_t vdc_reg_01;
    uint8_t vdc_reg_02;
    uint8_t vdc_reg_03;
    uint8_t vdc_reg_04;
    uint8_t vdc_reg_05;
    uint8_t vdc_reg_06;
    uint8_t vdc_reg_07;
    uint8_t vdc_reg_08;
    uint8_t vdc_reg_09;
    uint8_t vdc_reg_10;
    uint8_t vdc_reg_11;
    uint8_t vdc_reg_12;
    uint8_t vdc_reg_13;
    uint8_t vdc_reg_14;
    uint8_t vdc_reg_15;
    uint8_t vdc_reg_16;
    uint8_t vdc_reg_17;
    uint8_t rom_select;
    uint8_t ppi_port_a;
    uint8_t ppi_port_b;
    uint8_t ppi_port_c;
    uint8_t ppi_ctrl_p;
    uint8_t psg_reg_ix;
    uint8_t psg_reg_00;
    uint8_t psg_reg_01;
    uint8_t psg_reg_02;
    uint8_t psg_reg_03;
    uint8_t psg_reg_04;
    uint8_t psg_reg_05;
    uint8_t psg_reg_06;
    uint8_t psg_reg_07;
    uint8_t psg_reg_08;
    uint8_t psg_reg_09;
    uint8_t psg_reg_10;
    uint8_t psg_reg_11;
    uint8_t psg_reg_12;
    uint8_t psg_reg_13;
    uint8_t psg_reg_14;
    uint8_t psg_reg_15;
    uint8_t ram_size_l;
    uint8_t ram_size_h;
    uint8_t padding[147];
};

struct _XcpcSnapshotMemory
{
    uint8_t data[16384];
};

struct _XcpcSnapshot
{
    XcpcSnapshotHeader header;
    XcpcSnapshotMemory memory[32];
    unsigned int       banknum;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SNAPSHOT_IMPL_H__ */
