/*
 * snapshot.h - Copyright (c) 2001-2023 - Olivier Poncet
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
#ifndef __XCPC_SNAPSHOT_H__
#define __XCPC_SNAPSHOT_H__

#include <xcpc/snapshot/snapshot-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcSnapshot*      xcpc_snapshot_alloc        (void);
extern XcpcSnapshot*      xcpc_snapshot_free         (XcpcSnapshot* snapshot);
extern XcpcSnapshot*      xcpc_snapshot_construct    (XcpcSnapshot* snapshot);
extern XcpcSnapshot*      xcpc_snapshot_destruct     (XcpcSnapshot* snapshot);
extern XcpcSnapshot*      xcpc_snapshot_new          (void);
extern XcpcSnapshot*      xcpc_snapshot_delete       (XcpcSnapshot* snapshot);
extern XcpcSnapshotStatus xcpc_snapshot_sanity_check (XcpcSnapshot* snapshot);
extern XcpcSnapshotStatus xcpc_snapshot_load         (XcpcSnapshot* snapshot, const char* filename);
extern XcpcSnapshotStatus xcpc_snapshot_save         (XcpcSnapshot* snapshot, const char* filename);
extern const char*        xcpc_snapshot_strerror     (XcpcSnapshotStatus status);

extern XcpcSnapshot*      xcpc_snapshot_fetch_cpu_z80a (XcpcSnapshot* snapshot, XcpcCpuZ80a* cpu_z80a);
extern XcpcSnapshot*      xcpc_snapshot_fetch_vga_core (XcpcSnapshot* snapshot, XcpcVgaCore* vga_core);
extern XcpcSnapshot*      xcpc_snapshot_fetch_vdc_6845 (XcpcSnapshot* snapshot, XcpcVdc6845* vdc_6845);
extern XcpcSnapshot*      xcpc_snapshot_fetch_ppi_8255 (XcpcSnapshot* snapshot, XcpcPpi8255* ppi_8255);
extern XcpcSnapshot*      xcpc_snapshot_fetch_psg_8910 (XcpcSnapshot* snapshot, XcpcPsg8910* psg_8910);
extern XcpcSnapshot*      xcpc_snapshot_fetch_fdc_765a (XcpcSnapshot* snapshot, XcpcFdc765a* fdc_765a);
extern XcpcSnapshot*      xcpc_snapshot_fetch_ram_bank (XcpcSnapshot* snapshot, XcpcRamBank* ram_bank);
extern XcpcSnapshot*      xcpc_snapshot_fetch_ram_conf (XcpcSnapshot* snapshot, uint8_t*     ram_conf);
extern XcpcSnapshot*      xcpc_snapshot_fetch_rom_conf (XcpcSnapshot* snapshot, uint8_t*     rom_conf);
extern XcpcSnapshot*      xcpc_snapshot_fetch_ram_size (XcpcSnapshot* snapshot, uint32_t*    ram_size);

extern XcpcSnapshot*      xcpc_snapshot_store_cpu_z80a (XcpcSnapshot* snapshot, XcpcCpuZ80a* cpu_z80a);
extern XcpcSnapshot*      xcpc_snapshot_store_vga_core (XcpcSnapshot* snapshot, XcpcVgaCore* vga_core);
extern XcpcSnapshot*      xcpc_snapshot_store_vdc_6845 (XcpcSnapshot* snapshot, XcpcVdc6845* vdc_6845);
extern XcpcSnapshot*      xcpc_snapshot_store_ppi_8255 (XcpcSnapshot* snapshot, XcpcPpi8255* ppi_8255);
extern XcpcSnapshot*      xcpc_snapshot_store_psg_8910 (XcpcSnapshot* snapshot, XcpcPsg8910* psg_8910);
extern XcpcSnapshot*      xcpc_snapshot_store_fdc_765a (XcpcSnapshot* snapshot, XcpcFdc765a* fdc_765a);
extern XcpcSnapshot*      xcpc_snapshot_store_ram_bank (XcpcSnapshot* snapshot, XcpcRamBank* ram_bank);
extern XcpcSnapshot*      xcpc_snapshot_store_ram_conf (XcpcSnapshot* snapshot, uint8_t*     ram_conf);
extern XcpcSnapshot*      xcpc_snapshot_store_rom_conf (XcpcSnapshot* snapshot, uint8_t*     rom_conf);
extern XcpcSnapshot*      xcpc_snapshot_store_ram_size (XcpcSnapshot* snapshot, uint32_t*    ram_size);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SNAPSHOT_H__ */
