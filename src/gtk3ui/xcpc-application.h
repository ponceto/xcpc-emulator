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
    static bool file_exists(const std::string& filename)
    {
        const int rc = ::access(filename.c_str(), F_OK);
        if(rc == 0) {
            return true;
        }
        return false;
    };

    static bool file_readable(const std::string& filename)
    {
        const int rc = ::access(filename.c_str(), R_OK);
        if(rc == 0) {
            return true;
        }
        return false;
    };

    static bool file_writable(const std::string& filename)
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
    gtk3::MenuItem          _color_monitor;
    gtk3::MenuItem          _green_monitor;
    gtk3::MenuItem          _gray_monitor;
    gtk3::SeparatorMenuItem _separator1;
    gtk3::MenuItem          _refresh_50hz;
    gtk3::MenuItem          _refresh_60hz;
    gtk3::SeparatorMenuItem _separator2;
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
    virtual void load_snapshot(const std::string& filename) override final;

    virtual void save_snapshot(const std::string& filename) override final;

    virtual void exit() override final;

    virtual void play_emulator() override final;

    virtual void pause_emulator() override final;

    virtual void reset_emulator() override final;

    virtual void create_disk_into_drive0(const std::string& filename) override final;

    virtual void insert_disk_into_drive0(const std::string& filename) override final;

    virtual void remove_disk_from_drive0() override final;

    virtual void create_disk_into_drive1(const std::string& filename) override final;

    virtual void insert_disk_into_drive1(const std::string& filename) override final;

    virtual void remove_disk_from_drive1() override final;

    virtual void set_volume(const float value) override final;

    virtual void set_scanlines(const bool scanlines) override final;

    virtual void set_monitor_type(const std::string& monitor_type) override final;

    virtual void set_refresh_rate(const std::string& refresh_rate) override final;

    virtual void set_keyboard_type(const std::string& keyboard_type) override final;

    virtual void set_joystick0(const std::string& device) override final;

    virtual void set_joystick1(const std::string& device) override final;

public: // public signals
    virtual void on_startup() override final;

    virtual void on_shutdown() override final;

    virtual void on_statistics() override final;

    virtual void on_load_snapshot() override final;

    virtual void on_save_snapshot() override final;

    virtual void on_exit() override final;

    virtual void on_play_emulator() override final;

    virtual void on_pause_emulator() override final;

    virtual void on_reset_emulator() override final;

    virtual void on_color_monitor() override final;

    virtual void on_green_monitor() override final;

    virtual void on_gray_monitor() override final;

    virtual void on_refresh_50hz() override final;

    virtual void on_refresh_60hz() override final;

    virtual void on_english_keyboard() override final;

    virtual void on_french_keyboard() override final;

    virtual void on_german_keyboard() override final;

    virtual void on_spanish_keyboard() override final;

    virtual void on_danish_keyboard() override final;

    virtual void on_create_disk_into_drive0() override final;

    virtual void on_insert_disk_into_drive0() override final;

    virtual void on_remove_disk_from_drive0() override final;

    virtual void on_create_disk_into_drive1() override final;

    virtual void on_insert_disk_into_drive1() override final;

    virtual void on_remove_disk_from_drive1() override final;

    virtual void on_increase_volume() override final;

    virtual void on_decrease_volume() override final;

    virtual void on_enable_scanlines() override final;

    virtual void on_disable_scanlines() override final;

    virtual void on_joystick0() override final;

    virtual void on_joystick1() override final;

    virtual void on_help() override final;

    virtual void on_about() override final;

private: // private interface
    void start_timer();

    void stop_timer();

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

    void set_state(const std::string& state);

    void update_gui();

    void update_window_title();

    void update_status_label();

    void update_drive0_label();

    void update_drive1_label();

    void update_system_label();

    void update_volume_label();

    void update_fps_label();

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
