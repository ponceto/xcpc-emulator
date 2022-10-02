/*
 * xcpc-help-dialog.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include "xcpc-help-dialog.h"

#define NIL ""
#define EOL "\n"

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

    static const char primary_markup[];

    static const char secondary_markup[];
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits::primary_markup
// ---------------------------------------------------------------------------

namespace {

const char traits::primary_markup[] = ""
    "Xcpc, an Amstrad CPC emulator for Linux, BSD and Unix" NIL
    ;

}

// ---------------------------------------------------------------------------
// <anonymous>::traits::secondary_markup
// ---------------------------------------------------------------------------

namespace {

const char traits::secondary_markup[] = ""
    "<u>Hotkeys:</u>"                                                                         EOL
    ""                                                                                        EOL
    "<small>"                                                                                 NIL
    "<tt>"                                                                                    NIL
    "    - F1                help"                                                            EOL
    "    - F2                load snapshot"                                                   EOL
    "    - F3                save snapshot"                                                   EOL
    "    - F4                {not mapped}"                                                    EOL
    "    - F5                reset emulator"                                                  EOL
    "    - F6                insert disk into drive A"                                        EOL
    "    - F7                remove disk from drive A"                                        EOL
    "    - F8                insert disk into drive B"                                        EOL
    "    - F9                remove disk from drive B"                                        EOL
    "    - F10               {not mapped}"                                                    EOL
    "    - F11               {not mapped}"                                                    EOL
    "    - F12               {not mapped}"                                                    EOL
    "</tt>"                                                                                   NIL
    "</small>"                                                                                NIL
    ""                                                                                        EOL
    "<u>Keyboard emulation:</u>"                                                              EOL
    ""                                                                                        EOL
    "The left shift and control keys are forwarded to the simulation."                        EOL
    "You must use the right shift and control keys to compose characters."                    EOL
    ""                                                                                        EOL
    "<u>Joystick emulation:</u>"                                                              EOL
    ""                                                                                        EOL
    "<small>"                                                                                 NIL
    "<tt>"                                                                                    NIL
    "    - Home/End          enable/disable"                                                  EOL
    "    - Arrows            up/down/left/right"                                              EOL
    "    - Left Ctrl         fire1"                                                           EOL
    "    - Left Alt          fire2"                                                           EOL
    "</tt>"                                                                                   NIL
    "</small>"                                                                                NIL
    ""                                                                                        EOL
    "<u>Drag'n Drop:</u>"                                                                     EOL
    ""                                                                                        EOL
    "You can use your file manager to drag'n drop a supported file directly to the emulator." EOL
    ""                                                                                        EOL
    "The supported file extensions are: '.dsk', 'dsk.gz', 'dsk.bz2', '.sna'"                  NIL
    ;

}

// ---------------------------------------------------------------------------
// xcpc::HelpDialog
// ---------------------------------------------------------------------------

namespace xcpc {

HelpDialog::HelpDialog(Application& application)
    : base::HelpDialog(application)
{
}

void HelpDialog::run()
{
    auto run_dialog = [&](gtk3::MessageOtherDialog& dialog) -> void
    {
        dialog.set_title(_title);
        dialog.set_primary_markup(traits::primary_markup);
        dialog.set_secondary_markup(traits::secondary_markup);
        dialog.set_skip_taskbar_hint(true);

        return traits::run_dialog(dialog);
    };

    auto execute = [&]() -> void
    {
        gtk3::MessageOtherDialog dialog;

        return run_dialog(dialog);
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
