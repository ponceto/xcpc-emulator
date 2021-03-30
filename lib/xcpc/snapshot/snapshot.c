/*
 * snapshot.c - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snapshot-priv.h"
#include "snapshot-reader-priv.h"
#include "snapshot-writer-priv.h"

static const char snapshot_signature[8] = {
    'M', 'V', ' ', '-', ' ', 'S', 'N', 'A'
};

static void log_trace(const char* function)
{
    xcpc_log_trace("XcpcSnapshot::%s()", function);
}

XcpcSnapshot* xcpc_snapshot_alloc(void)
{
    log_trace("alloc");

    return xcpc_new(XcpcSnapshot);
}

XcpcSnapshot* xcpc_snapshot_free(XcpcSnapshot* self)
{
    log_trace("free");

    return xcpc_delete(XcpcSnapshot, self);
}

XcpcSnapshot* xcpc_snapshot_construct(XcpcSnapshot* self)
{
    log_trace("construct");

    /* clear instance */ {
        (void) memset(self, 0, sizeof(XcpcSnapshot));
    }
    /* initialize signature */ {
        (void) memcpy(self->header.signature, snapshot_signature, sizeof(snapshot_signature));
    }
    /* initialize version */ {
        self->header.version = XCPC_SNAPSHOT_VERSION_1;
    }
    /* initialize banknum */ {
        self->banknum = 0;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_destruct(XcpcSnapshot* self)
{
    log_trace("destruct");

    return self;
}

XcpcSnapshot* xcpc_snapshot_new(void)
{
    log_trace("new");

    return xcpc_snapshot_construct(xcpc_snapshot_alloc());
}

XcpcSnapshot* xcpc_snapshot_delete(XcpcSnapshot* self)
{
    log_trace("delete");

    return xcpc_snapshot_free(xcpc_snapshot_destruct(self));
}

XcpcSnapshotStatus xcpc_snapshot_sanity_check(XcpcSnapshot* self)
{
    XcpcSnapshotStatus status = XCPC_SNAPSHOT_STATUS_SUCCESS;

    log_trace("sanity_check");
    /* check header size */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            const size_t compiled_header_size = sizeof(self->header);
            const size_t expected_header_size = 256UL;
            if(compiled_header_size != expected_header_size) {
                status = XCPC_SNAPSHOT_STATUS_SANITY_CHECK_ERROR;
            }
        }
    }
    /* check memory size */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            const size_t compiled_memory_size = sizeof(self->memory);
            const size_t expected_memory_size = 512UL * 1024UL;
            if(compiled_memory_size != expected_memory_size) {
                status = XCPC_SNAPSHOT_STATUS_SANITY_CHECK_ERROR;
            }
        }
    }
    /* check signature */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            if(memcmp(self->header.signature, snapshot_signature, sizeof(snapshot_signature)) != 0) {
                status = XCPC_SNAPSHOT_STATUS_BAD_SIGNATURE;
            }
        }
    }
    /* check version */ {
        if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
            switch(self->header.version) {
                case XCPC_SNAPSHOT_VERSION_1:
                    break;
                case XCPC_SNAPSHOT_VERSION_2:
                    break;
                case XCPC_SNAPSHOT_VERSION_3:
                    break;
                default:
                    status = XCPC_SNAPSHOT_STATUS_BAD_VERSION;
                    break;
            }
        }
    }
    return status;
}

XcpcSnapshotStatus xcpc_snapshot_load(XcpcSnapshot* self, const char* filename)
{
    XcpcSnapshotStatus status = XCPC_SNAPSHOT_STATUS_SUCCESS;

    log_trace("load");
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
        status = xcpc_snapshot_sanity_check(self);
    }
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
        XcpcSnapshotReader* reader = xcpc_snapshot_reader_new(self);
        status = xcpc_snapshot_reader_load(reader, filename);
        reader = xcpc_snapshot_reader_delete(reader);
    }
    return status;
}

XcpcSnapshotStatus xcpc_snapshot_save(XcpcSnapshot* self, const char* filename)
{
    XcpcSnapshotStatus status = XCPC_SNAPSHOT_STATUS_SUCCESS;

    log_trace("save");
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
        status = xcpc_snapshot_sanity_check(self);
    }
    if(status == XCPC_SNAPSHOT_STATUS_SUCCESS) {
        XcpcSnapshotWriter* writer = xcpc_snapshot_writer_new(self);
        status = xcpc_snapshot_writer_save(writer, filename);
        writer = xcpc_snapshot_writer_delete(writer);
    }
    return status;
}

const char* xcpc_snapshot_strerror(XcpcSnapshotStatus status)
{
    switch(status) {
        case XCPC_SNAPSHOT_STATUS_FAILURE:
            return "XCPC_SNAPSHOT_STATUS_FAILURE";
        case XCPC_SNAPSHOT_STATUS_SUCCESS:
            return "XCPC_SNAPSHOT_STATUS_SUCCESS";
        case XCPC_SNAPSHOT_STATUS_SANITY_CHECK_ERROR:
            return "XCPC_SNAPSHOT_STATUS_SANITY_CHECK_ERROR";
        case XCPC_SNAPSHOT_STATUS_BAD_SIGNATURE:
            return "XCPC_SNAPSHOT_STATUS_BAD_SIGNATURE";
        case XCPC_SNAPSHOT_STATUS_BAD_VERSION:
            return "XCPC_SNAPSHOT_STATUS_BAD_VERSION";
        case XCPC_SNAPSHOT_STATUS_BAD_FILENAME:
            return "XCPC_SNAPSHOT_STATUS_BAD_FILENAME";
        case XCPC_SNAPSHOT_STATUS_FILE_ERROR:
            return "XCPC_SNAPSHOT_STATUS_FILE_ERROR";
        case XCPC_SNAPSHOT_STATUS_HEADER_ERROR:
            return "XCPC_SNAPSHOT_STATUS_HEADER_ERROR";
        case XCPC_SNAPSHOT_STATUS_MEMORY_ERROR:
            return "XCPC_SNAPSHOT_STATUS_MEMORY_ERROR";
        default:
            break;
    }
    return "XCPC_SNAPSHOT_UNKNOWN_ERROR";
}

XcpcSnapshot* xcpc_snapshot_fetch_cpu_z80a(XcpcSnapshot* self, XcpcCpuZ80a* cpu_z80a)
{
    log_trace("fetch_cpu_z80a");

    if(cpu_z80a != NULL) {
        (void) xcpc_cpu_z80a_set_af_l(cpu_z80a, self->header.cpu_p_af_l);
        (void) xcpc_cpu_z80a_set_af_h(cpu_z80a, self->header.cpu_p_af_h);
        (void) xcpc_cpu_z80a_set_bc_l(cpu_z80a, self->header.cpu_p_bc_l);
        (void) xcpc_cpu_z80a_set_bc_h(cpu_z80a, self->header.cpu_p_bc_h);
        (void) xcpc_cpu_z80a_set_de_l(cpu_z80a, self->header.cpu_p_de_l);
        (void) xcpc_cpu_z80a_set_de_h(cpu_z80a, self->header.cpu_p_de_h);
        (void) xcpc_cpu_z80a_set_hl_l(cpu_z80a, self->header.cpu_p_hl_l);
        (void) xcpc_cpu_z80a_set_hl_h(cpu_z80a, self->header.cpu_p_hl_h);
        (void) xcpc_cpu_z80a_set_ir_l(cpu_z80a, self->header.cpu_p_ir_l);
        (void) xcpc_cpu_z80a_set_ir_h(cpu_z80a, self->header.cpu_p_ir_h);
        (void) xcpc_cpu_z80a_set_iff1(cpu_z80a, self->header.cpu_p_iff1);
        (void) xcpc_cpu_z80a_set_iff2(cpu_z80a, self->header.cpu_p_iff2);
        (void) xcpc_cpu_z80a_set_ix_l(cpu_z80a, self->header.cpu_p_ix_l);
        (void) xcpc_cpu_z80a_set_ix_h(cpu_z80a, self->header.cpu_p_ix_h);
        (void) xcpc_cpu_z80a_set_iy_l(cpu_z80a, self->header.cpu_p_iy_l);
        (void) xcpc_cpu_z80a_set_iy_h(cpu_z80a, self->header.cpu_p_iy_h);
        (void) xcpc_cpu_z80a_set_sp_l(cpu_z80a, self->header.cpu_p_sp_l);
        (void) xcpc_cpu_z80a_set_sp_h(cpu_z80a, self->header.cpu_p_sp_h);
        (void) xcpc_cpu_z80a_set_pc_l(cpu_z80a, self->header.cpu_p_pc_l);
        (void) xcpc_cpu_z80a_set_pc_h(cpu_z80a, self->header.cpu_p_pc_h);
        (void) xcpc_cpu_z80a_set_im  (cpu_z80a, self->header.cpu_p_im_l);
        (void) xcpc_cpu_z80a_set_af_y(cpu_z80a, self->header.cpu_a_af_l);
        (void) xcpc_cpu_z80a_set_af_x(cpu_z80a, self->header.cpu_a_af_h);
        (void) xcpc_cpu_z80a_set_bc_y(cpu_z80a, self->header.cpu_a_bc_l);
        (void) xcpc_cpu_z80a_set_bc_x(cpu_z80a, self->header.cpu_a_bc_h);
        (void) xcpc_cpu_z80a_set_de_y(cpu_z80a, self->header.cpu_a_de_l);
        (void) xcpc_cpu_z80a_set_de_x(cpu_z80a, self->header.cpu_a_de_h);
        (void) xcpc_cpu_z80a_set_hl_y(cpu_z80a, self->header.cpu_a_hl_l);
        (void) xcpc_cpu_z80a_set_hl_x(cpu_z80a, self->header.cpu_a_hl_h);
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_vga_core(XcpcSnapshot* self, XcpcVgaCore* vga_core)
{
    log_trace("fetch_vga_core");

    if(vga_core != NULL) {
        vga_core->state.pen       = self->header.vga_ink_ix;
        vga_core->state.ink[0x00] = self->header.vga_ink_00;
        vga_core->state.ink[0x01] = self->header.vga_ink_01;
        vga_core->state.ink[0x02] = self->header.vga_ink_02;
        vga_core->state.ink[0x03] = self->header.vga_ink_03;
        vga_core->state.ink[0x04] = self->header.vga_ink_04;
        vga_core->state.ink[0x05] = self->header.vga_ink_05;
        vga_core->state.ink[0x06] = self->header.vga_ink_06;
        vga_core->state.ink[0x07] = self->header.vga_ink_07;
        vga_core->state.ink[0x08] = self->header.vga_ink_08;
        vga_core->state.ink[0x09] = self->header.vga_ink_09;
        vga_core->state.ink[0x0a] = self->header.vga_ink_10;
        vga_core->state.ink[0x0b] = self->header.vga_ink_11;
        vga_core->state.ink[0x0c] = self->header.vga_ink_12;
        vga_core->state.ink[0x0d] = self->header.vga_ink_13;
        vga_core->state.ink[0x0e] = self->header.vga_ink_14;
        vga_core->state.ink[0x0f] = self->header.vga_ink_15;
        vga_core->state.ink[0x10] = self->header.vga_ink_16;
        vga_core->state.rmr       = self->header.vga_config;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_vdc_6845(XcpcSnapshot* self, XcpcVdc6845* vdc_6845)
{
    log_trace("fetch_vdc_6845");

    if(vdc_6845 != NULL) {
        vdc_6845->state.regs.array.addr       = self->header.vdc_reg_ix;
        vdc_6845->state.regs.array.data[0x00] = self->header.vdc_reg_00;
        vdc_6845->state.regs.array.data[0x01] = self->header.vdc_reg_01;
        vdc_6845->state.regs.array.data[0x02] = self->header.vdc_reg_02;
        vdc_6845->state.regs.array.data[0x03] = self->header.vdc_reg_03;
        vdc_6845->state.regs.array.data[0x04] = self->header.vdc_reg_04;
        vdc_6845->state.regs.array.data[0x05] = self->header.vdc_reg_05;
        vdc_6845->state.regs.array.data[0x06] = self->header.vdc_reg_06;
        vdc_6845->state.regs.array.data[0x07] = self->header.vdc_reg_07;
        vdc_6845->state.regs.array.data[0x08] = self->header.vdc_reg_08;
        vdc_6845->state.regs.array.data[0x09] = self->header.vdc_reg_09;
        vdc_6845->state.regs.array.data[0x0a] = self->header.vdc_reg_10;
        vdc_6845->state.regs.array.data[0x0b] = self->header.vdc_reg_11;
        vdc_6845->state.regs.array.data[0x0c] = self->header.vdc_reg_12;
        vdc_6845->state.regs.array.data[0x0d] = self->header.vdc_reg_13;
        vdc_6845->state.regs.array.data[0x0e] = self->header.vdc_reg_14;
        vdc_6845->state.regs.array.data[0x0f] = self->header.vdc_reg_15;
        vdc_6845->state.regs.array.data[0x10] = self->header.vdc_reg_16;
        vdc_6845->state.regs.array.data[0x11] = self->header.vdc_reg_17;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_ppi_8255(XcpcSnapshot* self, XcpcPpi8255* ppi_8255)
{
    log_trace("fetch_ppi_8255");

    if(ppi_8255 != NULL) {
        ppi_8255->state.port_a = self->header.ppi_port_a;
        ppi_8255->state.port_b = self->header.ppi_port_b;
        ppi_8255->state.port_c = self->header.ppi_port_c;
        ppi_8255->state.ctrl_p = self->header.ppi_ctrl_p;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_psg_8910(XcpcSnapshot* self, XcpcPsg8910* psg_8910)
{
    log_trace("fetch_psg_8910");

    if(psg_8910 != NULL) {
        psg_8910->state.regs.array.addr       = self->header.psg_reg_ix;
        psg_8910->state.regs.array.data[0x00] = self->header.psg_reg_00;
        psg_8910->state.regs.array.data[0x01] = self->header.psg_reg_01;
        psg_8910->state.regs.array.data[0x02] = self->header.psg_reg_02;
        psg_8910->state.regs.array.data[0x03] = self->header.psg_reg_03;
        psg_8910->state.regs.array.data[0x04] = self->header.psg_reg_04;
        psg_8910->state.regs.array.data[0x05] = self->header.psg_reg_05;
        psg_8910->state.regs.array.data[0x06] = self->header.psg_reg_06;
        psg_8910->state.regs.array.data[0x07] = self->header.psg_reg_07;
        psg_8910->state.regs.array.data[0x08] = self->header.psg_reg_08;
        psg_8910->state.regs.array.data[0x09] = self->header.psg_reg_09;
        psg_8910->state.regs.array.data[0x0a] = self->header.psg_reg_10;
        psg_8910->state.regs.array.data[0x0b] = self->header.psg_reg_11;
        psg_8910->state.regs.array.data[0x0c] = self->header.psg_reg_12;
        psg_8910->state.regs.array.data[0x0d] = self->header.psg_reg_13;
        psg_8910->state.regs.array.data[0x0e] = self->header.psg_reg_14;
        psg_8910->state.regs.array.data[0x0f] = self->header.psg_reg_15;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_fdc_765a(XcpcSnapshot* self, XcpcFdc765a* fdc_765a)
{
    log_trace("fetch_fdc_765a");

    if(fdc_765a != NULL) {
        /* do nothing */
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_ram_bank(XcpcSnapshot* self, XcpcRamBank* ram_bank)
{
    log_trace("fetch_ram_bank");

    if(ram_bank != NULL) {
        if(self->banknum < countof(self->memory)) {
            uint8_t* dst = ram_bank->state.data;
            uint8_t* src = self->memory[self->banknum].data;
            size_t   len = sizeof(self->memory[self->banknum].data);
            (void) memcpy(dst, src, len);
            ++self->banknum;
        }
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_ram_conf(XcpcSnapshot* self, uint8_t* ram_conf)
{
    log_trace("fetch_ram_conf");

    if(ram_conf != NULL) {
        *ram_conf = self->header.ram_select;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_rom_conf(XcpcSnapshot* self, uint8_t* rom_conf)
{
    log_trace("fetch_rom_conf");

    if(rom_conf != NULL) {
        *rom_conf = self->header.rom_select;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_fetch_ram_size(XcpcSnapshot* self, uint32_t* ram_size)
{
    log_trace("fetch_ram_size");

    if(ram_size != NULL) {
        *ram_size = 0UL;
        *ram_size |= (((uint32_t)(self->header.ram_size_h)) << 18);
        *ram_size |= (((uint32_t)(self->header.ram_size_l)) << 10);
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_cpu_z80a(XcpcSnapshot* self, XcpcCpuZ80a* cpu_z80a)
{
    log_trace("store_cpu_z80a");

    if(cpu_z80a != NULL) {
        self->header.cpu_p_af_l = xcpc_cpu_z80a_get_af_l(cpu_z80a);
        self->header.cpu_p_af_h = xcpc_cpu_z80a_get_af_h(cpu_z80a);
        self->header.cpu_p_bc_l = xcpc_cpu_z80a_get_bc_l(cpu_z80a);
        self->header.cpu_p_bc_h = xcpc_cpu_z80a_get_bc_h(cpu_z80a);
        self->header.cpu_p_de_l = xcpc_cpu_z80a_get_de_l(cpu_z80a);
        self->header.cpu_p_de_h = xcpc_cpu_z80a_get_de_h(cpu_z80a);
        self->header.cpu_p_hl_l = xcpc_cpu_z80a_get_hl_l(cpu_z80a);
        self->header.cpu_p_hl_h = xcpc_cpu_z80a_get_hl_h(cpu_z80a);
        self->header.cpu_p_ir_l = xcpc_cpu_z80a_get_ir_l(cpu_z80a);
        self->header.cpu_p_ir_h = xcpc_cpu_z80a_get_ir_h(cpu_z80a);
        self->header.cpu_p_iff1 = xcpc_cpu_z80a_get_iff1(cpu_z80a);
        self->header.cpu_p_iff2 = xcpc_cpu_z80a_get_iff2(cpu_z80a);
        self->header.cpu_p_ix_l = xcpc_cpu_z80a_get_ix_l(cpu_z80a);
        self->header.cpu_p_ix_h = xcpc_cpu_z80a_get_ix_h(cpu_z80a);
        self->header.cpu_p_iy_l = xcpc_cpu_z80a_get_iy_l(cpu_z80a);
        self->header.cpu_p_iy_h = xcpc_cpu_z80a_get_iy_h(cpu_z80a);
        self->header.cpu_p_sp_l = xcpc_cpu_z80a_get_sp_l(cpu_z80a);
        self->header.cpu_p_sp_h = xcpc_cpu_z80a_get_sp_h(cpu_z80a);
        self->header.cpu_p_pc_l = xcpc_cpu_z80a_get_pc_l(cpu_z80a);
        self->header.cpu_p_pc_h = xcpc_cpu_z80a_get_pc_h(cpu_z80a);
        self->header.cpu_p_im_l = xcpc_cpu_z80a_get_im  (cpu_z80a);
        self->header.cpu_a_af_l = xcpc_cpu_z80a_get_af_y(cpu_z80a);
        self->header.cpu_a_af_h = xcpc_cpu_z80a_get_af_x(cpu_z80a);
        self->header.cpu_a_bc_l = xcpc_cpu_z80a_get_bc_y(cpu_z80a);
        self->header.cpu_a_bc_h = xcpc_cpu_z80a_get_bc_x(cpu_z80a);
        self->header.cpu_a_de_l = xcpc_cpu_z80a_get_de_y(cpu_z80a);
        self->header.cpu_a_de_h = xcpc_cpu_z80a_get_de_x(cpu_z80a);
        self->header.cpu_a_hl_l = xcpc_cpu_z80a_get_hl_y(cpu_z80a);
        self->header.cpu_a_hl_h = xcpc_cpu_z80a_get_hl_x(cpu_z80a);
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_vga_core(XcpcSnapshot* self, XcpcVgaCore* vga_core)
{
    log_trace("store_vga_core");

    if(vga_core != NULL) {
        self->header.vga_ink_ix = vga_core->state.pen;
        self->header.vga_ink_00 = vga_core->state.ink[0x00];
        self->header.vga_ink_01 = vga_core->state.ink[0x01];
        self->header.vga_ink_02 = vga_core->state.ink[0x02];
        self->header.vga_ink_03 = vga_core->state.ink[0x03];
        self->header.vga_ink_04 = vga_core->state.ink[0x04];
        self->header.vga_ink_05 = vga_core->state.ink[0x05];
        self->header.vga_ink_06 = vga_core->state.ink[0x06];
        self->header.vga_ink_07 = vga_core->state.ink[0x07];
        self->header.vga_ink_08 = vga_core->state.ink[0x08];
        self->header.vga_ink_09 = vga_core->state.ink[0x09];
        self->header.vga_ink_10 = vga_core->state.ink[0x0a];
        self->header.vga_ink_11 = vga_core->state.ink[0x0b];
        self->header.vga_ink_12 = vga_core->state.ink[0x0c];
        self->header.vga_ink_13 = vga_core->state.ink[0x0d];
        self->header.vga_ink_14 = vga_core->state.ink[0x0e];
        self->header.vga_ink_15 = vga_core->state.ink[0x0f];
        self->header.vga_ink_16 = vga_core->state.ink[0x10];
        self->header.vga_config = vga_core->state.rmr;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_vdc_6845(XcpcSnapshot* self, XcpcVdc6845* vdc_6845)
{
    log_trace("store_vdc_6845");

    if(vdc_6845 != NULL) {
        self->header.vdc_reg_ix = vdc_6845->state.regs.array.addr;
        self->header.vdc_reg_00 = vdc_6845->state.regs.array.data[0x00];
        self->header.vdc_reg_01 = vdc_6845->state.regs.array.data[0x01];
        self->header.vdc_reg_02 = vdc_6845->state.regs.array.data[0x02];
        self->header.vdc_reg_03 = vdc_6845->state.regs.array.data[0x03];
        self->header.vdc_reg_04 = vdc_6845->state.regs.array.data[0x04];
        self->header.vdc_reg_05 = vdc_6845->state.regs.array.data[0x05];
        self->header.vdc_reg_06 = vdc_6845->state.regs.array.data[0x06];
        self->header.vdc_reg_07 = vdc_6845->state.regs.array.data[0x07];
        self->header.vdc_reg_08 = vdc_6845->state.regs.array.data[0x08];
        self->header.vdc_reg_09 = vdc_6845->state.regs.array.data[0x09];
        self->header.vdc_reg_10 = vdc_6845->state.regs.array.data[0x0a];
        self->header.vdc_reg_11 = vdc_6845->state.regs.array.data[0x0b];
        self->header.vdc_reg_12 = vdc_6845->state.regs.array.data[0x0c];
        self->header.vdc_reg_13 = vdc_6845->state.regs.array.data[0x0d];
        self->header.vdc_reg_14 = vdc_6845->state.regs.array.data[0x0e];
        self->header.vdc_reg_15 = vdc_6845->state.regs.array.data[0x0f];
        self->header.vdc_reg_16 = vdc_6845->state.regs.array.data[0x10];
        self->header.vdc_reg_17 = vdc_6845->state.regs.array.data[0x11];
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_ppi_8255(XcpcSnapshot* self, XcpcPpi8255* ppi_8255)
{
    log_trace("store_ppi_8255");

    if(ppi_8255 != NULL) {
        self->header.ppi_port_a = ppi_8255->state.port_a;
        self->header.ppi_port_b = ppi_8255->state.port_b;
        self->header.ppi_port_c = ppi_8255->state.port_c;
        self->header.ppi_ctrl_p = ppi_8255->state.ctrl_p;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_psg_8910(XcpcSnapshot* self, XcpcPsg8910* psg_8910)
{
    log_trace("store_psg_8910");

    if(psg_8910 != NULL) {
        self->header.psg_reg_ix = psg_8910->state.regs.array.addr;
        self->header.psg_reg_00 = psg_8910->state.regs.array.data[0x00];
        self->header.psg_reg_01 = psg_8910->state.regs.array.data[0x01];
        self->header.psg_reg_02 = psg_8910->state.regs.array.data[0x02];
        self->header.psg_reg_03 = psg_8910->state.regs.array.data[0x03];
        self->header.psg_reg_04 = psg_8910->state.regs.array.data[0x04];
        self->header.psg_reg_05 = psg_8910->state.regs.array.data[0x05];
        self->header.psg_reg_06 = psg_8910->state.regs.array.data[0x06];
        self->header.psg_reg_07 = psg_8910->state.regs.array.data[0x07];
        self->header.psg_reg_08 = psg_8910->state.regs.array.data[0x08];
        self->header.psg_reg_09 = psg_8910->state.regs.array.data[0x09];
        self->header.psg_reg_10 = psg_8910->state.regs.array.data[0x0a];
        self->header.psg_reg_11 = psg_8910->state.regs.array.data[0x0b];
        self->header.psg_reg_12 = psg_8910->state.regs.array.data[0x0c];
        self->header.psg_reg_13 = psg_8910->state.regs.array.data[0x0d];
        self->header.psg_reg_14 = psg_8910->state.regs.array.data[0x0e];
        self->header.psg_reg_15 = psg_8910->state.regs.array.data[0x0f];
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_fdc_765a(XcpcSnapshot* self, XcpcFdc765a* fdc_765a)
{
    log_trace("store_fdc_765a");

    if(fdc_765a != NULL) {
        /* do nothing */
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_ram_bank(XcpcSnapshot* self, XcpcRamBank* ram_bank)
{
    log_trace("store_ram_bank");

    if(ram_bank != NULL) {
        if(self->banknum < countof(self->memory)) {
            uint8_t* src = ram_bank->state.data;
            uint8_t* dst = self->memory[self->banknum].data;
            size_t   len = sizeof(self->memory[self->banknum].data);
            (void) memcpy(dst, src, len);
            ++self->banknum;
        }
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_ram_conf(XcpcSnapshot* self, uint8_t* ram_conf)
{
    log_trace("store_ram_conf");

    if(ram_conf != NULL) {
        self->header.ram_select = *ram_conf;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_rom_conf(XcpcSnapshot* self, uint8_t* rom_conf)
{
    log_trace("store_rom_conf");

    if(rom_conf != NULL) {
        self->header.rom_select = *rom_conf;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_store_ram_size(XcpcSnapshot* self, uint32_t* ram_size)
{
    log_trace("store_ram_size");

    if(ram_size != NULL) {
        self->header.ram_size_l = ((uint8_t)(((*ram_size) >> 10) & 0xff));
        self->header.ram_size_h = ((uint8_t)(((*ram_size) >> 18) & 0xff));
    }
    return self;
}
