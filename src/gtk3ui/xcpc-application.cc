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
    static const char ico_increase_volume[];
    static const char ico_decrease_volume[];
};

const char IconTraits::ico_load_snapshot[]   = "document-open-symbolic";
const char IconTraits::ico_save_snapshot[]   = "document-save-symbolic";
const char IconTraits::ico_play_emulator[]   = "media-playback-start-symbolic";
const char IconTraits::ico_pause_emulator[]  = "media-playback-pause-symbolic";
const char IconTraits::ico_reset_emulator[]  = "media-playlist-repeat-symbolic";
const char IconTraits::ico_increase_volume[] = "audio-volume-high-symbolic";
const char IconTraits::ico_decrease_volume[] = "audio-volume-low-symbolic";

}

// ---------------------------------------------------------------------------
// <anonymous>::Callbacks
// ---------------------------------------------------------------------------

namespace {

struct Callbacks
{
    static gboolean on_statistics(xcpc::Application* self)
    {
        self->on_statistics();
        return TRUE;
    }

    static void on_ignore(GtkWidget* widget, xcpc::Application* self)
    {
    }

    static void on_load_snapshot(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_load_snapshot();
    }

    static void on_save_snapshot(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_save_snapshot();
    }

    static void on_exit(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_exit();
    }

    static void on_play_emulator(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_play_emulator();
    }

    static void on_pause_emulator(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_pause_emulator();
    }

    static void on_reset_emulator(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_reset_emulator();
    }

    static void on_color_monitor(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_color_monitor();
    }

    static void on_green_monitor(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_green_monitor();
    }

    static void on_gray_monitor(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_gray_monitor();
    }

    static void on_refresh_50hz(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_refresh_50hz();
    }

    static void on_refresh_60hz(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_refresh_60hz();
    }

    static void on_english_keyboard(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_english_keyboard();
    }

    static void on_french_keyboard(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_french_keyboard();
    }

    static void on_german_keyboard(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_german_keyboard();
    }

    static void on_spanish_keyboard(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_spanish_keyboard();
    }

    static void on_danish_keyboard(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_danish_keyboard();
    }

    static void on_create_disk_into_drive0(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_create_disk_into_drive0();
    }

    static void on_insert_disk_into_drive0(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_insert_disk_into_drive0();
    }

    static void on_remove_disk_from_drive0(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_remove_disk_from_drive0();
    }

    static void on_create_disk_into_drive1(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_create_disk_into_drive1();
    }

    static void on_insert_disk_into_drive1(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_insert_disk_into_drive1();
    }

    static void on_remove_disk_from_drive1(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_remove_disk_from_drive1();
    }

    static void on_increase_volume(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_increase_volume();
    }

    static void on_decrease_volume(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_decrease_volume();
    }

    static void on_enable_scanlines(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_enable_scanlines();
    }

    static void on_disable_scanlines(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_disable_scanlines();
    }

    static void on_joystick0(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_joystick0();
    }

    static void on_joystick1(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_joystick1();
    }

    static void on_help(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_help();
    }

    static void on_about(GtkWidget* widget, xcpc::Application* self)
    {
        self->on_about();
    }

    static void on_hotkey(GtkWidget* widget, KeySym* keysym, xcpc::Application* self)
    {
        if(keysym != nullptr) {
            switch(*keysym) {
                case XK_Pause:
                    if(self->work_wnd().disabled()) {
                        on_play_emulator(widget, self);
                    }
                    else {
                        on_pause_emulator(widget, self);
                    }
                    break;
                case XK_F1:
                    on_help(widget, self);
                    break;
                case XK_F2:
                    on_load_snapshot(widget, self);
                    break;
                case XK_F3:
                    on_save_snapshot(widget, self);
                    break;
                case XK_F4:
                    on_ignore(widget, self);
                    break;
                case XK_F5:
                    on_reset_emulator(widget, self);
                    break;
                case XK_F6:
                    on_insert_disk_into_drive0(widget, self);
                    break;
                case XK_F7:
                    on_remove_disk_from_drive0(widget, self);
                    break;
                case XK_F8:
                    on_insert_disk_into_drive1(widget, self);
                    break;
                case XK_F9:
                    on_remove_disk_from_drive1(widget, self);
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

    static void on_drag_data_received ( GtkWidget*         widget
                                      , GdkDragContext*    context
                                      , int                x
                                      , int                y
                                      , GtkSelectionData*  data
                                      , guint              info
                                      , guint              time
                                      , xcpc::Application* self )
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
        _load_snapshot.add_activate_callback(G_CALLBACK(&Callbacks::on_load_snapshot), &_application);
        _menu.append(_load_snapshot);
    };

    auto build_save_snapshot = [&]() -> void
    {
        _save_snapshot.create_menu_item_with_label(_("Save snapshot..."));
        _save_snapshot.set_accel(GDK_KEY_F3, GdkModifierType(0));
        _save_snapshot.add_activate_callback(G_CALLBACK(&Callbacks::on_save_snapshot), &_application);
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
        _play_emulator.add_activate_callback(G_CALLBACK(&Callbacks::on_play_emulator), &_application);
        _menu.append(_play_emulator);
    };

    auto build_pause = [&]() -> void
    {
        _pause_emulator.create_menu_item_with_label(_("Pause"));
        _pause_emulator.add_activate_callback(G_CALLBACK(&Callbacks::on_pause_emulator), &_application);
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
        _reset_emulator.add_activate_callback(G_CALLBACK(&Callbacks::on_reset_emulator), &_application);
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
    , _color_monitor(nullptr)
    , _green_monitor(nullptr)
    , _gray_monitor(nullptr)
    , _separator1(nullptr)
    , _refresh_50hz(nullptr)
    , _refresh_60hz(nullptr)
    , _separator2(nullptr)
    , _english_keyboard(nullptr)
    , _french_keyboard(nullptr)
    , _german_keyboard(nullptr)
    , _spanish_keyboard(nullptr)
    , _danish_keyboard(nullptr)
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

    auto build_color_monitor = [&]() -> void
    {
        _color_monitor.create_menu_item_with_label(_("Color monitor"));
        _color_monitor.add_activate_callback(G_CALLBACK(&Callbacks::on_color_monitor), &_application);
        _menu.append(_color_monitor);
    };

    auto build_green_monitor = [&]() -> void
    {
        _green_monitor.create_menu_item_with_label(_("Green monitor"));
        _green_monitor.add_activate_callback(G_CALLBACK(&Callbacks::on_green_monitor), &_application);
        _menu.append(_green_monitor);
    };

    auto build_gray_monitor = [&]() -> void
    {
        _gray_monitor.create_menu_item_with_label(_("Gray monitor"));
        _gray_monitor.add_activate_callback(G_CALLBACK(&Callbacks::on_gray_monitor), &_application);
        _menu.append(_gray_monitor);
    };

    auto build_separator1 = [&]() -> void
    {
        _separator1.create_separator_menu_item();
        _menu.append(_separator1);
    };

    auto build_refresh_50hz = [&]() -> void
    {
        _refresh_50hz.create_menu_item_with_label(_("50Hz refresh rate"));
        _refresh_50hz.add_activate_callback(G_CALLBACK(&Callbacks::on_refresh_50hz), &_application);
        _menu.append(_refresh_50hz);
    };

    auto build_refresh_60hz = [&]() -> void
    {
        _refresh_60hz.create_menu_item_with_label(_("60Hz refresh rate"));
        _refresh_60hz.add_activate_callback(G_CALLBACK(&Callbacks::on_refresh_60hz), &_application);
        _menu.append(_refresh_60hz);
    };

    auto build_separator2 = [&]() -> void
    {
        _separator2.create_separator_menu_item();
        _menu.append(_separator2);
    };

    auto build_english_keyboard = [&]() -> void
    {
        _english_keyboard.create_menu_item_with_label(_("English keyboard"));
        _english_keyboard.add_activate_callback(G_CALLBACK(&Callbacks::on_english_keyboard), &_application);
        _menu.append(_english_keyboard);
    };

    auto build_french_keyboard = [&]() -> void
    {
        _french_keyboard.create_menu_item_with_label(_("French keyboard"));
        _french_keyboard.add_activate_callback(G_CALLBACK(&Callbacks::on_french_keyboard), &_application);
        _menu.append(_french_keyboard);
    };

    auto build_german_keyboard = [&]() -> void
    {
        _german_keyboard.create_menu_item_with_label(_("German keyboard"));
        _german_keyboard.add_activate_callback(G_CALLBACK(&Callbacks::on_german_keyboard), &_application);
        _menu.append(_german_keyboard);
        _german_keyboard.set_sensitive(false);
    };

    auto build_spanish_keyboard = [&]() -> void
    {
        _spanish_keyboard.create_menu_item_with_label(_("Spanish keyboard"));
        _spanish_keyboard.add_activate_callback(G_CALLBACK(&Callbacks::on_spanish_keyboard), &_application);
        _menu.append(_spanish_keyboard);
        _spanish_keyboard.set_sensitive(false);
    };

    auto build_danish_keyboard = [&]() -> void
    {
        _danish_keyboard.create_menu_item_with_label(_("Danish keyboard"));
        _danish_keyboard.add_activate_callback(G_CALLBACK(&Callbacks::on_danish_keyboard), &_application);
        _menu.append(_danish_keyboard);
        _danish_keyboard.set_sensitive(false);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_color_monitor();
        build_green_monitor();
        build_gray_monitor();
        build_separator1();
        build_refresh_50hz();
        build_refresh_60hz();
        build_separator2();
        build_english_keyboard();
        build_french_keyboard();
        build_german_keyboard();
        build_spanish_keyboard();
        build_danish_keyboard();
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
        _create_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_create_disk_into_drive0), &_application);
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
        _insert_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_insert_disk_into_drive0), &_application);
        _menu.append(_insert_disk);
    };

    auto build_remove_disk = [&]() -> void
    {
        _remove_disk.create_menu_item_with_label(_("Remove disk..."));
        _remove_disk.set_accel(GDK_KEY_F7, GdkModifierType(0));
        _remove_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_remove_disk_from_drive0), &_application);
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
        _create_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_create_disk_into_drive1), &_application);
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
        _insert_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_insert_disk_into_drive1), &_application);
        _menu.append(_insert_disk);
    };

    auto build_remove_disk = [&]() -> void
    {
        _remove_disk.create_menu_item_with_label(_("Remove disk..."));
        _remove_disk.set_accel(GDK_KEY_F9, GdkModifierType(0));
        _remove_disk.add_activate_callback(G_CALLBACK(&Callbacks::on_remove_disk_from_drive1), &_application);
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
    , _increase_volume(nullptr)
    , _decrease_volume(nullptr)
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

    auto build_increase_volume = [&]() -> void
    {
        _increase_volume.create_menu_item_with_label(_("Increase volume"));
        _increase_volume.add_activate_callback(G_CALLBACK(&Callbacks::on_increase_volume), &_application);
        _menu.append(_increase_volume);
    };

    auto build_decrease_volume = [&]() -> void
    {
        _decrease_volume.create_menu_item_with_label(_("Decrease volume"));
        _decrease_volume.add_activate_callback(G_CALLBACK(&Callbacks::on_decrease_volume), &_application);
        _menu.append(_decrease_volume);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_increase_volume();
        build_decrease_volume();
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
    , _enable_scanlines(nullptr)
    , _disable_scanlines(nullptr)
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

    auto build_enable_scanlines = [&]() -> void
    {
        _enable_scanlines.create_menu_item_with_label(_("Enable scanlines"));
        _enable_scanlines.add_activate_callback(G_CALLBACK(&Callbacks::on_enable_scanlines), &_application);
        _menu.append(_enable_scanlines);
    };

    auto build_disable_scanlines = [&]() -> void
    {
        _disable_scanlines.create_menu_item_with_label(_("Disable scanlines"));
        _disable_scanlines.add_activate_callback(G_CALLBACK(&Callbacks::on_disable_scanlines), &_application);
        _menu.append(_disable_scanlines);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_menu();
        build_enable_scanlines();
        build_disable_scanlines();
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
    , _decrease_volume(nullptr)
    , _increase_volume(nullptr)
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
        _load_snapshot.add_clicked_callback(G_CALLBACK(&Callbacks::on_load_snapshot), &_application);
        _self.insert(_load_snapshot, -1);
    };

    auto build_save_snapshot = [&]() -> void
    {
        _save_snapshot.create_tool_button();
        _save_snapshot.set_icon_name(IconTraits::ico_save_snapshot);
        _save_snapshot.add_clicked_callback(G_CALLBACK(&Callbacks::on_save_snapshot), &_application);
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
        _play_emulator.add_clicked_callback(G_CALLBACK(&Callbacks::on_play_emulator), &_application);
        _self.insert(_play_emulator, -1);
    };

    auto build_pause_emulator = [&]() -> void
    {
        _pause_emulator.create_tool_button();
        _pause_emulator.set_icon_name(IconTraits::ico_pause_emulator);
        _pause_emulator.add_clicked_callback(G_CALLBACK(&Callbacks::on_pause_emulator), &_application);
        _self.insert(_pause_emulator, -1);
    };

    auto build_reset_emulator = [&]() -> void
    {
        _reset_emulator.create_tool_button();
        _reset_emulator.set_icon_name(IconTraits::ico_reset_emulator);
        _reset_emulator.add_clicked_callback(G_CALLBACK(&Callbacks::on_reset_emulator), &_application);
        _self.insert(_reset_emulator, -1);
    };

    auto build_separator2 = [&]() -> void
    {
        _separator2.create_separator_tool_item();
        _self.insert(_separator2, -1);
    };

    auto build_decrease_volume = [&]() -> void
    {
        _decrease_volume.create_tool_button();
        _decrease_volume.set_icon_name(IconTraits::ico_decrease_volume);
        _decrease_volume.add_clicked_callback(G_CALLBACK(&Callbacks::on_decrease_volume), &_application);
        _self.insert(_decrease_volume, -1);
    };

    auto build_increase_volume = [&]() -> void
    {
        _increase_volume.create_tool_button();
        _increase_volume.set_icon_name(IconTraits::ico_increase_volume);
        _increase_volume.add_clicked_callback(G_CALLBACK(&Callbacks::on_increase_volume), &_application);
        _self.insert(_increase_volume, -1);
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
        build_decrease_volume();
        build_increase_volume();
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
        _emulator.set_backend(_application.emulator().get_backend());
        _emulator.set_joystick(0, xcpc::Utils::get_joystick0());
        _emulator.set_joystick(1, xcpc::Utils::get_joystick0());
        _emulator.drag_dest_set(GTK_DEST_DEFAULT_ALL, target_entries, 1, GdkDragAction(GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK));
        _emulator.add_hotkey_callback(G_CALLBACK(&Callbacks::on_hotkey), &_application);
        _emulator.add_drag_data_received_callback(G_CALLBACK(&Callbacks::on_drag_data_received), &_application);
        _self.pack_start(_emulator, true, true, 0);
    };

    auto build_all = [&]() -> void
    {
        build_self();
        build_emulator();
        disable();
    };

    return build_all();
}

void WorkWnd::enable()
{
    set_sensitive(true);
    _emulator.set_sensitive(true);
    _emulator.grab_focus();
}

bool WorkWnd::enabled()
{
    return is_sensitive() != false;
}

void WorkWnd::disable()
{
    _emulator.grab_focus();
    _emulator.set_sensitive(false);
    set_sensitive(false);
}

bool WorkWnd::disabled()
{
    return is_sensitive() == false;
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
        _work_wnd.disable();
        _window.show_all();
        _work_wnd.enable();
    };

    auto play = [&]() -> void
    {
        _application.play_emulator();
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
    if(ready()) {
        create_application("org.gtk.xcpc");
    }
    return run(argc(), argv());
}

void Application::load_snapshot(const std::string& filename)
{
    try {
        ::xcpc_log_debug("load-snapshot <%s>", filename.c_str());
        _emulator.load_snapshot(filename);
        ::xcpc_log_debug("load-snapshot has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("load-snapshot has failed (%s)", e.what());
    }
}

void Application::save_snapshot(const std::string& filename)
{
    try {
        ::xcpc_log_debug("save-snapshot <%s>", filename.c_str());
        _emulator.save_snapshot(filename);
        ::xcpc_log_debug("save-snapshot has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("save-snapshot has failed (%s)", e.what());
    }
}

void Application::exit()
{
    try {
        ::xcpc_log_debug("exit-emulator");
        _main_window.destroy();
        ::xcpc_log_debug("exit-emulator has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("exit-emulator has failed (%s)", e.what());
    }
}

void Application::play_emulator()
{
    try {
        ::xcpc_log_debug("play-emulator");
        show_pause();
        hide_play();
        work_wnd().enable();
        set_state(_("Playing"));
        ::xcpc_log_debug("play-emulator has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("play-emulator has failed (%s)", e.what());
    }
    update_gui();
}

void Application::pause_emulator()
{
    try {
        ::xcpc_log_debug("pause-emulator");
        show_play();
        hide_pause();
        work_wnd().disable();
        set_state(_("Paused"));
        ::xcpc_log_debug("pause-emulator has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("pause-emulator has failed (%s)", e.what());
    }
    update_gui();
}

void Application::reset_emulator()
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
        _emulator.reset();
        set_state(_("Reset"));
    };

    try {
        ::xcpc_log_debug("reset-emulator");
        pause();
        reset();
        play();
        ::xcpc_log_debug("reset-emulator has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("reset-emulator has failed (%s)", e.what());
    }
    update_gui();
}

void Application::create_disk_into_drive0(const std::string& filename)
{
    try {
        ::xcpc_log_debug("create-disk-into-drive0 <%s>", filename.c_str());
        _emulator.create_disk_into_drive0(filename);
        ::xcpc_log_debug("create-disk-into-drive0 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("create-disk-into-drive0 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::insert_disk_into_drive0(const std::string& filename)
{
    try {
        ::xcpc_log_debug("insert-disk-into-drive0 <%s>", filename.c_str());
        _emulator.insert_disk_into_drive0(filename);
        ::xcpc_log_debug("insert-disk-into-drive0 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("insert-disk-into-drive0 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::remove_disk_from_drive0()
{
    try {
        ::xcpc_log_debug("remove-disk-from-drive0");
        _emulator.remove_disk_from_drive0();
        ::xcpc_log_debug("remove-disk-from-drive0 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("remove-disk-from-drive0 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::create_disk_into_drive1(const std::string& filename)
{
    try {
        ::xcpc_log_debug("create-disk-into-drive1 <%s>", filename.c_str());
        _emulator.create_disk_into_drive1(filename);
        ::xcpc_log_debug("create-disk-into-drive1 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("create-disk-into-drive1 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::insert_disk_into_drive1(const std::string& filename)
{
    try {
        ::xcpc_log_debug("insert-disk-into-drive1 <%s>", filename.c_str());
        _emulator.insert_disk_into_drive1(filename);
        ::xcpc_log_debug("insert-disk-into-drive1 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("insert-disk-into-drive1 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::remove_disk_from_drive1()
{
    try {
        ::xcpc_log_debug("remove-disk-from-drive1");
        _emulator.remove_disk_from_drive1();
        ::xcpc_log_debug("remove-disk-from-drive1 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("remove-disk-from-drive1 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_volume(const float volume)
{
    try {
        ::xcpc_log_debug("increase-volume <%d>", static_cast<int>(volume * 100.0f));
        _emulator.set_volume(volume);
        ::xcpc_log_debug("increase-volume has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("increase-volume has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_scanlines(const bool scanlines)
{
    try {
        ::xcpc_log_debug("set-scanlines <%d>", scanlines);
        _emulator.set_scanlines(scanlines);
        ::xcpc_log_debug("set-scanlines has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("set-scanlines has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_monitor_type(const std::string& monitor_type)
{
    try {
        ::xcpc_log_debug("set-monitor-type <%s>", monitor_type.c_str());
        _emulator.set_monitor_type(monitor_type);
        ::xcpc_log_debug("set-monitor-type has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("set-monitor-type has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_refresh_rate(const std::string& refresh_rate)
{
    try {
        ::xcpc_log_debug("set-refresh-rate <%s>", refresh_rate.c_str());
        _emulator.set_refresh_rate(refresh_rate);
        ::xcpc_log_debug("set-refresh-rate has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("set-refresh-rate has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_keyboard_type(const std::string& keyboard_type)
{
    try {
        ::xcpc_log_debug("set-keyboard-type <%s>", keyboard_type.c_str());
        _emulator.set_keyboard_type(keyboard_type);
        ::xcpc_log_debug("set-keyboard-type has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("set-keyboard-type has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_joystick0(const std::string& device)
{
    try {
        ::xcpc_log_debug("set-joystick0 <%s>", device.c_str());
        work_wnd().emulator().set_joystick(0, device);
        ::xcpc_log_debug("set-joystick0 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("set-joystick0 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::set_joystick1(const std::string& device)
{
    try {
        ::xcpc_log_debug("set-joystick1 <%s>", device.c_str());
        work_wnd().emulator().set_joystick(1, device);
        ::xcpc_log_debug("set-joystick1 has succeeded");
    }
    catch(const std::runtime_error& e) {
        ::xcpc_log_error("set-joystick1 has failed (%s)", e.what());
    }
    update_gui();
}

void Application::on_startup()
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

void Application::on_shutdown()
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

void Application::on_statistics()
{
    update_fps_label();
}

void Application::on_load_snapshot()
{
    LoadSnapshotDialog dialog(*this);

    run_dialog(dialog);
}

void Application::on_save_snapshot()
{
    SaveSnapshotDialog dialog(*this);

    run_dialog(dialog);
}

void Application::on_exit()
{
    exit();
}

void Application::on_play_emulator()
{
    play_emulator();
}

void Application::on_pause_emulator()
{
    pause_emulator();
}

void Application::on_reset_emulator()
{
    reset_emulator();
}

void Application::on_color_monitor()
{
    set_monitor_type("color");
}

void Application::on_green_monitor()
{
    set_monitor_type("green");
}

void Application::on_gray_monitor()
{
    set_monitor_type("gray");
}

void Application::on_refresh_50hz()
{
    set_refresh_rate("50hz");
}

void Application::on_refresh_60hz()
{
    set_refresh_rate("60hz");
}

void Application::on_english_keyboard()
{
    set_keyboard_type("english");
}

void Application::on_french_keyboard()
{
    set_keyboard_type("french");
}

void Application::on_german_keyboard()
{
    set_keyboard_type("german");
}

void Application::on_spanish_keyboard()
{
    set_keyboard_type("spanish");
}

void Application::on_danish_keyboard()
{
    set_keyboard_type("danish");
}

void Application::on_create_disk_into_drive0()
{
    CreateDiskDialog dialog(*this, CreateDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

void Application::on_insert_disk_into_drive0()
{
    InsertDiskDialog dialog(*this, InsertDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

void Application::on_remove_disk_from_drive0()
{
    RemoveDiskDialog dialog(*this, RemoveDiskDialog::DRIVE_A);

    run_dialog(dialog);
}

void Application::on_create_disk_into_drive1()
{
    CreateDiskDialog dialog(*this, CreateDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

void Application::on_insert_disk_into_drive1()
{
    InsertDiskDialog dialog(*this, InsertDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

void Application::on_remove_disk_from_drive1()
{
    RemoveDiskDialog dialog(*this, RemoveDiskDialog::DRIVE_B);

    run_dialog(dialog);
}

void Application::on_increase_volume()
{
    constexpr float increment = 0.05f;
    const     float volume    = _emulator.get_volume() + increment;

    set_volume(volume);
}

void Application::on_decrease_volume()
{
    constexpr float increment = 0.05f;
    const     float volume    = _emulator.get_volume() - increment;

    set_volume(volume);
}

void Application::on_enable_scanlines()
{
    set_scanlines(true);
}

void Application::on_disable_scanlines()
{
    set_scanlines(false);
}

void Application::on_joystick0()
{
    set_joystick0(xcpc::Utils::get_joystick0());
}

void Application::on_joystick1()
{
    set_joystick1(xcpc::Utils::get_joystick1());
}

void Application::on_help()
{
    HelpDialog dialog(*this);

    return run_dialog(dialog);
}

void Application::on_about()
{
    AboutDialog dialog(*this);

    return run_dialog(dialog);
}

void Application::start_timer()
{
    static constexpr guint interval = 1511;

    if(_timer != 0) {
        _timer = (static_cast<void>(::g_source_remove(_timer)), 0);
    }
    if(_timer == 0) {
        _timer = ::g_timeout_add(interval, G_SOURCE_FUNC(&Callbacks::on_statistics), this);
    }
}

void Application::stop_timer()
{
    if(_timer != 0) {
        _timer = (static_cast<void>(::g_source_remove(_timer)), 0);
    }
}

void Application::show_play()
{
    auto execute = [&]() -> void
    {
        _main_window.show_play();
    };

    return execute();
}

void Application::hide_play()
{
    auto execute = [&]() -> void
    {
        _main_window.hide_play();
    };

    return execute();
}

void Application::show_pause()
{
    auto execute = [&]() -> void
    {
        _main_window.show_pause();
    };

    return execute();
}

void Application::hide_pause()
{
    auto execute = [&]() -> void
    {
        _main_window.hide_pause();
    };

    return execute();
}

void Application::show_reset()
{
    auto execute = [&]() -> void
    {
        _main_window.show_reset();
    };

    return execute();
}

void Application::hide_reset()
{
    auto execute = [&]() -> void
    {
        _main_window.hide_reset();
    };

    return execute();
}

void Application::set_state(const std::string& state)
{
    auto update_app_state = [&]() -> void
    {
        _app_state = state;
    };

    auto execute = [&]() -> void
    {
        update_app_state();
        update_gui();
    };

    return execute();
}

void Application::update_gui()
{
    auto execute = [&]() -> void
    {
        update_window_title();
        update_status_label();
        update_drive0_label();
        update_drive1_label();
        update_system_label();
        update_volume_label();
        update_fps_label();
    };

    return execute();
}

void Application::update_window_title()
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

void Application::update_status_label()
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

void Application::update_drive0_label()
{
    std::string label(_emulator.get_drive0_filename());

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

void Application::update_drive1_label()
{
    std::string label(_emulator.get_drive1_filename());

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

void Application::update_system_label()
{
    auto execute = [&]() -> void
    {
        info_bar().update_system(_emulator.get_system_info());
    };

    return execute();
}

void Application::update_volume_label()
{
    char buffer[64];

    const int rc = ::snprintf(buffer, sizeof(buffer), "Vol: %d%% ", static_cast<int>((_emulator.get_volume() + 0.005f) * 100.0f));

    auto execute = [&]() -> void
    {
        if(rc > 0) {
            info_bar().update_volume(buffer);
        }
    };

    return execute();
}

void Application::update_fps_label()
{
    auto execute = [&]() -> void
    {
        info_bar().update_fps(_emulator.get_statistics());
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
