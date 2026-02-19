/*
 * gtk3-label.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-label.h"

// ---------------------------------------------------------------------------
// gtk3::LabelTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct LabelTraits
    : BasicTraits
{
    static auto create_label(const std::string& string = "label") -> GtkWidget*
    {
        return ::gtk_label_new(string.c_str());
    }

    static auto set_text(Label& label, const std::string& string) -> void
    {
        if(label) {
            ::gtk_label_set_text(label, string.c_str());
        }
    }

    static auto set_markup(Label& label, const std::string& string) -> void
    {
        if(label) {
            ::gtk_label_set_markup(label, string.c_str());
        }
    }

    static auto set_ellipsize(Label& label, PangoEllipsizeMode mode) -> void
    {
        if(label) {
            ::gtk_label_set_ellipsize(label, mode);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::LabelTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Label
// ---------------------------------------------------------------------------

namespace gtk3 {

Label::Label()
    : Label(traits::create_label())
{
}

Label::Label(GtkWidget* instance)
    : Widget(instance)
{
}

auto Label::create_label(const std::string& string) -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_label(string);
        traits::register_widget_instance(_instance);
    }
}

auto Label::set_text(const std::string& string) -> void
{
    return traits::set_text(*this, string);
}

auto Label::set_markup(const std::string& string) -> void
{
    return traits::set_markup(*this, string);
}

auto Label::set_ellipsize(PangoEllipsizeMode mode) -> void
{
    return traits::set_ellipsize(*this, mode);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
