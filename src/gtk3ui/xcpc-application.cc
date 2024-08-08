/*
 * xcpc-application.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include <epoxy/gl.h>
#include "xcpc-application.h"
#include "xcpc-snapshot-dialog.h"
#include "xcpc-disk-dialog.h"
#include "xcpc-help-dialog.h"
#include "xcpc-about-dialog.h"

// ---------------------------------------------------------------------------
// <anonymous>::Environ
// ---------------------------------------------------------------------------

namespace {

struct Environ
{
    static char GTK_DEBUG[];
    static char GDK_DEBUG[];
    static char GDK_BACKEND[];

    static void set(char* variable)
    {
        const int rc = ::putenv(variable);
        if(rc != 0) {
            throw std::runtime_error("putenv() has failed");
        }
    }

    static void initialize()
    {
        set(GTK_DEBUG);
        set(GDK_DEBUG);
        set(GDK_BACKEND);
    }
};

char Environ::GTK_DEBUG[]   = "GDK_DEBUG=none";
char Environ::GDK_DEBUG[]   = "GDK_DEBUG=none";
char Environ::GDK_BACKEND[] = "GDK_BACKEND=x11";

}

// ---------------------------------------------------------------------------
// <anonymous>::IconTraits
// ---------------------------------------------------------------------------

namespace {

struct IconTraits
{
    static const char ico_load_snapshot[];
    static const char ico_save_snapshot[];
    static const char ico_play_emulator[];
    static const char ico_pause_emulator[];
    static const char ico_reset_emulator[];
    static const char ico_volume_increase[];
    static const char ico_volume_decrease[];
};

const char IconTraits::ico_load_snapshot[]   = "document-open-symbolic";
const char IconTraits::ico_save_snapshot[]   = "document-save-symbolic";
const char IconTraits::ico_play_emulator[]   = "media-playback-start-symbolic";
const char IconTraits::ico_pause_emulator[]  = "media-playback-pause-symbolic";
const char IconTraits::ico_reset_emulator[]  = "media-playlist-repeat-symbolic";
const char IconTraits::ico_volume_increase[] = "audio-volume-high-symbolic";
const char IconTraits::ico_volume_decrease[] = "audio-volume-low-symbolic";

}

// ---------------------------------------------------------------------------
// <anonymous>::Callbacks
// ---------------------------------------------------------------------------

namespace {

struct Callbacks
{
    using Application = xcpc::Application;
    using Canvas      = impl::Canvas;

    static auto on_statistics(Application* application) -> gboolean
    {
        if(application != nullptr) {
            application->on_statistics();
        }
        return TRUE;
    }

    static auto on_ignore(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            /* do nothing */
        }
    }

    static auto on_snapshot_load(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_snapshot_load();
        }
    }

    static auto on_snapshot_save(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_snapshot_save();
        }
    }

    static auto on_exit(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_exit();
        }
    }

    static auto on_emulator_play(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_emulator_play();
        }
    }

    static auto on_emulator_pause(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_emulator_pause();
        }
    }

    static auto on_emulator_reset(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_emulator_reset();
        }
    }

    static auto on_machine_cpc464(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_machine_cpc464();
        }
    }

    static auto on_machine_cpc664(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_machine_cpc664();
        }
    }

    static auto on_machine_cpc6128(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_machine_cpc6128();
        }
    }

    static auto on_company_isp(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_isp();
        }
    }

    static auto on_company_triumph(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_triumph();
        }
    }

    static auto on_company_saisho(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_saisho();
        }
    }

    static auto on_company_solavox(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_solavox();
        }
    }

    static auto on_company_awa(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_awa();
        }
    }

    static auto on_company_schneider(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_schneider();
        }
    }

    static auto on_company_orion(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_orion();
        }
    }

    static auto on_company_amstrad(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_company_amstrad();
        }
    }

    static auto on_monitor_color(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_monitor_color();
        }
    }

    static auto on_monitor_green(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_monitor_green();
        }
    }

    static auto on_monitor_gray(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_monitor_gray();
        }
    }

    static auto on_refresh_50hz(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_refresh_50hz();
        }
    }

    static auto on_refresh_60hz(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_refresh_60hz();
        }
    }

    static auto on_keyboard_english(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_keyboard_english();
        }
    }

    static auto on_keyboard_french(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_keyboard_french();
        }
    }

    static auto on_keyboard_german(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_keyboard_german();
        }
    }

    static auto on_keyboard_spanish(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_keyboard_spanish();
        }
    }

    static auto on_keyboard_danish(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_keyboard_danish();
        }
    }

    static auto on_drive0_disk_create(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_drive0_disk_create();
        }
    }

    static auto on_drive0_disk_insert(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_drive0_disk_insert();
        }
    }

    static auto on_drive0_disk_remove(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_drive0_disk_remove();
        }
    }

    static auto on_drive1_disk_create(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_drive1_disk_create();
        }
    }

    static auto on_drive1_disk_insert(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_drive1_disk_insert();
        }
    }

    static auto on_drive1_disk_remove(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_drive1_disk_remove();
        }
    }

    static auto on_volume_increase(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_volume_increase();
        }
    }

    static auto on_volume_decrease(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_volume_decrease();
        }
    }

    static auto on_scanlines_enable(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_scanlines_enable();
        }
    }

    static auto on_scanlines_disable(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_scanlines_disable();
        }
    }

    static auto on_joystick0_connect(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_joystick0_connect();
        }
    }

    static auto on_joystick0_disconnect(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_joystick0_disconnect();
        }
    }

    static auto on_joystick1_connect(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_joystick1_connect();
        }
    }

    static auto on_joystick1_disconnect(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_joystick1_disconnect();
        }
    }

    static auto on_help(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_help();
        }
    }

    static auto on_about(GtkWidget* widget, Application* application) -> void
    {
        if(application != nullptr) {
            application->on_about();
        }
    }

    static auto on_hotkey(GtkWidget* widget, KeySym* keysym, Application* application) -> void
    {
        if(keysym != nullptr) {
            switch(*keysym) {
                case XK_Pause:
                    on_emulator_pause(widget, application);
                    break;
                case XK_F1:
                    on_help(widget, application);
                    break;
                case XK_F2:
                    on_snapshot_load(widget, application);
                    break;
                case XK_F3:
                    on_snapshot_save(widget, application);
                    break;
                case XK_F4:
                    on_ignore(widget, application);
                    break;
                case XK_F5:
                    on_emulator_reset(widget, application);
                    break;
                case XK_F6:
                    on_drive0_disk_insert(widget, application);
                    break;
                case XK_F7:
                    on_drive0_disk_remove(widget, application);
                    break;
                case XK_F8:
                    on_drive1_disk_insert(widget, application);
                    break;
                case XK_F9:
                    on_drive1_disk_remove(widget, application);
                    break;
                case XK_F10:
                    on_ignore(widget, application);
                    break;
                case XK_F11:
                    on_ignore(widget, application);
                    break;
                case XK_F12:
                    on_ignore(widget, application);
                    break;
                default:
                    break;
            }
        }
    }

    static auto on_canvas_realize(GtkWidget* widget, Canvas* canvas) -> void
    {
        if(canvas != nullptr) {
            canvas->on_canvas_realize();
        }
    }

    static auto on_canvas_unrealize(GtkWidget* widget, Canvas* canvas) -> void
    {
        if(canvas != nullptr) {
            canvas->on_canvas_unrealize();
        }
    }

    static auto on_canvas_render(GtkWidget* widget, GdkGLContext* context, Canvas* canvas) -> gboolean
    {
        if(canvas != nullptr) {
            canvas->on_canvas_render(*context);
        }
        return TRUE;
    }

    static auto on_canvas_resize(GtkWidget* widget, gint width, gint height, Canvas* canvas) -> void
    {
        if(canvas != nullptr) {
            canvas->on_canvas_resize(width, height);
        }
    }

    static auto on_canvas_key_press(GtkWidget* widget, GdkEventKey* event, Canvas* canvas) -> gboolean
    {
        if(canvas != nullptr) {
            ::gtk_widget_grab_focus(widget);
            canvas->on_canvas_key_press(*event);
        }
        return TRUE;
    }

    static auto on_canvas_key_release(GtkWidget* widget, GdkEventKey* event, Canvas* canvas) -> gboolean
    {
        if(canvas != nullptr) {
            ::gtk_widget_grab_focus(widget);
            canvas->on_canvas_key_release(*event);
        }
        return TRUE;
    }

    static auto on_canvas_button_press(GtkWidget* widget, GdkEventButton* event, Canvas* canvas) -> gboolean
    {
        if(canvas != nullptr) {
            ::gtk_widget_grab_focus(widget);
            canvas->on_canvas_button_press(*event);
        }
        return TRUE;
    }

    static auto on_canvas_button_release(GtkWidget* widget, GdkEventButton* event, Canvas* canvas) -> gboolean
    {
        if(canvas != nullptr) {
            ::gtk_widget_grab_focus(widget);
            canvas->on_canvas_button_release(*event);
        }
        return TRUE;
    }

    static auto on_canvas_motion_notify(GtkWidget* widget, GdkEventMotion* event, Canvas* canvas) -> gboolean
    {
        if(canvas != nullptr) {
            canvas->on_canvas_motion_notify(*event);
        }
        return TRUE;
    }

    static auto has_extension(const char* filename, const char* extension) -> bool
    {
        if((filename != nullptr) && (extension != nullptr)) {
            const int filename_length  = ::strlen(filename);
            const int extension_length = ::strlen(extension);
            if(filename_length >= extension_length) {
                if(::strcasecmp(&filename[filename_length - extension_length], extension) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    static auto open_file(Application& application, const char* filename) -> void
    {
        if(filename != nullptr) {
            const char* file_scheme_str = "file://";
            const int   file_scheme_len = ::strlen(file_scheme_str);
            if(::strncmp(filename, file_scheme_str, file_scheme_len) == 0) {
                filename += file_scheme_len;
            }
            if(has_extension(filename, ".sna") != false) {
                application.load_snapshot(filename);
                application.play_emulator();
            }
            else if(has_extension(filename, ".dsk") != false) {
                application.insert_disk_into_drive0(filename);
                application.play_emulator();
            }
            else if(has_extension(filename, ".dsk.gz") != false) {
                application.insert_disk_into_drive0(filename);
                application.play_emulator();
            }
            else if(has_extension(filename, ".dsk.bz2") != false) {
                application.insert_disk_into_drive0(filename);
                application.play_emulator();
            }
        }
    }

    static auto on_drag_data_received ( GtkWidget*         widget
                                      , GdkDragContext*    context
                                      , int                x
                                      , int                y
                                      , GtkSelectionData*  data
                                      , guint              info
                                      , guint              time
                                      , Application*       application ) -> void
    {
        gchar** uris = ::gtk_selection_data_get_uris(data);

        if(uris != nullptr) {
            for(int index = 0; uris[index] != nullptr; ++index) {
                open_file(*application, uris[index]);
            }
            uris = (::g_strfreev(uris), nullptr);
        }
    }
};

}

// ---------------------------------------------------------------------------
// impl::AppWidget
// ---------------------------------------------------------------------------

namespace impl {

AppWidget::AppWidget(Application& application)
    : _application(application)
{
}

}

// ---------------------------------------------------------------------------
// impl::Canvas
// ---------------------------------------------------------------------------

namespace impl {

Canvas::Canvas(Application& application)
    : AppWidget(application)
    , gtk3::GLArea(nullptr)
    , _self(*this)
{
}

void Canvas::build()
{
    auto build_self = [&]() -> void
    {
#ifdef XCPC_ENABLE_GL_AREA
        _self.create_gl_area();
        _self.set_can_focus(true);
        _self.add_realize_callback(G_CALLBACK(&Callbacks::on_canvas_realize), this);
        _self.add_unrealize_callback(G_CALLBACK(&Callbacks::on_canvas_unrealize), this);
        _self.add_render_callback(G_CALLBACK(&Callbacks::on_canvas_render), this);
        _self.add_resize_callback(G_CALLBACK(&Callbacks::on_canvas_resize), this);
        _self.add_key_press_event_callback(G_CALLBACK(&Callbacks::on_canvas_key_press), this);
        _self.add_key_release_event_callback(G_CALLBACK(&Callbacks::on_canvas_key_release), this);
        _self.add_button_press_event_callback(G_CALLBACK(&Callbacks::on_canvas_button_press), this);
        _self.add_button_release_event_callback(G_CALLBACK(&Callbacks::on_canvas_button_release), this);
        _self.add_motion_notify_event_callback(G_CALLBACK(&Callbacks::on_canvas_motion_notify), this);
#endif
    };

    auto build_all = [&]() -> void
    {
        build_self();
    };

    return build_all();
}

auto Canvas::on_canvas_realize() -> void
{
    ::xcpc_log_debug("on_canvas_realize");
}

auto Canvas::on_canvas_unrealize() -> void
{
    ::xcpc_log_debug("on_canvas_unrealize");
}

auto Canvas::on_canvas_render(GdkGLContext& context) -> void
{
    ::xcpc_log_debug("on_canvas_render");
    ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ::glClear(GL_COLOR_BUFFER_BIT);
    ::glFlush();
}

auto Canvas::on_canvas_resize(gint width, gint height) -> void
{
    ::xcpc_log_debug("on_canvas_resize(%d, %d)", width, height);
}

auto Canvas::on_canvas_key_press(GdkEventKey& event) -> void
{
    ::xcpc_log_debug("on_canvas_key_press");
}

auto Canvas::on_canvas_key_release(GdkEventKey& event) -> void
{
    ::xcpc_log_debug("on_canvas_key_release");
}

auto Canvas::on_canvas_button_press(GdkEventButton& event) -> void
{
    ::xcpc_log_debug("on_canvas_button_press");
}

auto Canvas::on_canvas_button_release(GdkEventButton& event) -> void
{
    ::xcpc_log_debug("on_canvas_button_release");
}

auto Canvas::on_canvas_motion_notify(GdkEventMotion& event) -> void
{
    ::xcpc_log_debug("on_canvas_motion_notify");
}

}

// ---------------------------------------------------------------------------
// impl::FileMenu
// ---------------------------------------------------------------------------

namespace impl {

FileMenu::FileMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _snapshot_load(nullptr)
    , _snapshot_save(nullptr)
    , _separator(nullptr)
    , _exit(nullptr)
{
}

void FileMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("File"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_snapshot_load = [&]() -> void
    {
        _snapshot_load.create_menu_item_with_label(_("Load snapshot..."));
        _snapshot_load.set_accel(GDK_KEY_F2, GdkModifierType(0));
        _snapshot_load.add_activate_callback(G_CALLBACK(&Callbacks::on_snapshot_load), &_application);
        _menu.append(_snapshot_load);
    };

    auto build_snapshot_save = [&]() -> void
    {
        _snapshot_save.create_menu_item_with_label(_("Save snapshot..."));
        _snapshot_save.set_accel(GDK_KEY_F3, GdkModifierType(0));
        _snapshot_save.add_activate_callback(G_CALLBACK(&Callbacks::on_snapshot_save), &_application);
        _menu.append(_snapshot_save);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_exit = [&]() -> void
    {
        _exit.create_menu_item_with_label(_("Exit"));
        _exit.add_activate_callback(G_CALLBACK(&Callbacks::on_exit), &_application);
        _menu.append(_exit);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_snapshot_load();
        build_snapshot_save();
        build_separator();
        build_exit();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::ControlsMenu
// ---------------------------------------------------------------------------

namespace impl {

ControlsMenu::ControlsMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _emulator_play(nullptr)
    , _emulator_pause(nullptr)
    , _separator(nullptr)
    , _emulator_reset(nullptr)
{
}

void ControlsMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Controls"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_emulator_play = [&]() -> void
    {
        _emulator_play.create_menu_item_with_label(_("Play"));
        _emulator_play.add_activate_callback(G_CALLBACK(&Callbacks::on_emulator_play), &_application);
        _menu.append(_emulator_play);
    };

    auto build_emulator_pause = [&]() -> void
    {
        _emulator_pause.create_menu_item_with_label(_("Pause"));
        _emulator_pause.add_activate_callback(G_CALLBACK(&Callbacks::on_emulator_pause), &_application);
        _menu.append(_emulator_pause);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_emulator_reset = [&]() -> void
    {
        _emulator_reset.create_menu_item_with_label(_("Reset"));
        _emulator_reset.set_accel(GDK_KEY_F5, GdkModifierType(0));
        _emulator_reset.add_activate_callback(G_CALLBACK(&Callbacks::on_emulator_reset), &_application);
        _menu.append(_emulator_reset);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_emulator_play();
        build_emulator_pause();
        build_separator();
        build_emulator_reset();
    };

    return build_all();
}

void ControlsMenu::show_play()
{
    _emulator_play.show();
}

void ControlsMenu::hide_play()
{
    _emulator_play.hide();
}

void ControlsMenu::show_pause()
{
    _emulator_pause.show();
}

void ControlsMenu::hide_pause()
{
    _emulator_pause.hide();
}

void ControlsMenu::show_reset()
{
    _emulator_reset.show();
}

void ControlsMenu::hide_reset()
{
    _emulator_reset.hide();
}

}

// ---------------------------------------------------------------------------
// impl::MachineMenu
// ---------------------------------------------------------------------------

namespace impl {

MachineMenu::MachineMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _machine(nullptr)
    , _machine_menu(nullptr)
    , _machine_cpc464(nullptr)
    , _machine_cpc664(nullptr)
    , _machine_cpc6128(nullptr)
    , _company(nullptr)
    , _company_menu(nullptr)
    , _company_isp(nullptr)
    , _company_triumph(nullptr)
    , _company_saisho(nullptr)
    , _company_solavox(nullptr)
    , _company_awa(nullptr)
    , _company_schneider(nullptr)
    , _company_orion(nullptr)
    , _company_amstrad(nullptr)
    , _monitor(nullptr)
    , _monitor_menu(nullptr)
    , _monitor_color(nullptr)
    , _monitor_green(nullptr)
    , _monitor_gray(nullptr)
    , _refresh(nullptr)
    , _refresh_menu(nullptr)
    , _refresh_50hz(nullptr)
    , _refresh_60hz(nullptr)
    , _keyboard(nullptr)
    , _keyboard_menu(nullptr)
    , _keyboard_english(nullptr)
    , _keyboard_french(nullptr)
    , _keyboard_german(nullptr)
    , _keyboard_spanish(nullptr)
    , _keyboard_danish(nullptr)
{
}

void MachineMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Machine"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_machine = [&]() -> void
    {
        _machine.create_menu_item_with_label(_("Machine"));
        _menu.append(_machine);
        _machine_menu.create_menu();
        _machine.set_submenu(_machine_menu);
    };

    auto build_machine_cpc464 = [&]() -> void
    {
        _machine_cpc464.create_menu_item_with_label(_("CPC 464"));
        _machine_cpc464.add_activate_callback(G_CALLBACK(&Callbacks::on_machine_cpc464), &_application);
        _machine_menu.append(_machine_cpc464);
    };

    auto build_machine_cpc664 = [&]() -> void
    {
        _machine_cpc664.create_menu_item_with_label(_("CPC 664"));
        _machine_cpc664.add_activate_callback(G_CALLBACK(&Callbacks::on_machine_cpc664), &_application);
        _machine_menu.append(_machine_cpc664);
    };

    auto build_machine_cpc6128 = [&]() -> void
    {
        _machine_cpc6128.create_menu_item_with_label(_("CPC 6128"));
        _machine_cpc6128.add_activate_callback(G_CALLBACK(&Callbacks::on_machine_cpc6128), &_application);
        _machine_menu.append(_machine_cpc6128);
    };

    auto build_company = [&]() -> void
    {
        _company.create_menu_item_with_label(_("Company"));
        _menu.append(_company);
        _company_menu.create_menu();
        _company.set_submenu(_company_menu);
    };

    auto build_company_isp = [&]() -> void
    {
        _company_isp.create_menu_item_with_label(_("Isp"));
        _company_isp.add_activate_callback(G_CALLBACK(&Callbacks::on_company_isp), &_application);
        _company_menu.append(_company_isp);
    };

    auto build_company_triumph = [&]() -> void
    {
        _company_triumph.create_menu_item_with_label(_("Triumph"));
        _company_triumph.add_activate_callback(G_CALLBACK(&Callbacks::on_company_triumph), &_application);
        _company_menu.append(_company_triumph);
    };

    auto build_company_saisho = [&]() -> void
    {
        _company_saisho.create_menu_item_with_label(_("Saisho"));
        _company_saisho.add_activate_callback(G_CALLBACK(&Callbacks::on_company_saisho), &_application);
        _company_menu.append(_company_saisho);
    };

    auto build_company_solavox = [&]() -> void
    {
        _company_solavox.create_menu_item_with_label(_("Solavox"));
        _company_solavox.add_activate_callback(G_CALLBACK(&Callbacks::on_company_solavox), &_application);
        _company_menu.append(_company_solavox);
    };

    auto build_company_awa = [&]() -> void
    {
        _company_awa.create_menu_item_with_label(_("Awa"));
        _company_awa.add_activate_callback(G_CALLBACK(&Callbacks::on_company_awa), &_application);
        _company_menu.append(_company_awa);
    };

    auto build_company_schneider = [&]() -> void
    {
        _company_schneider.create_menu_item_with_label(_("Schneider"));
        _company_schneider.add_activate_callback(G_CALLBACK(&Callbacks::on_company_schneider), &_application);
        _company_menu.append(_company_schneider);
    };

    auto build_company_orion = [&]() -> void
    {
        _company_orion.create_menu_item_with_label(_("Orion"));
        _company_orion.add_activate_callback(G_CALLBACK(&Callbacks::on_company_orion), &_application);
        _company_menu.append(_company_orion);
    };

    auto build_company_amstrad = [&]() -> void
    {
        _company_amstrad.create_menu_item_with_label(_("Amstrad"));
        _company_amstrad.add_activate_callback(G_CALLBACK(&Callbacks::on_company_amstrad), &_application);
        _company_menu.append(_company_amstrad);
    };

    auto build_monitor = [&]() -> void
    {
        _monitor.create_menu_item_with_label(_("Monitor"));
        _menu.append(_monitor);
        _monitor_menu.create_menu();
        _monitor.set_submenu(_monitor_menu);
    };

    auto build_monitor_color = [&]() -> void
    {
        _monitor_color.create_menu_item_with_label(_("Color"));
        _monitor_color.add_activate_callback(G_CALLBACK(&Callbacks::on_monitor_color), &_application);
        _monitor_menu.append(_monitor_color);
    };

    auto build_monitor_green = [&]() -> void
    {
        _monitor_green.create_menu_item_with_label(_("Green"));
        _monitor_green.add_activate_callback(G_CALLBACK(&Callbacks::on_monitor_green), &_application);
        _monitor_menu.append(_monitor_green);
    };

    auto build_monitor_gray = [&]() -> void
    {
        _monitor_gray.create_menu_item_with_label(_("Gray"));
        _monitor_gray.add_activate_callback(G_CALLBACK(&Callbacks::on_monitor_gray), &_application);
        _monitor_menu.append(_monitor_gray);
    };

    auto build_refresh = [&]() -> void
    {
        _refresh.create_menu_item_with_label(_("Refresh"));
        _menu.append(_refresh);
        _refresh_menu.create_menu();
        _refresh.set_submenu(_refresh_menu);
    };

    auto build_refresh_50hz = [&]() -> void
    {
        _refresh_50hz.create_menu_item_with_label(_("50Hz"));
        _refresh_50hz.add_activate_callback(G_CALLBACK(&Callbacks::on_refresh_50hz), &_application);
        _refresh_menu.append(_refresh_50hz);
    };

    auto build_refresh_60hz = [&]() -> void
    {
        _refresh_60hz.create_menu_item_with_label(_("60Hz"));
        _refresh_60hz.add_activate_callback(G_CALLBACK(&Callbacks::on_refresh_60hz), &_application);
        _refresh_menu.append(_refresh_60hz);
    };

    auto build_keyboard = [&]() -> void
    {
        _keyboard.create_menu_item_with_label(_("Keyboard"));
        _menu.append(_keyboard);
        _keyboard_menu.create_menu();
        _keyboard.set_submenu(_keyboard_menu);
    };

    auto build_keyboard_english = [&]() -> void
    {
        _keyboard_english.create_menu_item_with_label(_("English"));
        _keyboard_english.add_activate_callback(G_CALLBACK(&Callbacks::on_keyboard_english), &_application);
        _keyboard_menu.append(_keyboard_english);
    };

    auto build_keyboard_french = [&]() -> void
    {
        _keyboard_french.create_menu_item_with_label(_("French"));
        _keyboard_french.add_activate_callback(G_CALLBACK(&Callbacks::on_keyboard_french), &_application);
        _keyboard_menu.append(_keyboard_french);
    };

    auto build_keyboard_german = [&]() -> void
    {
        _keyboard_german.create_menu_item_with_label(_("German"));
        _keyboard_german.add_activate_callback(G_CALLBACK(&Callbacks::on_keyboard_german), &_application);
        _keyboard_menu.append(_keyboard_german);
        _keyboard_german.set_sensitive(false);
    };

    auto build_keyboard_spanish = [&]() -> void
    {
        _keyboard_spanish.create_menu_item_with_label(_("Spanish"));
        _keyboard_spanish.add_activate_callback(G_CALLBACK(&Callbacks::on_keyboard_spanish), &_application);
        _keyboard_menu.append(_keyboard_spanish);
        _keyboard_spanish.set_sensitive(false);
    };

    auto build_keyboard_danish = [&]() -> void
    {
        _keyboard_danish.create_menu_item_with_label(_("Danish"));
        _keyboard_danish.add_activate_callback(G_CALLBACK(&Callbacks::on_keyboard_danish), &_application);
        _keyboard_menu.append(_keyboard_danish);
        _keyboard_danish.set_sensitive(false);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_machine();
        build_machine_cpc464();
        build_machine_cpc664();
        build_machine_cpc6128();
        build_company();
        build_company_isp();
        build_company_triumph();
        build_company_saisho();
        build_company_solavox();
        build_company_awa();
        build_company_schneider();
        build_company_orion();
        build_company_amstrad();
        build_monitor();
        build_monitor_color();
        build_monitor_green();
        build_monitor_gray();
        build_refresh();
        build_refresh_50hz();
        build_refresh_60hz();
        build_keyboard();
        build_keyboard_english();
        build_keyboard_french();
        build_keyboard_german();
        build_keyboard_spanish();
        build_keyboard_danish();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::Drive0Menu
// ---------------------------------------------------------------------------

namespace impl {

Drive0Menu::Drive0Menu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _disk_create(nullptr)
    , _separator(nullptr)
    , _disk_insert(nullptr)
    , _disk_remove(nullptr)
{
}

void Drive0Menu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Drive A"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_disk_create = [&]() -> void
    {
        _disk_create.create_menu_item_with_label(_("Create disk..."));
        _disk_create.add_activate_callback(G_CALLBACK(&Callbacks::on_drive0_disk_create), &_application);
        _menu.append(_disk_create);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_disk_insert = [&]() -> void
    {
        _disk_insert.create_menu_item_with_label(_("Insert disk..."));
        _disk_insert.set_accel(GDK_KEY_F6, GdkModifierType(0));
        _disk_insert.add_activate_callback(G_CALLBACK(&Callbacks::on_drive0_disk_insert), &_application);
        _menu.append(_disk_insert);
    };

    auto build_disk_remove = [&]() -> void
    {
        _disk_remove.create_menu_item_with_label(_("Remove disk..."));
        _disk_remove.set_accel(GDK_KEY_F7, GdkModifierType(0));
        _disk_remove.add_activate_callback(G_CALLBACK(&Callbacks::on_drive0_disk_remove), &_application);
        _menu.append(_disk_remove);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_disk_create();
        build_separator();
        build_disk_insert();
        build_disk_remove();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::Drive1Menu
// ---------------------------------------------------------------------------

namespace impl {

Drive1Menu::Drive1Menu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _disk_create(nullptr)
    , _separator(nullptr)
    , _disk_insert(nullptr)
    , _disk_remove(nullptr)
{
}

void Drive1Menu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Drive B"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_disk_create = [&]() -> void
    {
        _disk_create.create_menu_item_with_label(_("Create disk..."));
        _disk_create.add_activate_callback(G_CALLBACK(&Callbacks::on_drive1_disk_create), &_application);
        _menu.append(_disk_create);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_disk_insert = [&]() -> void
    {
        _disk_insert.create_menu_item_with_label(_("Insert disk..."));
        _disk_insert.set_accel(GDK_KEY_F8, GdkModifierType(0));
        _disk_insert.add_activate_callback(G_CALLBACK(&Callbacks::on_drive1_disk_insert), &_application);
        _menu.append(_disk_insert);
    };

    auto build_disk_remove = [&]() -> void
    {
        _disk_remove.create_menu_item_with_label(_("Remove disk..."));
        _disk_remove.set_accel(GDK_KEY_F9, GdkModifierType(0));
        _disk_remove.add_activate_callback(G_CALLBACK(&Callbacks::on_drive1_disk_remove), &_application);
        _menu.append(_disk_remove);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_disk_create();
        build_separator();
        build_disk_insert();
        build_disk_remove();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::AudioMenu
// ---------------------------------------------------------------------------

namespace impl {

AudioMenu::AudioMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _volume_increase(nullptr)
    , _volume_decrease(nullptr)
{
}

void AudioMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Audio"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_volume_increase = [&]() -> void
    {
        _volume_increase.create_menu_item_with_label(_("Increase volume"));
        _volume_increase.add_activate_callback(G_CALLBACK(&Callbacks::on_volume_increase), &_application);
        _menu.append(_volume_increase);
    };

    auto build_volume_decrease = [&]() -> void
    {
        _volume_decrease.create_menu_item_with_label(_("Decrease volume"));
        _volume_decrease.add_activate_callback(G_CALLBACK(&Callbacks::on_volume_decrease), &_application);
        _menu.append(_volume_decrease);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_volume_increase();
        build_volume_decrease();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::VideoMenu
// ---------------------------------------------------------------------------

namespace impl {

VideoMenu::VideoMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _scanlines_enable(nullptr)
    , _scanlines_disable(nullptr)
{
}

void VideoMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Video"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_scanlines_enable = [&]() -> void
    {
        _scanlines_enable.create_menu_item_with_label(_("Enable scanlines"));
        _scanlines_enable.add_activate_callback(G_CALLBACK(&Callbacks::on_scanlines_enable), &_application);
        _menu.append(_scanlines_enable);
    };

    auto build_scanlines_disable = [&]() -> void
    {
        _scanlines_disable.create_menu_item_with_label(_("Disable scanlines"));
        _scanlines_disable.add_activate_callback(G_CALLBACK(&Callbacks::on_scanlines_disable), &_application);
        _menu.append(_scanlines_disable);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_scanlines_enable();
        build_scanlines_disable();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::InputMenu
// ---------------------------------------------------------------------------

namespace impl {

InputMenu::InputMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _joystick0(nullptr)
    , _joystick0_menu(nullptr)
    , _joystick0_connect(nullptr)
    , _joystick0_disconnect(nullptr)
    , _joystick1(nullptr)
    , _joystick1_menu(nullptr)
    , _joystick1_connect(nullptr)
    , _joystick1_disconnect(nullptr)
{
}

void InputMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Input"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_joystick0 = [&]() -> void
    {
        std::string label(_("Joystick 0"));
        label += ' ';
        label += '(';
        label += Utils::get_joystick0();
        label += ')';
        _joystick0.create_menu_item_with_label(label);
        _menu.append(_joystick0);
        _joystick0_menu.create_menu();
        _joystick0.set_submenu(_joystick0_menu);
    };

    auto build_joystick0_connect = [&]() -> void
    {
        _joystick0_connect.create_menu_item_with_label(_("Connect"));
        _joystick0_connect.add_activate_callback(G_CALLBACK(&Callbacks::on_joystick0_connect), &_application);
        _joystick0_menu.append(_joystick0_connect);
    };

    auto build_joystick0_disconnect = [&]() -> void
    {
        _joystick0_disconnect.create_menu_item_with_label(_("Disconnect"));
        _joystick0_disconnect.add_activate_callback(G_CALLBACK(&Callbacks::on_joystick0_disconnect), &_application);
        _joystick0_menu.append(_joystick0_disconnect);
    };

    auto build_joystick1 = [&]() -> void
    {
        std::string label(_("Joystick 1"));
        label += ' ';
        label += '(';
        label += Utils::get_joystick1();
        label += ')';
        _joystick1.create_menu_item_with_label(label);
        _menu.append(_joystick1);
        _joystick1_menu.create_menu();
        _joystick1.set_submenu(_joystick1_menu);
    };

    auto build_joystick1_connect = [&]() -> void
    {
        _joystick1_connect.create_menu_item_with_label(_("Connect"));
        _joystick1_connect.add_activate_callback(G_CALLBACK(&Callbacks::on_joystick1_connect), &_application);
        _joystick1_menu.append(_joystick1_connect);
    };

    auto build_joystick1_disconnect = [&]() -> void
    {
        _joystick1_disconnect.create_menu_item_with_label(_("Disconnect"));
        _joystick1_disconnect.add_activate_callback(G_CALLBACK(&Callbacks::on_joystick1_disconnect), &_application);
        _joystick1_menu.append(_joystick1_disconnect);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_joystick0();
        build_joystick0_connect();
        build_joystick0_disconnect();
        build_joystick1();
        build_joystick1_connect();
        build_joystick1_disconnect();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::HelpMenu
// ---------------------------------------------------------------------------

namespace impl {

HelpMenu::HelpMenu(Application& application)
    : AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _self(*this)
    , _menu(nullptr)
    , _help(nullptr)
    , _separator(nullptr)
    , _about(nullptr)
{
}

void HelpMenu::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Help"));
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_help = [&]() -> void
    {
        _help.create_menu_item_with_label(_("Help"));
        _help.set_accel(GDK_KEY_F1, GdkModifierType(0));
        _help.add_activate_callback(G_CALLBACK(&Callbacks::on_help), &_application);
        _menu.append(_help);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_about = [&]() -> void
    {
        _about.create_menu_item_with_label(_("About"));
        _about.add_activate_callback(G_CALLBACK(&Callbacks::on_about), &_application);
        _menu.append(_about);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_help();
        build_separator();
        build_about();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::MenuBar
// ---------------------------------------------------------------------------

namespace impl {

MenuBar::MenuBar(Application& application)
    : AppWidget(application)
    , gtk3::MenuBar(nullptr)
    , _self(*this)
    , _file_menu(_application)
    , _controls_menu(_application)
    , _machine_menu(_application)
    , _drive0_menu(_application)
    , _drive1_menu(_application)
    , _audio_menu(_application)
    , _video_menu(_application)
    , _input_menu(_application)
    , _help_menu(_application)
{
}

void MenuBar::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_menu_bar();
    };

    auto build_file_menu = [&]() -> void
    {
        _file_menu.build();
        _self.append(_file_menu);
    };

    auto build_controls_menu = [&]() -> void
    {
        _controls_menu.build();
        _self.append(_controls_menu);
    };

    auto build_machine_menu = [&]() -> void
    {
        _machine_menu.build();
        _self.append(_machine_menu);
    };

    auto build_drive0_menu = [&]() -> void
    {
        _drive0_menu.build();
        _self.append(_drive0_menu);
    };

    auto build_drive1_menu = [&]() -> void
    {
        _drive1_menu.build();
        _self.append(_drive1_menu);
    };

    auto build_audio_menu = [&]() -> void
    {
        _audio_menu.build();
        _self.append(_audio_menu);
    };

    auto build_video_menu = [&]() -> void
    {
        _video_menu.build();
        _self.append(_video_menu);
    };

    auto build_input_menu = [&]() -> void
    {
        _input_menu.build();
        _self.append(_input_menu);
    };

    auto build_help_menu = [&]() -> void
    {
        _help_menu.build();
        _self.append(_help_menu);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_file_menu();
        build_controls_menu();
        build_machine_menu();
        build_drive0_menu();
        build_drive1_menu();
        build_audio_menu();
        build_video_menu();
        build_input_menu();
        build_help_menu();
    };

    return build_all();
}

void MenuBar::show_play()
{
    _controls_menu.show_play();
}

void MenuBar::hide_play()
{
    _controls_menu.hide_play();
}

void MenuBar::show_pause()
{
    _controls_menu.show_pause();
}

void MenuBar::hide_pause()
{
    _controls_menu.hide_pause();
}

void MenuBar::show_reset()
{
    _controls_menu.show_reset();
}

void MenuBar::hide_reset()
{
    _controls_menu.hide_reset();
}

}

// ---------------------------------------------------------------------------
// impl::ToolBar
// ---------------------------------------------------------------------------

namespace impl {

ToolBar::ToolBar(Application& application)
    : AppWidget(application)
    , gtk3::Toolbar(nullptr)
    , _self(*this)
    , _snapshot_load(nullptr)
    , _snapshot_save(nullptr)
    , _separator1(nullptr)
    , _emulator_play(nullptr)
    , _emulator_pause(nullptr)
    , _emulator_reset(nullptr)
    , _separator2(nullptr)
    , _volume_decrease(nullptr)
    , _volume_increase(nullptr)
{
}

void ToolBar::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_toolbar();
    };

    auto build_load_snapshot = [&]() -> void
    {
        _snapshot_load.create_tool_button();
        _snapshot_load.set_icon_name(IconTraits::ico_load_snapshot);
        _snapshot_load.add_clicked_callback(G_CALLBACK(&Callbacks::on_snapshot_load), &_application);
        _self.insert(_snapshot_load, -1);
    };

    auto build_save_snapshot = [&]() -> void
    {
        _snapshot_save.create_tool_button();
        _snapshot_save.set_icon_name(IconTraits::ico_save_snapshot);
        _snapshot_save.add_clicked_callback(G_CALLBACK(&Callbacks::on_snapshot_save), &_application);
        _self.insert(_snapshot_save, -1);
    };

    auto build_separator1 = [&]() -> void
    {
        _separator1.create_separator_tool_item();
        _self.insert(_separator1, -1);
    };

    auto build_play_emulator = [&]() -> void
    {
        _emulator_play.create_tool_button();
        _emulator_play.set_icon_name(IconTraits::ico_play_emulator);
        _emulator_play.add_clicked_callback(G_CALLBACK(&Callbacks::on_emulator_play), &_application);
        _self.insert(_emulator_play, -1);
    };

    auto build_pause_emulator = [&]() -> void
    {
        _emulator_pause.create_tool_button();
        _emulator_pause.set_icon_name(IconTraits::ico_pause_emulator);
        _emulator_pause.add_clicked_callback(G_CALLBACK(&Callbacks::on_emulator_pause), &_application);
        _self.insert(_emulator_pause, -1);
    };

    auto build_reset_emulator = [&]() -> void
    {
        _emulator_reset.create_tool_button();
        _emulator_reset.set_icon_name(IconTraits::ico_reset_emulator);
        _emulator_reset.add_clicked_callback(G_CALLBACK(&Callbacks::on_emulator_reset), &_application);
        _self.insert(_emulator_reset, -1);
    };

    auto build_separator2 = [&]() -> void
    {
        _separator2.create_separator_tool_item();
        _self.insert(_separator2, -1);
    };

    auto build_volume_decrease = [&]() -> void
    {
        _volume_decrease.create_tool_button();
        _volume_decrease.set_icon_name(IconTraits::ico_volume_decrease);
        _volume_decrease.add_clicked_callback(G_CALLBACK(&Callbacks::on_volume_decrease), &_application);
        _self.insert(_volume_decrease, -1);
    };

    auto build_volume_increase = [&]() -> void
    {
        _volume_increase.create_tool_button();
        _volume_increase.set_icon_name(IconTraits::ico_volume_increase);
        _volume_increase.add_clicked_callback(G_CALLBACK(&Callbacks::on_volume_increase), &_application);
        _self.insert(_volume_increase, -1);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_load_snapshot();
        build_save_snapshot();
        build_separator1();
        build_play_emulator();
        build_pause_emulator();
        build_reset_emulator();
        build_separator2();
        build_volume_decrease();
        build_volume_increase();
    };

    return build_all();
}

void ToolBar::show_play()
{
    _emulator_play.show();
}

void ToolBar::hide_play()
{
    _emulator_play.hide();
}

void ToolBar::show_pause()
{
    _emulator_pause.show();
}

void ToolBar::hide_pause()
{
    _emulator_pause.hide();
}

void ToolBar::show_reset()
{
    _emulator_reset.show();
}

void ToolBar::hide_reset()
{
    _emulator_reset.hide();
}

}

// ---------------------------------------------------------------------------
// impl::InfoBar
// ---------------------------------------------------------------------------

namespace impl {

InfoBar::InfoBar(Application& application)
    : AppWidget(application)
    , gtk3::HBox(nullptr)
    , _self(*this)
    , _state(nullptr)
    , _drive0(nullptr)
    , _drive1(nullptr)
    , _system(nullptr)
    , _volume(nullptr)
    , _stats(nullptr)
{
}

void InfoBar::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_hbox();
    };

    auto build_state = [&]() -> void
    {
        _state.create_label(_("State"));
        _self.pack_start(_state, false, true, 2);
    };

    auto build_drive0 = [&]() -> void
    {
        _drive0.create_label(_("Drive A"));
        _drive0.set_ellipsize(PANGO_ELLIPSIZE_MIDDLE);
        _self.pack_start(_drive0, false, true, 2);
    };

    auto build_drive1 = [&]() -> void
    {
        _drive1.create_label(_("Drive B"));
        _drive1.set_ellipsize(PANGO_ELLIPSIZE_MIDDLE);
        _self.pack_start(_drive1, false, true, 2);
    };

    auto build_system = [&]() -> void
    {
        _system.create_label(_("System"));
        _system.set_ellipsize(PANGO_ELLIPSIZE_MIDDLE);
        _self.pack_start(_system, true, true, 2);
    };

    auto build_volume = [&]() -> void
    {
        _volume.create_label(_("Volume"));
        _self.pack_start(_volume, false, true, 2);
    };

    auto build_stats = [&]() -> void
    {
        _stats.create_label(_("Stats"));
        _self.pack_end(_stats, false, true, 2);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_state();
        build_drive0();
        build_drive1();
        build_system();
        build_volume();
        build_stats();
    };

    return build_all();
}

void InfoBar::set_state(const std::string& state)
{
    std::string label(_("{unknown}"));

    auto format_label = [&]() -> void
    {
        const char* format = "<span foreground='grey90' background='darkgreen'> %s </span>";
        char*       string = ::g_markup_printf_escaped(format, state.c_str());
        if(string != nullptr) {
            std::string(string).swap(label);
            string = (::g_free(string), nullptr);
        }
    };

    auto update_label = [&]() -> void
    {
        _state.set_markup(label);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return update();
}

void InfoBar::set_drive0(const std::string& drive0)
{
    std::string label(_("{unknown}"));

    auto format_label = [&]() -> void
    {
        const char* format = "<span foreground='yellow' background='darkblue'> A: %s </span>";
        char*       string = ::g_markup_printf_escaped(format, drive0.c_str());
        if(string != nullptr) {
            std::string(string).swap(label);
            string = (::g_free(string), nullptr);
        }
    };

    auto update_label = [&]() -> void
    {
        _drive0.set_markup(label);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return update();
}

void InfoBar::set_drive1(const std::string& drive1)
{
    std::string label(_("{unknown}"));

    auto format_label = [&]() -> void
    {
        const char* format = "<span foreground='yellow' background='darkblue'> B: %s </span>";
        char*       string = ::g_markup_printf_escaped(format, drive1.c_str());
        if(string != nullptr) {
            std::string(string).swap(label);
            string = (::g_free(string), nullptr);
        }
    };

    auto update_label = [&]() -> void
    {
        _drive1.set_markup(label);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return update();
}

void InfoBar::set_system(const std::string& system)
{
    std::string label(_("{unknown}"));

    auto format_label = [&]() -> void
    {
        const char* format = "<span> %s </span>";
        char*       string = ::g_markup_printf_escaped(format, system.c_str());
        if(string != nullptr) {
            std::string(string).swap(label);
            string = (::g_free(string), nullptr);
        }
    };

    auto update_label = [&]() -> void
    {
        _system.set_markup(label);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return update();
}

void InfoBar::set_volume(const std::string& volume)
{
    std::string label(_("{unknown}"));

    auto format_label = [&]() -> void
    {
        const char* format = "%s";
        char*       string = ::g_markup_printf_escaped(format, volume.c_str());
        if(string != nullptr) {
            std::string(string).swap(label);
            string = (::g_free(string), nullptr);
        }
    };

    auto update_label = [&]() -> void
    {
        _volume.set_markup(label);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return update();
}

void InfoBar::set_stats(const std::string& stats)
{
    std::string label(_("{unknown}"));

    auto format_label = [&]() -> void
    {
        const char* format = "%s";
        char*       string = ::g_markup_printf_escaped(format, stats.c_str());
        if(string != nullptr) {
            std::string(string).swap(label);
            string = (::g_free(string), nullptr);
        }
    };

    auto update_label = [&]() -> void
    {
        _stats.set_markup(label);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return update();
}

}

// ---------------------------------------------------------------------------
// impl::WorkWnd
// ---------------------------------------------------------------------------

namespace impl {

WorkWnd::WorkWnd(Application& application)
    : AppWidget(application)
    , gtk3::HBox(nullptr)
    , _self(*this)
    , _emulator(nullptr)
    , _canvas(application)
{
}

void WorkWnd::build()
{
    auto build_self = [&]() -> void
    {
        _self.create_hbox();
    };

    auto build_emulator = [&]() -> void
    {
        static gchar target[] = "text/uri-list";
        static const GtkTargetEntry target_entries[] = {
            { target, 0, 1 },
        };
        _emulator.create_emulator();
        _emulator.set_backend(_application.get_backend());
        _emulator.set_joystick(0, Utils::get_joystick0());
        _emulator.set_joystick(1, Utils::get_joystick0());
        _emulator.drag_dest_set(GTK_DEST_DEFAULT_ALL, target_entries, 1, GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK));
        _emulator.add_hotkey_callback(G_CALLBACK(&Callbacks::on_hotkey), &_application);
        _emulator.add_drag_data_received_callback(G_CALLBACK(&Callbacks::on_drag_data_received), &_application);
        _self.pack_start(_emulator, true, true, 0);
    };

    auto build_canvas = [&]() -> void
    {
        _canvas.build();
        _self.pack_start(_canvas, true, true, 0);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_emulator();
        build_canvas();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::AppWindow
// ---------------------------------------------------------------------------

namespace impl {

AppWindow::AppWindow(Application& application)
    : AppWidget(application)
    , gtk3::ApplicationWindow(nullptr)
    , _layout(nullptr)
    , _menu_bar(_application)
    , _tool_bar(_application)
    , _work_wnd(_application)
    , _info_bar(_application)
{
}

void AppWindow::build()
{
    auto& _app_context(_application.app_context());
    auto& _app_title(_application.app_title());
    auto& _app_icon(_application.app_icon());
    auto& _window(*this);

    auto build_window = [&]() -> void
    {
        _window.create_application_window(_app_context);
        _window.set_title(_app_title);
        _window.set_icon(_app_icon);
    };

    auto build_layout = [&]() -> void
    {
        _layout.create_vbox();
        _window.add(_layout);
    };

    auto build_menu_bar = [&]() -> void
    {
        _menu_bar.build();
        _layout.pack_start(_menu_bar, false, true, 0);
    };

    auto build_tool_bar = [&]() -> void
    {
        _tool_bar.build();
        _layout.pack_start(_tool_bar, false, true, 0);
    };

    auto build_work_wnd = [&]() -> void
    {
        _work_wnd.build();
        _layout.pack_start(_work_wnd, true, true, 0);
    };

    auto build_info_bar = [&]() -> void
    {
        _info_bar.build();
        _layout.pack_start(_info_bar, false, true, 4);
    };

    auto show_all = [&]() -> void
    {
        _window.show_all();
    };

    auto play = [&]() -> void
    {
        _application.play_emulator();
        _work_wnd.emulator().grab_focus();
    };

    auto build_all = [&]() -> void
    {
        build_window();
        build_layout();
        build_menu_bar();
        build_tool_bar();
        build_work_wnd();
        build_info_bar();
        show_all();
        play();
    };

    return build_all();
}

void AppWindow::show_play()
{
    _menu_bar.show_play();
    _tool_bar.show_play();
}

void AppWindow::hide_play()
{
    _menu_bar.hide_play();
    _tool_bar.hide_play();
}

void AppWindow::show_pause()
{
    _menu_bar.show_pause();
    _tool_bar.show_pause();
}

void AppWindow::hide_pause()
{
    _menu_bar.hide_pause();
    _tool_bar.hide_pause();
}

void AppWindow::show_reset()
{
    _menu_bar.show_reset();
    _tool_bar.show_reset();
}

void AppWindow::hide_reset()
{
    _menu_bar.hide_reset();
    _tool_bar.hide_reset();
}

}

// ---------------------------------------------------------------------------
// xcpc::Application
// ---------------------------------------------------------------------------

namespace xcpc {

Application::Application(int& argc, char**& argv)
    : base::Application(argc, argv)
    , gtk3::Application(nullptr)
    , _app_title(_("Xcpc - Amstrad CPC emulator"))
    , _app_state(_("Unknown"))
    , _app_icon(nullptr)
    , _app_window(*this)
    , _timer(0)
{
    Environ::initialize();
}

Application::~Application()
{
    stop_timer();
}

int Application::main()
{
    if(_settings->quit() == false) {
        create_application("org.gtk.xcpc");
    }
    return run(_argc, _argv);
}

auto Application::load_snapshot(const std::string& filename) -> void
{
    try {
        _machine->load_snapshot(filename);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("load-snapshot has failed (%s)", e.what());
    }
    update_all();
}

auto Application::save_snapshot(const std::string& filename) -> void
{
    try {
        _machine->save_snapshot(filename);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("save-snapshot has failed (%s)", e.what());
    }
    update_all();
}

auto Application::exit() -> void
{
    try {
        _app_window.destroy();
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("exit-emulator has failed (%s)", e.what());
    }
}

auto Application::play_emulator() -> void
{
    try {
        _machine->play();
        set_state(_("Playing"));
        show_pause();
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("play-emulator has failed (%s)", e.what());
    }
    update_all();
}

auto Application::pause_emulator() -> void
{
    try {
        _machine->pause();
        set_state(_("Paused"));
        show_play();
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("pause-emulator has failed (%s)", e.what());
    }
    update_all();
}

auto Application::reset_emulator() -> void
{
    try {
        pause_emulator();
        _machine->reset();
        play_emulator();
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("reset-emulator has failed (%s)", e.what());
    }
    update_all();
}

auto Application::create_disk_into_drive0(const std::string& filename) -> void
{
    try {
        _machine->create_disk_into_drive0(filename);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("create-disk-into-drive0 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::insert_disk_into_drive0(const std::string& filename) -> void
{
    try {
        _machine->insert_disk_into_drive0(filename);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("insert-disk-into-drive0 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::remove_disk_from_drive0() -> void
{
    try {
        _machine->remove_disk_from_drive0();
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("remove-disk-from-drive0 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::create_disk_into_drive1(const std::string& filename) -> void
{
    try {
        _machine->create_disk_into_drive1(filename);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("create-disk-into-drive1 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::insert_disk_into_drive1(const std::string& filename) -> void
{
    try {
        _machine->insert_disk_into_drive1(filename);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("insert-disk-into-drive1 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::remove_disk_from_drive1() -> void
{
    try {
        _machine->remove_disk_from_drive1();
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("remove-disk-from-drive1 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_volume(const float volume) -> void
{
    try {
        _machine->set_volume(volume);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("increase-volume has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_scanlines(const bool scanlines) -> void
{
    try {
        _machine->set_scanlines(scanlines);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-scanlines has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_machine_type(const std::string& machine_type) -> void
{
    try {
        _machine->set_machine_type(machine_type);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-machine-type has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_company_name(const std::string& company_name) -> void
{
    try {
        _machine->set_company_name(company_name);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-company-name has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_monitor_type(const std::string& monitor_type) -> void
{
    try {
        _machine->set_monitor_type(monitor_type);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-monitor-type has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_refresh_rate(const std::string& refresh_rate) -> void
{
    try {
        _machine->set_refresh_rate(refresh_rate);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-refresh-rate has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_keyboard_type(const std::string& keyboard_type) -> void
{
    try {
        _machine->set_keyboard_type(keyboard_type);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-keyboard-type has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_joystick0(const std::string& device) -> void
{
    try {
        work_wnd().emulator().set_joystick(0, device);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-joystick0 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::set_joystick1(const std::string& device) -> void
{
    try {
        work_wnd().emulator().set_joystick(1, device);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-joystick1 has failed (%s)", e.what());
    }
    update_all();
}

auto Application::on_open(GFile** files, int num_files) -> void
{
    const int count = num_files;
    for(int index = 0; index < count; ++index) {
        char* path = ::g_file_get_path(files[index]);
        if(path != nullptr) {
            Callbacks::open_file(*this, path);
            path = (::g_free(path), nullptr);
        }
    }
}

auto Application::on_startup() -> void
{
    auto create_app_icon = [&](const std::string& datadir, const std::string& directory, const std::string& filename) -> void
    {
        _app_icon.create_from_file(datadir + '/' + directory + '/' + filename);
    };

    auto create_main_window = [&]() -> void
    {
        _app_window.build();
    };

    auto do_startup = [&]() -> void
    {
        create_app_icon(Utils::get_datdir(), "pixmaps", "xcpc.png");
        create_main_window();
        start_timer();
    };

    return do_startup();
}

auto Application::on_shutdown() -> void
{
    auto destroy_app_icon = [&]() -> void
    {
        _app_icon.unref();
    };

    auto destroy_main_window = [&]() -> void
    {
        _app_window.destroy();
    };

    auto do_shutdown = [&]() -> void
    {
        destroy_main_window();
        destroy_app_icon();
    };

    return do_shutdown();
}

auto Application::on_statistics() -> void
{
    update_stats();
}

auto Application::on_snapshot_load() -> void
{
    LoadSnapshotDialog dialog(*this);

    run_dialog(dialog);
}

auto Application::on_snapshot_save() -> void
{
    SaveSnapshotDialog dialog(*this);

    run_dialog(dialog);
}

auto Application::on_exit() -> void
{
    exit();
}

auto Application::on_emulator_play() -> void
{
    play_emulator();
}

auto Application::on_emulator_pause() -> void
{
    pause_emulator();
}

auto Application::on_emulator_reset() -> void
{
    reset_emulator();
}

auto Application::on_machine_cpc464() -> void
{
    set_machine_type("cpc464");
}

auto Application::on_machine_cpc664() -> void
{
    set_machine_type("cpc664");
}

auto Application::on_machine_cpc6128() -> void
{
    set_machine_type("cpc6128");
}

auto Application::on_company_isp() -> void
{
    set_company_name("isp");
}

auto Application::on_company_triumph() -> void
{
    set_company_name("triumph");
}

auto Application::on_company_saisho() -> void
{
    set_company_name("saisho");
}

auto Application::on_company_solavox() -> void
{
    set_company_name("solavox");
}

auto Application::on_company_awa() -> void
{
    set_company_name("awa");
}

auto Application::on_company_schneider() -> void
{
    set_company_name("schneider");
}

auto Application::on_company_orion() -> void
{
    set_company_name("orion");
}

auto Application::on_company_amstrad() -> void
{
    set_company_name("amstrad");
}

auto Application::on_monitor_color() -> void
{
    set_monitor_type("color");
}

auto Application::on_monitor_green() -> void
{
    set_monitor_type("green");
}

auto Application::on_monitor_gray() -> void
{
    set_monitor_type("gray");
}

auto Application::on_refresh_50hz() -> void
{
    set_refresh_rate("50hz");
}

auto Application::on_refresh_60hz() -> void
{
    set_refresh_rate("60hz");
}

auto Application::on_keyboard_english() -> void
{
    set_keyboard_type("english");
}

auto Application::on_keyboard_french() -> void
{
    set_keyboard_type("french");
}

auto Application::on_keyboard_german() -> void
{
    set_keyboard_type("german");
}

auto Application::on_keyboard_spanish() -> void
{
    set_keyboard_type("spanish");
}

auto Application::on_keyboard_danish() -> void
{
    set_keyboard_type("danish");
}

auto Application::on_drive0_disk_create() -> void
{
    CreateDiskDialog dialog(*this, CreateDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

auto Application::on_drive0_disk_insert() -> void
{
    InsertDiskDialog dialog(*this, InsertDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

auto Application::on_drive0_disk_remove() -> void
{
    RemoveDiskDialog dialog(*this, RemoveDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

auto Application::on_drive1_disk_create() -> void
{
    CreateDiskDialog dialog(*this, CreateDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

auto Application::on_drive1_disk_insert() -> void
{
    InsertDiskDialog dialog(*this, InsertDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

auto Application::on_drive1_disk_remove() -> void
{
    RemoveDiskDialog dialog(*this, RemoveDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

auto Application::on_volume_increase() -> void
{
    constexpr float increment = 0.05f;
    const     float volume    = _machine->get_volume() + increment;

    set_volume(volume);
}

auto Application::on_volume_decrease() -> void
{
    constexpr float increment = 0.05f;
    const     float volume    = _machine->get_volume() - increment;

    set_volume(volume);
}

auto Application::on_scanlines_enable() -> void
{
    set_scanlines(true);
}

auto Application::on_scanlines_disable() -> void
{
    set_scanlines(false);
}

auto Application::on_joystick0_connect() -> void
{
    set_joystick0(Utils::get_joystick0());
}

auto Application::on_joystick0_disconnect() -> void
{
    set_joystick0("");
}

auto Application::on_joystick1_connect() -> void
{
    set_joystick1(Utils::get_joystick1());
}

auto Application::on_joystick1_disconnect() -> void
{
    set_joystick1("");
}

auto Application::on_help() -> void
{
    HelpDialog dialog(*this);

    return run_dialog(dialog);
}

auto Application::on_about() -> void
{
    AboutDialog dialog(*this);

    return run_dialog(dialog);
}

auto Application::start_timer() -> void
{
    static constexpr guint interval = 1511;

    if(_timer != 0) {
        _timer = (static_cast<void>(::g_source_remove(_timer)), 0);
    }
    if(_timer == 0) {
        _timer = ::g_timeout_add(interval, G_SOURCE_FUNC(&Callbacks::on_statistics), this);
    }
}

auto Application::stop_timer() -> void
{
    if(_timer != 0) {
        _timer = (static_cast<void>(::g_source_remove(_timer)), 0);
    }
}

auto Application::show_play() -> void
{
    _app_window.show_play();
    _app_window.hide_pause();
}

auto Application::hide_play() -> void
{
    _app_window.show_pause();
    _app_window.hide_play();
}

auto Application::show_pause() -> void
{
    _app_window.show_pause();
    _app_window.hide_play();
}

auto Application::hide_pause() -> void
{
    _app_window.show_play();
    _app_window.hide_pause();
}

auto Application::show_reset() -> void
{
    _app_window.show_reset();
}

auto Application::hide_reset() -> void
{
    _app_window.hide_reset();
}

auto Application::set_state(const std::string& state) -> void
{
    _app_state = state;
    update_state();
}

auto Application::update_title() -> void
{
    std::string title;

    auto format_title = [&]() -> void
    {
        title += _app_title;
        title += ' ';
        title += '-';
        title += ' ';
        title += _app_state;
    };

    auto update_title = [&]() -> void
    {
        _app_window.set_title(title);
    };

    auto do_update = [&]() -> void
    {
        format_title();
        update_title();
    };

    return do_update();
}

auto Application::update_state() -> void
{
    std::string label(_app_state);

    auto format_label = [&]() -> void
    {
        if(label.empty()) {
            std::string(_("{unknown}")).swap(label);
        }
    };

    auto update_label = [&]() -> void
    {
        info_bar().set_state(label);
    };

    auto do_update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return do_update();
}

auto Application::update_drive0() -> void
{
    std::string label(_machine->get_drive0_filename());

    auto format_label = [&]() -> void
    {
        const char* c_str = label.c_str();
        const char* slash = ::strrchr(c_str, '/');
        if(slash != nullptr) {
            std::string(slash + 1).swap(label);
        }
        if(label.empty()) {
            std::string(_("{empty}")).swap(label);
        }
    };

    auto update_label = [&]() -> void
    {
        info_bar().set_drive0(label);
    };

    auto do_update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return do_update();
}

auto Application::update_drive1() -> void
{
    std::string label(_machine->get_drive1_filename());

    auto format_label = [&]() -> void
    {
        const char* c_str = label.c_str();
        const char* slash = ::strrchr(c_str, '/');
        if(slash != nullptr) {
            std::string(slash + 1).swap(label);
        }
        if(label.empty()) {
            std::string(_("{empty}")).swap(label);
        }
    };

    auto update_label = [&]() -> void
    {
        info_bar().set_drive1(label);
    };

    auto do_update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return do_update();
}

auto Application::update_system() -> void
{
    auto do_update = [&]() -> void
    {
        info_bar().set_system(_machine->get_system_info());
    };

    return do_update();
}

auto Application::update_volume() -> void
{
    std::string label(_("Vol:"));

    auto format_label = [&]() -> void
    {
        label += ' ';
        label += std::to_string(static_cast<int>((_machine->get_volume() + 0.005f) * 100.0f));
        label += '%';
    };

    auto update_label = [&]() -> void
    {
        info_bar().set_volume(label);
    };

    auto do_update = [&]() -> void
    {
        format_label();
        update_label();
    };

    return do_update();
}

auto Application::update_stats() -> void
{
    auto do_update = [&]() -> void
    {
        info_bar().set_stats(_machine->get_statistics());
    };

    return do_update();
}

auto Application::update_all() -> void
{
    update_title();
    update_state();
    update_drive0();
    update_drive1();
    update_system();
    update_volume();
    update_stats();
}

}

// ---------------------------------------------------------------------------
// xcpc_main
// ---------------------------------------------------------------------------

int xcpc_main(int* argc, char*** argv)
{
    const auto application(std::make_unique<xcpc::Application>(*argc, *argv));

    return application->main();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
