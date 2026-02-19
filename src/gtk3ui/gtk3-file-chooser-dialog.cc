/*
 * gtk3-file-chooser-dialog.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <iostream>
#include <stdexcept>
#include "gtk3-file-chooser-dialog.h"

// ---------------------------------------------------------------------------
// gtk3::FileChooserDialogTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct FileChooserDialogTraits
    : BasicTraits
{
    static GtkWidget* create_file_chooser_dialog()
    {
        return nullptr;
    }

    static GtkWidget* create_file_chooser_open_dialog()
    {
        return ::gtk_file_chooser_dialog_new ( nullptr
                                             , nullptr
                                             , GTK_FILE_CHOOSER_ACTION_OPEN
                                             , "gtk-cancel", GTK_RESPONSE_CANCEL
                                             , "gtk-open"  , GTK_RESPONSE_ACCEPT
                                             , nullptr );
    }

    static GtkWidget* create_file_chooser_save_dialog()
    {
        return ::gtk_file_chooser_dialog_new ( nullptr
                                             , nullptr
                                             , GTK_FILE_CHOOSER_ACTION_SAVE
                                             , "gtk-cancel", GTK_RESPONSE_CANCEL
                                             , "gtk-save"  , GTK_RESPONSE_ACCEPT
                                             , nullptr );
    }

    static std::string get_filename(FileChooserDialog& file_chooser_dialog)
    {
        gchar* filename = ::gtk_file_chooser_get_filename(file_chooser_dialog);
        const std::string result(filename);
        filename = (g_free(filename), nullptr);
        return result;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::FileChooserDialogTraits;

}

// ---------------------------------------------------------------------------
// gtk3::FileChooserDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

FileChooserDialog::FileChooserDialog()
    : FileChooserDialog(traits::create_file_chooser_dialog())
{
}

FileChooserDialog::FileChooserDialog(GtkWidget* instance)
    : Dialog(instance)
{
}

std::string FileChooserDialog::get_filename()
{
    return traits::get_filename(*this);
}

}

// ---------------------------------------------------------------------------
// gtk3::FileChooserOpenDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

FileChooserOpenDialog::FileChooserOpenDialog()
    : FileChooserOpenDialog(traits::create_file_chooser_open_dialog())
{
}

FileChooserOpenDialog::FileChooserOpenDialog(GtkWidget* instance)
    : FileChooserDialog(instance)
{
}

void FileChooserOpenDialog::create_file_chooser_open_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_file_chooser_open_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::FileChooserSaveDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

FileChooserSaveDialog::FileChooserSaveDialog()
    : FileChooserSaveDialog(traits::create_file_chooser_save_dialog())
{
}

FileChooserSaveDialog::FileChooserSaveDialog(GtkWidget* instance)
    : FileChooserDialog(instance)
{
}

void FileChooserSaveDialog::create_file_chooser_save_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_file_chooser_save_dialog();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
