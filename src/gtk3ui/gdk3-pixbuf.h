/*
 * gdk3-pixbuf.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GDK3_CXX_PIXBUF_H__
#define __GDK3_CXX_PIXBUF_H__

#include <gtk3ui/gtk3-base.h>

// ---------------------------------------------------------------------------
// gdk3::Pixbuf
// ---------------------------------------------------------------------------

namespace gdk3 {

class Pixbuf
{
public: // public interface
    Pixbuf();

    Pixbuf(GdkPixbuf*);

    virtual ~Pixbuf();

    operator GdkPixbuf*() const
    {
        return _instance;
    }

    GdkPixbuf* operator*() const
    {
        return _instance;
    }

    void create_from_file(const std::string& filename);

    void create_from_resource(const std::string& resource);

    void unref();

protected: // protected data
    GdkPixbuf* _instance;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GDK3_CXX_PIXBUF_H__ */
