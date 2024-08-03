/*
 * cpc-settings.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_CPC_SETTINGS_H__
#define __XCPC_CPC_SETTINGS_H__

#include <xcpc/libxcpc-cxx.h>

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace cpc {

class Machine;
class Mainboard;
class Settings;

using Utils            = xcpc::Utils;
using Backend          = xcpc::Backend;
using BackendClosure   = xcpc::BackendClosure;
using CompanyName      = xcpc::CompanyName;
using MachineType      = xcpc::MachineType;
using MonitorType      = xcpc::MonitorType;
using RefreshRate      = xcpc::RefreshRate;
using KeyboardType     = xcpc::KeyboardType;
using MemorySize       = xcpc::MemorySize;
using Mutex            = xcpc::Mutex;
using MutexLock        = xcpc::MutexLock;
using AudioConfig      = xcpc::AudioConfig;
using AudioDevice      = xcpc::AudioDevice;
using AudioProcessor   = xcpc::AudioProcessor;
using MonoFrameInt16   = xcpc::MonoFrameInt16;
using MonoFrameInt32   = xcpc::MonoFrameInt32;
using MonoFrameFlt32   = xcpc::MonoFrameFlt32;
using StereoFrameInt16 = xcpc::StereoFrameInt16;
using StereoFrameInt32 = xcpc::StereoFrameInt32;
using StereoFrameFlt32 = xcpc::StereoFrameFlt32;

}

// ---------------------------------------------------------------------------
// cpc::Settings
// ---------------------------------------------------------------------------

namespace cpc {

class Settings final
    : public xcpc::Settings
{
public: // public interface
    Settings();

    Settings(int& argc, char**& argv);

    Settings(const Settings&) = delete;

    Settings& operator=(const Settings&) = delete;

    virtual ~Settings() = default;

    void parse(int& argc, char**& argv);

    void usage();

    void version();

    bool quit();

public: // public data
    std::string opt_program;
    std::string opt_company;
    std::string opt_machine;
    std::string opt_monitor;
    std::string opt_refresh;
    std::string opt_keyboard;
    std::string opt_memory;
    std::string opt_sysrom;
    std::string opt_rom000;
    std::string opt_rom001;
    std::string opt_rom002;
    std::string opt_rom003;
    std::string opt_rom004;
    std::string opt_rom005;
    std::string opt_rom006;
    std::string opt_rom007;
    std::string opt_rom008;
    std::string opt_rom009;
    std::string opt_rom010;
    std::string opt_rom011;
    std::string opt_rom012;
    std::string opt_rom013;
    std::string opt_rom014;
    std::string opt_rom015;
    std::string opt_drive0;
    std::string opt_drive1;
    std::string opt_snapshot;
    std::string opt_speedup;
    bool        opt_xshm;
    bool        opt_scanlines;
    bool        opt_help;
    bool        opt_version;
    int         opt_loglevel;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_CPC_SETTINGS_H__ */
