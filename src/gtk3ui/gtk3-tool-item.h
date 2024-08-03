/*
 * gtk3-menu-item.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_TOOL_ITEM_H__
#define __GTK3_CXX_TOOL_ITEM_H__

#include <gtk3ui/gtk3-bin.h>

// ---------------------------------------------------------------------------
// gtk3::ToolItem
// ---------------------------------------------------------------------------

namespace gtk3 {

class ToolItem
    : public Bin
{
public: // public interface
    ToolItem();

    ToolItem(GtkWidget*);

    ToolItem(const ToolItem&) = delete;

    ToolItem& operator=(const ToolItem&) = delete;

    virtual ~ToolItem() = default;

    operator GtkToolItem*() const
    {
        return GTK_TOOL_ITEM(_instance);
    }

    void create_tool_item();
};

}

// ---------------------------------------------------------------------------
// gtk3::ToolButton
// ---------------------------------------------------------------------------

namespace gtk3 {

class ToolButton
    : public ToolItem
{
public: // public interface
    ToolButton();

    ToolButton(GtkWidget*);

    virtual ~ToolButton() = default;

    operator GtkToolButton*() const
    {
        return GTK_TOOL_BUTTON(_instance);
    }

    void create_tool_button();

    void set_icon_name(const std::string& icon_name);

    void add_clicked_callback(GCallback callback, void* data);
};

}

// ---------------------------------------------------------------------------
// gtk3::SeparatorToolItem
// ---------------------------------------------------------------------------

namespace gtk3 {

class SeparatorToolItem
    : public ToolItem
{
public: // public interface
    SeparatorToolItem();

    SeparatorToolItem(GtkWidget*);

    virtual ~SeparatorToolItem() = default;

    operator GtkSeparatorToolItem*() const
    {
        return GTK_SEPARATOR_TOOL_ITEM(_instance);
    }

    void create_separator_tool_item();
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_TOOL_ITEM_H__ */
