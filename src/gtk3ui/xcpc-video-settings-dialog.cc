/*
 * xcpc-video-settings-dialog.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "xcpc-video-settings-dialog.h"

// ---------------------------------------------------------------------------
// xcpc::VideoSettingsDialog
// ---------------------------------------------------------------------------

namespace xcpc {

VideoSettingsDialog::VideoSettingsDialog(Application& application)
    : base::VideoSettingsDialog(application)
    , _dialog(nullptr)
    , _frame(nullptr)
    , _grid(nullptr)
    , _label_hsampling(nullptr)
    , _label_vsampling(nullptr)
    , _label_curvature(nullptr)
    , _label_corner(nullptr)
    , _label_dotline(nullptr)
    , _label_dotmask(nullptr)
    , _label_vignetting(nullptr)
    , _label_brightness(nullptr)
    , _scale_hsampling(nullptr)
    , _scale_vsampling(nullptr)
    , _scale_curvature(nullptr)
    , _scale_corner(nullptr)
    , _scale_dotline(nullptr)
    , _scale_dotmask(nullptr)
    , _scale_vignetting(nullptr)
    , _scale_brightness(nullptr)
{
}

auto VideoSettingsDialog::build() -> void
{
    auto& application = dynamic_cast<Application&>(_application);
    auto& settings    = application.video_settings();

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
        _frame.create_frame(_("Video emulation settings (OpenGL)"));
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

    auto build_label_hsampling = [&]() -> void
    {
        _label_hsampling.create_label(_("Horizontal sampling factor"));
        _label_hsampling.set_xalign(0.0f);
        _grid.attach(_label_hsampling, 0, 0, 1, 1);
    };

    auto build_label_vsampling = [&]() -> void
    {
        _label_vsampling.create_label(_("Vertical sampling factor"));
        _label_vsampling.set_xalign(0.0f);
        _grid.attach(_label_vsampling, 0, 1, 1, 1);
    };

    auto build_label_curvature = [&]() -> void
    {
        _label_curvature.create_label(_("Screen curvature"));
        _label_curvature.set_xalign(0.0f);
        _grid.attach(_label_curvature, 0, 2, 1, 1);
    };

    auto build_label_corner = [&]() -> void
    {
        _label_corner.create_label(_("Rounded corners"));
        _label_corner.set_xalign(0.0f);
        _grid.attach(_label_corner, 0, 3, 1, 1);
    };

    auto build_label_dotline = [&]() -> void
    {
        _label_dotline.create_label(_("CRT scanline effect"));
        _label_dotline.set_xalign(0.0f);
        _grid.attach(_label_dotline, 0, 4, 1, 1);
    };

    auto build_label_dotmask = [&]() -> void
    {
        _label_dotmask.create_label(_("CRT phosphor mask effect"));
        _label_dotmask.set_xalign(0.0f);
        _grid.attach(_label_dotmask, 0, 5, 1, 1);
    };

    auto build_label_vignetting = [&]() -> void
    {
        _label_vignetting.create_label(_("Vignetting effect"));
        _label_vignetting.set_xalign(0.0f);
        _grid.attach(_label_vignetting, 0, 6, 1, 1);
    };

    auto build_label_brightness = [&]() -> void
    {
        _label_brightness.create_label(_("Brightness"));
        _label_brightness.set_xalign(0.0f);
        _grid.attach(_label_brightness, 0, 7, 1, 1);
    };

    auto build_scale_hsampling = [&]() -> void
    {
        _scale_hsampling.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 5.00, 0.01);
        _scale_hsampling.set_digits(2);
        _scale_hsampling.set_value_pos(GTK_POS_RIGHT);
        _scale_hsampling.set_hexpand(true);
        _scale_hsampling.set_value(settings.u_hsampling);
        _grid.attach(_scale_hsampling, 1, 0, 1, 1);
    };

    auto build_scale_vsampling = [&]() -> void
    {
        _scale_vsampling.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 5.00, 0.01);
        _scale_vsampling.set_digits(2);
        _scale_vsampling.set_value_pos(GTK_POS_RIGHT);
        _scale_vsampling.set_hexpand(true);
        _scale_vsampling.set_value(settings.u_vsampling);
        _grid.attach(_scale_vsampling, 1, 1, 1, 1);
    };

    auto build_scale_curvature = [&]() -> void
    {
        _scale_curvature.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 1.00, 0.01);
        _scale_curvature.set_digits(2);
        _scale_curvature.set_value_pos(GTK_POS_RIGHT);
        _scale_curvature.set_hexpand(true);
        _scale_curvature.set_value(settings.u_curvature);
        _grid.attach(_scale_curvature, 1, 2, 1, 1);
    };

    auto build_scale_corner = [&]() -> void
    {
        _scale_corner.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 1.00, 0.01);
        _scale_corner.set_digits(2);
        _scale_corner.set_value_pos(GTK_POS_RIGHT);
        _scale_corner.set_hexpand(true);
        _scale_corner.set_value(settings.u_corner);
        _grid.attach(_scale_corner, 1, 3, 1, 1);
    };

    auto build_scale_dotline = [&]() -> void
    {
        _scale_dotline.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 1.00, 0.01);
        _scale_dotline.set_digits(2);
        _scale_dotline.set_value_pos(GTK_POS_RIGHT);
        _scale_dotline.set_hexpand(true);
        _scale_dotline.set_value(settings.u_dotline);
        _grid.attach(_scale_dotline, 1, 4, 1, 1);
    };

    auto build_scale_dotmask = [&]() -> void
    {
        _scale_dotmask.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 1.00, 0.01);
        _scale_dotmask.set_digits(2);
        _scale_dotmask.set_value_pos(GTK_POS_RIGHT);
        _scale_dotmask.set_hexpand(true);
        _scale_dotmask.set_value(settings.u_dotmask);
        _grid.attach(_scale_dotmask, 1, 5, 1, 1);
    };

    auto build_scale_vignetting = [&]() -> void
    {
        _scale_vignetting.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.00, 2.00, 0.01);
        _scale_vignetting.set_digits(2);
        _scale_vignetting.set_value_pos(GTK_POS_RIGHT);
        _scale_vignetting.set_hexpand(true);
        _scale_vignetting.set_value(settings.u_vignetting);
        _grid.attach(_scale_vignetting, 1, 6, 1, 1);
    };

    auto build_scale_brightness = [&]() -> void
    {
        _scale_brightness.create_scale(GTK_ORIENTATION_HORIZONTAL, 0.50, 2.00, 0.01);
        _scale_brightness.set_digits(2);
        _scale_brightness.set_value_pos(GTK_POS_RIGHT);
        _scale_brightness.set_hexpand(true);
        _scale_brightness.set_value(settings.u_brightness);
        _grid.attach(_scale_brightness, 1, 7, 1, 1);
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
        build_label_hsampling();
        build_scale_hsampling();
        build_label_vsampling();
        build_scale_vsampling();
        build_label_curvature();
        build_scale_curvature();
        build_label_corner();
        build_scale_corner();
        build_label_dotline();
        build_scale_dotline();
        build_label_dotmask();
        build_scale_dotmask();
        build_label_vignetting();
        build_scale_vignetting();
        build_label_brightness();
        build_scale_brightness();
        build_content_area();
    };

    return build_all();
}

auto VideoSettingsDialog::run() -> void
{
    auto& application = dynamic_cast<Application&>(_application);
    auto& settings    = application.video_settings();

    auto run_dialog = [&]() -> void
    {
        /* build widgets */ {
            build();
        }
        /* run dialog and apply on OK */ {
            const int response = _dialog.run();

            if(response == GTK_RESPONSE_OK) {
                settings.u_hsampling  = _scale_hsampling.get_value();
                settings.u_vsampling  = _scale_vsampling.get_value();
                settings.u_curvature  = _scale_curvature.get_value();
                settings.u_corner     = _scale_corner.get_value();
                settings.u_dotline    = _scale_dotline.get_value();
                settings.u_dotmask    = _scale_dotmask.get_value();
                settings.u_vignetting = _scale_vignetting.get_value();
                settings.u_brightness = _scale_brightness.get_value();
            }
        }
    };

    return run_dialog();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
