/*
 * gtk3-application-window.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_APPLICATION_WINDOW_H__
#define __GTK3_CXX_APPLICATION_WINDOW_H__

#include <gtk3ui/gtk3-application.h>
#include <gtk3ui/gtk3-window.h>

// ---------------------------------------------------------------------------
// gtk3::ApplicationWindow
// ---------------------------------------------------------------------------

namespace gtk3 {

class ApplicationWindow
    : public Window
{
public: // public interface
    ApplicationWindow(Application&);

    ApplicationWindow(GtkWidget*);

    ApplicationWindow(const ApplicationWindow&) = delete;

    ApplicationWindow& operator=(const ApplicationWindow&) = delete;

    virtual ~ApplicationWindow() = default;

    operator GtkApplicationWindow*() const
    {
        return GTK_APPLICATION_WINDOW(_instance);
    }

    void create_application_window(Application&);
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_APPLICATION_WINDOW_H__ */
