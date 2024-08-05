/*
 * xcpc.h - Copyright (c) 2001-2024 - Olivier Poncet
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

// ---------------------------------------------------------------------------
// translation_traits
// ---------------------------------------------------------------------------

struct translation_traits
{
    static const char* translate(const char* string)
    {
        return string;
    }

};

// ---------------------------------------------------------------------------
// translation macro
// ---------------------------------------------------------------------------

#ifndef _
#define _(string) translation_traits::translate(string)
#endif

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace base {

class Application;
class Dialog;
class SnapshotDialog;
class LoadSnapshotDialog;
class SaveSnapshotDialog;
class DiskDialog;
class CreateDiskDialog;
class InsertDiskDialog;
class RemoveDiskDialog;
class HelpDialog;
class AboutDialog;
class ScopedOperation;
class ScopedPause;
class ScopedReset;

}

// ---------------------------------------------------------------------------
// base::Application
// ---------------------------------------------------------------------------

namespace base {

class Application
{
public: // public interface
    Application(int& argc, char**& argv);

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    virtual ~Application() = default;

    virtual auto main() ->int = 0;

    auto get_backend() const -> const xcpc::Backend*
    {
        return _machine->get_backend();
    }

public: // public methods
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

    virtual auto set_scanlines(const bool scanlines) -> void = 0;

    virtual auto set_company_name(const std::string& company_name) -> void = 0;

    virtual auto set_monitor_type(const std::string& monitor_type) -> void = 0;

    virtual auto set_refresh_rate(const std::string& refresh_rate) -> void = 0;

    virtual auto set_keyboard_type(const std::string& keyboard_type) -> void = 0;

    virtual auto set_joystick0(const std::string& device) -> void = 0;

    virtual auto set_joystick1(const std::string& device) -> void = 0;

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

    virtual auto on_drive0_create_disk() -> void = 0;

    virtual auto on_drive0_insert_disk() -> void = 0;

    virtual auto on_drive0_remove_disk() -> void = 0;

    virtual auto on_drive1_create_disk() -> void = 0;

    virtual auto on_drive1_insert_disk() -> void = 0;

    virtual auto on_drive1_remove_disk() -> void = 0;

    virtual auto on_volume_increase() -> void = 0;

    virtual auto on_volume_decrease() -> void = 0;

    virtual auto on_scanlines_enable() -> void = 0;

    virtual auto on_scanlines_disable() -> void = 0;

    virtual auto on_joystick0() -> void = 0;

    virtual auto on_joystick1() -> void = 0;

    virtual auto on_help() -> void = 0;

    virtual auto on_about() -> void = 0;

protected: // protected interface
    using SettingsPtr = std::unique_ptr<cpc::Settings>;
    using MachinePtr  = std::unique_ptr<cpc::Machine>;

    virtual auto run_dialog(Dialog&) -> void;

protected: // protected data
    int&              _argc;
    char**&           _argv;
    const SettingsPtr _settings;
    const MachinePtr  _machine;
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

    Dialog(const Dialog&) = delete;

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

    SnapshotDialog(const SnapshotDialog&) = delete;

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

    LoadSnapshotDialog(const LoadSnapshotDialog&) = delete;

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

    SaveSnapshotDialog(const SaveSnapshotDialog&) = delete;

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

    DiskDialog(const DiskDialog&) = delete;

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

    CreateDiskDialog(const CreateDiskDialog&) = delete;

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

    InsertDiskDialog(const InsertDiskDialog&) = delete;

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

    RemoveDiskDialog(const RemoveDiskDialog&) = delete;

    RemoveDiskDialog& operator=(const RemoveDiskDialog&) = delete;

    virtual ~RemoveDiskDialog() = default;
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

    HelpDialog(const HelpDialog&) = delete;

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

    AboutDialog(const AboutDialog&) = delete;

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
    ScopedOperation(Application& application)
        : _application(application)
    {
    }

    ScopedOperation(const ScopedOperation&) = delete;

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
    ScopedPause(Application& application)
        : ScopedOperation(application)
    {
        enter_pause();
    }

    virtual ~ScopedPause()
    {
        leave_pause();
    }

protected: // protected interface
    auto enter_pause() -> void
    {
        return _application.pause_emulator();
    };

    auto leave_pause() -> void
    {
        return _application.play_emulator();
    };
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
    ScopedReset(Application& application)
        : ScopedOperation(application)
    {
        enter_reset();
    }

    virtual ~ScopedReset()
    {
        leave_reset();
    }

protected: // protected interface
    auto enter_reset() -> void
    {
        return _application.reset_emulator();
    };

    auto leave_reset() -> void
    {
        return _application.reset_emulator();
    };
};

}

// ---------------------------------------------------------------------------
// Xcpc
// ---------------------------------------------------------------------------

class Xcpc
{
public: // public interface
    Xcpc(int& argc, char**& argv);

    Xcpc(const Xcpc&) = delete;

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
