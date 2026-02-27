/*
 * xcpc-video-settings-dialog.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_GTK3UI_VIDEO_SETTINGS_DIALOG_H__
#define __XCPC_GTK3UI_VIDEO_SETTINGS_DIALOG_H__

#include "xcpc-application.h"

// ---------------------------------------------------------------------------
// xcpc::VideoSettingsDialog
// ---------------------------------------------------------------------------

namespace xcpc {

class VideoSettingsDialog final
    : public base::SettingsDialog
{
public: // public interface
    VideoSettingsDialog(Application&);

    virtual ~VideoSettingsDialog() = default;

    virtual auto run() -> void override;

    auto build() -> void;

private: // private data
    gtk3::Dialog _dialog;
    gtk3::Frame  _frame;
    gtk3::Grid   _grid;
    gtk3::Label  _label_curvature;
    gtk3::Label  _label_corner;
    gtk3::Label  _label_dotline;
    gtk3::Label  _label_dotmask;
    gtk3::Label  _label_vignetting;
    gtk3::Label  _label_brightness;
    gtk3::Scale  _scale_curvature;
    gtk3::Scale  _scale_corner;
    gtk3::Scale  _scale_dotline;
    gtk3::Scale  _scale_dotmask;
    gtk3::Scale  _scale_vignetting;
    gtk3::Scale  _scale_brightness;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_GTK3UI_VIDEO_SETTINGS_DIALOG_H__ */
