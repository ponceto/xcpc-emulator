/*
 * gtk3-box.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-box.h"

// ---------------------------------------------------------------------------
// gtk3::BoxTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct BoxTraits
    : BasicTraits
{
    static auto create_box() -> GtkWidget*
    {
        return nullptr;
    }

    static auto create_hbox(int spacing = 0) -> GtkWidget*
    {
        return ::gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
    }

    static auto create_vbox(int spacing = 0) -> GtkWidget*
    {
        return ::gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
    }

    static auto set_spacing(Box& box, int spacing) -> void
    {
        if(box) {
            ::gtk_box_set_spacing(box, spacing);
        }
    }

    static auto set_homogeneous(Box& box, bool homogeneous) -> void
    {
        if(box) {
            ::gtk_box_set_homogeneous(box, homogeneous);
        }
    }

    static auto pack_start(Box& box, Widget& child, bool expand, bool fill, unsigned int padding) -> void
    {
        if(box && child) {
            ::gtk_box_pack_start(box, child, expand, fill, padding);
        }
    }

    static auto pack_end(Box& box, Widget& child, bool expand, bool fill, unsigned int padding) -> void
    {
        if(box && child) {
            ::gtk_box_pack_end(box, child, expand, fill, padding);
        }
    }

    static auto set_center_widget(Box& box, Widget& child) -> void
    {
        if(box && child) {
            ::gtk_box_set_center_widget(box, child);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::BoxTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Box
// ---------------------------------------------------------------------------

namespace gtk3 {

Box::Box()
    : Box(traits::create_box())
{
}

Box::Box(GtkWidget* instance)
    : Container(instance)
{
}

auto Box::set_spacing(int spacing) -> void
{
    return traits::set_spacing(*this, spacing);
}

auto Box::set_homogeneous(bool homogeneous) -> void
{
    return traits::set_homogeneous(*this, homogeneous);
}

auto Box::pack_start(Widget& child, bool expand, bool fill, unsigned int padding) -> void
{
    return traits::pack_start(*this, child, expand, fill, padding);
}

auto Box::pack_end(Widget& child, bool expand, bool fill, unsigned int padding) -> void
{
    return traits::pack_end(*this, child, expand, fill, padding);
}

auto Box::set_center_widget(Widget& child) -> void
{
    return traits::set_center_widget(*this, child);
}

}

// ---------------------------------------------------------------------------
// gtk3::HBox
// ---------------------------------------------------------------------------

namespace gtk3 {

HBox::HBox()
    : HBox(traits::create_hbox())
{
}

HBox::HBox(GtkWidget* instance)
    : Box(instance)
{
}

auto HBox::create_hbox() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_hbox();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::VBox
// ---------------------------------------------------------------------------

namespace gtk3 {

VBox::VBox()
    : VBox(traits::create_vbox())
{
}

VBox::VBox(GtkWidget* instance)
    : Box(instance)
{
}

auto VBox::create_vbox() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_vbox();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
