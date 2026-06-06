/*
 * xcpc.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <unistd.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include "xcpc.h"

// ---------------------------------------------------------------------------
// TranslationTraits
// ---------------------------------------------------------------------------

auto TranslationTraits::translate(const char* string) -> const char*
{
    return string;
}

// ---------------------------------------------------------------------------
// PosixTraits
// ---------------------------------------------------------------------------

auto PosixTraits::file_exists(const std::string& filename) -> bool
{
    const int rc = ::access(filename.c_str(), F_OK);
    if(rc == 0) {
        return true;
    }
    return false;
}

auto PosixTraits::file_readable(const std::string& filename) -> bool
{
    const int rc = ::access(filename.c_str(), R_OK);
    if(rc == 0) {
        return true;
    }
    return false;
}

auto PosixTraits::file_writable(const std::string& filename) -> bool
{
    const int rc = ::access(filename.c_str(), W_OK);
    if(rc == 0) {
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// base::Environ
// ---------------------------------------------------------------------------

namespace base {

auto Environ::setenv(const std::string& variable, const std::string& value) -> void
{
    const int rc = ::setenv(variable.c_str(), value.c_str(), 1);

    if(rc != 0) {
        throw std::runtime_error("setenv() has failed");
    }
}

}

// ---------------------------------------------------------------------------
// <anonymous>::SettingsTraits
// ---------------------------------------------------------------------------

namespace {

struct SettingsTraits
{
    static auto load_globals_from_ini() -> base::GlobalSettings
    {
        base::GlobalSettings globals;

        auto load_audio_settings = [&](base::SettingsFile& settings_file) -> void
        {
            const auto& audio(settings_file.table("audio"));

            globals.audio.volume = audio.entry("volume").get_double(globals.audio.volume);
        };

        auto load_video_settings = [&](base::SettingsFile& settings_file) -> void
        {
            const auto& video(settings_file.table("video"));

            globals.video.renderer      = video.entry("renderer"     ).get_string(globals.video.renderer     );
            globals.video.crt_emulation = video.entry("crt_emulation").get_bool  (globals.video.crt_emulation);
            globals.video.u_hsampling   = video.entry("u_hsampling"  ).get_double(globals.video.u_hsampling  );
            globals.video.u_vsampling   = video.entry("u_vsampling"  ).get_double(globals.video.u_vsampling  );
            globals.video.u_curvature   = video.entry("u_curvature"  ).get_double(globals.video.u_curvature  );
            globals.video.u_corner      = video.entry("u_corner"     ).get_double(globals.video.u_corner     );
            globals.video.u_dotline     = video.entry("u_dotline"    ).get_double(globals.video.u_dotline    );
            globals.video.u_dotmask     = video.entry("u_dotmask"    ).get_double(globals.video.u_dotmask    );
            globals.video.u_vignetting  = video.entry("u_vignetting" ).get_double(globals.video.u_vignetting );
            globals.video.u_brightness  = video.entry("u_brightness" ).get_double(globals.video.u_brightness );
        };

        auto load_input_settings = [&](base::SettingsFile& settings_file) -> void
        {
            const auto& input = settings_file.table("input");

            globals.input.joystick_emulation = input.entry("joystick_emulation").get_bool(globals.input.joystick_emulation);
        };

        try {
            base::SettingsFile settings_file("xcpc.conf");
            settings_file.load();
            load_audio_settings(settings_file);
            load_video_settings(settings_file);
            load_input_settings(settings_file);
        }
        catch(const std::exception& e) {
            ::xcpc_log_alert("load_settings() has failed (%s)", e.what());
        }
        return globals;
    }

    static auto make_settings(const base::GlobalSettings& globals, int& argc, char**& argv) -> cpc::Settings*
    {
        std::unique_ptr<cpc::Settings> settings(new cpc::Settings());

        /* seed cli defaults from ini values */ {
            if(globals.video.renderer != "default") {
                settings->opt_renderer = globals.video.renderer;
            }
            settings->opt_crt_emulation = globals.video.crt_emulation;
        }
        /* parse cli, overriding any seeded defaults */ {
            settings->parse(argc, argv);
        }
        return settings.release();
    }
};

}

// ---------------------------------------------------------------------------
// base::Application
// ---------------------------------------------------------------------------

namespace base {

Application::Application(int& argc, char**& argv)
    : _argc(argc)
    , _argv(argv)
    , _globals(SettingsTraits::load_globals_from_ini())
    , _settings(SettingsTraits::make_settings(_globals, argc, argv))
    , _machine(new cpc::Machine(*_settings))
{
}

auto Application::run_dialog(Dialog& dialog) -> void
{
    const ScopedPause pause(*this);

    return dialog.run();
}

auto Application::load_settings() -> void
{
    _globals = SettingsTraits::load_globals_from_ini();
}

auto Application::save_settings() -> void
{
    auto save_audio_settings = [&](SettingsFile& settings_file) -> void
    {
        auto& audio(settings_file.table("audio"));

        audio.entry("volume").set_double(_globals.audio.volume);
    };

    auto save_video_settings = [&](SettingsFile& settings_file) -> void
    {
        auto& video(settings_file.table("video"));

        video.entry("renderer"     ).set_string(_globals.video.renderer     );
        video.entry("crt_emulation").set_bool  (_globals.video.crt_emulation);
        video.entry("u_hsampling"  ).set_double(_globals.video.u_hsampling  );
        video.entry("u_vsampling"  ).set_double(_globals.video.u_vsampling  );
        video.entry("u_curvature"  ).set_double(_globals.video.u_curvature  );
        video.entry("u_corner"     ).set_double(_globals.video.u_corner     );
        video.entry("u_dotline"    ).set_double(_globals.video.u_dotline    );
        video.entry("u_dotmask"    ).set_double(_globals.video.u_dotmask    );
        video.entry("u_vignetting" ).set_double(_globals.video.u_vignetting );
        video.entry("u_brightness" ).set_double(_globals.video.u_brightness );
    };

    auto save_input_settings = [&](SettingsFile& settings_file) -> void
    {
        auto& input = settings_file.table("input");

        input.entry("joystick_emulation").set_bool(_globals.input.joystick_emulation);
    };

    auto save_all = [&]() -> void
    {
        try {
            SettingsFile settings_file("xcpc.conf");
            save_audio_settings(settings_file);
            save_video_settings(settings_file);
            save_input_settings(settings_file);
            settings_file.save();
        }
        catch(const std::exception& e) {
            ::xcpc_log_alert("save_settings() has failed (%s)", e.what());
        }
    };

    return save_all();
}

auto Application::set_parameterb(const std::string& parameter, bool value) -> void
{
    try {
        if(bool(_machine) != false) {
            _machine->set_parameterb(parameter, value);
        }
    }
    catch(const std::exception& e) {
        ::xcpc_log_alert("set_parameterb() has failed (%s)", e.what());
    }
}

auto Application::set_parameteri(const std::string& parameter, int value) -> void
{
    try {
        if(bool(_machine) != false) {
            _machine->set_parameteri(parameter, value);
        }
    }
    catch(const std::exception& e) {
        ::xcpc_log_alert("set_parameteri() has failed (%s)", e.what());
    }
}

auto Application::set_parameterf(const std::string& parameter, float value) -> void
{
    try {
        if(bool(_machine) != false) {
            _machine->set_parameterf(parameter, value);
        }
    }
    catch(const std::exception& e) {
        ::xcpc_log_alert("set_parameterf() has failed (%s)", e.what());
    }
}

}

// ---------------------------------------------------------------------------
// base::Dialog
// ---------------------------------------------------------------------------

namespace base {

Dialog::Dialog(Application& application, const std::string& title)
    : _application(application)
    , _title(title)
{
}

auto Dialog::load_snapshot(const std::string& filename) -> void
{
    _application.load_snapshot(filename);
}

auto Dialog::save_snapshot(const std::string& filename) -> void
{
    _application.save_snapshot(filename);
}

auto Dialog::exit() -> void
{
    _application.exit();
}

auto Dialog::play_emulator() -> void
{
    _application.play_emulator();
}

auto Dialog::pause_emulator() -> void
{
    _application.pause_emulator();
}

auto Dialog::reset_emulator() -> void
{
    _application.reset_emulator();
}

auto Dialog::create_disk_into_drive0(const std::string& filename) -> void
{
    _application.create_disk_into_drive0(filename);
}

auto Dialog::insert_disk_into_drive0(const std::string& filename) -> void
{
    _application.insert_disk_into_drive0(filename);
}

auto Dialog::remove_disk_from_drive0() -> void
{
    _application.remove_disk_from_drive0();
}

auto Dialog::create_disk_into_drive1(const std::string& filename) -> void
{
    _application.create_disk_into_drive1(filename);
}

auto Dialog::insert_disk_into_drive1(const std::string& filename) -> void
{
    _application.insert_disk_into_drive1(filename);
}

auto Dialog::remove_disk_from_drive1() -> void
{
    _application.remove_disk_from_drive1();
}

}

// ---------------------------------------------------------------------------
// base::SnapshotDialog
// ---------------------------------------------------------------------------

namespace base {

SnapshotDialog::SnapshotDialog(Application& application, const std::string& title)
    : Dialog(application, title)
    , _filename()
{
}

}

// ---------------------------------------------------------------------------
// base::LoadSnapshotDialog
// ---------------------------------------------------------------------------

namespace base {

LoadSnapshotDialog::LoadSnapshotDialog(Application& application)
    : SnapshotDialog(application, _("Load snapshot"))
{
}

}

// ---------------------------------------------------------------------------
// base::SaveSnapshotDialog
// ---------------------------------------------------------------------------

namespace base {

SaveSnapshotDialog::SaveSnapshotDialog(Application& application)
    : SnapshotDialog(application, _("Save snapshot"))
{
}

}

// ---------------------------------------------------------------------------
// base::DiskDialog
// ---------------------------------------------------------------------------

namespace base {

DiskDialog::DiskDialog(Application& application, const std::string& title, const char drive)
    : Dialog(application, title)
    , _drive(drive)
    , _filename()
{
}

auto DiskDialog::create_disk(const std::string& filename) -> void
{
    switch(_drive) {
        case DRIVE_A:
            create_disk_into_drive0(filename);
            break;
        case DRIVE_B:
            create_disk_into_drive1(filename);
            break;
        default:
            break;
    }
}

auto DiskDialog::insert_disk(const std::string& filename) -> void
{
    switch(_drive) {
        case DRIVE_A:
            insert_disk_into_drive0(filename);
            break;
        case DRIVE_B:
            insert_disk_into_drive1(filename);
            break;
        default:
            break;
    }
}

auto DiskDialog::remove_disk() -> void
{
    switch(_drive) {
        case DRIVE_A:
            remove_disk_from_drive0();
            break;
        case DRIVE_B:
            remove_disk_from_drive1();
            break;
        default:
            break;
    }
}

}

// ---------------------------------------------------------------------------
// base::CreateDiskDialog
// ---------------------------------------------------------------------------

namespace base {

CreateDiskDialog::CreateDiskDialog(Application& application, const char drive)
    : DiskDialog(application, _("Create disk"), drive)
{
    switch(_drive) {
        case DRIVE_A:
            _title = _("Drive A - Create disk");
            break;
        case DRIVE_B:
            _title = _("Drive B - Create disk");
            break;
        default:
            break;
    }
}

}

// ---------------------------------------------------------------------------
// base::InsertDiskDialog
// ---------------------------------------------------------------------------

namespace base {

InsertDiskDialog::InsertDiskDialog(Application& application, const char drive)
    : DiskDialog(application, _("Insert disk"), drive)
{
    switch(_drive) {
        case DRIVE_A:
            _title = _("Drive A - Insert disk");
            break;
        case DRIVE_B:
            _title = _("Drive B - Insert disk");
            break;
        default:
            break;
    }
}

}

// ---------------------------------------------------------------------------
// base::RemoveDiskDialog
// ---------------------------------------------------------------------------

namespace base {

RemoveDiskDialog::RemoveDiskDialog(Application& application, const char drive)
    : DiskDialog(application, _("Remove disk"), drive)
{
    switch(_drive) {
        case DRIVE_A:
            _title = _("Drive A - Remove disk");
            break;
        case DRIVE_B:
            _title = _("Drive B - Remove disk");
            break;
        default:
            break;
    }
}

}

// ---------------------------------------------------------------------------
// base::SettingsDialog
// ---------------------------------------------------------------------------

namespace base {

SettingsDialog::SettingsDialog(Application& application, const std::string& title)
    : Dialog(application, title)
{
}

}

// ---------------------------------------------------------------------------
// base::AudioSettingsDialog
// ---------------------------------------------------------------------------

namespace base {

AudioSettingsDialog::AudioSettingsDialog(Application& application)
    : SettingsDialog(application, _("Audio settings"))
{
}

}

// ---------------------------------------------------------------------------
// base::VideoSettingsDialog
// ---------------------------------------------------------------------------

namespace base {

VideoSettingsDialog::VideoSettingsDialog(Application& application)
    : SettingsDialog(application, _("Video settings"))
{
}

}

// ---------------------------------------------------------------------------
// base::InputSettingsDialog
// ---------------------------------------------------------------------------

namespace base {

InputSettingsDialog::InputSettingsDialog(Application& application)
    : SettingsDialog(application, _("Input settings"))
{
}

}

// ---------------------------------------------------------------------------
// base::HelpDialog
// ---------------------------------------------------------------------------

namespace base {

HelpDialog::HelpDialog(Application& application)
    : Dialog(application, _("Help"))
{
}

}

// ---------------------------------------------------------------------------
// base::AboutDialog
// ---------------------------------------------------------------------------

namespace base {

AboutDialog::AboutDialog(Application& application)
    : Dialog(application, _("About Xcpc"))
{
}

}

// ---------------------------------------------------------------------------
// base::ScopedOperation
// ---------------------------------------------------------------------------

namespace base {

ScopedOperation::ScopedOperation(Application& application)
    : _application(application)
{
}

}

// ---------------------------------------------------------------------------
// base::ScopedPause
// ---------------------------------------------------------------------------

namespace base {

ScopedPause::ScopedPause(Application& application)
    : ScopedOperation(application)
{
    _application.pause_emulator();
}

ScopedPause::~ScopedPause()
{
    _application.play_emulator();
}

}

// ---------------------------------------------------------------------------
// base::ScopedReset
// ---------------------------------------------------------------------------

namespace base {

ScopedReset::ScopedReset(Application& application)
    : ScopedOperation(application)
{
    _application.reset_emulator();
}

ScopedReset::~ScopedReset()
{
    _application.reset_emulator();
}

}

// ---------------------------------------------------------------------------
// Xcpc
// ---------------------------------------------------------------------------

Xcpc::Xcpc(int& argc, char**& argv)
    : _argc(argc)
    , _argv(argv)
{
    ::xcpc_begin();
}

Xcpc::~Xcpc()
{
    ::xcpc_end();
}

auto Xcpc::main() const -> int
{
    return ::xcpc_main(&_argc, &_argv);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const Xcpc xcpc(argc, argv);

    return xcpc.main();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
