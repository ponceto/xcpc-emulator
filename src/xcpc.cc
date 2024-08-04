/*
 * xcpc.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
// base::Emulator
// ---------------------------------------------------------------------------

namespace base {

Emulator::Emulator(int& argc, char**& argv)
    : _settings(new cpc::Settings(argc, argv))
    , _machine(new cpc::Machine(*_settings))
{
}

auto Emulator::ready() -> bool
{
    if(_settings->quit()) {
        return false;
    }
    return true;
}

auto Emulator::reset() -> void
{
    return _machine->reset();
}

auto Emulator::load_snapshot(const std::string& filename) const -> void
{
    return _machine->load_snapshot(filename);
}

auto Emulator::save_snapshot(const std::string& filename) const -> void
{
    return _machine->save_snapshot(filename);
}

auto Emulator::create_disk_into_drive0(const std::string& filename) const -> void
{
    return _machine->create_disk_into_drive0(filename);
}

auto Emulator::insert_disk_into_drive0(const std::string& filename) const -> void
{
    return _machine->insert_disk_into_drive0(filename);
}

auto Emulator::remove_disk_from_drive0() const -> void
{
    return _machine->remove_disk_from_drive0();
}

auto Emulator::create_disk_into_drive1(const std::string& filename) const -> void
{
    return _machine->create_disk_into_drive1(filename);
}

auto Emulator::insert_disk_into_drive1(const std::string& filename) const -> void
{
    return _machine->insert_disk_into_drive1(filename);
}

auto Emulator::remove_disk_from_drive1() const -> void
{
    return _machine->remove_disk_from_drive1();
}

auto Emulator::set_volume(const float volume) const -> void
{
    return _machine->set_volume(volume);
}

auto Emulator::set_scanlines(const bool scanlines) const -> void
{
    return _machine->set_scanlines(scanlines);
}

auto Emulator::set_company_name(const std::string& company_name) const -> void
{
    return _machine->set_company_name(company_name);
}

auto Emulator::set_monitor_type(const std::string& monitor_type) const -> void
{
    return _machine->set_monitor_type(monitor_type);
}

auto Emulator::set_refresh_rate(const std::string& refresh_rate) const -> void
{
    return _machine->set_refresh_rate(refresh_rate);
}

auto Emulator::set_keyboard_type(const std::string& keyboard_type) const -> void
{
    return _machine->set_keyboard_type(keyboard_type);
}

auto Emulator::get_volume() const -> float
{
    return _machine->get_volume();
}

auto Emulator::get_system_info() const -> std::string
{
    return _machine->get_system_info();
}

auto Emulator::get_company_name() const -> std::string
{
    return _machine->get_company_name();
}

auto Emulator::get_machine_type() const -> std::string
{
    return _machine->get_machine_type();
}

auto Emulator::get_memory_size() const -> std::string
{
    return _machine->get_memory_size();
}

auto Emulator::get_monitor_type() const -> std::string
{
    return _machine->get_monitor_type();
}

auto Emulator::get_refresh_rate() const -> std::string
{
    return _machine->get_refresh_rate();
}

auto Emulator::get_keyboard_type() const -> std::string
{
    return _machine->get_keyboard_type();
}

auto Emulator::get_drive0_filename() const -> std::string
{
    return _machine->get_drive0_filename();
}

auto Emulator::get_drive1_filename() const -> std::string
{
    return _machine->get_drive1_filename();
}

auto Emulator::get_statistics() const -> std::string
{
    return _machine->get_statistics();
}

auto Emulator::get_backend() const -> const xcpc::Backend*
{
    return _machine->get_backend();
}

}

// ---------------------------------------------------------------------------
// base::Application
// ---------------------------------------------------------------------------

namespace base {

Application::Application(int& argc, char**& argv)
    : _emulator(argc, argv)
    , _argc(argc)
    , _argv(argv)
{
}

void Application::run_dialog(Dialog& dialog)
{
    const base::ScopedPause pause(*this);

    return dialog.run();
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
