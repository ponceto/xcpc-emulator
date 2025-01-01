/*
 * xcpc-about-dialog.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "xcpc-about-dialog.h"

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

struct traits
{
    static void run_dialog(gtk3::Dialog& dialog)
    {
        switch(dialog.run()) {
            default:
                break;
        }
    }
};

}

// ---------------------------------------------------------------------------
// xcpc::AboutDialog
// ---------------------------------------------------------------------------

namespace xcpc {

AboutDialog::AboutDialog(Application& application)
    : base::AboutDialog(application)
{
}

void AboutDialog::run()
{
    auto run_dialog = [&](gtk3::AboutDialog& dialog) -> void
    {
        dialog.set_logo(dynamic_cast<Application&>(_application).app_icon());
        dialog.set_program_name(_("Xcpc"));
        dialog.set_version(xcpc::Utils::get_version());
        dialog.set_copyright(xcpc::Utils::get_copyright());
        dialog.set_comments(xcpc::Utils::get_comments());
        dialog.set_website(xcpc::Utils::get_website());
        dialog.set_license(xcpc::Utils::get_license());
        dialog.set_title(_title);
        dialog.set_skip_taskbar_hint(true);

        return traits::run_dialog(dialog);
    };

    auto execute = [&]() -> void
    {
        gtk3::AboutDialog dialog;

        return run_dialog(dialog);
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
