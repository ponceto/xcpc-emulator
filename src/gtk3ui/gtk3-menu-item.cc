/*
 * gtk3-menu-item.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include "gtk3-menu-item.h"

// ---------------------------------------------------------------------------
// gtk3::MenuItemTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct MenuItemTraits
    : BasicTraits
{
    static GtkWidget* create_menu_item()
    {
        return ::gtk_menu_item_new();
    }

    static GtkWidget* create_menu_item_with_label(const std::string& label)
    {
        return ::gtk_menu_item_new_with_label(label.c_str());
    }

    static GtkWidget* create_menu_item_with_mnemonic(const std::string& label)
    {
        return ::gtk_menu_item_new_with_mnemonic(label.c_str());
    }

    static GtkWidget* create_separator_menu_item()
    {
        return ::gtk_separator_menu_item_new();
    }

    static void set_submenu(MenuItem& menu_item, Widget& widget)
    {
        if(menu_item && widget) {
            ::gtk_menu_item_set_submenu(menu_item, widget);
        }
    }

    static void set_accel(MenuItem& menu_item, unsigned int accelerator_key, GdkModifierType accelerator_mods)
    {
        if(menu_item) {
            GtkWidget* accel_label = ::gtk_bin_get_child(menu_item);
            ::gtk_accel_label_set_accel(GTK_ACCEL_LABEL(accel_label), accelerator_key, accelerator_mods);
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::MenuItemTraits;

}

// ---------------------------------------------------------------------------
// gtk3::MenuItem
// ---------------------------------------------------------------------------

namespace gtk3 {

MenuItem::MenuItem()
    : MenuItem(traits::create_menu_item())
{
}

MenuItem::MenuItem(GtkWidget* instance)
    : Bin(instance)
{
}

void MenuItem::create_menu_item()
{
    if(_instance == nullptr) {
        _instance = traits::create_menu_item();
        traits::register_widget_instance(_instance);
    }
}

void MenuItem::create_menu_item_with_label(const std::string& label)
{
    if(_instance == nullptr) {
        _instance = traits::create_menu_item_with_label(label);
        traits::register_widget_instance(_instance);
    }
}

void MenuItem::create_menu_item_with_mnemonic(const std::string& label)
{
    if(_instance == nullptr) {
        _instance = traits::create_menu_item_with_mnemonic(label);
        traits::register_widget_instance(_instance);
    }
}

void MenuItem::set_submenu(Widget& widget)
{
    return traits::set_submenu(*this, widget);
}

void MenuItem::set_accel(unsigned int accelerator_key, GdkModifierType accelerator_mods)
{
    return traits::set_accel(*this, accelerator_key, accelerator_mods);
}

void MenuItem::add_activate_callback(GCallback callback, void* data)
{
    return signal_connect(sig_activate, callback, data);
}

void MenuItem::add_select_callback(GCallback callback, void* data)
{
    return signal_connect(sig_select, callback, data);
}

void MenuItem::add_deselect_callback(GCallback callback, void* data)
{
    return signal_connect(sig_deselect, callback, data);
}

}

// ---------------------------------------------------------------------------
// gtk3::SeparatorMenuItem
// ---------------------------------------------------------------------------

namespace gtk3 {

SeparatorMenuItem::SeparatorMenuItem()
    : SeparatorMenuItem(traits::create_separator_menu_item())
{
}

SeparatorMenuItem::SeparatorMenuItem(GtkWidget* instance)
    : MenuItem(instance)
{
}

void SeparatorMenuItem::create_separator_menu_item()
{
    if(_instance == nullptr) {
        _instance = traits::create_separator_menu_item();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
