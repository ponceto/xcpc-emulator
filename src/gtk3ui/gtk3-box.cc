/*
 * gtk3-box.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
    static GtkWidget* create_box()
    {
        return nullptr;
    }

    static GtkWidget* create_hbox(int spacing = 0)
    {
        return ::gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
    }

    static GtkWidget* create_vbox(int spacing = 0)
    {
        return ::gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
    }

    static void set_spacing(Box& box, int spacing)
    {
        if(box) {
            ::gtk_box_set_spacing(box, spacing);
        }
    }

    static void set_homogeneous(Box& box, bool homogeneous)
    {
        if(box) {
            ::gtk_box_set_homogeneous(box, homogeneous);
        }
    }

    static void pack_start(Box& box, Widget& child, bool expand, bool fill, unsigned int padding)
    {
        if(box && child) {
            ::gtk_box_pack_start(box, child, expand, fill, padding);
        }
    }

    static void pack_end(Box& box, Widget& child, bool expand, bool fill, unsigned int padding)
    {
        if(box && child) {
            ::gtk_box_pack_end(box, child, expand, fill, padding);
        }
    }

    static void set_center_widget(Box& box, Widget& child)
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

void Box::set_spacing(int spacing)
{
    return traits::set_spacing(*this, spacing);
}

void Box::set_homogeneous(bool homogeneous)
{
    return traits::set_homogeneous(*this, homogeneous);
}

void Box::pack_start(Widget& child, bool expand, bool fill, unsigned int padding)
{
    return traits::pack_start(*this, child, expand, fill, padding);
}

void Box::pack_end(Widget& child, bool expand, bool fill, unsigned int padding)
{
    return traits::pack_end(*this, child, expand, fill, padding);
}

void Box::set_center_widget(Widget& child)
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

void HBox::create_hbox()
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

void VBox::create_vbox()
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
