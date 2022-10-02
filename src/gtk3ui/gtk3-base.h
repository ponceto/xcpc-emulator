/*
 * gtk3-base.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK3_CXX_BASE_H__
#define __GTK3_CXX_BASE_H__

#include <gtk/gtk.h>
#include <gtk3ui/gtkemulator.h>

// ---------------------------------------------------------------------------
// gtk3::signals
// ---------------------------------------------------------------------------

namespace gtk3 {

extern const char sig_destroy[];
extern const char sig_open[];
extern const char sig_startup[];
extern const char sig_shutdown[];
extern const char sig_activate[];
extern const char sig_clicked[];
extern const char sig_select[];
extern const char sig_deselect[];
extern const char sig_drag_data_received[];
extern const char sig_hotkey[];

}

// ---------------------------------------------------------------------------
// gtk3::BasicTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct BasicTraits
{
    template <typename T>
    static void signal_connect(T* instance, const char* signal, GCallback callback, void* data)
    {
        if(instance != nullptr) {
            static_cast<void>(::g_signal_connect(G_OBJECT(instance), signal, callback, data));
        }
    }

    static void register_widget_instance(GtkWidget*& instance);
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __GTK3_CXX_BASE_H__ */
