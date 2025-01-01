/*
 * gtk3-about-dialog.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "gtk3-about-dialog.h"

// ---------------------------------------------------------------------------
// gtk3::AboutDialogTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct AboutDialogTraits
    : BasicTraits
{
    static GtkWidget* create_about_dialog()
    {
        return ::gtk_about_dialog_new();
    }

    static void set_logo(AboutDialog& about_dialog, GdkPixbuf* logo)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_logo(about_dialog, logo);
        }
    }

    static void set_logo_icon_name(AboutDialog& about_dialog, const std::string& icon_name)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_program_name(about_dialog, icon_name.c_str());
        }
    }

    static void set_program_name(AboutDialog& about_dialog, const std::string& program_name)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_program_name(about_dialog, program_name.c_str());
        }
    }

    static void set_version(AboutDialog& about_dialog, const std::string& version)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_version(about_dialog, version.c_str());
        }
    }

    static void set_copyright(AboutDialog& about_dialog, const std::string& copyright)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_copyright(about_dialog, copyright.c_str());
        }
    }

    static void set_comments(AboutDialog& about_dialog, const std::string& comments)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_comments(about_dialog, comments.c_str());
        }
    }

    static void set_website(AboutDialog& about_dialog, const std::string& website)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_website(about_dialog, website.c_str());
        }
    }

    static void set_license(AboutDialog& about_dialog, const std::string& license)
    {
        if(about_dialog) {
            ::gtk_about_dialog_set_license(about_dialog, license.c_str());
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::AboutDialogTraits;

}

// ---------------------------------------------------------------------------
// gtk3::AboutDialog
// ---------------------------------------------------------------------------

namespace gtk3 {

AboutDialog::AboutDialog()
    : AboutDialog(traits::create_about_dialog())
{
}

AboutDialog::AboutDialog(GtkWidget* instance)
    : Dialog(instance)
{
}

void AboutDialog::create_about_dialog()
{
    if(_instance == nullptr) {
        _instance = traits::create_about_dialog();
        traits::register_widget_instance(_instance);
    }
}

void AboutDialog::set_logo(GdkPixbuf* logo)
{
    return traits::set_logo(*this, logo);
}

void AboutDialog::set_logo_icon_name(const std::string& icon_name)
{
    return traits::set_program_name(*this, icon_name);
}

void AboutDialog::set_program_name(const std::string& program_name)
{
    return traits::set_program_name(*this, program_name);
}

void AboutDialog::set_version(const std::string& version)
{
    return traits::set_version(*this, version);
}

void AboutDialog::set_copyright(const std::string& copyright)
{
    return traits::set_copyright(*this, copyright);
}

void AboutDialog::set_comments(const std::string& comments)
{
    return traits::set_comments(*this, comments);
}

void AboutDialog::set_website(const std::string& website)
{
    return traits::set_website(*this, website);
}

void AboutDialog::set_license(const std::string& license)
{
    return traits::set_license(*this, license);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
