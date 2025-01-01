/*
 * gtk3-menu-item.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __GTK3_CXX_MENU_ITEM_H__
#define __GTK3_CXX_MENU_ITEM_H__

#include <gtk3ui/gtk3-bin.h>

// ---------------------------------------------------------------------------
// gtk3::MenuItem
// ---------------------------------------------------------------------------

namespace gtk3 {

class MenuItem
    : public Bin
{
public: // public interface
    MenuItem();

    MenuItem(GtkWidget*);

    MenuItem(const MenuItem&) = delete;

    MenuItem& operator=(const MenuItem&) = delete;

    virtual ~MenuItem() = default;

    operator GtkMenuItem*() const
    {
        return GTK_MENU_ITEM(_instance);
    }

    void create_menu_item();

    void create_menu_item_with_label(const std::string& label);

    void create_menu_item_with_mnemonic(const std::string& label);

    void set_submenu(Widget&);

    void set_accel(unsigned int accelerator_key, GdkModifierType accelerator_mods);

    void add_activate_callback(GCallback callback, void* data);

    void add_select_callback(GCallback callback, void* data);

    void add_deselect_callback(GCallback callback, void* data);
};

}

// ---------------------------------------------------------------------------
// gtk3::SeparatorMenuItem
// ---------------------------------------------------------------------------

namespace gtk3 {

class SeparatorMenuItem
    : public MenuItem
{
public: // public interface
    SeparatorMenuItem();

    SeparatorMenuItem(GtkWidget*);

    virtual ~SeparatorMenuItem() = default;

    operator GtkSeparatorMenuItem*() const
    {
        return GTK_SEPARATOR_MENU_ITEM(_instance);
    }

    void create_separator_menu_item();
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_MENU_ITEM_H__ */
