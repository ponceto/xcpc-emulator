/*
 * cpc-machine.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_CPC_MACHINE_H__
#define __XCPC_CPC_MACHINE_H__

#include <xcpc/amstrad/cpc/cpc-settings.h>
#include <xcpc/amstrad/cpc/cpc-mainboard.h>

// ---------------------------------------------------------------------------
// cpc::Machine
// ---------------------------------------------------------------------------

namespace cpc {

class Machine final
    : public xcpc::Machine
{
public: // public interface
    Machine(Settings& settings);

    Machine(const Machine&) = delete;

    Machine& operator=(const Machine&) = delete;

    virtual ~Machine();

    virtual auto play() -> void override final;

    virtual auto pause() -> void override final;

    virtual auto reset() -> void override final;

    virtual auto clock() -> void override final;

public: // public interface
    auto load_snapshot(const std::string& filename) -> void;

    auto save_snapshot(const std::string& filename) -> void;

    auto create_disk_into_drive0(const std::string& filename) -> void;

    auto insert_disk_into_drive0(const std::string& filename) -> void;

    auto remove_disk_from_drive0() -> void;

    auto create_disk_into_drive1(const std::string& filename) -> void;

    auto insert_disk_into_drive1(const std::string& filename) -> void;

    auto remove_disk_from_drive1() -> void;

    auto set_volume(const float volume) -> void;

    auto set_scanlines(const bool scanlines) -> void;

    auto set_company_name(const std::string& company_name) -> void;

    auto set_machine_type(const std::string& machine_type) -> void;

    auto set_monitor_type(const std::string& monitor_type) -> void;

    auto set_refresh_rate(const std::string& refresh_rate) -> void;

    auto set_keyboard_type(const std::string& keyboard_type) -> void;

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

    auto get_backend() const -> const Backend*;

    auto get_audio_device() -> AudioDevice&
    {
        return _audio;
    }

private: // private data
    AudioDevice _audio;
    Backend     _backend;
    Mainboard   _mainboard;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_CPC_MACHINE_H__ */
