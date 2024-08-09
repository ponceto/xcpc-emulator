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
// impl forward declarations
// ---------------------------------------------------------------------------

namespace impl {

using namespace xcpc;

class AppWidget;
class AppWindow;
class Canvas;
class FileMenu;
class ControlsMenu;
class MachineMenu;
class Drive0Menu;
class Drive1Menu;
class AudioMenu;
class VideoMenu;
class InputMenu;
class HelpMenu;
class MenuBar;
class ToolBar;
class InfoBar;
class WorkWnd;

}

// ---------------------------------------------------------------------------
// xcpc forward declarations
// ---------------------------------------------------------------------------

namespace xcpc {

class Environ;
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
// impl::AppWidget
// ---------------------------------------------------------------------------

namespace impl {

class AppWidget
{
public: // public interface
    AppWidget(Application&);

    AppWidget(const AppWidget&) = delete;

    AppWidget& operator=(const AppWidget&) = delete;

    virtual ~AppWidget() = default;

    virtual void build() = 0;

protected: // protected data
    Application& _application;
};

}

// ---------------------------------------------------------------------------
// impl::Canvas
// ---------------------------------------------------------------------------

namespace impl {

class Canvas final
    : public AppWidget
    , public gtk3::GLArea
{
public: // public interface
    Canvas(Application&);

    Canvas(const Canvas&) = delete;

    Canvas& operator=(const Canvas&) = delete;

    virtual ~Canvas() = default;

    virtual void build() override final;

public: // public signals
    auto on_canvas_realize() -> void;

    auto on_canvas_unrealize() -> void;

    auto on_canvas_render(GdkGLContext& context) -> void;

    auto on_canvas_resize(gint width, gint height) -> void;

    auto on_canvas_key_press(GdkEventKey& event) -> void;

    auto on_canvas_key_release(GdkEventKey& event) -> void;

    auto on_canvas_button_press(GdkEventButton& event) -> void;

    auto on_canvas_button_release(GdkEventButton& event) -> void;

    auto on_canvas_motion_notify(GdkEventMotion& event) -> void;

private: // private data
    gtk3::GLArea& _self;
};

}

// ---------------------------------------------------------------------------
// impl::FileMenu
// ---------------------------------------------------------------------------

namespace impl {

class FileMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    FileMenu(Application&);

    FileMenu(const FileMenu&) = delete;

    FileMenu& operator=(const FileMenu&) = delete;

    virtual ~FileMenu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem&         _self;
    gtk3::Menu              _menu;
    gtk3::MenuItem          _snapshot_load;
    gtk3::MenuItem          _snapshot_save;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _exit;
};

}

// ---------------------------------------------------------------------------
// impl::ControlsMenu
// ---------------------------------------------------------------------------

namespace impl {

class ControlsMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    ControlsMenu(Application&);

    ControlsMenu(const ControlsMenu&) = delete;

    ControlsMenu& operator=(const ControlsMenu&) = delete;

    virtual ~ControlsMenu() = default;

    virtual void build() override final;

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    gtk3::MenuItem&         _self;
    gtk3::Menu              _menu;
    gtk3::MenuItem          _emulator_play;
    gtk3::MenuItem          _emulator_pause;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _emulator_reset;
};

}

// ---------------------------------------------------------------------------
// impl::MachineMenu
// ---------------------------------------------------------------------------

namespace impl {

class MachineMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    MachineMenu(Application&);

    MachineMenu(const MachineMenu&) = delete;

    MachineMenu& operator=(const MachineMenu&) = delete;

    virtual ~MachineMenu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem& _self;
    gtk3::Menu      _menu;
    gtk3::MenuItem  _machine;
    gtk3::Menu      _machine_menu;
    gtk3::MenuItem  _machine_cpc464;
    gtk3::MenuItem  _machine_cpc664;
    gtk3::MenuItem  _machine_cpc6128;
    gtk3::MenuItem  _company;
    gtk3::Menu      _company_menu;
    gtk3::MenuItem  _company_isp;
    gtk3::MenuItem  _company_triumph;
    gtk3::MenuItem  _company_saisho;
    gtk3::MenuItem  _company_solavox;
    gtk3::MenuItem  _company_awa;
    gtk3::MenuItem  _company_schneider;
    gtk3::MenuItem  _company_orion;
    gtk3::MenuItem  _company_amstrad;
    gtk3::MenuItem  _monitor;
    gtk3::Menu      _monitor_menu;
    gtk3::MenuItem  _monitor_color;
    gtk3::MenuItem  _monitor_green;
    gtk3::MenuItem  _monitor_gray;
    gtk3::MenuItem  _refresh;
    gtk3::Menu      _refresh_menu;
    gtk3::MenuItem  _refresh_50hz;
    gtk3::MenuItem  _refresh_60hz;
    gtk3::MenuItem  _keyboard;
    gtk3::Menu      _keyboard_menu;
    gtk3::MenuItem  _keyboard_english;
    gtk3::MenuItem  _keyboard_french;
    gtk3::MenuItem  _keyboard_german;
    gtk3::MenuItem  _keyboard_spanish;
    gtk3::MenuItem  _keyboard_danish;
};

}

// ---------------------------------------------------------------------------
// impl::Drive0Menu
// ---------------------------------------------------------------------------

namespace impl {

class Drive0Menu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    Drive0Menu(Application&);

    Drive0Menu(const Drive0Menu&) = delete;

    Drive0Menu& operator=(const Drive0Menu&) = delete;

    virtual ~Drive0Menu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem&         _self;
    gtk3::Menu              _menu;
    gtk3::MenuItem          _disk_create;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _disk_insert;
    gtk3::MenuItem          _disk_remove;
};

}

// ---------------------------------------------------------------------------
// impl::Drive1Menu
// ---------------------------------------------------------------------------

namespace impl {

class Drive1Menu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    Drive1Menu(Application&);

    Drive1Menu(const Drive1Menu&) = delete;

    Drive1Menu& operator=(const Drive1Menu&) = delete;

    virtual ~Drive1Menu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem&         _self;
    gtk3::Menu              _menu;
    gtk3::MenuItem          _disk_create;
    gtk3::SeparatorMenuItem _separator;
    gtk3::MenuItem          _disk_insert;
    gtk3::MenuItem          _disk_remove;
};

}

// ---------------------------------------------------------------------------
// impl::AudioMenu
// ---------------------------------------------------------------------------

namespace impl {

class AudioMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    AudioMenu(Application&);

    AudioMenu(const AudioMenu&) = delete;

    AudioMenu& operator=(const AudioMenu&) = delete;

    virtual ~AudioMenu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem& _self;
    gtk3::Menu      _menu;
    gtk3::MenuItem  _volume_increase;
    gtk3::MenuItem  _volume_decrease;
};

}

// ---------------------------------------------------------------------------
// impl::VideoMenu
// ---------------------------------------------------------------------------

namespace impl {

class VideoMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    VideoMenu(Application&);

    VideoMenu(const VideoMenu&) = delete;

    VideoMenu& operator=(const VideoMenu&) = delete;

    virtual ~VideoMenu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem& _self;
    gtk3::Menu      _menu;
    gtk3::MenuItem  _scanlines_enable;
    gtk3::MenuItem  _scanlines_disable;
};

}

// ---------------------------------------------------------------------------
// impl::InputMenu
// ---------------------------------------------------------------------------

namespace impl {

class InputMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    InputMenu(Application&);

    InputMenu(const InputMenu&) = delete;

    InputMenu& operator=(const InputMenu&) = delete;

    virtual ~InputMenu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem& _self;
    gtk3::Menu      _menu;
    gtk3::MenuItem  _joystick0;
    gtk3::Menu      _joystick0_menu;
    gtk3::MenuItem  _joystick0_connect;
    gtk3::MenuItem  _joystick0_disconnect;
    gtk3::MenuItem  _joystick1;
    gtk3::Menu      _joystick1_menu;
    gtk3::MenuItem  _joystick1_connect;
    gtk3::MenuItem  _joystick1_disconnect;
};

}

// ---------------------------------------------------------------------------
// impl::HelpMenu
// ---------------------------------------------------------------------------

namespace impl {

class HelpMenu final
    : public AppWidget
    , public gtk3::MenuItem
{
public: // public interface
    HelpMenu(Application&);

    HelpMenu(const HelpMenu&) = delete;

    HelpMenu& operator=(const HelpMenu&) = delete;

    virtual ~HelpMenu() = default;

    virtual void build() override final;

private: // private data
    gtk3::MenuItem&         _self;
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
    : public AppWidget
    , public gtk3::MenuBar
{
public: // public interface
    MenuBar(Application&);

    MenuBar(const MenuBar&) = delete;

    MenuBar& operator=(const MenuBar&) = delete;

    virtual ~MenuBar() = default;

    virtual void build() override final;

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    gtk3::MenuBar& _self;
    FileMenu       _file_menu;
    ControlsMenu   _controls_menu;
    MachineMenu    _machine_menu;
    Drive0Menu     _drive0_menu;
    Drive1Menu     _drive1_menu;
    AudioMenu      _audio_menu;
    VideoMenu      _video_menu;
    InputMenu      _input_menu;
    HelpMenu       _help_menu;
};

}

// ---------------------------------------------------------------------------
// impl::ToolBar
// ---------------------------------------------------------------------------

namespace impl {

class ToolBar final
    : public AppWidget
    , public gtk3::Toolbar
{
public: // public interface
    ToolBar(Application&);

    ToolBar(const ToolBar&) = delete;

    ToolBar& operator=(const ToolBar&) = delete;

    virtual ~ToolBar() = default;

    virtual void build() override final;

    void show_play();

    void hide_play();

    void show_pause();

    void hide_pause();

    void show_reset();

    void hide_reset();

private: // private data
    gtk3::Toolbar&          _self;
    gtk3::ToolButton        _snapshot_load;
    gtk3::ToolButton        _snapshot_save;
    gtk3::SeparatorToolItem _separator1;
    gtk3::ToolButton        _emulator_play;
    gtk3::ToolButton        _emulator_pause;
    gtk3::ToolButton        _emulator_reset;
    gtk3::SeparatorToolItem _separator2;
    gtk3::ToolButton        _volume_decrease;
    gtk3::ToolButton        _volume_increase;
};

}

// ---------------------------------------------------------------------------
// impl::InfoBar
// ---------------------------------------------------------------------------

namespace impl {

class InfoBar final
    : public AppWidget
    , public gtk3::HBox
{
public: // public interface
    InfoBar(Application&);

    InfoBar(const InfoBar&) = delete;

    InfoBar& operator=(const InfoBar&) = delete;

    virtual ~InfoBar() = default;

    virtual void build() override final;

    void set_state(const std::string& state);

    void set_drive0(const std::string& drive0);

    void set_drive1(const std::string& drive1);

    void set_system(const std::string& system);

    void set_volume(const std::string& volume);

    void set_stats(const std::string& stats);

private: // private data
    gtk3::HBox& _self;
    gtk3::Label _state;
    gtk3::Label _drive0;
    gtk3::Label _drive1;
    gtk3::Label _system;
    gtk3::Label _volume;
    gtk3::Label _stats;
};

}

// ---------------------------------------------------------------------------
// impl::WorkWnd
// ---------------------------------------------------------------------------

namespace impl {

class WorkWnd final
    : public AppWidget
    , public gtk3::HBox
{
public: // public interface
    WorkWnd(Application&);

    WorkWnd(const WorkWnd&) = delete;

    WorkWnd& operator=(const WorkWnd&) = delete;

    virtual ~WorkWnd() = default;

    virtual void build() override final;

    auto emulator() -> gtk3::Emulator&
    {
        return _emulator;
    }

private: // private data
    gtk3::HBox&    _self;
    gtk3::Emulator _emulator;
    Canvas         _canvas;
};

}

// ---------------------------------------------------------------------------
// impl::AppWindow
// ---------------------------------------------------------------------------

namespace impl {

class AppWindow final
    : public AppWidget
    , public gtk3::ApplicationWindow
{
public: // public interface
    AppWindow(Application&);

    AppWindow(const AppWindow&) = delete;

    AppWindow& operator=(const AppWindow&) = delete;

    virtual ~AppWindow() = default;

    virtual void build() override final;

    auto menu_bar() -> auto&
    {
        return _menu_bar;
    }

    auto tool_bar() -> auto&
    {
        return _tool_bar;
    }

    auto work_wnd() -> auto&
    {
        return _work_wnd;
    }

    auto info_bar() -> auto&
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
    gtk3::VBox _layout;
    MenuBar    _menu_bar;
    ToolBar    _tool_bar;
    WorkWnd    _work_wnd;
    InfoBar    _info_bar;
};

}

// ---------------------------------------------------------------------------
// Environ
// ---------------------------------------------------------------------------

namespace xcpc {

class Environ
    : public base::Environ
{
public: // public interface
    Environ();

    Environ(const Environ&) = delete;

    Environ& operator=(const Environ&) = delete;

    virtual ~Environ() = default;
};

}

// ---------------------------------------------------------------------------
// Application
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

    virtual int main() override final;

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

    auto main_window() -> auto&
    {
        return _app_window;
    }

    auto menu_bar() -> auto&
    {
        return _app_window.menu_bar();
    }

    auto tool_bar() -> auto&
    {
        return _app_window.tool_bar();
    }

    auto work_wnd() -> auto&
    {
        return _app_window.work_wnd();
    }

    auto info_bar() -> auto&
    {
        return _app_window.info_bar();
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

    virtual auto set_machine_type(const std::string& machine_type) -> void override final;

    virtual auto set_company_name(const std::string& company_name) -> void override final;

    virtual auto set_monitor_type(const std::string& monitor_type) -> void override final;

    virtual auto set_refresh_rate(const std::string& refresh_rate) -> void override final;

    virtual auto set_keyboard_type(const std::string& keyboard_type) -> void override final;

    virtual auto set_joystick0(const std::string& device) -> void override final;

    virtual auto set_joystick1(const std::string& device) -> void override final;

public: // public signals
    virtual auto on_open(GFile** files, int num_files) -> void override final;

    virtual auto on_startup() -> void override final;

    virtual auto on_shutdown() -> void override final;

    virtual auto on_statistics() -> void override final;

    virtual auto on_snapshot_load() -> void override final;

    virtual auto on_snapshot_save() -> void override final;

    virtual auto on_exit() -> void override final;

    virtual auto on_emulator_play() -> void override final;

    virtual auto on_emulator_pause() -> void override final;

    virtual auto on_emulator_reset() -> void override final;

    virtual auto on_machine_cpc464() -> void override final;

    virtual auto on_machine_cpc664() -> void override final;

    virtual auto on_machine_cpc6128() -> void override final;

    virtual auto on_company_isp() -> void override final;

    virtual auto on_company_triumph() -> void override final;

    virtual auto on_company_saisho() -> void override final;

    virtual auto on_company_solavox() -> void override final;

    virtual auto on_company_awa() -> void override final;

    virtual auto on_company_schneider() -> void override final;

    virtual auto on_company_orion() -> void override final;

    virtual auto on_company_amstrad() -> void override final;

    virtual auto on_monitor_color() -> void override final;

    virtual auto on_monitor_green() -> void override final;

    virtual auto on_monitor_gray() -> void override final;

    virtual auto on_refresh_50hz() -> void override final;

    virtual auto on_refresh_60hz() -> void override final;

    virtual auto on_keyboard_english() -> void override final;

    virtual auto on_keyboard_french() -> void override final;

    virtual auto on_keyboard_german() -> void override final;

    virtual auto on_keyboard_spanish() -> void override final;

    virtual auto on_keyboard_danish() -> void override final;

    virtual auto on_drive0_disk_create() -> void override final;

    virtual auto on_drive0_disk_insert() -> void override final;

    virtual auto on_drive0_disk_remove() -> void override final;

    virtual auto on_drive1_disk_create() -> void override final;

    virtual auto on_drive1_disk_insert() -> void override final;

    virtual auto on_drive1_disk_remove() -> void override final;

    virtual auto on_volume_increase() -> void override final;

    virtual auto on_volume_decrease() -> void override final;

    virtual auto on_scanlines_enable() -> void override final;

    virtual auto on_scanlines_disable() -> void override final;

    virtual auto on_joystick0_connect() -> void override final;

    virtual auto on_joystick0_disconnect() -> void override final;

    virtual auto on_joystick1_connect() -> void override final;

    virtual auto on_joystick1_disconnect() -> void override final;

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

    auto update_title() -> void;

    auto update_state() -> void;

    auto update_drive0() -> void;

    auto update_drive1() -> void;

    auto update_system() -> void;

    auto update_volume() -> void;

    auto update_stats() -> void;

    auto update_all() -> void;

private: // private data
    std::string     _app_title;
    std::string     _app_state;
    gdk3::Pixbuf    _app_icon;
    impl::AppWindow _app_window;
    guint           _timer;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_GTK3UI_APPLICATION_H__ */
