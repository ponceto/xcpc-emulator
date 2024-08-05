/*
 * libxcpc-cxx.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_LIBXCPC_CXX_H__
#define __XCPC_LIBXCPC_CXX_H__

#include <xcpc/libxcpc.h>
#include <miniaudio/miniaudio.h>

// ---------------------------------------------------------------------------
// some types aliases
// ---------------------------------------------------------------------------

namespace xcpc {

using LogLevel                  = XcpcLogLevel;
using CompanyName               = XcpcCompanyName;
using MachineType               = XcpcMachineType;
using MonitorType               = XcpcMonitorType;
using RefreshRate               = XcpcRefreshRate;
using KeyboardType              = XcpcKeyboardType;
using MemorySize                = XcpcMemorySize;
using Backend                   = XcpcBackend;
using BackendHandler            = XcpcBackendHandler;
using BackendClosure            = XcpcBackendClosure;
using BackendAnyEvent           = XcpcBackendAnyEvent;
using BackendClockEvent         = XcpcBackendClockEvent;
using BackendCreateWindowEvent  = XcpcBackendCreateWindowEvent;
using BackendDeleteWindowEvent  = XcpcBackendDeleteWindowEvent;
using BackendResizeWindowEvent  = XcpcBackendResizeWindowEvent;
using BackendExposeWindowEvent  = XcpcBackendExposeWindowEvent;
using BackendKeyPressEvent      = XcpcBackendKeyPressEvent;
using BackendKeyReleaseEvent    = XcpcBackendKeyReleaseEvent;
using BackendButtonPressEvent   = XcpcBackendButtonPressEvent;
using BackendButtonReleaseEvent = XcpcBackendButtonReleaseEvent;
using BackendMotionNotifyEvent  = XcpcBackendMotionNotifyEvent;
using AudioDeviceType           = ma_device_type;
using MiniAudioConfig           = ma_device_config;
using MiniAudioDevice           = ma_device;
using Mutex                     = std::mutex;
using MutexLock                 = std::unique_lock<std::mutex>;

}

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace xcpc {

class Machine;
class Mainboard;
class Device;
class Peripheral;
class Settings;
class AudioConfig;
class AudioDevice;
class AudioProcessor;

}

// ---------------------------------------------------------------------------
// xcpc::Utils
// ---------------------------------------------------------------------------

namespace xcpc {

struct Utils
{
    static auto get_major_version() -> int;

    static auto get_minor_version() -> int;

    static auto get_micro_version() -> int;

    static auto get_version() -> std::string;

    static auto get_copyright() -> std::string;

    static auto get_comments() -> std::string;

    static auto get_website() -> std::string;

    static auto get_license() -> std::string;

    static auto get_bindir() -> std::string;

    static auto get_libdir() -> std::string;

    static auto get_datdir() -> std::string;

    static auto get_docdir() -> std::string;

    static auto get_resdir() -> std::string;

    static auto get_romdir() -> std::string;

    static auto get_dskdir() -> std::string;

    static auto get_snadir() -> std::string;

    static auto get_joystick0() -> std::string;

    static auto get_joystick1() -> std::string;

    static auto get_loglevel() -> int;

    static auto set_loglevel(const int loglevel) -> int;

    static auto company_name_from_string(const std::string& string) -> CompanyName;

    static auto machine_type_from_string(const std::string& string) -> MachineType;

    static auto monitor_type_from_string(const std::string& string) -> MonitorType;

    static auto refresh_rate_from_string(const std::string& string) -> RefreshRate;

    static auto keyboard_type_from_string(const std::string& string) -> KeyboardType;

    static auto memory_size_from_string(const std::string& string) -> MemorySize;

    static auto company_name_to_string(const CompanyName value) -> std::string;

    static auto machine_type_to_string(const MachineType value) -> std::string;

    static auto monitor_type_to_string(const MonitorType value) -> std::string;

    static auto refresh_rate_to_string(const RefreshRate value) -> std::string;

    static auto keyboard_type_to_string(const KeyboardType value) -> std::string;

    static auto memory_size_to_string(const MemorySize value) -> std::string;
};

}

// ---------------------------------------------------------------------------
// xcpc::Machine
// ---------------------------------------------------------------------------

namespace xcpc {

class Machine
{
public: // public interface
    Machine() = default;

    Machine(const Machine&) = delete;

    Machine& operator=(const Machine&) = delete;

    virtual ~Machine() = default;

    virtual auto play() -> void = 0;

    virtual auto pause() -> void = 0;

    virtual auto reset() -> void = 0;

    virtual auto clock() -> void = 0;
};

}

// ---------------------------------------------------------------------------
// xcpc::Mainboard
// ---------------------------------------------------------------------------

namespace xcpc {

class Mainboard
{
public: // public interface
    Mainboard() = default;

    Mainboard(const Mainboard&) = delete;

    Mainboard& operator=(const Mainboard&) = delete;

    virtual ~Mainboard() = default;

    virtual auto play() -> void = 0;

    virtual auto pause() -> void = 0;

    virtual auto reset() -> void = 0;

    virtual auto clock() -> void = 0;
};

}

// ---------------------------------------------------------------------------
// xcpc::Device
// ---------------------------------------------------------------------------

namespace xcpc {

class Device
{
public: // public interface
    Device() = default;

    Device(const Device&) = delete;

    Device& operator=(const Device&) = delete;

    virtual ~Device() = default;

    virtual auto reset() -> void = 0;

    virtual auto clock() -> void = 0;
};

}

// ---------------------------------------------------------------------------
// xcpc::Peripheral
// ---------------------------------------------------------------------------

namespace xcpc {

class Peripheral
{
public: // public interface
    Peripheral() = default;

    Peripheral(const Peripheral&) = delete;

    Peripheral& operator=(const Peripheral&) = delete;

    virtual ~Peripheral() = default;

    virtual auto reset() -> void = 0;

    virtual auto clock() -> void = 0;
};

}

// ---------------------------------------------------------------------------
// xcpc::Settings
// ---------------------------------------------------------------------------

namespace xcpc {

class Settings
{
public: // public interface
    Settings() = default;

    Settings(const Settings&) = delete;

    Settings& operator=(const Settings&) = delete;

    virtual ~Settings() = default;
};

}

// ---------------------------------------------------------------------------
// xcpc::MonoFrame<T>
// ---------------------------------------------------------------------------

namespace xcpc {

template <typename T>
struct MonoFrame
{
    T mono;
};

using MonoFrameInt16 = MonoFrame<int16_t>;
using MonoFrameInt32 = MonoFrame<int32_t>;
using MonoFrameFlt32 = MonoFrame<float>;

static_assert(sizeof(MonoFrameInt16) == 2);
static_assert(sizeof(MonoFrameInt32) == 4);
static_assert(sizeof(MonoFrameFlt32) == 4);

}

// ---------------------------------------------------------------------------
// xcpc::StereoFrame<T>
// ---------------------------------------------------------------------------

namespace xcpc {

template <typename T>
struct StereoFrame
{
    T left;
    T right;
};

using StereoFrameInt16 = StereoFrame<int16_t>;
using StereoFrameInt32 = StereoFrame<int32_t>;
using StereoFrameFlt32 = StereoFrame<float>;

static_assert(sizeof(StereoFrameInt16) == 4);
static_assert(sizeof(StereoFrameInt32) == 8);
static_assert(sizeof(StereoFrameFlt32) == 8);

}

// ---------------------------------------------------------------------------
// xcpc::AudioConfig
// ---------------------------------------------------------------------------

namespace xcpc {

class AudioConfig
{
public: // public interface
    AudioConfig(const AudioDeviceType type);

   ~AudioConfig() = default;

    auto get() -> MiniAudioConfig*
    {
        return &_impl;
    }

    auto operator->() -> MiniAudioConfig*
    {
        return get();
    }

private: // private data
    MiniAudioConfig _impl;
};

}

// ---------------------------------------------------------------------------
// xcpc::AudioDevice
// ---------------------------------------------------------------------------

namespace xcpc {

class AudioDevice
{
public: // public interface
    AudioDevice();

    AudioDevice(const AudioConfig& config);

    AudioDevice(const AudioDevice&) = delete;

    AudioDevice& operator=(const AudioDevice&) = delete;

    virtual ~AudioDevice();

    void start();

    void stop();

    void attach(AudioProcessor& processor);

    void detach(AudioProcessor& processor);

    auto get() const -> const MiniAudioDevice*
    {
        return &_impl;
    }

    auto operator->() const -> const MiniAudioDevice*
    {
        return get();
    }

private: // private data
    MiniAudioDevice _impl;
    AudioProcessor* _processor;
};

}

// ---------------------------------------------------------------------------
// xcpc::AudioProcessor
// ---------------------------------------------------------------------------

namespace xcpc {

class AudioProcessor
{
public: // public interface
    AudioProcessor(AudioDevice& device);

    AudioProcessor(const AudioProcessor&) = delete;

    AudioProcessor& operator=(const AudioProcessor&) = delete;

    virtual ~AudioProcessor();

    virtual void process(const void* input, void* output, const uint32_t count) = 0;

protected: // protected data
    AudioDevice& _device;
    Mutex        _mutex;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_LIBXCPC_CXX_H__ */
