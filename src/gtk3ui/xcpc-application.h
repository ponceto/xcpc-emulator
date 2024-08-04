/*
 * xcpc-application.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_GTK3UI_APPLICATION_H__
#define __XCPC_GTK3UI_APPLICATION_H__

#include <gtk3ui/all.h>
#include "xcpc.h"

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace xcpc {

class Application;
class LoadSnapshotDialog;
class SaveSnapshotDialog;
class CreateDiskDialog;
class InsertDiskDialog;
class RemoveDiskDialog;
class HelpDialog;
class AboutDialog;

}

// ---------------------------------------------------------------------------
// posix_traits
// ---------------------------------------------------------------------------

struct posix_traits
{
    static auto file_exists(const std::string& filename) -> bool
    {
        const int rc = ::access(filename.c_str(), F_OK);
        if(rc == 0) {
            return true;
        }
        return false;
    };

    static auto file_readable(const std::string& filename) -> bool
    {
        const int rc = ::access(filename.c_str(), R_OK);
        if(rc == 0) {
            return true;
        }
        return false;
    };

    static auto file_writable(const std::string& filename) -> bool
    {
        const int rc = ::access(filename.c_str(), W_OK);
        if(rc == 0) {
            return true;
        }
        return false;
    };
};

// ---------------------------------------------------------------------------
// impl::AppWidget
// ---------------------------------------------------------------------------

namespace impl {

class AppWidget
{
public: // public interface
    AppWidget(xcpc::Application&);

    AppWidget(const AppWidget&) = delete;

    AppWidget& operator=(const AppWidget&) = delete;

    virtual ~AppWidget() = default;

    virtual void build() = 0;

protected: // protected data
    xcpc::Application& _application;
};

}

// ---------------------------------------------------------------------------
// impl::FileMenu
// ---------------------------------------------------------------------------

namespace impl {

class FileMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    FileMenu(xcpc::Application&);

    FileMenu(const FileMenu&) = delete;

    FileMenu& operator=(const FileMenu&) = delete;

    virtual ~FileMenu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu              _menu;
    gtk3::MenuItem          _load_snapshot;
    gtk3::MenuItem          _save_snapshot;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _exit;
};

}

// ---------------------------------------------------------------------------
// impl::ControlsMenu
// ---------------------------------------------------------------------------

namespace impl {

class ControlsMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    ControlsMenu(xcpc::Application&);

    ControlsMenu(const ControlsMenu&) = delete;

    ControlsMenu& operator=(const ControlsMenu&) = delete;

    virtual ~ControlsMenu() = default;

    virtual void build() override;

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    gtk3::Menu              _menu;
    gtk3::MenuItem          _play_emulator;
    gtk3::MenuItem          _pause_emulator;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _reset_emulator;
};

}

// ---------------------------------------------------------------------------
// impl::MachineMenu
// ---------------------------------------------------------------------------

namespace impl {

class MachineMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    MachineMenu(xcpc::Application&);

    MachineMenu(const MachineMenu&) = delete;

    MachineMenu& operator=(const MachineMenu&) = delete;

    virtual ~MachineMenu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu              _menu;
    gtk3::MenuItem          _company_isp;
    gtk3::MenuItem          _company_triumph;
    gtk3::MenuItem          _company_saisho;
    gtk3::MenuItem          _company_solavox;
    gtk3::MenuItem          _company_awa;
    gtk3::MenuItem          _company_schneider;
    gtk3::MenuItem          _company_orion;
    gtk3::MenuItem          _company_amstrad;
    gtk3::SeparatorMenuItem _separator1;
    gtk3::MenuItem          _color_monitor;
    gtk3::MenuItem          _green_monitor;
    gtk3::MenuItem          _gray_monitor;
    gtk3::SeparatorMenuItem _separator2;
    gtk3::MenuItem          _refresh_50hz;
    gtk3::MenuItem          _refresh_60hz;
    gtk3::SeparatorMenuItem _separator3;
    gtk3::MenuItem          _english_keyboard;
    gtk3::MenuItem          _french_keyboard;
    gtk3::MenuItem          _german_keyboard;
    gtk3::MenuItem          _spanish_keyboard;
    gtk3::MenuItem          _danish_keyboard;
};

}

// ---------------------------------------------------------------------------
// impl::Drive0Menu
// ---------------------------------------------------------------------------

namespace impl {

class Drive0Menu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    Drive0Menu(xcpc::Application&);

    Drive0Menu(const Drive0Menu&) = delete;

    Drive0Menu& operator=(const Drive0Menu&) = delete;

    virtual ~Drive0Menu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu              _menu;
    gtk3::MenuItem          _create_disk;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _insert_disk;
    gtk3::MenuItem          _remove_disk;
};

}

// ---------------------------------------------------------------------------
// impl::Drive1Menu
// ---------------------------------------------------------------------------

namespace impl {

class Drive1Menu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    Drive1Menu(xcpc::Application&);

    Drive1Menu(const Drive1Menu&) = delete;

    Drive1Menu& operator=(const Drive1Menu&) = delete;

    virtual ~Drive1Menu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu              _menu;
    gtk3::MenuItem          _create_disk;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _insert_disk;
    gtk3::MenuItem          _remove_disk;
};

}

// ---------------------------------------------------------------------------
// impl::AudioMenu
// ---------------------------------------------------------------------------

namespace impl {

class AudioMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    AudioMenu(xcpc::Application&);

    AudioMenu(const AudioMenu&) = delete;

    AudioMenu& operator=(const AudioMenu&) = delete;

    virtual ~AudioMenu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu      _menu;
    gtk3::MenuItem  _increase_volume;
    gtk3::MenuItem  _decrease_volume;
};

}

// ---------------------------------------------------------------------------
// impl::VideoMenu
// ---------------------------------------------------------------------------

namespace impl {

class VideoMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    VideoMenu(xcpc::Application&);

    VideoMenu(const VideoMenu&) = delete;

    VideoMenu& operator=(const VideoMenu&) = delete;

    virtual ~VideoMenu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu     _menu;
    gtk3::MenuItem _enable_scanlines;
    gtk3::MenuItem _disable_scanlines;
};

}

// ---------------------------------------------------------------------------
// impl::InputMenu
// ---------------------------------------------------------------------------

namespace impl {

class InputMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    InputMenu(xcpc::Application&);

    InputMenu(const InputMenu&) = delete;

    InputMenu& operator=(const InputMenu&) = delete;

    virtual ~InputMenu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu     _menu;
    gtk3::MenuItem _joystick0;
    gtk3::MenuItem _joystick1;
};

}

// ---------------------------------------------------------------------------
// impl::HelpMenu
// ---------------------------------------------------------------------------

namespace impl {

class HelpMenu final
    : public impl::AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    HelpMenu(xcpc::Application&);

    HelpMenu(const HelpMenu&) = delete;

    HelpMenu& operator=(const HelpMenu&) = delete;

    virtual ~HelpMenu() = default;

    virtual void build() override;

private: // private data
    gtk3::Menu              _menu;
    gtk3::MenuItem          _help;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _about;
};

}

// ---------------------------------------------------------------------------
// MenuBar
// ---------------------------------------------------------------------------

namespace impl {

class MenuBar final
    : public impl::AppWidget
    , public gtk3::MenuBar
{
public: // public interface
    MenuBar(xcpc::Application&);

    MenuBar(const MenuBar&) = delete;

    MenuBar& operator=(const MenuBar&) = delete;

    virtual ~MenuBar() = default;

    virtual void build() override;

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    impl::FileMenu     _file_menu;
    impl::ControlsMenu _controls_menu;
    impl::MachineMenu  _machine_menu;
    impl::Drive0Menu   _drive0_menu;
    impl::Drive1Menu   _drive1_menu;
    impl::AudioMenu    _audio_menu;
    impl::VideoMenu    _video_menu;
    impl::InputMenu    _input_menu;
    impl::HelpMenu     _help_menu;
};

}

// ---------------------------------------------------------------------------
// impl::ToolBar
// ---------------------------------------------------------------------------

namespace impl {

class ToolBar final
    : public impl::AppWidget
    , public gtk3::Toolbar
{
public: // public interface
    ToolBar(xcpc::Application&);

    ToolBar(const ToolBar&) = delete;

    ToolBar& operator=(const ToolBar&) = delete;

    virtual ~ToolBar() = default;

    virtual void build() override;

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    gtk3::ToolButton        _load_snapshot;
    gtk3::ToolButton        _save_snapshot;
    gtk3::SeparatorToolItem _separator1;
    gtk3::ToolButton        _play_emulator;
    gtk3::ToolButton        _pause_emulator;
    gtk3::ToolButton        _reset_emulator;
    gtk3::SeparatorToolItem _separator2;
    gtk3::ToolButton        _decrease_volume;
    gtk3::ToolButton        _increase_volume;
};

}

// ---------------------------------------------------------------------------
// impl::InfoBar
// ---------------------------------------------------------------------------

namespace impl {

class InfoBar final
    : public impl::AppWidget
    , public gtk3::HBox
{
public: // public interface
    InfoBar(xcpc::Application&);

    InfoBar(const InfoBar&) = delete;

    InfoBar& operator=(const InfoBar&) = delete;

    virtual ~InfoBar() = default;

    virtual void build() override;

    void update_status(const std::string& status);

    void update_drive0(const std::string& filename);

    void update_drive1(const std::string& filename);

    void update_system(const std::string& system);

    void update_volume(const std::string& volume);

    void update_fps(const std::string& fps);

private: // private data
    gtk3::Label _status;
    gtk3::Label _drive0;
    gtk3::Label _drive1;
    gtk3::Label _system;
    gtk3::Label _volume;
    gtk3::Label _fps;
};

}

// ---------------------------------------------------------------------------
// impl::WorkWnd
// ---------------------------------------------------------------------------

namespace impl {

class WorkWnd final
    : public impl::AppWidget
    , public gtk3::HBox
{
public: // public interface
    WorkWnd(xcpc::Application&);

    WorkWnd(const WorkWnd&) = delete;

    WorkWnd& operator=(const WorkWnd&) = delete;

    virtual ~WorkWnd() = default;

    virtual void build() override;

    void enable();

    bool enabled();

    void disable();

    bool disabled();

    auto emulator() -> gtk3::Emulator&
    {
        return _emulator;
    }

private: // private data
    gtk3::Emulator _emulator;
};

}

// ---------------------------------------------------------------------------
// impl::MainWindow
// ---------------------------------------------------------------------------

namespace impl {

class MainWindow final
    : public impl::AppWidget
    , public gtk3::ApplicationWindow
{
public: // public interface
    MainWindow(xcpc::Application&);

    MainWindow(const MainWindow&) = delete;

    MainWindow& operator=(const MainWindow&) = delete;

    virtual ~MainWindow() = default;

    virtual void build() override;

    auto menu_bar() -> impl::MenuBar&
    {
        return _menu_bar;
    }

    auto tool_bar() -> impl::ToolBar&
    {
        return _tool_bar;
    }

    auto work_wnd() -> impl::WorkWnd&
    {
        return _work_wnd;
    }

    auto info_bar() -> impl::InfoBar&
    {
        return _info_bar;
    }

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    gtk3::VBox    _layout;
    impl::MenuBar _menu_bar;
    impl::ToolBar _tool_bar;
    impl::WorkWnd _work_wnd;
    impl::InfoBar _info_bar;
};

}

// ---------------------------------------------------------------------------
// xcpc::Application
// ---------------------------------------------------------------------------

namespace xcpc {

class Application final
    : public base::Application
    , public gtk3::Application
{
public: // public interface
    Application(int& argc, char**& argv);

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    virtual ~Application();

    virtual int main() override;

public: // public accessors
    auto app_context() -> gtk3::Application&
    {
        return *this;
    }

    auto app_title() -> std::string&
    {
        return _app_title;
    }

    auto app_state() -> std::string&
    {
        return _app_state;
    }

    auto app_icon() -> gdk3::Pixbuf&
    {
        return _app_icon;
    }

    auto emulator() -> base::Emulator&
    {
        return _emulator;
    }

    auto main_window() -> impl::MainWindow&
    {
        return _main_window;
    }

    auto menu_bar() -> impl::MenuBar&
    {
        return _main_window.menu_bar();
    }

    auto tool_bar() -> impl::ToolBar&
    {
        return _main_window.tool_bar();
    }

    auto work_wnd() -> impl::WorkWnd&
    {
        return _main_window.work_wnd();
    }

    auto info_bar() -> impl::InfoBar&
    {
        return _main_window.info_bar();
    }

public: // public methods
    virtual auto load_snapshot(const std::string& filename) -> void override final;

    virtual auto save_snapshot(const std::string& filename) -> void override final;

    virtual auto exit() -> void override final;

    virtual auto play_emulator() -> void override final;

    virtual auto pause_emulator() -> void override final;

    virtual auto reset_emulator() -> void override final;

    virtual auto create_disk_into_drive0(const std::string& filename) -> void override final;

    virtual auto insert_disk_into_drive0(const std::string& filename) -> void override final;

    virtual auto remove_disk_from_drive0() -> void override final;

    virtual auto create_disk_into_drive1(const std::string& filename) -> void override final;

    virtual auto insert_disk_into_drive1(const std::string& filename) -> void override final;

    virtual auto remove_disk_from_drive1() -> void override final;

    virtual auto set_volume(const float value) -> void override final;

    virtual auto set_scanlines(const bool scanlines) -> void override final;

    virtual auto set_company_name(const std::string& company_name) -> void override final;

    virtual auto set_monitor_type(const std::string& monitor_type) -> void override final;

    virtual auto set_refresh_rate(const std::string& refresh_rate) -> void override final;

    virtual auto set_keyboard_type(const std::string& keyboard_type) -> void override final;

    virtual auto set_joystick0(const std::string& device) -> void override final;

    virtual auto set_joystick1(const std::string& device) -> void override final;

public: // public signals
    virtual auto on_startup() -> void override final;

    virtual auto on_shutdown() -> void override final;

    virtual auto on_statistics() -> void override final;

    virtual auto on_load_snapshot() -> void override final;

    virtual auto on_save_snapshot() -> void override final;

    virtual auto on_exit() -> void override final;

    virtual auto on_play_emulator() -> void override final;

    virtual auto on_pause_emulator() -> void override final;

    virtual auto on_reset_emulator() -> void override final;

    virtual auto on_company_isp() -> void override final;

    virtual auto on_company_triumph() -> void override final;

    virtual auto on_company_saisho() -> void override final;

    virtual auto on_company_solavox() -> void override final;

    virtual auto on_company_awa() -> void override final;

    virtual auto on_company_schneider() -> void override final;

    virtual auto on_company_orion() -> void override final;

    virtual auto on_company_amstrad() -> void override final;

    virtual auto on_color_monitor() -> void override final;

    virtual auto on_green_monitor() -> void override final;

    virtual auto on_gray_monitor() -> void override final;

    virtual auto on_refresh_50hz() -> void override final;

    virtual auto on_refresh_60hz() -> void override final;

    virtual auto on_english_keyboard() -> void override final;

    virtual auto on_french_keyboard() -> void override final;

    virtual auto on_german_keyboard() -> void override final;

    virtual auto on_spanish_keyboard() -> void override final;

    virtual auto on_danish_keyboard() -> void override final;

    virtual auto on_create_disk_into_drive0() -> void override final;

    virtual auto on_insert_disk_into_drive0() -> void override final;

    virtual auto on_remove_disk_from_drive0() -> void override final;

    virtual auto on_create_disk_into_drive1() -> void override final;

    virtual auto on_insert_disk_into_drive1() -> void override final;

    virtual auto on_remove_disk_from_drive1() -> void override final;

    virtual auto on_increase_volume() -> void override final;

    virtual auto on_decrease_volume() -> void override final;

    virtual auto on_enable_scanlines() -> void override final;

    virtual auto on_disable_scanlines() -> void override final;

    virtual auto on_joystick0() -> void override final;

    virtual auto on_joystick1() -> void override final;

    virtual auto on_help() -> void override final;

    virtual auto on_about() -> void override final;

private: // private interface
    auto start_timer() -> void;

    auto stop_timer() -> void;

    auto show_play() -> void;

    auto hide_play() -> void;

    auto show_pause() -> void;

    auto hide_pause() -> void;

    auto show_reset() -> void;

    auto hide_reset() -> void;

    auto set_state(const std::string& state) -> void;

    auto update_gui() -> void;

    auto update_window_title() -> void;

    auto update_status_label() -> void;

    auto update_drive0_label() -> void;

    auto update_drive1_label() -> void;

    auto update_system_label() -> void;

    auto update_volume_label() -> void;

    auto update_fps_label() -> void;

private: // private data
    std::string      _app_title;
    std::string      _app_state;
    gdk3::Pixbuf     _app_icon;
    impl::MainWindow _main_window;
    guint            _timer;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_GTK3UI_APPLICATION_H__ */
