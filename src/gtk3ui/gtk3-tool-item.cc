/*
 * gtk3-tool-item.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-tool-item.h"

// ---------------------------------------------------------------------------
// gtk3::ToolItemTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct ToolItemTraits
    : BasicTraits
{
    static auto create_tool_item() -> GtkWidget*
    {
        return reinterpret_cast<GtkWidget*>(::gtk_tool_item_new());
    }

    static auto create_tool_button() -> GtkWidget*
    {
        return reinterpret_cast<GtkWidget*>(::gtk_tool_button_new(nullptr, nullptr));
    }

    static auto create_separator_tool_item() -> GtkWidget*
    {
        return reinterpret_cast<GtkWidget*>(::gtk_separator_tool_item_new());
    }

    static auto set_icon_name(ToolButton& tool_button, const std::string& icon_name) -> void
    {
        if(tool_button) {
            ::gtk_tool_button_set_icon_name(tool_button, icon_name.c_str());
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::ToolItemTraits;

}

// ---------------------------------------------------------------------------
// gtk3::ToolItem
// ---------------------------------------------------------------------------

namespace gtk3 {

ToolItem::ToolItem()
    : ToolItem(traits::create_tool_item())
{
}

ToolItem::ToolItem(GtkWidget* instance)
    : Bin(instance)
{
}

auto ToolItem::create_tool_item() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_tool_item();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// gtk3::ToolButton
// ---------------------------------------------------------------------------

namespace gtk3 {

ToolButton::ToolButton()
    : ToolButton(traits::create_tool_button())
{
}

ToolButton::ToolButton(GtkWidget* instance)
    : ToolItem(instance)
{
}

auto ToolButton::create_tool_button() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_tool_button();
        traits::register_widget_instance(_instance);
    }
}

auto ToolButton::set_icon_name(const std::string& icon_name) -> void
{
    return traits::set_icon_name(*this, icon_name);
}

auto ToolButton::add_clicked_callback(GCallback callback, void* data) -> void
{
    return signal_connect(sig_clicked, callback, data);
}

}

// ---------------------------------------------------------------------------
// gtk3::SeparatorToolItem
// ---------------------------------------------------------------------------

namespace gtk3 {

SeparatorToolItem::SeparatorToolItem()
    : SeparatorToolItem(traits::create_separator_tool_item())
{
}

SeparatorToolItem::SeparatorToolItem(GtkWidget* instance)
    : ToolItem(instance)
{
}

auto SeparatorToolItem::create_separator_tool_item() -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_separator_tool_item();
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
