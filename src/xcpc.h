/*
 * xcpc.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_H__
#define __XCPC_H__

#include <xcpc/libxcpc-cxx.h>
#include <xcpc/amstrad/cpc/cpc-machine.h>
#include "xcpc-settings.h"

// ---------------------------------------------------------------------------
// TranslationTraits
// ---------------------------------------------------------------------------

struct TranslationTraits
{
    static auto translate(const char* string) -> const char*;
};

// ---------------------------------------------------------------------------
// translation macro
// ---------------------------------------------------------------------------

#ifndef _
#define _(string) TranslationTraits::translate(string)
#endif

// ---------------------------------------------------------------------------
// PosixTraits
// ---------------------------------------------------------------------------

struct PosixTraits
{
    static auto file_exists(const std::string& filename) -> bool;

    static auto file_readable(const std::string& filename) -> bool;

    static auto file_writable(const std::string& filename) -> bool;
};

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace base {

class Environ;
class Application;
class Dialog;
class SnapshotDialog;
class LoadSnapshotDialog;
class SaveSnapshotDialog;
class DiskDialog;
class CreateDiskDialog;
class InsertDiskDialog;
class RemoveDiskDialog;
class SettingsDialog;
class HelpDialog;
class AboutDialog;
class ScopedOperation;
class ScopedPause;
class ScopedReset;

}

// ---------------------------------------------------------------------------
// base::AudioSettings
// ---------------------------------------------------------------------------

namespace base {

struct AudioSettings
{
    float volume = 0.50f;
};

}

// ---------------------------------------------------------------------------
// base::VideoSettings
// ---------------------------------------------------------------------------

namespace base {

struct VideoSettings
{
    std::string renderer      = "default";
    bool        crt_emulation = true;
    float       u_hsampling   = 0.75f;
    float       u_vsampling   = 0.25f;
    float       u_curvature   = 0.10f;
    float       u_corner      = 0.15f;
    float       u_dotline     = 0.30f;
    float       u_dotmask     = 0.10f;
    float       u_vignetting  = 1.00f;
    float       u_brightness  = 1.30f;
};

}

// ---------------------------------------------------------------------------
// base::InputSettings
// ---------------------------------------------------------------------------

namespace base {

struct InputSettings
{
    bool joystick_emulation = false;
};

}

// ---------------------------------------------------------------------------
// base::GlobalSettings
// ---------------------------------------------------------------------------

namespace base {

struct GlobalSettings
{
    AudioSettings audio;
    VideoSettings video;
    InputSettings input;
};

}

// ---------------------------------------------------------------------------
// base::Environ
// ---------------------------------------------------------------------------

namespace base {

class Environ
{
public: // public interface
    Environ() = default;

    Environ(Environ&&) = delete;

    Environ(const Environ&) = delete;

    Environ& operator=(Environ&&) = delete;

    Environ& operator=(const Environ&) = delete;

    virtual ~Environ() = default;

    static auto setenv(const std::string& variable, const std::string& value) -> void;
};

}

// ---------------------------------------------------------------------------
// base::Application
// ---------------------------------------------------------------------------

namespace base {

class Application
{
public: // public interface
    Application(int& argc, char**& argv);

    Application(Application&&) = delete;

    Application(const Application&) = delete;

    Application& operator=(Application&&) = delete;

    Application& operator=(const Application&) = delete;

    virtual ~Application() = default;

    virtual auto main() ->int = 0;

    auto get_backend() const -> const xcpc::Backend*
    {
        return _machine->get_backend();
    }

    auto audio_settings() -> AudioSettings&
    {
        return _globals.audio;
    }

    auto audio_settings() const -> const AudioSettings&
    {
        return _globals.audio;
    }

    auto video_settings() -> VideoSettings&
    {
        return _globals.video;
    }

    auto video_settings() const -> const VideoSettings&
    {
        return _globals.video;
    }

    auto input_settings() -> InputSettings&
    {
        return _globals.input;
    }

    auto input_settings() const -> const InputSettings&
    {
        return _globals.input;
    }

public: // public methods
    virtual auto has_ximage() -> bool = 0;

    virtual auto has_opengl() -> bool = 0;

    virtual auto load_snapshot(const std::string& filename) -> void = 0;

    virtual auto save_snapshot(const std::string& filename) -> void = 0;

    virtual auto exit() -> void = 0;

    virtual auto play_emulator() -> void = 0;

    virtual auto pause_emulator() -> void = 0;

    virtual auto reset_emulator() -> void = 0;

    virtual auto create_disk_into_drive0(const std::string& filename) -> void = 0;

    virtual auto insert_disk_into_drive0(const std::string& filename) -> void = 0;

    virtual auto remove_disk_from_drive0() -> void = 0;

    virtual auto create_disk_into_drive1(const std::string& filename) -> void = 0;

    virtual auto insert_disk_into_drive1(const std::string& filename) -> void = 0;

    virtual auto remove_disk_from_drive1() -> void = 0;

    virtual auto set_volume(const float volume) -> void = 0;

    virtual auto set_crt_emulation(const bool crt_emulation) -> void = 0;

    virtual auto set_company_name(const std::string& company_name) -> void = 0;

    virtual auto set_machine_type(const std::string& machine_type) -> void = 0;

    virtual auto set_monitor_type(const std::string& monitor_type) -> void = 0;

    virtual auto set_refresh_rate(const std::string& refresh_rate) -> void = 0;

    virtual auto set_keyboard_type(const std::string& keyboard_type) -> void = 0;

    virtual auto set_renderer_type(const std::string& renderer_type) -> void = 0;

    virtual auto set_joystick_emulation(const bool enabled) -> void = 0;

    virtual auto set_joystick0(const std::string& device) -> void = 0;

    virtual auto set_joystick1(const std::string& device) -> void = 0;

    auto get_machine_type() const -> const std::string
    {
        return _machine->get_machine_type();
    }

    auto get_company_name() const -> const std::string
    {
        return _machine->get_company_name();
    }

    auto get_monitor_type() const -> const std::string
    {
        return _machine->get_monitor_type();
    }

    auto get_refresh_rate() const -> const std::string
    {
        return _machine->get_refresh_rate();
    }

    auto get_keyboard_type() const -> const std::string
    {
        return _machine->get_keyboard_type();
    }

    auto get_renderer_type() const -> const std::string
    {
        return _machine->get_renderer_type();
    }

public: // public signals
    virtual auto on_startup() -> void = 0;

    virtual auto on_shutdown() -> void = 0;

    virtual auto on_statistics() -> void = 0;

    virtual auto on_snapshot_load() -> void = 0;

    virtual auto on_snapshot_save() -> void = 0;

    virtual auto on_exit() -> void = 0;

    virtual auto on_emulator_play() -> void = 0;

    virtual auto on_emulator_pause() -> void = 0;

    virtual auto on_emulator_reset() -> void = 0;

    virtual auto on_machine_cpc464() -> void = 0;

    virtual auto on_machine_cpc664() -> void = 0;

    virtual auto on_machine_cpc6128() -> void = 0;

    virtual auto on_company_isp() -> void = 0;

    virtual auto on_company_triumph() -> void = 0;

    virtual auto on_company_saisho() -> void = 0;

    virtual auto on_company_solavox() -> void = 0;

    virtual auto on_company_awa() -> void = 0;

    virtual auto on_company_schneider() -> void = 0;

    virtual auto on_company_orion() -> void = 0;

    virtual auto on_company_amstrad() -> void = 0;

    virtual auto on_monitor_color() -> void = 0;

    virtual auto on_monitor_green() -> void = 0;

    virtual auto on_monitor_gray() -> void = 0;

    virtual auto on_refresh_50hz() -> void = 0;

    virtual auto on_refresh_60hz() -> void = 0;

    virtual auto on_keyboard_english() -> void = 0;

    virtual auto on_keyboard_french() -> void = 0;

    virtual auto on_keyboard_german() -> void = 0;

    virtual auto on_keyboard_spanish() -> void = 0;

    virtual auto on_keyboard_danish() -> void = 0;

    virtual auto on_drive0_disk_create() -> void = 0;

    virtual auto on_drive0_disk_insert() -> void = 0;

    virtual auto on_drive0_disk_remove() -> void = 0;

    virtual auto on_drive1_disk_create() -> void = 0;

    virtual auto on_drive1_disk_insert() -> void = 0;

    virtual auto on_drive1_disk_remove() -> void = 0;

    virtual auto on_volume_increase() -> void = 0;

    virtual auto on_volume_decrease() -> void = 0;

    virtual auto on_audio_settings() -> void = 0;

    virtual auto on_renderer_ximage() -> void = 0;

    virtual auto on_renderer_opengl() -> void = 0;

    virtual auto on_crt_emulation_enable() -> void = 0;

    virtual auto on_crt_emulation_disable() -> void = 0;

    virtual auto on_video_settings() -> void = 0;

    virtual auto on_joystick0_connect() -> void = 0;

    virtual auto on_joystick0_disconnect() -> void = 0;

    virtual auto on_joystick1_connect() -> void = 0;

    virtual auto on_joystick1_disconnect() -> void = 0;

    virtual auto on_joystick_emulation_enable() -> void = 0;

    virtual auto on_joystick_emulation_disable() -> void = 0;

    virtual auto on_help() -> void = 0;

    virtual auto on_about() -> void = 0;

protected: // protected interface
    using SettingsPtr = std::unique_ptr<cpc::Settings>;
    using MachinePtr  = std::unique_ptr<cpc::Machine>;

    virtual auto run_dialog(Dialog&) -> void;

    auto load_settings() -> void;

    auto save_settings() -> void;

    auto set_parameterb(const std::string& parameter, bool value) -> void;

    auto set_parameteri(const std::string& parameter, int value) -> void;

    auto set_parameterf(const std::string& parameter, float value) -> void;

protected: // protected data
    int&              _argc;
    char**&           _argv;
    const SettingsPtr _settings;
    const MachinePtr  _machine;
    GlobalSettings    _globals;
};

}

// ---------------------------------------------------------------------------
// base::Dialog
// ---------------------------------------------------------------------------

namespace base {

class Dialog
{
public: // public interface
    Dialog(Application&, const std::string& title);

    Dialog(Dialog&&) = delete;

    Dialog(const Dialog&) = delete;

    Dialog& operator=(Dialog&&) = delete;

    Dialog& operator=(const Dialog&) = delete;

    virtual ~Dialog() = default;

    virtual auto run() -> void = 0;

protected: // protected methods
    auto load_snapshot(const std::string& filename) -> void;

    auto save_snapshot(const std::string& filename) -> void;

    auto exit() -> void;

    auto play_emulator() -> void;

    auto pause_emulator() -> void;

    auto reset_emulator() -> void;

    auto create_disk_into_drive0(const std::string& filename) -> void;

    auto insert_disk_into_drive0(const std::string& filename) -> void;

    auto remove_disk_from_drive0() -> void;

    auto create_disk_into_drive1(const std::string& filename) -> void;

    auto insert_disk_into_drive1(const std::string& filename) -> void;

    auto remove_disk_from_drive1() -> void;

protected: // protected data
    Application& _application;
    std::string  _title;
};

}

// ---------------------------------------------------------------------------
// base::SnapshotDialog
// ---------------------------------------------------------------------------

namespace base {

class SnapshotDialog
    : public Dialog
{
public: // public interface
    SnapshotDialog(Application&, const std::string& title);

    SnapshotDialog(SnapshotDialog&&) = delete;

    SnapshotDialog(const SnapshotDialog&) = delete;

    SnapshotDialog& operator=(SnapshotDialog&&) = delete;

    SnapshotDialog& operator=(const SnapshotDialog&) = delete;

    virtual ~SnapshotDialog() = default;

protected: // protected data
    std::string _filename;
};

}

// ---------------------------------------------------------------------------
// base::LoadSnapshotDialog
// ---------------------------------------------------------------------------

namespace base {

class LoadSnapshotDialog
    : public SnapshotDialog
{
public: // public interface
    LoadSnapshotDialog(Application&);

    LoadSnapshotDialog(LoadSnapshotDialog&&) = delete;

    LoadSnapshotDialog(const LoadSnapshotDialog&) = delete;

    LoadSnapshotDialog& operator=(LoadSnapshotDialog&&) = delete;

    LoadSnapshotDialog& operator=(const LoadSnapshotDialog&) = delete;

    virtual ~LoadSnapshotDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::SaveSnapshotDialog
// ---------------------------------------------------------------------------

namespace base {

class SaveSnapshotDialog
    : public SnapshotDialog
{
public: // public interface
    SaveSnapshotDialog(Application&);

    SaveSnapshotDialog(SaveSnapshotDialog&&) = delete;

    SaveSnapshotDialog(const SaveSnapshotDialog&) = delete;

    SaveSnapshotDialog& operator=(SaveSnapshotDialog&&) = delete;

    SaveSnapshotDialog& operator=(const SaveSnapshotDialog&) = delete;

    virtual ~SaveSnapshotDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::DiskDialog
// ---------------------------------------------------------------------------

namespace base {

class DiskDialog
    : public Dialog
{
public: // public interface
    DiskDialog(Application&, const std::string& title, const char drive);

    DiskDialog(DiskDialog&&) = delete;

    DiskDialog(const DiskDialog&) = delete;

    DiskDialog& operator=(DiskDialog&&) = delete;

    DiskDialog& operator=(const DiskDialog&) = delete;

    virtual ~DiskDialog() = default;

public: // public static data
    static constexpr char DRIVE_A = 'A';
    static constexpr char DRIVE_B = 'B';

protected: // protected interface
    auto create_disk(const std::string& filename) -> void;

    auto insert_disk(const std::string& filename) -> void;

    auto remove_disk() -> void;

protected: // protected data
    const char  _drive;
    std::string _filename;
};

}

// ---------------------------------------------------------------------------
// base::CreateDiskDialog
// ---------------------------------------------------------------------------

namespace base {

class CreateDiskDialog
    : public DiskDialog
{
public: // public interface
    CreateDiskDialog(Application&, const char drive);

    CreateDiskDialog(CreateDiskDialog&&) = delete;

    CreateDiskDialog(const CreateDiskDialog&) = delete;

    CreateDiskDialog& operator=(CreateDiskDialog&&) = delete;

    CreateDiskDialog& operator=(const CreateDiskDialog&) = delete;

    virtual ~CreateDiskDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::InsertDiskDialog
// ---------------------------------------------------------------------------

namespace base {

class InsertDiskDialog
    : public DiskDialog
{
public: // public interface
    InsertDiskDialog(Application&, const char drive);

    InsertDiskDialog(InsertDiskDialog&&) = delete;

    InsertDiskDialog(const InsertDiskDialog&) = delete;

    InsertDiskDialog& operator=(InsertDiskDialog&&) = delete;

    InsertDiskDialog& operator=(const InsertDiskDialog&) = delete;

    virtual ~InsertDiskDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::RemoveDiskDialog
// ---------------------------------------------------------------------------

namespace base {

class RemoveDiskDialog
    : public DiskDialog
{
public: // public interface
    RemoveDiskDialog(Application&, const char drive);

    RemoveDiskDialog(RemoveDiskDialog&&) = delete;

    RemoveDiskDialog(const RemoveDiskDialog&) = delete;

    RemoveDiskDialog& operator=(RemoveDiskDialog&&) = delete;

    RemoveDiskDialog& operator=(const RemoveDiskDialog&) = delete;

    virtual ~RemoveDiskDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::SettingsDialog
// ---------------------------------------------------------------------------

namespace base {

class SettingsDialog
    : public Dialog
{
public: // public interface
    SettingsDialog(Application&, const std::string& title);

    SettingsDialog(SettingsDialog&&) = delete;

    SettingsDialog(const SettingsDialog&) = delete;

    SettingsDialog& operator=(SettingsDialog&&) = delete;

    SettingsDialog& operator=(const SettingsDialog&) = delete;

    virtual ~SettingsDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::AudioSettingsDialog
// ---------------------------------------------------------------------------

namespace base {

class AudioSettingsDialog
    : public SettingsDialog
{
public: // public interface
    AudioSettingsDialog(Application&);

    AudioSettingsDialog(AudioSettingsDialog&&) = delete;

    AudioSettingsDialog(const AudioSettingsDialog&) = delete;

    AudioSettingsDialog& operator=(AudioSettingsDialog&&) = delete;

    AudioSettingsDialog& operator=(const AudioSettingsDialog&) = delete;

    virtual ~AudioSettingsDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::VideoSettingsDialog
// ---------------------------------------------------------------------------

namespace base {

class VideoSettingsDialog
    : public SettingsDialog
{
public: // public interface
    VideoSettingsDialog(Application&);

    VideoSettingsDialog(VideoSettingsDialog&&) = delete;

    VideoSettingsDialog(const VideoSettingsDialog&) = delete;

    VideoSettingsDialog& operator=(VideoSettingsDialog&&) = delete;

    VideoSettingsDialog& operator=(const VideoSettingsDialog&) = delete;

    virtual ~VideoSettingsDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::InputSettingsDialog
// ---------------------------------------------------------------------------

namespace base {

class InputSettingsDialog
    : public SettingsDialog
{
public: // public interface
    InputSettingsDialog(Application&);

    InputSettingsDialog(InputSettingsDialog&&) = delete;

    InputSettingsDialog(const InputSettingsDialog&) = delete;

    InputSettingsDialog& operator=(InputSettingsDialog&&) = delete;

    InputSettingsDialog& operator=(const InputSettingsDialog&) = delete;

    virtual ~InputSettingsDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::HelpDialog
// ---------------------------------------------------------------------------

namespace base {

class HelpDialog
    : public Dialog
{
public: // public interface
    HelpDialog(Application&);

    HelpDialog(HelpDialog&&) = delete;

    HelpDialog(const HelpDialog&) = delete;

    HelpDialog& operator=(HelpDialog&&) = delete;

    HelpDialog& operator=(const HelpDialog&) = delete;

    virtual ~HelpDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::AboutDialog
// ---------------------------------------------------------------------------

namespace base {

class AboutDialog
    : public Dialog
{
public: // public interface
    AboutDialog(Application&);

    AboutDialog(AboutDialog&&) = delete;

    AboutDialog(const AboutDialog&) = delete;

    AboutDialog& operator=(AboutDialog&&) = delete;

    AboutDialog& operator=(const AboutDialog&) = delete;

    virtual ~AboutDialog() = default;
};

}

// ---------------------------------------------------------------------------
// base::ScopedOperation
// ---------------------------------------------------------------------------

namespace base {

class ScopedOperation
{
public: // public interface
    ScopedOperation(Application&);

    ScopedOperation(ScopedOperation&&) = delete;

    ScopedOperation(const ScopedOperation&) = delete;

    ScopedOperation& operator=(ScopedOperation&&) = delete;

    ScopedOperation& operator=(const ScopedOperation&) = delete;

    virtual ~ScopedOperation() = default;

protected: // protected data
    Application& _application;
};

}

// ---------------------------------------------------------------------------
// base::ScopedPause
// ---------------------------------------------------------------------------

namespace base {

class ScopedPause final
    : public ScopedOperation
{
public: // public interface
    ScopedPause(Application&);

    ScopedPause(ScopedPause&&) = delete;

    ScopedPause(const ScopedPause&) = delete;

    ScopedPause& operator=(ScopedPause&&) = delete;

    ScopedPause& operator=(const ScopedPause&) = delete;

    virtual ~ScopedPause();
};

}

// ---------------------------------------------------------------------------
// base::ScopedReset
// ---------------------------------------------------------------------------

namespace base {

class ScopedReset final
    : public ScopedOperation
{
public: // public interface
    ScopedReset(Application&);

    ScopedReset(ScopedReset&&) = delete;

    ScopedReset(const ScopedReset&) = delete;

    ScopedReset& operator=(ScopedReset&&) = delete;

    ScopedReset& operator=(const ScopedReset&) = delete;

    virtual ~ScopedReset();
};

}

// ---------------------------------------------------------------------------
// Xcpc
// ---------------------------------------------------------------------------

class Xcpc
{
public: // public interface
    Xcpc(int& argc, char**& argv);

    Xcpc(Xcpc&&) = delete;

    Xcpc(const Xcpc&) = delete;

    Xcpc& operator=(Xcpc&&) = delete;

    Xcpc& operator=(const Xcpc&) = delete;

    virtual ~Xcpc();

    auto main() const -> int;

private: // private data
    int&    _argc;
    char**& _argv;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_H__ */
