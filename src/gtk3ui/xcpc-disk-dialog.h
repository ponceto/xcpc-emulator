/*
 * xcpc-disk-dialog.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_GTK3UI_DISK_DIALOG_H__
#define __XCPC_GTK3UI_DISK_DIALOG_H__

#include "xcpc-application.h"

// ---------------------------------------------------------------------------
// xcpc::CreateDiskDialog
// ---------------------------------------------------------------------------

namespace xcpc {

class CreateDiskDialog final
    : public base::CreateDiskDialog
{
public: // public interface
    CreateDiskDialog(Application&, const char drive);

    virtual ~CreateDiskDialog() = default;

    virtual auto run() -> void override;
};

}

// ---------------------------------------------------------------------------
// xcpc::InsertDiskDialog
// ---------------------------------------------------------------------------

namespace xcpc {

class InsertDiskDialog final
    : public base::InsertDiskDialog
{
public: // public interface
    InsertDiskDialog(Application&, const char drive);

    virtual ~InsertDiskDialog() = default;

    virtual auto run() -> void override;
};

}

// ---------------------------------------------------------------------------
// xcpc::RemoveDiskDialog
// ---------------------------------------------------------------------------

namespace xcpc {

class RemoveDiskDialog final
    : public base::RemoveDiskDialog
{
public: // public interface
    RemoveDiskDialog(Application&, const char drive);

    virtual ~RemoveDiskDialog() = default;

    virtual auto run() -> void override;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_GTK3UI_DISK_DIALOG_H__ */
