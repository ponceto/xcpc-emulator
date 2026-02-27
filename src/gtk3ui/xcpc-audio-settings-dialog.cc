/*
 * xcpc-audio-settings-dialog.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "xcpc-audio-settings-dialog.h"

// ---------------------------------------------------------------------------
// xcpc::AudioSettingsDialog
// ---------------------------------------------------------------------------

namespace xcpc {

AudioSettingsDialog::AudioSettingsDialog(Application& application)
    : base::AudioSettingsDialog(application)
    , _dialog(nullptr)
    , _frame(nullptr)
    , _grid(nullptr)
    , _label_volume(nullptr)
    , _scale_volume(nullptr)
{
}

auto AudioSettingsDialog::build() -> void
{
    auto& application = dynamic_cast<Application&>(_application);
    auto& settings    = application.audio_settings();

    auto build_dialog = [&]() -> void
    {
        _dialog.create_dialog();
        _dialog.set_title(_title);
        _dialog.set_modal(true);
        _dialog.set_skip_taskbar_hint(true);
        _dialog.add_button(_("Cancel"), GTK_RESPONSE_CANCEL);
        _dialog.add_button(_("OK"),     GTK_RESPONSE_OK);
    };

    auto build_frame = [&]() -> void
    {
        _frame.create_frame(_("Audio emulation settings"));
        _frame.set_margin_start(8);
        _frame.set_margin_end(8);
        _frame.set_margin_top(8);
        _frame.set_margin_bottom(8);
    };

    auto build_grid = [&]() -> void
    {
        _grid.create_grid();
        _grid.set_row_spacing(8);
        _grid.set_row_homogeneous(false);
        _grid.set_column_spacing(8);
        _grid.set_column_homogeneous(true);
        _grid.set_margin_start(8);
        _grid.set_margin_end(8);
        _grid.set_margin_top(8);
        _grid.set_margin_bottom(8);
        _frame.add(_grid);

    };

    auto build_label_volume = [&]() -> void
    {
        _label_volume.create_label(_("Global volume"));
        _label_volume.set_xalign(0.0f);
        _grid.attach(_label_volume, 0, 0, 1, 1);
    };

    auto build_scale_volume = [&]() -> void
    {
        _scale_volume.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 1.00, 0.01);
        _scale_volume.set_digits(2);
        _scale_volume.set_value_pos(GTK_POS_RIGHT);
        _scale_volume.set_hexpand(true);
        _scale_volume.set_value(settings.volume);
        _grid.attach(_scale_volume, 1, 0, 1, 1);
    };

    auto build_content_area = [&]() -> void
    {
        _dialog.pack_start(_frame, TRUE, TRUE, 0);
    };

    auto build_all = [&]() -> void
    {
        build_dialog();
        build_frame();
        build_grid();
        build_label_volume();
        build_scale_volume();
        build_content_area();
    };

    return build_all();
}

auto AudioSettingsDialog::run() -> void
{
    auto& application = dynamic_cast<Application&>(_application);
    auto& settings    = application.audio_settings();

    auto run_dialog = [&]() -> void
    {
        /* build widgets */ {
            build();
        }
        /* run dialog and apply on OK */ {
            const int response = _dialog.run();

            if(response == GTK_RESPONSE_OK) {
                settings.volume = static_cast<float>(_scale_volume.get_value());
            }
        }
    };

    return run_dialog();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
