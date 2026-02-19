/*
 * gtk3-file-chooser-dialog.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_FILE_CHOOSER_DIALOG_H__
#define __GTK3_CXX_FILE_CHOOSER_DIALOG_H__

#include <gtk3ui/gtk3-dialog.h>

// ---------------------------------------------------------------------------
// gtk3::FileChooserDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class FileChooserDialog
    : public Dialog
{
public: // public interface
    FileChooserDialog();

    FileChooserDialog(GtkWidget*);

    FileChooserDialog(const FileChooserDialog&) = delete;

    FileChooserDialog& operator=(const FileChooserDialog&) = delete;

    virtual ~FileChooserDialog() = default;

    operator GtkFileChooser*() const
    {
        return GTK_FILE_CHOOSER(_instance);
    }

    operator GtkFileChooserDialog*() const
    {
        return GTK_FILE_CHOOSER_DIALOG(_instance);
    }

    auto get_filename() -> std::string;
};

}

// ---------------------------------------------------------------------------
// gtk3::FileChooserOpenDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class FileChooserOpenDialog
    : public FileChooserDialog
{
public: // public interface
    FileChooserOpenDialog();

    FileChooserOpenDialog(GtkWidget*);

    virtual ~FileChooserOpenDialog() = default;

    auto create_file_chooser_open_dialog() -> void;
};

}

// ---------------------------------------------------------------------------
// gtk3::FileChooserSaveDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class FileChooserSaveDialog
    : public FileChooserDialog
{
public: // public interface
    FileChooserSaveDialog();

    FileChooserSaveDialog(GtkWidget*);

    virtual ~FileChooserSaveDialog() = default;

    auto create_file_chooser_save_dialog() -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_FILE_CHOOSER_DIALOG_H__ */
