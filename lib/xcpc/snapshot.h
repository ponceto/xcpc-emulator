/*
 * snapshot.h - Copyright (c) 2001-2020 - Olivier Poncet
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

#include <xcpc/snapshot-impl.h>

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

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SNAPSHOT_H__ */
