/*
 * gtk3-container.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "gtk3-container.h"

// ---------------------------------------------------------------------------
// gtk3::ContainerTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ContainerTraits
    : BasicTraits
{
    static GtkWidget* create_container()
    {
        return nullptr;
    }

    static void add(Container& container, Widget& widget)
    {
        if(container && widget) {
            ::gtk_container_add(container, widget);
        }        
    }

    static void remove(Container& container, Widget& widget)
    {
        if(container && widget) {
            ::gtk_container_remove(container, widget);
        }        
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ContainerTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Container
// ---------------------------------------------------------------------------

namespace gtk3 {

Container::Container()
    : Container(traits::create_container())
{
}

Container::Container(GtkWidget* instance)
    : Widget(instance)
{
}

void Container::add(Widget& widget)
{
    return traits::add(*this, widget);
}

void Container::remove(Widget& widget)
{
    return traits::remove(*this, widget);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
