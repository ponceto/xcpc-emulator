/*
 * gtk3-box.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_BOX_H__
#define __GTK3_CXX_BOX_H__

#include <gtk3ui/gtk3-container.h>

// ---------------------------------------------------------------------------
// gtk3::Box
// ---------------------------------------------------------------------------

namespace gtk3 {

class Box
    : public Container
{
public: // public interface
    Box();

    Box(GtkWidget*);

    Box(const Box&) = delete;

    Box& operator=(const Box&) = delete;

    virtual ~Box() = default;

    operator GtkBox*() const
    {
        return GTK_BOX(_instance);
    }

    void set_spacing(int spacing);

    void set_homogeneous(bool homogeneous);

    void pack_start(Widget& child, bool expand, bool fill, unsigned int padding);

    void pack_end(Widget& child, bool expand, bool fill, unsigned int padding);

    void set_center_widget(Widget& child);
};

}

// ---------------------------------------------------------------------------
// gtk3::HBox
// ---------------------------------------------------------------------------

namespace gtk3 {

class HBox
    : public Box
{
public: // public interface
    HBox();

    HBox(GtkWidget*);

    virtual ~HBox() = default;

    void create_hbox();
};

}

// ---------------------------------------------------------------------------
// gtk3::VBox
// ---------------------------------------------------------------------------

namespace gtk3 {

class VBox
    : public Box
{
public: // public interface
    VBox();

    VBox(GtkWidget*);

    virtual ~VBox() = default;

    void create_vbox();
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_BOX_H__ */
