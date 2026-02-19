/*
 * xcpc-snapshot-dialog.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "xcpc-snapshot-dialog.h"

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
// xcpc::LoadSnapshotDialog
// ---------------------------------------------------------------------------

namespace xcpc {

LoadSnapshotDialog::LoadSnapshotDialog(Application& application)
    : base::LoadSnapshotDialog(application)
{
}

void LoadSnapshotDialog::run()
{
    auto run_dialog = [&](gtk3::FileChooserOpenDialog& dialog) -> bool
    {
        dialog.set_title(_title);

        return traits::run_dialog(dialog);
    };

    auto execute = [&]() -> void
    {
        gtk3::FileChooserOpenDialog dialog;

        if(run_dialog(dialog)) {
            _filename = dialog.get_filename();
            load_snapshot(_filename);
        }
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// xcpc::SaveSnapshotDialog
// ---------------------------------------------------------------------------

namespace xcpc {

SaveSnapshotDialog::SaveSnapshotDialog(Application& application)
    : base::SaveSnapshotDialog(application)
{
}

void SaveSnapshotDialog::run()
{
    auto run_dialog = [&](gtk3::FileChooserSaveDialog& dialog) -> bool
    {
        dialog.set_title(_title);

        return traits::run_dialog(dialog);
    };

    auto execute = [&]() -> void
    {
        gtk3::FileChooserSaveDialog dialog;

        if(run_dialog(dialog)) {
            _filename = dialog.get_filename();
            save_snapshot(_filename);
        }
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
