/*
 * gdk3-pixbuf.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gdk3-pixbuf.h"

// ---------------------------------------------------------------------------
// gdk3::PixbufTraits
// ---------------------------------------------------------------------------

namespace gdk3 {

struct PixbufTraits
{
    static GdkPixbuf* create_from_pixbuf(GdkPixbuf* pixbuf)
    {
        return pixbuf;
    }

    static GdkPixbuf* create_from_file(const std::string& filename)
    {
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), nullptr);

        if(pixbuf == nullptr) {
            throw std::runtime_error("gdk_pixbuf_new_from_file() has failed");
        }
        return pixbuf;
    }

    static GdkPixbuf* create_from_resource(const std::string& resource)
    {
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_resource(resource.c_str(), nullptr);

        if(pixbuf == nullptr) {
            throw std::runtime_error("gdk_pixbuf_new_from_resource() has failed");
        }
        return pixbuf;
    }

    static GdkPixbuf* unref(GdkPixbuf* pixbuf)
    {
        if(pixbuf != nullptr) {
            pixbuf = (g_object_unref(G_OBJECT(pixbuf)), nullptr);
        }
        return pixbuf;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gdk3::PixbufTraits;

}

// ---------------------------------------------------------------------------
// gdk3::Pixbuf
// ---------------------------------------------------------------------------

namespace gdk3 {

Pixbuf::Pixbuf()
    : _instance(traits::create_from_pixbuf(nullptr))
{
}

Pixbuf::Pixbuf(GdkPixbuf* instance)
    : _instance(traits::create_from_pixbuf(instance))
{
}

Pixbuf::~Pixbuf()
{
    _instance = traits::unref(_instance);
}

void Pixbuf::create_from_file(const std::string& filename)
{
    _instance = traits::unref(_instance);
    _instance = traits::create_from_file(filename);
}

void Pixbuf::create_from_resource(const std::string& resource)
{
    _instance = traits::unref(_instance);
    _instance = traits::create_from_resource(resource);
}

void Pixbuf::unref()
{
    _instance = traits::unref(_instance);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
