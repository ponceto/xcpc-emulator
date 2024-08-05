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
    static auto on_statistics(xcpc::Application* self) -> gboolean
    {
        self->on_statistics();
        return TRUE;
    }

    static auto on_ignore(GtkWidget* widget, xcpc::Application* self) -> void
    {
    }

    static auto on_snapshot_load(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_snapshot_load();
    }

    static auto on_snapshot_save(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_snapshot_save();
    }

    static auto on_exit(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_exit();
    }

    static auto on_emulator_play(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_emulator_play();
    }

    static auto on_emulator_pause(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_emulator_pause();
    }

    static auto on_emulator_reset(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_emulator_reset();
    }

    static auto on_company_isp(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_isp();
    }

    static auto on_company_triumph(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_triumph();
    }

    static auto on_company_saisho(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_saisho();
    }

    static auto on_company_solavox(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_solavox();
    }

    static auto on_company_awa(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_awa();
    }

    static auto on_company_schneider(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_schneider();
    }

    static auto on_company_orion(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_orion();
    }

    static auto on_company_amstrad(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_company_amstrad();
    }

    static auto on_monitor_color(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_monitor_color();
    }

    static auto on_monitor_green(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_monitor_green();
    }

    static auto on_monitor_gray(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_monitor_gray();
    }

    static auto on_refresh_50hz(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_refresh_50hz();
    }

    static auto on_refresh_60hz(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_refresh_60hz();
    }

    static auto on_keyboard_english(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_keyboard_english();
    }

    static auto on_keyboard_french(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_keyboard_french();
    }

    static auto on_keyboard_german(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_keyboard_german();
    }

    static auto on_keyboard_spanish(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_keyboard_spanish();
    }

    static auto on_keyboard_danish(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_keyboard_danish();
    }

    static auto on_drive0_create_disk(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_drive0_create_disk();
    }

    static auto on_drive0_insert_disk(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_drive0_insert_disk();
    }

    static auto on_drive0_remove_disk(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_drive0_remove_disk();
    }

    static auto on_drive1_create_disk(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_drive1_create_disk();
    }

    static auto on_drive1_insert_disk(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_drive1_insert_disk();
    }

    static auto on_drive1_remove_disk(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_drive1_remove_disk();
    }

    static auto on_volume_increase(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_volume_increase();
    }

    static auto on_volume_decrease(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_volume_decrease();
    }

    static auto on_scanlines_enable(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_scanlines_enable();
    }

    static auto on_scanlines_disable(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_scanlines_disable();
    }

    static auto on_joystick0(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_joystick0();
    }

    static auto on_joystick1(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_joystick1();
    }

    static auto on_help(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_help();
    }

    static auto on_about(GtkWidget* widget, xcpc::Application* self) -> void
    {
        self->on_about();
    }

    static auto on_hotkey(GtkWidget* widget, KeySym* keysym, xcpc::Application* self) -> void
    {
        if(keysym != nullptr) {
            switch(*keysym) {
                case XK_Pause:
                    on_emulator_pause(widget, self);
                    break;
                case XK_F1:
                    on_help(widget, self);
                    break;
                case XK_F2:
                    on_snapshot_load(widget, self);
                    break;
                case XK_F3:
                    on_snapshot_save(widget, self);
                    break;
                case XK_F4:
                    on_ignore(widget, self);
                    break;
                case XK_F5:
                    on_emulator_reset(widget, self);
                    break;
                case XK_F6:
                    on_drive0_insert_disk(widget, self);
                    break;
                case XK_F7:
                    on_drive0_remove_disk(widget, self);
                    break;
                case XK_F8:
                    on_drive1_insert_disk(widget, self);
                    break;
                case XK_F9:
                    on_drive1_remove_disk(widget, self);
                    break;
                case XK_F10:
                    on_ignore(widget, self);
                    break;
                case XK_F11:
                    on_ignore(widget, self);
                    break;
                case XK_F12:
                    on_ignore(widget, self);
                    break;
                default:
                    break;
            }
        }
    }

    static auto on_gl_render(GtkWidget* widget, GdkGLContext* context, xcpc::Application* self) -> void
    {
        ::xcpc_log_debug("on_gl_render()");
    }

    static auto on_gl_resize(GtkWidget* widget, gint width, gint height, xcpc::Application* self) -> void
    {
        ::xcpc_log_debug("on_gl_resize(%d, %d)", width, height);
    }

    static bool check_extension(const char* filename, const char* extension)
    {
        const int filename_length  = ::strlen(filename);
        const int extension_length = ::strlen(extension);

        if(filename_length >= extension_length) {
            if(::strcasecmp(&filename[filename_length - extension_length], extension) == 0) {
                return true;
            }
        }
        return false;
    }

    static auto on_drag_data_received ( GtkWidget*         widget
                                      , GdkDragContext*    context
                                      , int                x
                                      , int                y
                                      , GtkSelectionData*  data
                                      , guint              info
                                      , guint              time
                                      , xcpc::Application* self ) -> void
    {
        const char* prefix_str = "file://";
        const int   prefix_len = ::strlen(prefix_str);
        gchar**     uri_list   = ::gtk_selection_data_get_uris(data);
        gchar*      filename   = nullptr;

        if(uri_list != nullptr) {
            filename = uri_list[0];
        }
        if(filename != nullptr) {
            if(::strncmp(filename, prefix_str, prefix_len) == 0) {
                filename += prefix_len;
            }
            if(check_extension(filename, ".sna") != false) {
                self->load_snapshot(filename);
                self->play_emulator();
            }
            else if(check_extension(filename, ".dsk") != false) {
                self->insert_disk_into_drive0(filename);
                self->play_emulator();
            }
            else if(check_extension(filename, ".dsk.gz") != false) {
                self->insert_disk_into_drive0(filename);
                self->play_emulator();
            }
            else if(check_extension(filename, ".dsk.bz2") != false) {
                self->insert_disk_into_drive0(filename);
                self->play_emulator();
            }
        }
        if(uri_list != nullptr) {
            uri_list = (::g_strfreev(uri_list), nullptr);
        }
    }
};

}

// ---------------------------------------------------------------------------
// impl::AppWidget
// ---------------------------------------------------------------------------

namespace impl {

AppWidget::AppWidget(xcpc::Application& application)
    : _application(application)
{
}

}

// ---------------------------------------------------------------------------
// impl::FileMenu
// ---------------------------------------------------------------------------

namespace impl {

FileMenu::FileMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _load_snapshot(nullptr)
    , _save_snapshot(nullptr)
    , _separator(nullptr)
    , _exit(nullptr)
{
}

void FileMenu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("File"));
        _application.menu_bar().append(_self);
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_load_snapshot = [&]() -> void
    {
        _load_snapshot.create_menu_item_with_label(_("Load snapshot..."));
        _load_snapshot.set_accel(GDK_KEY_F2, GdkModifierType(0));
        _load_snapshot.add_activate_callback(G_CALLBACK(&Callbacks::on_snapshot_load), &_application);
        _menu.append(_load_snapshot);
    };

    auto build_save_snapshot = [&]() -> void
    {
        _save_snapshot.create_menu_item_with_label(_("Save snapshot..."));
        _save_snapshot.set_accel(GDK_KEY_F3, GdkModifierType(0));
        _save_snapshot.add_activate_callback(G_CALLBACK(&Callbacks::on_snapshot_save), &_application);
        _menu.append(_save_snapshot);
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
        build_load_snapshot();
        build_save_snapshot();
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

ControlsMenu::ControlsMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _play_emulator(nullptr)
    , _pause_emulator(nullptr)
    , _separator(nullptr)
    , _reset_emulator(nullptr)
{
}

void ControlsMenu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Controls"));
        _application.menu_bar().append(_self);
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_play = [&]() -> void
    {
        _play_emulator.create_menu_item_with_label(_("Play"));
        _play_emulator.add_activate_callback(G_CALLBACK(&Callbacks::on_emulator_play), &_application);
        _menu.append(_play_emulator);
    };

    auto build_pause = [&]() -> void
    {
        _pause_emulator.create_menu_item_with_label(_("Pause"));
        _pause_emulator.add_activate_callback(G_CALLBACK(&Callbacks::on_emulator_pause), &_application);
        _menu.append(_pause_emulator);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_reset = [&]() -> void
    {
        _reset_emulator.create_menu_item_with_label(_("Reset"));
        _reset_emulator.set_accel(GDK_KEY_F5, GdkModifierType(0));
        _reset_emulator.add_activate_callback(G_CALLBACK(&Callbacks::on_emulator_reset), &_application);
        _menu.append(_reset_emulator);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_play();
        build_pause();
        build_separator();
        build_reset();
    };

    return build_all();
}

void ControlsMenu::show_play()
{
    _play_emulator.show();
}

void ControlsMenu::hide_play()
{
    _play_emulator.hide();
}

void ControlsMenu::show_pause()
{
    _pause_emulator.show();
}

void ControlsMenu::hide_pause()
{
    _pause_emulator.hide();
}

void ControlsMenu::show_reset()
{
    _reset_emulator.show();
}

void ControlsMenu::hide_reset()
{
    _reset_emulator.hide();
}

}

// ---------------------------------------------------------------------------
// impl::MachineMenu
// ---------------------------------------------------------------------------

namespace impl {

MachineMenu::MachineMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
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
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Machine"));
        _application.menu_bar().append(_self);
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
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

Drive0Menu::Drive0Menu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _create_disk(nullptr)
    , _separator(nullptr)
    , _insert_disk(nullptr)
    , _remove_disk(nullptr)
{
}

void Drive0Menu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Drive A"));
        _application.menu_bar().append(_self);
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_create_disk = [&]() -> void
    {
        _create_disk.create_menu_item_with_label(_("Create disk..."));
        _create_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_drive0_create_disk), &_application);
        _menu.append(_create_disk);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_insert_disk = [&]() -> void
    {
        _insert_disk.create_menu_item_with_label(_("Insert disk..."));
        _insert_disk.set_accel(GDK_KEY_F6, GdkModifierType(0));
        _insert_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_drive0_insert_disk), &_application);
        _menu.append(_insert_disk);
    };

    auto build_remove_disk = [&]() -> void
    {
        _remove_disk.create_menu_item_with_label(_("Remove disk..."));
        _remove_disk.set_accel(GDK_KEY_F7, GdkModifierType(0));
        _remove_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_drive0_remove_disk), &_application);
        _menu.append(_remove_disk);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_create_disk();
        build_separator();
        build_insert_disk();
        build_remove_disk();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::Drive1Menu
// ---------------------------------------------------------------------------

namespace impl {

Drive1Menu::Drive1Menu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _create_disk(nullptr)
    , _separator(nullptr)
    , _insert_disk(nullptr)
    , _remove_disk(nullptr)
{
}

void Drive1Menu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Drive B"));
        _application.menu_bar().append(_self);
    };

    auto build_menu = [&]() -> void
    {
        _menu.create_menu();
        _self.set_submenu(_menu);
    };

    auto build_create_disk = [&]() -> void
    {
        _create_disk.create_menu_item_with_label(_("Create disk..."));
        _create_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_drive1_create_disk), &_application);
        _menu.append(_create_disk);
    };

    auto build_separator = [&]() -> void
    {
        _separator.create_separator_menu_item();
        _menu.append(_separator);
    };

    auto build_insert_disk = [&]() -> void
    {
        _insert_disk.create_menu_item_with_label(_("Insert disk..."));
        _insert_disk.set_accel(GDK_KEY_F8, GdkModifierType(0));
        _insert_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_drive1_insert_disk), &_application);
        _menu.append(_insert_disk);
    };

    auto build_remove_disk = [&]() -> void
    {
        _remove_disk.create_menu_item_with_label(_("Remove disk..."));
        _remove_disk.set_accel(GDK_KEY_F9, GdkModifierType(0));
        _remove_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_drive1_remove_disk), &_application);
        _menu.append(_remove_disk);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_create_disk();
        build_separator();
        build_insert_disk();
        build_remove_disk();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::AudioMenu
// ---------------------------------------------------------------------------

namespace impl {

AudioMenu::AudioMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _volume_increase(nullptr)
    , _volume_decrease(nullptr)
{
}

void AudioMenu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Audio"));
        _application.menu_bar().append(_self);
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

VideoMenu::VideoMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _scanlines_enable(nullptr)
    , _scanlines_disable(nullptr)
{
}

void VideoMenu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Video"));
        _application.menu_bar().append(_self);
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

InputMenu::InputMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _joystick0(nullptr)
    , _joystick1(nullptr)
{
}

void InputMenu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Input"));
        _application.menu_bar().append(_self);
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
        label += xcpc::Utils::get_joystick0();
        label += ')';
        _joystick0.create_menu_item_with_label(label);
        _joystick0.add_activate_callback(G_CALLBACK(&Callbacks::on_joystick0), &_application);
        _menu.append(_joystick0);
    };

    auto build_joystick1 = [&]() -> void
    {
        std::string label(_("Joystick 1"));
        label += ' ';
        label += '(';
        label += xcpc::Utils::get_joystick1();
        label += ')';
        _joystick1.create_menu_item_with_label(label);
        _joystick1.add_activate_callback(G_CALLBACK(&Callbacks::on_joystick1), &_application);
        _menu.append(_joystick1);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_joystick0();
        build_joystick1();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::HelpMenu
// ---------------------------------------------------------------------------

namespace impl {

HelpMenu::HelpMenu(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuItem(nullptr)
    , _menu(nullptr)
    , _help(nullptr)
    , _separator(nullptr)
    , _about(nullptr)
{
}

void HelpMenu::build()
{
    gtk3::MenuItem& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_item_with_label(_("Help"));
        _application.menu_bar().append(_self);
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

MenuBar::MenuBar(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::MenuBar(nullptr)
    , _file_menu(application)
    , _controls_menu(application)
    , _machine_menu(application)
    , _drive0_menu(application)
    , _drive1_menu(application)
    , _audio_menu(application)
    , _video_menu(application)
    , _input_menu(application)
    , _help_menu(application)
{
}

void MenuBar::build()
{
    auto& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_menu_bar();
    };

    auto build_file_menu = [&]() -> void
    {
        _file_menu.build();
    };

    auto build_controls_menu = [&]() -> void
    {
        _controls_menu.build();
    };

    auto build_machine_menu = [&]() -> void
    {
        _machine_menu.build();
    };

    auto build_drive0_menu = [&]() -> void
    {
        _drive0_menu.build();
    };

    auto build_drive1_menu = [&]() -> void
    {
        _drive1_menu.build();
    };

    auto build_audio_menu = [&]() -> void
    {
        _audio_menu.build();
    };

    auto build_video_menu = [&]() -> void
    {
        _video_menu.build();
    };

    auto build_input_menu = [&]() -> void
    {
        _input_menu.build();
    };

    auto build_help_menu = [&]() -> void
    {
        _help_menu.build();
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

ToolBar::ToolBar(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::Toolbar(nullptr)
    , _load_snapshot(nullptr)
    , _save_snapshot(nullptr)
    , _separator1(nullptr)
    , _play_emulator(nullptr)
    , _pause_emulator(nullptr)
    , _reset_emulator(nullptr)
    , _separator2(nullptr)
    , _volume_decrease(nullptr)
    , _volume_increase(nullptr)
{
}

void ToolBar::build()
{
    auto& _self(*this);

    auto build_toolbar = [&]() -> void
    {
        _self.create_toolbar();
    };

    auto build_load_snapshot = [&]() -> void
    {
        _load_snapshot.create_tool_button();
        _load_snapshot.set_icon_name(IconTraits::ico_load_snapshot);
        _load_snapshot.add_clicked_callback(G_CALLBACK(&Callbacks::on_snapshot_load), &_application);
        _self.insert(_load_snapshot, -1);
    };

    auto build_save_snapshot = [&]() -> void
    {
        _save_snapshot.create_tool_button();
        _save_snapshot.set_icon_name(IconTraits::ico_save_snapshot);
        _save_snapshot.add_clicked_callback(G_CALLBACK(&Callbacks::on_snapshot_save), &_application);
        _self.insert(_save_snapshot, -1);
    };

    auto build_separator1 = [&]() -> void
    {
        _separator1.create_separator_tool_item();
        _self.insert(_separator1, -1);
    };

    auto build_play_emulator = [&]() -> void
    {
        _play_emulator.create_tool_button();
        _play_emulator.set_icon_name(IconTraits::ico_play_emulator);
        _play_emulator.add_clicked_callback(G_CALLBACK(&Callbacks::on_emulator_play), &_application);
        _self.insert(_play_emulator, -1);
    };

    auto build_pause_emulator = [&]() -> void
    {
        _pause_emulator.create_tool_button();
        _pause_emulator.set_icon_name(IconTraits::ico_pause_emulator);
        _pause_emulator.add_clicked_callback(G_CALLBACK(&Callbacks::on_emulator_pause), &_application);
        _self.insert(_pause_emulator, -1);
    };

    auto build_reset_emulator = [&]() -> void
    {
        _reset_emulator.create_tool_button();
        _reset_emulator.set_icon_name(IconTraits::ico_reset_emulator);
        _reset_emulator.add_clicked_callback(G_CALLBACK(&Callbacks::on_emulator_reset), &_application);
        _self.insert(_reset_emulator, -1);
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
        build_toolbar();
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
    _play_emulator.show();
}

void ToolBar::hide_play()
{
    _play_emulator.hide();
}

void ToolBar::show_pause()
{
    _pause_emulator.show();
}

void ToolBar::hide_pause()
{
    _pause_emulator.hide();
}

void ToolBar::show_reset()
{
    _reset_emulator.show();
}

void ToolBar::hide_reset()
{
    _reset_emulator.hide();
}

}

// ---------------------------------------------------------------------------
// impl::InfoBar
// ---------------------------------------------------------------------------

namespace impl {

InfoBar::InfoBar(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::HBox(nullptr)
    , _status(nullptr)
    , _drive0(nullptr)
    , _drive1(nullptr)
    , _system(nullptr)
    , _volume(nullptr)
    , _fps(nullptr)
{
}

void InfoBar::build()
{
    auto& _self(*this);

    auto build_self = [&]() -> void
    {
        _self.create_hbox();
    };

    auto build_status = [&]() -> void
    {
        _status.create_label(_("Status"));
        _self.pack_start(_status, false, true, 2);
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
        _volume.create_label(_("---"));
        _self.pack_start(_volume, false, true, 2);
    };

    auto build_fps = [&]() -> void
    {
        _fps.create_label(_("---"));
        _self.pack_end(_fps, false, true, 2);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_status();
        build_drive0();
        build_drive1();
        build_system();
        build_volume();
        build_fps();
    };

    return build_all();
}

void InfoBar::update_status(const std::string& status)
{
    const char* format = "<span foreground='grey90' background='darkgreen'> %s </span>";
    char*       string = nullptr;

    auto format_label = [&]() -> void
    {
        string = ::g_markup_printf_escaped(format, status.c_str());
    };

    auto update_label = [&]() -> void
    {
        _status.set_markup(string);
    };

    auto delete_label = [&]() -> void
    {
        string = (::g_free(string), nullptr);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
        delete_label();
    };

    return update();
}

void InfoBar::update_drive0(const std::string& filename)
{
    const char* format = "<span foreground='yellow' background='darkblue'> A: %s </span>";
    char*       string = nullptr;

    auto format_label = [&]() -> void
    {
        string = ::g_markup_printf_escaped(format, filename.c_str());
    };

    auto update_label = [&]() -> void
    {
        _drive0.set_markup(string);
    };

    auto delete_label = [&]() -> void
    {
        string = (::g_free(string), nullptr);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
        delete_label();
    };

    return update();
}

void InfoBar::update_drive1(const std::string& filename)
{
    const char* format = "<span foreground='yellow' background='darkblue'> B: %s </span>";
    char*       string = nullptr;

    auto format_label = [&]() -> void
    {
        string = ::g_markup_printf_escaped(format, filename.c_str());
    };

    auto update_label = [&]() -> void
    {
        _drive1.set_markup(string);
    };

    auto delete_label = [&]() -> void
    {
        string = (::g_free(string), nullptr);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
        delete_label();
    };

    return update();
}

void InfoBar::update_system(const std::string& system)
{
    const char* format = "<span> %s </span>";
    char*       string = nullptr;

    auto format_label = [&]() -> void
    {
        string = ::g_markup_printf_escaped(format, system.c_str());
    };

    auto update_label = [&]() -> void
    {
        _system.set_markup(string);
    };

    auto delete_label = [&]() -> void
    {
        string = (::g_free(string), nullptr);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
        delete_label();
    };

    return update();
}

void InfoBar::update_volume(const std::string& volume)
{
    const char* format = "%s";
    char*       string = nullptr;

    auto format_label = [&]() -> void
    {
        string = ::g_markup_printf_escaped(format, volume.c_str());
    };

    auto update_label = [&]() -> void
    {
        _volume.set_markup(string);
    };

    auto delete_label = [&]() -> void
    {
        string = (::g_free(string), nullptr);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
        delete_label();
    };

    return update();
}

void InfoBar::update_fps(const std::string& fps)
{
    const char* format = "%s";
    char*       string = nullptr;

    auto format_label = [&]() -> void
    {
        string = ::g_markup_printf_escaped(format, fps.c_str());
    };

    auto update_label = [&]() -> void
    {
        _fps.set_markup(string);
    };

    auto delete_label = [&]() -> void
    {
        string = (::g_free(string), nullptr);
    };

    auto update = [&]() -> void
    {
        format_label();
        update_label();
        delete_label();
    };

    return update();
}

}

// ---------------------------------------------------------------------------
// impl::WorkWnd
// ---------------------------------------------------------------------------

namespace impl {

WorkWnd::WorkWnd(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::HBox(nullptr)
    , _emulator(nullptr)
    , _gl_area(nullptr)
{
}

void WorkWnd::build()
{
    auto& _self(*this);

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
        _emulator.set_joystick(0, xcpc::Utils::get_joystick0());
        _emulator.set_joystick(1, xcpc::Utils::get_joystick0());
        _emulator.drag_dest_set(GTK_DEST_DEFAULT_ALL, target_entries, 1, GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK));
        _emulator.add_hotkey_callback(G_CALLBACK(&Callbacks::on_hotkey), &_application);
        _emulator.add_drag_data_received_callback(G_CALLBACK(&Callbacks::on_drag_data_received), &_application);
        _self.pack_start(_emulator, true, true, 0);
    };

    auto build_gl_area = [&]() -> void
    {
#if XCPC_ENABLE_GL_AREA
        _gl_area.create_gl_area();
        _gl_area.add_render_callback(G_CALLBACK(&Callbacks::on_gl_render), &_application);
        _gl_area.add_resize_callback(G_CALLBACK(&Callbacks::on_gl_resize), &_application);
        _self.pack_start(_gl_area, true, true, 0);
#endif
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_emulator();
        build_gl_area();
    };

    return build_all();
}

}

// ---------------------------------------------------------------------------
// impl::MainWindow
// ---------------------------------------------------------------------------

namespace impl {

MainWindow::MainWindow(xcpc::Application& application)
    : impl::AppWidget(application)
    , gtk3::ApplicationWindow(nullptr)
    , _layout(nullptr)
    , _menu_bar(application)
    , _tool_bar(application)
    , _work_wnd(application)
    , _info_bar(application)
{
}

void MainWindow::build()
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

void MainWindow::show_play()
{
    _menu_bar.show_play();
    _tool_bar.show_play();
}

void MainWindow::hide_play()
{
    _menu_bar.hide_play();
    _tool_bar.hide_play();
}

void MainWindow::show_pause()
{
    _menu_bar.show_pause();
    _tool_bar.show_pause();
}

void MainWindow::hide_pause()
{
    _menu_bar.hide_pause();
    _tool_bar.hide_pause();
}

void MainWindow::show_reset()
{
    _menu_bar.show_reset();
    _tool_bar.show_reset();
}

void MainWindow::hide_reset()
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
    , _main_window(*this)
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
        ::xcpc_log_debug("load-snapshot <%s>", filename.c_str());
        _machine->load_snapshot(filename);
        ::xcpc_log_debug("load-snapshot has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("load-snapshot has failed (%s)", e.what());
    }
}

auto Application::save_snapshot(const std::string& filename) -> void
{
    try {
        ::xcpc_log_debug("save-snapshot <%s>", filename.c_str());
        _machine->save_snapshot(filename);
        ::xcpc_log_debug("save-snapshot has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("save-snapshot has failed (%s)", e.what());
    }
}

auto Application::exit() -> void
{
    try {
        ::xcpc_log_debug("exit-emulator");
        _main_window.destroy();
        ::xcpc_log_debug("exit-emulator has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("exit-emulator has failed (%s)", e.what());
    }
}

auto Application::play_emulator() -> void
{
    try {
        ::xcpc_log_debug("play-emulator");
        show_pause();
        hide_play();
        _machine->play();
        set_state(_("Playing"));
        ::xcpc_log_debug("play-emulator has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("play-emulator has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::pause_emulator() -> void
{
    try {
        ::xcpc_log_debug("pause-emulator");
        show_play();
        hide_pause();
        _machine->pause();
        set_state(_("Paused"));
        ::xcpc_log_debug("pause-emulator has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("pause-emulator has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::reset_emulator() -> void
{
    auto play = [&]() -> void
    {
        play_emulator();
    };

    auto pause = [&]() -> void
    {
        pause_emulator();
    };

    auto reset = [&]() -> void
    {
        _machine->reset();
        set_state(_("Reset"));
    };

    try {
        ::xcpc_log_debug("reset-emulator");
        pause();
        reset();
        play();
        ::xcpc_log_debug("reset-emulator has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("reset-emulator has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::create_disk_into_drive0(const std::string& filename) -> void
{
    try {
        ::xcpc_log_debug("create-disk-into-drive0 <%s>", filename.c_str());
        _machine->create_disk_into_drive0(filename);
        ::xcpc_log_debug("create-disk-into-drive0 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("create-disk-into-drive0 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::insert_disk_into_drive0(const std::string& filename) -> void
{
    try {
        ::xcpc_log_debug("insert-disk-into-drive0 <%s>", filename.c_str());
        _machine->insert_disk_into_drive0(filename);
        ::xcpc_log_debug("insert-disk-into-drive0 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("insert-disk-into-drive0 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::remove_disk_from_drive0() -> void
{
    try {
        ::xcpc_log_debug("remove-disk-from-drive0");
        _machine->remove_disk_from_drive0();
        ::xcpc_log_debug("remove-disk-from-drive0 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("remove-disk-from-drive0 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::create_disk_into_drive1(const std::string& filename) -> void
{
    try {
        ::xcpc_log_debug("create-disk-into-drive1 <%s>", filename.c_str());
        _machine->create_disk_into_drive1(filename);
        ::xcpc_log_debug("create-disk-into-drive1 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("create-disk-into-drive1 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::insert_disk_into_drive1(const std::string& filename) -> void
{
    try {
        ::xcpc_log_debug("insert-disk-into-drive1 <%s>", filename.c_str());
        _machine->insert_disk_into_drive1(filename);
        ::xcpc_log_debug("insert-disk-into-drive1 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("insert-disk-into-drive1 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::remove_disk_from_drive1() -> void
{
    try {
        ::xcpc_log_debug("remove-disk-from-drive1");
        _machine->remove_disk_from_drive1();
        ::xcpc_log_debug("remove-disk-from-drive1 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("remove-disk-from-drive1 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_volume(const float volume) -> void
{
    try {
        ::xcpc_log_debug("increase-volume <%d>", static_cast<int>(volume * 100.0f));
        _machine->set_volume(volume);
        ::xcpc_log_debug("increase-volume has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("increase-volume has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_scanlines(const bool scanlines) -> void
{
    try {
        ::xcpc_log_debug("set-scanlines <%d>", scanlines);
        _machine->set_scanlines(scanlines);
        ::xcpc_log_debug("set-scanlines has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-scanlines has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_company_name(const std::string& company_name) -> void
{
    try {
        ::xcpc_log_debug("set-company-name <%s>", company_name.c_str());
        _machine->set_company_name(company_name);
        ::xcpc_log_debug("set-company-name has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-company-name has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_monitor_type(const std::string& monitor_type) -> void
{
    try {
        ::xcpc_log_debug("set-monitor-type <%s>", monitor_type.c_str());
        _machine->set_monitor_type(monitor_type);
        ::xcpc_log_debug("set-monitor-type has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-monitor-type has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_refresh_rate(const std::string& refresh_rate) -> void
{
    try {
        ::xcpc_log_debug("set-refresh-rate <%s>", refresh_rate.c_str());
        _machine->set_refresh_rate(refresh_rate);
        ::xcpc_log_debug("set-refresh-rate has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-refresh-rate has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_keyboard_type(const std::string& keyboard_type) -> void
{
    try {
        ::xcpc_log_debug("set-keyboard-type <%s>", keyboard_type.c_str());
        _machine->set_keyboard_type(keyboard_type);
        ::xcpc_log_debug("set-keyboard-type has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-keyboard-type has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_joystick0(const std::string& device) -> void
{
    try {
        ::xcpc_log_debug("set-joystick0 <%s>", device.c_str());
        work_wnd().emulator().set_joystick(0, device);
        ::xcpc_log_debug("set-joystick0 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-joystick0 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::set_joystick1(const std::string& device) -> void
{
    try {
        ::xcpc_log_debug("set-joystick1 <%s>", device.c_str());
        work_wnd().emulator().set_joystick(1, device);
        ::xcpc_log_debug("set-joystick1 has succeeded");
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("set-joystick1 has failed (%s)", e.what());
    }
    update_gui();
}

auto Application::on_startup() -> void
{
    auto create_app_icon = [&](const std::string& datadir, const std::string& directory, const std::string& filename) -> void
    {
        _app_icon.create_from_file(datadir + '/' + directory + '/' + filename);
    };

    auto create_main_window = [&]() -> void
    {
        _main_window.build();
    };

    auto execute = [&]() -> void
    {
        create_app_icon(xcpc::Utils::get_datdir(), "pixmaps", "xcpc.png");
        create_main_window();
        start_timer();
    };

    return execute();
}

auto Application::on_shutdown() -> void
{
    auto destroy_app_icon = [&]() -> void
    {
        _app_icon.unref();
    };

    auto destroy_main_window = [&]() -> void
    {
        _main_window.destroy();
    };

    auto execute = [&]() -> void
    {
        destroy_main_window();
        destroy_app_icon();
    };

    return execute();
}

auto Application::on_statistics() -> void
{
    update_fps_label();
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

auto Application::on_drive0_create_disk() -> void
{
    CreateDiskDialog dialog(*this, CreateDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

auto Application::on_drive0_insert_disk() -> void
{
    InsertDiskDialog dialog(*this, InsertDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

auto Application::on_drive0_remove_disk() -> void
{
    RemoveDiskDialog dialog(*this, RemoveDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

auto Application::on_drive1_create_disk() -> void
{
    CreateDiskDialog dialog(*this, CreateDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

auto Application::on_drive1_insert_disk() -> void
{
    InsertDiskDialog dialog(*this, InsertDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

auto Application::on_drive1_remove_disk() -> void
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

auto Application::on_joystick0() -> void
{
    set_joystick0(xcpc::Utils::get_joystick0());
}

auto Application::on_joystick1() -> void
{
    set_joystick1(xcpc::Utils::get_joystick1());
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
    _main_window.show_play();
}

auto Application::hide_play() -> void
{
    _main_window.hide_play();
}

auto Application::show_pause() -> void
{
    _main_window.show_pause();
}

auto Application::hide_pause() -> void
{
    _main_window.hide_pause();
}

auto Application::show_reset() -> void
{
    _main_window.show_reset();
}

auto Application::hide_reset() -> void
{
    _main_window.hide_reset();
}

auto Application::set_state(const std::string& state) -> void
{
    _app_state = state;
    update_gui();
}

auto Application::update_gui() -> void
{
    update_window_title();
    update_status_label();
    update_drive0_label();
    update_drive1_label();
    update_system_label();
    update_volume_label();
    update_fps_label();
}

auto Application::update_window_title() -> void
{
    char buffer[256];

    auto format_title = [&]() -> void
    {
        const int rc = snprintf ( buffer, sizeof(buffer)
                                , "%s - %s"
                                , _app_title.c_str()
                                , _app_state.c_str() );
        if(rc <= 0) {
            buffer[0] = '\0';
        }
    };

    auto update_title = [&]() -> void
    {
        _main_window.set_title(buffer);
    };

    auto execute = [&]() -> void
    {
        format_title();
        update_title();
    };

    return execute();
}

auto Application::update_status_label() -> void
{
    std::string label(_app_state);

    auto format_label = [&]() -> void
    {
        if(label.empty()) {
            std::string("{unknown}").swap(label);
        }
    };

    auto update_label = [&]() -> void
    {
        info_bar().update_status(label);
    };

    auto execute = [&]() -> void
    {
        format_label();
        update_label();
    };

    return execute();
}

auto Application::update_drive0_label() -> void
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
            std::string("{empty}").swap(label);
        }
    };

    auto update_label = [&]() -> void
    {
        info_bar().update_drive0(label);
    };

    auto execute = [&]() -> void
    {
        format_label();
        update_label();
    };

    return execute();
}

auto Application::update_drive1_label() -> void
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
            std::string("{empty}").swap(label);
        }
    };

    auto update_label = [&]() -> void
    {
        info_bar().update_drive1(label);
    };

    auto execute = [&]() -> void
    {
        format_label();
        update_label();
    };

    return execute();
}

auto Application::update_system_label() -> void
{
    auto execute = [&]() -> void
    {
        info_bar().update_system(_machine->get_system_info());
    };

    return execute();
}

auto Application::update_volume_label() -> void
{
    char buffer[64];

    const int rc = ::snprintf(buffer, sizeof(buffer), "Vol: %d%% ", static_cast<int>((_machine->get_volume() + 0.005f) * 100.0f));

    auto execute = [&]() -> void
    {
        if(rc > 0) {
            info_bar().update_volume(buffer);
        }
    };

    return execute();
}

auto Application::update_fps_label() -> void
{
    auto execute = [&]() -> void
    {
        info_bar().update_fps(_machine->get_statistics());
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// xcpc_main
// ---------------------------------------------------------------------------

int xcpc_main(int* argc, char*** argv)
{
    xcpc::Application application(*argc, *argv);

    return application.main();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
