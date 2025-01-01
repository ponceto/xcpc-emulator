/*
 * gtk3-toolbar.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "gtk3-toolbar.h"

// ---------------------------------------------------------------------------
// gtk3::ToolbarTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ToolbarTraits
    : BasicTraits
{
    static GtkWidget* create_toolbar()
    {
        return ::gtk_toolbar_new();
    }

    static void insert(Toolbar& toolbar, Widget& widget, int position)
    {
        if(toolbar && widget) {
            GtkWidget* tool_item = widget;
            ::gtk_toolbar_insert(toolbar, GTK_TOOL_ITEM(tool_item), position);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ToolbarTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Toolbar
// ---------------------------------------------------------------------------

namespace gtk3 {

Toolbar::Toolbar()
    : Toolbar(traits::create_toolbar())
{
}

Toolbar::Toolbar(GtkWidget* instance)
    : Container(instance)
{
}

void Toolbar::create_toolbar()
{
    if(_instance == nullptr) {
        _instance = traits::create_toolbar();
        traits::register_widget_instance(_instance);
    }
}

void Toolbar::insert(Widget& widget, int position)
{
    return traits::insert(*this, widget, position);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
