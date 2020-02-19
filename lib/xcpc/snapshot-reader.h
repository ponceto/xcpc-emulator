/*
 * snapshot-reader.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_SNAPSHOT_READER_H__
#define __XCPC_SNAPSHOT_READER_H__

#include <xcpc/snapshot-reader-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcSnapshotReader* xcpc_snapshot_reader_alloc     (void);
extern XcpcSnapshotReader* xcpc_snapshot_reader_free      (XcpcSnapshotReader* snapshot_reader);
extern XcpcSnapshotReader* xcpc_snapshot_reader_construct (XcpcSnapshotReader* snapshot_reader, XcpcSnapshot* snapshot);
extern XcpcSnapshotReader* xcpc_snapshot_reader_destruct  (XcpcSnapshotReader* snapshot_reader);
extern XcpcSnapshotReader* xcpc_snapshot_reader_new       (XcpcSnapshot* snapshot);
extern XcpcSnapshotReader* xcpc_snapshot_reader_delete    (XcpcSnapshotReader* snapshot_reader);
extern XcpcSnapshotStatus  xcpc_snapshot_reader_load      (XcpcSnapshotReader* snapshot_reader, const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_SNAPSHOT_READER_H__ */
