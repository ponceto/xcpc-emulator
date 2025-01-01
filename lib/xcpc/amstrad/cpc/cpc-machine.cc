/*
 * cpc-machine.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include <xcpc/libxcpc-priv.h>
#include "cpc-machine.h"

// ---------------------------------------------------------------------------
// cpc::Machine
// ---------------------------------------------------------------------------

namespace cpc {

Machine::Machine(Settings& settings)
    : xcpc::Machine()
    , _audio()
    , _backend()
    , _mainboard(*this, settings)
{
    _backend.instance          = this;
    _backend.on_reset          = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_reset(*event);          };
    _backend.on_clock          = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_clock(*event);          };
    _backend.on_create_window  = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_create_window(*event);  };
    _backend.on_delete_window  = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_delete_window(*event);  };
    _backend.on_resize_window  = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_resize_window(*event);  };
    _backend.on_expose_window  = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_expose_window(*event);  };
    _backend.on_key_press      = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_key_press(*event);      };
    _backend.on_key_release    = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_key_release(*event);    };
    _backend.on_button_press   = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_button_press(*event);   };
    _backend.on_button_release = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_button_release(*event); };
    _backend.on_motion_notify  = [](void* instance, Event* event) -> unsigned long { return reinterpret_cast<Machine*>(instance)->_mainboard.on_motion_notify(*event);  };
    _audio.start();
}

Machine::~Machine()
{
    _audio.stop();
    _backend.instance          = nullptr;
    _backend.on_reset          = nullptr;
    _backend.on_clock          = nullptr;
    _backend.on_create_window  = nullptr;
    _backend.on_delete_window  = nullptr;
    _backend.on_resize_window  = nullptr;
    _backend.on_expose_window  = nullptr;
    _backend.on_key_press      = nullptr;
    _backend.on_key_release    = nullptr;
    _backend.on_button_press   = nullptr;
    _backend.on_button_release = nullptr;
    _backend.on_motion_notify  = nullptr;
}

auto Machine::play() -> void
{
    _mainboard.play();
}

auto Machine::pause() -> void
{
    _mainboard.pause();
}

auto Machine::reset() -> void
{
    _mainboard.reset();
}

auto Machine::clock() -> void
{
    _mainboard.clock();
}

auto Machine::load_snapshot(const std::string& filename) -> void
{
    return _mainboard.load_snapshot(filename);
}

auto Machine::save_snapshot(const std::string& filename) -> void
{
    return _mainboard.save_snapshot(filename);
}

auto Machine::create_disk_into_drive0(const std::string& filename) -> void
{
    return _mainboard.create_disk_into_drive0(filename);
}

auto Machine::insert_disk_into_drive0(const std::string& filename) -> void
{
    return _mainboard.insert_disk_into_drive0(filename);
}

auto Machine::remove_disk_from_drive0() -> void
{
    return _mainboard.remove_disk_from_drive0();
}

auto Machine::create_disk_into_drive1(const std::string& filename) -> void
{
    return _mainboard.create_disk_into_drive1(filename);
}

auto Machine::insert_disk_into_drive1(const std::string& filename) -> void
{
    return _mainboard.insert_disk_into_drive1(filename);
}

auto Machine::remove_disk_from_drive1() -> void
{
    return _mainboard.remove_disk_from_drive1();
}

auto Machine::set_volume(const float volume) -> void
{
    return _mainboard.set_volume(volume);
}

auto Machine::set_scanlines(const bool scanlines) -> void
{
    return _mainboard.set_scanlines(scanlines);
}

auto Machine::set_company_name(const std::string& company_name) -> void
{
    return _mainboard.set_company_name(company_name);
}

auto Machine::set_machine_type(const std::string& machine_type) -> void
{
    return _mainboard.set_machine_type(machine_type);
}

auto Machine::set_monitor_type(const std::string& monitor_type) -> void
{
    return _mainboard.set_monitor_type(monitor_type);
}

auto Machine::set_refresh_rate(const std::string& refresh_rate) -> void
{
    return _mainboard.set_refresh_rate(refresh_rate);
}

auto Machine::set_keyboard_type(const std::string& keyboard_type) -> void
{
    return _mainboard.set_keyboard_type(keyboard_type);
}

auto Machine::get_volume() const -> float
{
    return _mainboard.get_volume();
}

auto Machine::get_system_info() const -> std::string
{
    return _mainboard.get_system_info();
}

auto Machine::get_company_name() const -> std::string
{
    return _mainboard.get_company_name();
}

auto Machine::get_machine_type() const -> std::string
{
    return _mainboard.get_machine_type();
}

auto Machine::get_memory_size() const -> std::string
{
    return _mainboard.get_memory_size();
}

auto Machine::get_monitor_type() const -> std::string
{
    return _mainboard.get_monitor_type();
}

auto Machine::get_refresh_rate() const -> std::string
{
    return _mainboard.get_refresh_rate();
}

auto Machine::get_keyboard_type() const -> std::string
{
    return _mainboard.get_keyboard_type();
}

auto Machine::get_drive0_filename() const -> std::string
{
    return _mainboard.get_drive0_filename();
}

auto Machine::get_drive1_filename() const -> std::string
{
    return _mainboard.get_drive1_filename();
}

auto Machine::get_statistics() const -> std::string
{
    return _mainboard.get_statistics();
}

auto Machine::get_backend() const -> const Backend*
{
    return &_backend;
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
