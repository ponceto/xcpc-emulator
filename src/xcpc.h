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

class Emulator;
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
// base::Emulator
// ---------------------------------------------------------------------------

namespace base {

class Emulator
{
public: // public interface
    Emulator(int& argc, char**& argv);

    Emulator(const Emulator&) = delete;

    Emulator& operator=(const Emulator&) = delete;

    virtual ~Emulator() = default;

    auto ready() -> bool;

    auto reset() -> void;

public: // public interface
    auto load_snapshot(const std::string& filename) const -> void;

    auto save_snapshot(const std::string& filename) const -> void;

    auto create_disk_into_drive0(const std::string& filename) const -> void;

    auto insert_disk_into_drive0(const std::string& filename) const -> void;

    auto remove_disk_from_drive0() const -> void;

    auto create_disk_into_drive1(const std::string& filename) const -> void;

    auto insert_disk_into_drive1(const std::string& filename) const -> void;

    auto remove_disk_from_drive1() const -> void;

    auto set_volume(const float volume) const -> void;

    auto set_scanlines(const bool scanlines) const -> void;

    auto set_monitor_type(const std::string& monitor_type) const -> void;

    auto set_refresh_rate(const std::string& refresh_rate) const -> void;

    auto set_keyboard_type(const std::string& keyboard_type) const -> void;

    auto get_volume() const -> float;

    auto get_system_info() const -> std::string;

    auto get_company_name() const -> std::string;

    auto get_machine_type() const -> std::string;

    auto get_memory_size() const -> std::string;

    auto get_monitor_type() const -> std::string;

    auto get_refresh_rate() const -> std::string;

    auto get_keyboard_type() const -> std::string;

    auto get_drive0_filename() const -> std::string;

    auto get_drive1_filename() const -> std::string;

    auto get_statistics() const -> std::string;

    auto get_backend() const -> const xcpc::Backend*;

private: // private declarations
    using SettingsPtr = std::unique_ptr<cpc::Settings>;
    using MachinePtr  = std::unique_ptr<cpc::Machine>;

private: // private data
    const SettingsPtr _settings;
    const MachinePtr  _machine;
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

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    virtual ~Application() = default;

    virtual int main() = 0;

    auto argc() -> int&
    {
        return _argc;
    };

    auto argv() -> char**&
    {
        return _argv;
    };

    auto ready() -> bool
    {
        return _emulator.ready();
    };

public: // public methods
    virtual void load_snapshot(const std::string& filename) = 0;

    virtual void save_snapshot(const std::string& filename) = 0;

    virtual void exit() = 0;

    virtual void play_emulator() = 0;

    virtual void pause_emulator() = 0;

    virtual void reset_emulator() = 0;

    virtual void create_disk_into_drive0(const std::string& filename) = 0;

    virtual void insert_disk_into_drive0(const std::string& filename) = 0;

    virtual void remove_disk_from_drive0() = 0;

    virtual void create_disk_into_drive1(const std::string& filename) = 0;

    virtual void insert_disk_into_drive1(const std::string& filename) = 0;

    virtual void remove_disk_from_drive1() = 0;

    virtual void increase_volume(const float value) = 0;

    virtual void decrease_volume(const float value) = 0;

    virtual void set_scanlines(const bool scanlines) = 0;

    virtual void set_monitor_type(const std::string& monitor_type) = 0;

    virtual void set_refresh_rate(const std::string& refresh_rate) = 0;

    virtual void set_keyboard_type(const std::string& keyboard_type) = 0;

    virtual void set_joystick0(const std::string& device) = 0;

    virtual void set_joystick1(const std::string& device) = 0;

public: // public signals
    virtual void on_startup() = 0;

    virtual void on_shutdown() = 0;

    virtual void on_statistics() = 0;

    virtual void on_load_snapshot() = 0;

    virtual void on_save_snapshot() = 0;

    virtual void on_exit() = 0;

    virtual void on_play_emulator() = 0;

    virtual void on_pause_emulator() = 0;

    virtual void on_reset_emulator() = 0;

    virtual void on_color_monitor() = 0;

    virtual void on_green_monitor() = 0;

    virtual void on_gray_monitor() = 0;

    virtual void on_refresh_50hz() = 0;

    virtual void on_refresh_60hz() = 0;

    virtual void on_english_keyboard() = 0;

    virtual void on_french_keyboard() = 0;

    virtual void on_german_keyboard() = 0;

    virtual void on_spanish_keyboard() = 0;

    virtual void on_danish_keyboard() = 0;

    virtual void on_create_disk_into_drive0() = 0;

    virtual void on_insert_disk_into_drive0() = 0;

    virtual void on_remove_disk_from_drive0() = 0;

    virtual void on_create_disk_into_drive1() = 0;

    virtual void on_insert_disk_into_drive1() = 0;

    virtual void on_remove_disk_from_drive1() = 0;

    virtual void on_increase_volume() = 0;

    virtual void on_decrease_volume() = 0;

    virtual void on_enable_scanlines() = 0;

    virtual void on_disable_scanlines() = 0;

    virtual void on_joystick0() = 0;

    virtual void on_joystick1() = 0;

    virtual void on_help() = 0;

    virtual void on_about() = 0;

protected: // protected interface
    virtual void run_dialog(Dialog&);

protected: // protected data
    Emulator _emulator;
    int&     _argc;
    char**&  _argv;
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

    virtual void run() = 0;

protected: // protected methods
    void load_snapshot(const std::string& filename);

    void save_snapshot(const std::string& filename);

    void exit();

    void play_emulator();

    void pause_emulator();

    void reset_emulator();

    void create_disk_into_drive0(const std::string& filename);

    void insert_disk_into_drive0(const std::string& filename);

    void remove_disk_from_drive0();

    void create_disk_into_drive1(const std::string& filename);

    void insert_disk_into_drive1(const std::string& filename);

    void remove_disk_from_drive1();

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
    void create_disk(const std::string& filename);

    void insert_disk(const std::string& filename);

    void remove_disk();

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

    virtual int main() const;

private: // private data
    int&    _argc;
    char**& _argv;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_H__ */
