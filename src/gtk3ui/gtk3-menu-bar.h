/*
 * gtk3-menu-bar.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __GTK3_CXX_MENU_BAR_H__
#define __GTK3_CXX_MENU_BAR_H__

#include <gtk3ui/gtk3-menu-shell.h>

// ---------------------------------------------------------------------------
// gtk3::MenuBar
// ---------------------------------------------------------------------------

namespace gtk3 {

class MenuBar
    : public MenuShell
{
public: // public interface
    MenuBar();

    MenuBar(GtkWidget*);

    MenuBar(const MenuBar&) = delete;

    MenuBar& operator=(const MenuBar&) = delete;

    virtual ~MenuBar() = default;

    operator GtkMenuBar*() const
    {
        return GTK_MENU_BAR(_instance);
    }

    void create_menu_bar();
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_MENU_BAR_H__ */
