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

void xcpc_snapshot_trace(const char* function)
{
    g_log ( XCPC_LOG_DOMAIN, G_LOG_LEVEL_DEBUG
          , "XcpcSnapshot::%s()"
          , function );
}

XcpcSnapshot* xcpc_snapshot_alloc(void)
{
    xcpc_snapshot_trace("alloc");

    return xcpc_new(XcpcSnapshot);
}

XcpcSnapshot* xcpc_snapshot_free(XcpcSnapshot* self)
{
    xcpc_snapshot_trace("free");

    return xcpc_delete(XcpcSnapshot, self);
}

XcpcSnapshot* xcpc_snapshot_construct(XcpcSnapshot* self)
{
    xcpc_snapshot_trace("construct");

    /* clear instance */ {
        (void) memset(self, 0, sizeof(XcpcSnapshot));
    }
    /* initialize signature */ {
        (void) memcpy(self->header.signature, snapshot_signature, sizeof(snapshot_signature));
    }
    /* initialize version */ {
        self->header.version = XCPC_SNAPSHOT_VERSION_1;
    }
    return self;
}

XcpcSnapshot* xcpc_snapshot_destruct(XcpcSnapshot* self)
{
    xcpc_snapshot_trace("destruct");

    return self;
}

XcpcSnapshot* xcpc_snapshot_new(void)
{
    xcpc_snapshot_trace("new");

    return xcpc_snapshot_construct(xcpc_snapshot_alloc());
}

XcpcSnapshot* xcpc_snapshot_delete(XcpcSnapshot* self)
{
    xcpc_snapshot_trace("delete");

    return xcpc_snapshot_free(xcpc_snapshot_destruct(self));
}

XcpcSnapshotStatus xcpc_snapshot_sanity_check(XcpcSnapshot* self)
{
    XcpcSnapshotStatus status = XCPC_SNAPSHOT_STATUS_SUCCESS;

    xcpc_snapshot_trace("sanity_check");
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

    xcpc_snapshot_trace("load");
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

    xcpc_snapshot_trace("save");
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
