/*
 * snapshot-writer.h - Copyright (c) 2001-2023 - Olivier Poncet
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
#ifndef __XCPC_SNAPSHOT_WRITER_H__
#define __XCPC_SNAPSHOT_WRITER_H__

#include <xcpc/snapshot/snapshot-writer-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcSnapshotWriter* xcpc_snapshot_writer_alloc     (void);
extern XcpcSnapshotWriter* xcpc_snapshot_writer_free      (XcpcSnapshotWriter* snapshot_writer);
extern XcpcSnapshotWriter* xcpc_snapshot_writer_construct (XcpcSnapshotWriter* snapshot_writer, XcpcSnapshot* snapshot);
extern XcpcSnapshotWriter* xcpc_snapshot_writer_destruct  (XcpcSnapshotWriter* snapshot_writer);
extern XcpcSnapshotWriter* xcpc_snapshot_writer_new       (XcpcSnapshot* snapshot);
extern XcpcSnapshotWriter* xcpc_snapshot_writer_delete    (XcpcSnapshotWriter* snapshot_writer);
extern XcpcSnapshotStatus  xcpc_snapshot_writer_save      (XcpcSnapshotWriter* snapshot_writer, const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SNAPSHOT_WRITER_H__ */
