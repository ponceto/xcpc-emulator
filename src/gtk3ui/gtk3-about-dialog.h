/*
 * gtk3-about-dialog.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_ABOUT_DIALOG_H__
#define __GTK3_CXX_ABOUT_DIALOG_H__

#include <gtk3ui/gtk3-dialog.h>

// ---------------------------------------------------------------------------
// gtk3::AboutDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

class AboutDialog
    : public Dialog
{
public: // public interface
    AboutDialog();

    AboutDialog(GtkWidget*);

    AboutDialog(const AboutDialog&) = delete;

    AboutDialog& operator=(const AboutDialog&) = delete;

    virtual ~AboutDialog() = default;

    operator GtkAboutDialog*() const
    {
        return GTK_ABOUT_DIALOG(_instance);
    }

    void create_about_dialog();

    void set_logo(GdkPixbuf* logo);

    void set_logo_icon_name(const std::string& icon_name);

    void set_program_name(const std::string& program_name);

    void set_version(const std::string& version);

    void set_copyright(const std::string& copyright);

    void set_comments(const std::string& comments);

    void set_website(const std::string& website);

    void set_license(const std::string& license);
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_ABOUT_DIALOG_H__ */
