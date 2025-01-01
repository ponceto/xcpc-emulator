/*
 * xcpc-disk-dialog.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include "xcpc-disk-dialog.h"

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

struct traits
{
    static bool run_dialog(gtk3::Dialog& dialog)
    {
        switch(dialog.run()) {
            case GTK_RESPONSE_OK:
            case GTK_RESPONSE_YES:
            case GTK_RESPONSE_APPLY:
            case GTK_RESPONSE_ACCEPT:
                return true;
            default:
                break;
        }
        return false;
    }
};

}

// ---------------------------------------------------------------------------
// xcpc::CreateDiskDialog
// ---------------------------------------------------------------------------

namespace xcpc {

CreateDiskDialog::CreateDiskDialog(Application& application, const char drive)
    : base::CreateDiskDialog(application, drive)
{
}

void CreateDiskDialog::run()
{
    auto run_confirm_dialog = [&](gtk3::MessageQuestionDialog& dialog) -> bool
    {
        dialog.set_title(_title);
        dialog.set_primary_markup(_("The disk image already exists..."));
        dialog.set_secondary_markup(_("Do you want to overwrite?"));

        return traits::run_dialog(dialog);
    };

    auto confirm_create = [&]() -> bool
    {
        if(PosixTraits::file_exists(_filename)) {
            gtk3::MessageQuestionDialog dialog;

            return run_confirm_dialog(dialog);
        }
        return true;
    };

    auto run_create_dialog = [&](gtk3::FileChooserSaveDialog& dialog) -> bool
    {
        dialog.set_title(_title);

        return traits::run_dialog(dialog);
    };

    auto ask_create = [&]() -> bool
    {
        gtk3::FileChooserSaveDialog dialog;

        if(run_create_dialog(dialog)) {
            _filename = dialog.get_filename();
            return confirm_create();
        }
        return false;
    };

    auto execute = [&]() -> void
    {
        if(ask_create()) {
            create_disk(_filename);
        }
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// xcpc::InsertDiskDialog
// ---------------------------------------------------------------------------

namespace xcpc {

InsertDiskDialog::InsertDiskDialog(Application& application, const char drive)
    : base::InsertDiskDialog(application, drive)
{
}

void InsertDiskDialog::run()
{
    auto confirm_insert = [&]() -> bool
    {
        if(PosixTraits::file_exists(_filename)) {
            return true;
        }
        return false;
    };

    auto run_insert_dialog = [&](gtk3::FileChooserOpenDialog& dialog) -> bool
    {
        dialog.set_title(_title);

        return traits::run_dialog(dialog);
    };

    auto ask_insert = [&]() -> bool
    {
        gtk3::FileChooserOpenDialog dialog;

        if(run_insert_dialog(dialog)) {
            _filename = dialog.get_filename();
            return confirm_insert();
        }
        return false;
    };

    auto execute = [&]() -> void
    {
        if(ask_insert()) {
            insert_disk(_filename);
        }
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// xcpc::RemoveDiskDialog
// ---------------------------------------------------------------------------

namespace xcpc {

RemoveDiskDialog::RemoveDiskDialog(Application& application, const char drive)
    : base::RemoveDiskDialog(application, drive)
{
}

void RemoveDiskDialog::run()
{
    auto confirm_remove = [&]() -> bool
    {
        return true;
    };

    auto ask_remove = [&]() -> bool
    {
        return confirm_remove();
    };

    auto execute = [&]() -> void
    {
        if(ask_remove()) {
            remove_disk();
        }
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
