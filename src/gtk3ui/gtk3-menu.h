/*
 * gtk3-menu.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_MENU_H__
#define __GTK3_CXX_MENU_H__

#include <gtk3ui/gtk3-menu-shell.h>

// ---------------------------------------------------------------------------
// gtk3::Menu
// ---------------------------------------------------------------------------

namespace gtk3 {

class Menu
    : public MenuShell
{
public: // public interface
    Menu();

    Menu(GtkWidget*);

    Menu(const Menu&) = delete;

    Menu& operator=(const Menu&) = delete;

    virtual ~Menu() = default;

    operator GtkMenu*() const
    {
        return GTK_MENU(_instance);
    }

    auto create_menu() -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_MENU_H__ */
