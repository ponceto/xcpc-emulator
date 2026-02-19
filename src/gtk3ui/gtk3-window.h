/*
 * gtk3-window.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __GTK3_CXX_WINDOW_H__
#define __GTK3_CXX_WINDOW_H__

#include <gtk3ui/gtk3-bin.h>

// ---------------------------------------------------------------------------
// gtk3::Window
// ---------------------------------------------------------------------------

namespace gtk3 {

class Window
    : public Bin
{
public: // public interface
    Window();

    Window(GtkWidget*);

    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    virtual ~Window() = default;

    operator GtkWindow*() const
    {
        return GTK_WINDOW(_instance);
    }

    auto set_title(const std::string& title) -> void;

    auto set_icon(GdkPixbuf* icon) -> void;

    auto set_skip_taskbar_hint(bool taskbar_hint) -> void;
};

}

// ---------------------------------------------------------------------------
// gtk3::ToplevelWindow
// ---------------------------------------------------------------------------

namespace gtk3 {

class ToplevelWindow
    : public Window
{
public: // public interface
    ToplevelWindow();

    ToplevelWindow(GtkWidget*);

    virtual ~ToplevelWindow() = default;

    auto create_toplevel_window() -> void;
};

}

// ---------------------------------------------------------------------------
// gtk3::PopupWindow
// ---------------------------------------------------------------------------

namespace gtk3 {

class PopupWindow
    : public Window
{
public: // public interface
    PopupWindow();

    PopupWindow(GtkWidget*);

    virtual ~PopupWindow() = default;

    auto create_popup_window() -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_WINDOW_H__ */
