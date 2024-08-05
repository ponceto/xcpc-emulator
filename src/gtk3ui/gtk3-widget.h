/*
 * gtk3-widget.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_WIDGET_H__
#define __GTK3_CXX_WIDGET_H__

#include <gtk3ui/gtk3-base.h>

// ---------------------------------------------------------------------------
// gtk3::Widget
// ---------------------------------------------------------------------------

namespace gtk3 {

class Widget
{
public: // public interface
    Widget();

    Widget(GtkWidget*);

    Widget(const Widget&) = delete;

    Widget& operator=(const Widget&) = delete;

    virtual ~Widget();

    operator bool() const
    {
        return _instance != nullptr;
    }

    operator GtkWidget*() const
    {
        return GTK_WIDGET(_instance);
    }

    GtkWidget* operator*() const
    {
        return _instance;
    }

    void destroy();

    void show_all();

    void show();

    void hide();

    void grab_focus();

    void set_sensitive(bool sensitive);

    bool is_sensitive();

    void drag_dest_set(GtkDestDefaults flags, const GtkTargetEntry* targets, int num_targets, GdkDragAction actions);

    void signal_connect(const char* signal, GCallback callback, void* data);

    void add_realize_callback(GCallback callback, void* data);

    void add_unrealize_callback(GCallback callback, void* data);

    void add_key_press_event_callback(GCallback callback, void* data);

    void add_key_release_event_callback(GCallback callback, void* data);

    void add_button_press_event_callback(GCallback callback, void* data);

    void add_button_release_event_callback(GCallback callback, void* data);

    void add_motion_notify_event_callback(GCallback callback, void* data);

    void add_drag_data_received_callback(GCallback callback, void* data);

protected: // protected data
    GtkWidget* _instance;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_WIDGET_H__ */
