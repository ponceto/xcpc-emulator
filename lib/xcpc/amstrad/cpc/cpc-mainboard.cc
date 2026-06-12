/*
 * cpc-mainboard.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <cmath>
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
#include "cpc-mainboard.h"

// ---------------------------------------------------------------------------
// <anonymous>::Traits
// ---------------------------------------------------------------------------

namespace {

struct Traits
{
    using TimeVal   = cpc::TimeVal;
    using Mainboard = cpc::Mainboard;
    using Setup     = cpc::Mainboard::Setup;
    using Stats     = cpc::Mainboard::Stats;
    using Clock     = cpc::Mainboard::Clock;
    using Funcs     = cpc::Mainboard::Funcs;
    using State     = cpc::Mainboard::State;
    using Audio     = cpc::Mainboard::Audio;
    using Video     = cpc::Mainboard::Video;

    static auto gettimeofday(TimeVal& tv) -> void
    {
        if(::gettimeofday(&tv, nullptr) != 0) {
            throw std::runtime_error("gettimeofday() has failed");
        }
    }

    static auto construct(Setup& setup) -> void
    {
        setup.company_name  = XCPC_COMPANY_NAME_UNKNOWN;
        setup.machine_type  = XCPC_MACHINE_TYPE_UNKNOWN;
        setup.monitor_type  = XCPC_MONITOR_TYPE_UNKNOWN;
        setup.refresh_rate  = XCPC_REFRESH_RATE_UNKNOWN;
        setup.keyboard_type = XCPC_KEYBOARD_TYPE_UNKNOWN;
        setup.memory_size   = XCPC_MEMORY_SIZE_UNKNOWN;
        setup.renderer_type = XCPC_RENDERER_TYPE_UNKNOWN;
        setup.speedup       = 1;
        setup.xshm          = true;
        setup.crt_emulation = true;
    }

    static auto construct(Stats& stats) -> void
    {
        stats.frame_count = 0;
        stats.frame_drawn = 0;
        stats.buffer[0]   = 0;
    }

    static auto construct(Clock& clock) -> void
    {
        Traits::gettimeofday(clock.currtime);
        Traits::gettimeofday(clock.deadline);
        Traits::gettimeofday(clock.proftime);
    }

    static auto construct(Funcs& funcs) -> void
    {
        funcs.render_func = [](Mainboard* mainboard) -> void {};
    }

    static auto construct(State& state) -> void
    {
        state.cpc_flags   = 0;
        state.cpc_clock   = 4000000;
        state.cpc_ticks   = 0;
        state.cpu_clock   = 4000000;
        state.cpu_ticks   = 0;
        state.vdc_clock   = 1000000;
        state.vdc_ticks   = 0;
        state.psg_clock   = 1000000;
        state.psg_ticks   = 0;
        state.snd_clock   = 44100;
        state.snd_ticks   = 0;
        state.vdc_hsync   = 0; /* no hsync      */
        state.vdc_vsync   = 0; /* no vsync      */
        state.lnk_lk1     = 1; /* amstrad       */
        state.lnk_lk2     = 1; /* amstrad       */
        state.lnk_lk3     = 1; /* amstrad       */
        state.lnk_lk4     = 1; /* 50Hz          */
        state.exp_busy    = 1; /* not connected */
        state.prt_busy    = 1; /* not connected */
        state.psg_data    = 0; /* no data       */
        state.psg_bdir    = 0; /* no data       */
        state.psg_bc1     = 0; /* no data       */
        state.psg_bc2     = 1; /* always set    */
        state.cas_motor   = 0; /* no data       */
        state.cas_sound   = 0; /* no data       */
        state.cas_rd_data = 0; /* no data       */
        state.cas_wr_data = 0; /* no data       */
        state.ram_conf    = 0; /* default       */
        state.rom_conf    = 0; /* default       */
        for(uint8_t*& pal_rd : state.pal_rd) {
            pal_rd = nullptr;
        }
        for(uint8_t*& pal_wr : state.pal_wr) {
            pal_wr = nullptr;
        }
    }

    static auto construct(Audio& audio) -> void
    {
        for(auto& sample : audio.channel0) {
            sample = 0.0f;
        }
        for(auto& sample : audio.channel1) {
            sample = 0.0f;
        }
        for(auto& sample : audio.channel2) {
            sample = 0.0f;
        }
        audio.volume = 0.5f;
        audio.rd_index = 0;
        audio.wr_index = 0;
        for(auto& value : audio.dcb_input) {
            value = 0.0f;
        }
        for(auto& value : audio.dcb_output) {
            value = 0.0f;
        }
    }

    static auto construct(Video& video) -> void
    {
        video.frame_rate = 50;
        video.frame_time = 20000;
    }

    static auto destruct(Setup& setup) -> void
    {
        setup = Setup();
    }

    static auto destruct(Stats& stats) -> void
    {
        stats = Stats();
    }

    static auto destruct(Clock& clock) -> void
    {
        clock = Clock();
    }

    static auto destruct(Funcs& funcs) -> void
    {
        funcs = Funcs();
    }

    static auto destruct(State& state) -> void
    {
        state = State();
    }

    static auto destruct(Audio& audio) -> void
    {
        audio = Audio();
    }

    static auto destruct(Video& video) -> void
    {
        video = Video();
    }

    static auto reset(Setup& setup) -> void
    {
    }

    static auto reset(Stats& stats) -> void
    {
        stats.frame_count &= 0;
        stats.frame_drawn &= 0;
        stats.buffer[0]   &= 0;
    }

    static auto reset(Clock& clock) -> void
    {
        Traits::gettimeofday(clock.currtime);
        Traits::gettimeofday(clock.deadline);
        Traits::gettimeofday(clock.proftime);
    }

    static auto reset(Funcs& funcs) -> void
    {
    }

    static auto reset(State& state) -> void
    {
        state.cpc_flags   &= 0;
        state.cpc_clock   |= 0;
        state.cpc_ticks   &= 0;
        state.cpu_clock   |= 0;
        state.cpu_ticks   &= 0;
        state.vdc_clock   |= 0;
        state.vdc_ticks   &= 0;
        state.psg_clock   |= 0;
        state.psg_ticks   &= 0;
        state.snd_clock   |= 0;
        state.snd_ticks   &= 0;
        state.vdc_hsync   &= 0;
        state.vdc_vsync   &= 0;
        state.lnk_lk1     |= 0;
        state.lnk_lk2     |= 0;
        state.lnk_lk3     |= 0;
        state.lnk_lk4     |= 0;
        state.exp_busy    |= 0;
        state.prt_busy    |= 0;
        state.psg_data    &= 0;
        state.psg_bdir    &= 0;
        state.psg_bc1     &= 0;
        state.psg_bc2     |= 0;
        state.cas_motor   &= 0;
        state.cas_sound   &= 0;
        state.cas_rd_data &= 0;
        state.cas_wr_data &= 0;
        state.ram_conf    &= 0;
        state.rom_conf    &= 0;
        for(uint8_t*& pal_rd : state.pal_rd) {
            pal_rd = nullptr;
        }
        for(uint8_t*& pal_wr : state.pal_wr) {
            pal_wr = nullptr;
        }
    }

    static auto reset(Audio& audio) -> void
    {
        for(auto& sample : audio.channel0) {
            sample = 0.0f;
        }
        for(auto& sample : audio.channel1) {
            sample = 0.0f;
        }
        for(auto& sample : audio.channel2) {
            sample = 0.0f;
        }
        audio.rd_index &= 0;
        audio.wr_index &= 0;
        for(auto& value : audio.dcb_input) {
            value = 0.0f;
        }
        for(auto& value : audio.dcb_output) {
            value = 0.0f;
        }
    }

    static auto reset(Video& video) -> void
    {
        video.frame_rate |= 0;
        video.frame_time |= 0;
    }

    static auto reset(dpy::Instance* dpy)
    {
        if(dpy != nullptr) {
            dpy->reset();
        }
    }

    static auto reset(kbd::Instance* kbd)
    {
        if(kbd != nullptr) {
            kbd->reset();
        }
    }

    static auto reset(cpu::Instance* cpu)
    {
        if(cpu != nullptr) {
            cpu->reset();
        }
    }

    static auto reset(vga::Instance* vga)
    {
        if(vga != nullptr) {
            vga->reset();
        }
    }

    static auto reset(vdc::Instance* vdc)
    {
        if(vdc != nullptr) {
            vdc->reset();
        }
    }

    static auto reset(ppi::Instance* ppi)
    {
        if(ppi != nullptr) {
            ppi->reset();
        }
    }

    static auto reset(psg::Instance* psg)
    {
        if(psg != nullptr) {
            psg->reset();
        }
    }

    static auto reset(fdc::Instance* fdc)
    {
        if(fdc != nullptr) {
            fdc->reset();
        }
    }

    static auto reset(mem::Instance* mem)
    {
        if(mem != nullptr) {
            mem->reset();
        }
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::HorzProps
// ---------------------------------------------------------------------------

namespace {

struct HorzProps
{
    int cw;  /* h. char width    : pixels */
    int ht;  /* h. total         : chars  */
    int hd;  /* h. displayed     : chars  */
    int hsp; /* h. sync position : chars  */
    int hsw; /* h. sync width    : pixels */
};

}

// ---------------------------------------------------------------------------
// <anonymous>::VertProps
// ---------------------------------------------------------------------------

namespace {

struct VertProps
{
    int ch;  /* v. char height   : pixels */
    int vt;  /* v. total         : chars  */
    int vd;  /* v. displayed     : chars  */
    int vsp; /* v. sync position : chars  */
    int vsw; /* v. sync width    : pixels */
};

}

// ---------------------------------------------------------------------------
// <anonymous>::Borders
// ---------------------------------------------------------------------------

namespace {

struct Borders
{
    int top; /* top border    : pixels */
    int bot; /* bottom border : pixels */
    int lft; /* left border   : pixels */
    int rgt; /* right border  : pixels */
};

}

// ---------------------------------------------------------------------------
// some useful macros
// ---------------------------------------------------------------------------

#define XCPC_TIMESTAMP_OF(tv)  ((static_cast<long long>((tv)->tv_sec) * 1000000LL) + (static_cast<long long>((tv)->tv_usec) * 1LL))
#define XCPC_BYTE_PTR(pointer) (reinterpret_cast<uint8_t*>(pointer))
#define XCPC_WORD_PTR(pointer) (reinterpret_cast<uint16_t*>(pointer))
#define XCPC_LONG_PTR(pointer) (reinterpret_cast<uint32_t*>(pointer))

// ---------------------------------------------------------------------------
// cpc::Mainboard
// ---------------------------------------------------------------------------

namespace cpc {

Mainboard::Mainboard(Machine& machine)
    : xcpc::Mainboard()
    , AudioProcessor(machine.get_audio_device())
    , dpy::Interface()
    , kbd::Interface()
    , cpu::Interface()
    , vga::Interface()
    , vdc::Interface()
    , ppi::Interface()
    , psg::Interface()
    , fdc::Interface()
    , mem::Interface()
    , _machine(machine)
    , _setup()
    , _stats()
    , _clock()
    , _funcs()
    , _state()
    , _audio()
    , _video()
    , _dpy()
    , _kbd()
    , _cpu()
    , _vga()
    , _vdc()
    , _ppi()
    , _psg()
    , _fdc()
    , _ram()
    , _rom()
    , _exp()
{
    Traits::construct(_setup);
    Traits::construct(_stats);
    Traits::construct(_clock);
    Traits::construct(_funcs);
    Traits::construct(_state);
    Traits::construct(_audio);
    Traits::construct(_video);
    if(_dpy == nullptr) {
        _dpy = new dpy::Instance(*this);
    }
    if(_kbd == nullptr) {
        _kbd = new kbd::Instance(*this);
    }
    if(_cpu == nullptr) {
        _cpu = new cpu::Instance(*this);
    }
    if(_vga == nullptr) {
        _vga = new vga::Instance(*this);
    }
    if(_vdc == nullptr) {
        _vdc = new vdc::Instance(*this);
    }
    if(_ppi == nullptr) {
        _ppi = new ppi::Instance(*this);
    }
    if(_psg == nullptr) {
        _psg = new psg::Instance(*this);
    }
    if(_fdc == nullptr) {
        _fdc = new fdc::Instance(*this);
        _fdc->attach_drive(fdc::FDC_DRIVE0);
        _fdc->attach_drive(fdc::FDC_DRIVE1);
    }
    for(auto& ram : _ram) {
        if(ram == nullptr) {
            ram = new mem::Instance(mem::RAM_BANK, *this);
        }
    }
    for(auto& rom : _rom) {
        if(rom == nullptr) {
            rom = new mem::Instance(mem::ROM_BANK, *this);
        }
    }
    for(auto& exp : _exp) {
        if(exp == nullptr) {
            exp = nullptr;
        }
    }
}

Mainboard::Mainboard(Machine& machine, const Settings& settings)
    : Mainboard(machine)
{
    try {
        configure(settings);
    }
    catch(const std::exception& e) {
        ::xcpc_log_error("error while initializing machine: %s", e.what());
    }
}

Mainboard::~Mainboard()
{
    for(auto& exp : _exp) {
        if(exp != nullptr) {
            exp = (delete exp, nullptr);
        }
    }
    for(auto& rom : _rom) {
        if(rom != nullptr) {
            rom = (delete rom, nullptr);
        }
    }
    for(auto& ram : _ram) {
        if(ram != nullptr) {
            ram = (delete ram, nullptr);
        }
    }
    if(_fdc != nullptr) {
        _fdc = (delete _fdc, nullptr);
    }
    if(_psg != nullptr) {
        _psg = (delete _psg, nullptr);
    }
    if(_ppi != nullptr) {
        _ppi = (delete _ppi, nullptr);
    }
    if(_vdc != nullptr) {
        _vdc = (delete _vdc, nullptr);
    }
    if(_vga != nullptr) {
        _vga = (delete _vga, nullptr);
    }
    if(_cpu != nullptr) {
        _cpu = (delete _cpu, nullptr);
    }
    if(_kbd != nullptr) {
        _kbd = (delete _kbd, nullptr);
    }
    if(_dpy != nullptr) {
        _dpy = (delete _dpy, nullptr);
    }
    Traits::destruct(_video);
    Traits::destruct(_audio);
    Traits::destruct(_state);
    Traits::destruct(_funcs);
    Traits::destruct(_clock);
    Traits::destruct(_stats);
    Traits::destruct(_setup);
}

auto Mainboard::play() -> void
{
    _state.cpc_flags &= ~FLAG_PAUSE;
}

auto Mainboard::pause() -> void
{
    _state.cpc_flags |= FLAG_PAUSE;
}

auto Mainboard::reset() -> void
{
    Traits::reset(_setup);
    Traits::reset(_stats);
    Traits::reset(_clock);
    Traits::reset(_funcs);
    Traits::reset(_state);
    Traits::reset(_audio);
    Traits::reset(_video);
    Traits::reset(_dpy);
    Traits::reset(_kbd);
    Traits::reset(_cpu);
    Traits::reset(_vga);
    Traits::reset(_vdc);
    Traits::reset(_ppi);
    Traits::reset(_psg);
    Traits::reset(_fdc);
    for(auto& ram : _ram) {
        Traits::reset(ram);
    }
    for(auto& rom : _rom) {
        Traits::reset(rom);
    }
    for(auto& exp : _exp) {
        Traits::reset(exp);
    }
    update_pal();
}

auto Mainboard::clock() -> void
{
    const MutexLock lock(_mutex);

    auto clock_cpu = [&]() -> void
    {
        if((_state.cpu_ticks += _state.cpu_clock) >= _state.cpc_clock) {
            _state.cpu_ticks -= _state.cpc_clock;
            _cpu->clock();
        }
    };

    auto clock_vdc = [&]() -> void
    {
        if((_state.vdc_ticks += _state.vdc_clock) >= _state.cpc_clock) {
            _state.vdc_ticks -= _state.cpc_clock;
            _vdc->clock();
        }
    };

    auto clock_psg = [&]() -> void
    {
        if((_state.psg_ticks += _state.psg_clock) >= _state.cpc_clock) {
            _state.psg_ticks -= _state.cpc_clock;
            _psg->clock();
        }
    };

    auto dc_block = [&](const int channel, const float input) -> float
    {
        constexpr float attenuation = 0.999f;
        const float output = (input - _audio.dcb_input[channel]) + (attenuation * _audio.dcb_output[channel]);
        _audio.dcb_input[channel]  = input;
        _audio.dcb_output[channel] = output;
        return output;
    };

    auto clock_snd = [&]() -> void
    {
        if((_state.snd_ticks += _state.snd_clock) >= _state.cpc_clock) {
            _state.snd_ticks -= _state.cpc_clock;
            const auto rd_index = ((_audio.rd_index + 0) % SND_BUFSIZE);
            const auto wr_index = ((_audio.wr_index + 1) % SND_BUFSIZE);
            if(wr_index != rd_index) {
                const auto& output = _psg->get_output();
                _audio.channel0[_audio.wr_index] = dc_block(0, output.channel0);
                _audio.channel1[_audio.wr_index] = dc_block(1, output.channel1);
                _audio.channel2[_audio.wr_index] = dc_block(2, output.channel2);
                _audio.wr_index = wr_index;
            }
        }
    };

    auto emulate = [&]() -> void
    {
        if((_state.cpc_flags & FLAG_PAUSE) != 0) {
            return;
        }
        while(_state.cpc_ticks < _state.cpc_clock) {
            _state.cpc_ticks += _video.frame_rate;
            clock_vdc();
            clock_cpu();
            clock_psg();
            clock_snd();
        }
        _state.cpc_ticks -= _state.cpc_clock;
    };

    return emulate();
}

auto Mainboard::load_snapshot(const std::string& filename) -> void
{
    sna::Snapshot snapshot;

    try {
        snapshot.load(filename);
        load_cpc(snapshot);
    }
    catch(...) {
        reset();
        throw;
    }
}

auto Mainboard::save_snapshot(const std::string& filename) -> void
{
    sna::Snapshot snapshot;

    try {
        save_cpc(snapshot);
        snapshot.save(filename);
    }
    catch(...) {
    //  reset();
        throw;
    }
}

auto Mainboard::create_disk_into_drive0(const std::string& filename) -> void
{
    if(filename.empty() == false) {
        dsk::Disk disk(filename);
        disk.create();
    }
    if(_fdc != nullptr) {
        _fdc->insert_disk(fdc::FDC_DRIVE0, filename);
    }
}

auto Mainboard::insert_disk_into_drive0(const std::string& filename) -> void
{
    if(_fdc != nullptr) {
        _fdc->insert_disk(fdc::FDC_DRIVE0, filename);
    }
}

auto Mainboard::remove_disk_from_drive0() -> void
{
    if(_fdc != nullptr) {
        _fdc->remove_disk(fdc::FDC_DRIVE0);
    }
}

auto Mainboard::create_disk_into_drive1(const std::string& filename) -> void
{
    if(filename.empty() == false) {
        dsk::Disk disk(filename);
        disk.create();
    }
    if(_fdc != nullptr) {
        _fdc->insert_disk(fdc::FDC_DRIVE1, filename);
    }
}

auto Mainboard::insert_disk_into_drive1(const std::string& filename) -> void
{
    if(_fdc != nullptr) {
        _fdc->insert_disk(fdc::FDC_DRIVE1, filename);
    }
}

auto Mainboard::remove_disk_from_drive1() -> void
{
    if(_fdc != nullptr) {
        _fdc->remove_disk(fdc::FDC_DRIVE1);
    }
}

auto Mainboard::set_parameterb(const std::string& parameter, bool value) -> void
{
    if(parameter.compare(0, 6, "video.") == 0) {
        if(_dpy != nullptr) {
            _dpy->set_parameterb(parameter, value);
        }
        if(parameter == "video.crt_emulation") {
            const auto old_crt_emulation = _setup.crt_emulation;
            const auto new_crt_emulation = _setup.crt_emulation = value;
            if(new_crt_emulation != old_crt_emulation) {
                update_vga();
            }
        }
    }
}

auto Mainboard::set_parameteri(const std::string& parameter, int value) -> void
{
    if(parameter.compare(0, 6, "video.") == 0) {
        if(_dpy != nullptr) {
            _dpy->set_parameteri(parameter, value);
        }
    }
}

auto Mainboard::set_parameterf(const std::string& parameter, float value) -> void
{
    auto set_volume = [&](float volume) -> void
    {
        _audio.volume = volume;
        if(_audio.volume < 0.0f) {
            _audio.volume = 0.0f;
        }
        if(_audio.volume > 1.0f) {
            _audio.volume = 1.0f;
        }
    };

    if(parameter.compare(0, 6, "audio.") == 0) {
        if(parameter == "audio.volume") {
            set_volume(value);
        }
    }
    if(parameter.compare(0, 6, "video.") == 0) {
        if(_dpy != nullptr) {
            _dpy->set_parameterf(parameter, value);
        }
    }
}

auto Mainboard::set_company_name(const std::string& string) -> void
{
    auto company_name = Utils::company_name_from_string(string);

    auto update = [&](const uint8_t lnk_lk3, const uint8_t lnk_lk2, const uint8_t lnk_lk1) -> void
    {
        if(company_name != _setup.company_name) {
            _setup.company_name = company_name;
            _state.lnk_lk1      = lnk_lk1;
            _state.lnk_lk2      = lnk_lk2;
            _state.lnk_lk3      = lnk_lk3;
            reset();
        }
    };

    if(company_name == XCPC_COMPANY_NAME_DEFAULT) {
        company_name = XCPC_COMPANY_NAME_AMSTRAD;
    }
    switch(company_name) {
        case XCPC_COMPANY_NAME_ISP:
            update(0, 0, 0);
            break;
        case XCPC_COMPANY_NAME_TRIUMPH:
            update(0, 0, 1);
            break;
        case XCPC_COMPANY_NAME_SAISHO:
            update(0, 1, 0);
            break;
        case XCPC_COMPANY_NAME_SOLAVOX:
            update(0, 1, 1);
            break;
        case XCPC_COMPANY_NAME_AWA:
            update(1, 0, 0);
            break;
        case XCPC_COMPANY_NAME_SCHNEIDER:
            update(1, 0, 1);
            break;
        case XCPC_COMPANY_NAME_ORION:
            update(1, 1, 0);
            break;
        case XCPC_COMPANY_NAME_AMSTRAD:
            update(1, 1, 1);
            break;
        default:
            throw std::runtime_error("unsupported company name");
            break;
    }
}

auto Mainboard::set_machine_type(const std::string& string) -> void
{
    auto machine_type = Utils::machine_type_from_string(string);

    auto set_lower_rom = [&](const std::string& filename) -> void
    {
        try {
            load_lower_rom(filename);
        }
        catch(const std::exception& e) {
            ::xcpc_log_error("error while loading lower rom: %s", e.what());
        }
    };

    auto set_upper_rom = [&](const std::string& filename) -> void
    {
        try {
            load_upper_rom(filename);
        }
        catch(const std::exception& e) {
            ::xcpc_log_error("error while loading upper rom: %s", e.what());
        }
    };

    auto set_amsdos_rom = [&](const std::string& filename) -> void
    {
        try {
            load_expansion(filename, 7);
        }
        catch(const std::exception& e) {
            ::xcpc_log_error("error while loading amsdos rom: %s", e.what());
        }
    };

    auto update = [&](const MemorySize memory_size, const std::string& firmware, const std::string& amsdos) -> void
    {
        if(machine_type != _setup.machine_type) {
            _setup.machine_type = machine_type;
            if(_setup.memory_size < memory_size) {
                _setup.memory_size = memory_size;
            }
            set_lower_rom(firmware);
            set_upper_rom(firmware);
            set_amsdos_rom(amsdos);
            reset();
        }
    };

    if(machine_type == XCPC_MACHINE_TYPE_DEFAULT) {
        machine_type = XCPC_MACHINE_TYPE_CPC6128;
    }
    switch(machine_type) {
        case XCPC_MACHINE_TYPE_CPC464:
            update(XCPC_MEMORY_SIZE_64K, "cpc464en.rom", "amsdos.rom");
            break;
        case XCPC_MACHINE_TYPE_CPC664:
            update(XCPC_MEMORY_SIZE_64K, "cpc664en.rom", "amsdos.rom");
            break;
        case XCPC_MACHINE_TYPE_CPC6128:
            update(XCPC_MEMORY_SIZE_128K, "cpc6128en.rom", "amsdos.rom");
            break;
        default:
            throw std::runtime_error("unsupported machine type");
            break;
    }
}

auto Mainboard::set_monitor_type(const std::string& string) -> void
{
    auto monitor_type = Utils::monitor_type_from_string(string);

    auto update = [&]() -> void
    {
        if(monitor_type != _setup.monitor_type) {
            _setup.monitor_type = monitor_type;
            if(_dpy != nullptr) {
                _dpy->set_monitor_type(monitor_type);
            }
            update_vga();
        }
    };

    if(monitor_type == XCPC_MONITOR_TYPE_DEFAULT) {
        monitor_type = XCPC_MONITOR_TYPE_COLOR;
    }
    switch(monitor_type) {
        case XCPC_MONITOR_TYPE_COLOR:
        case XCPC_MONITOR_TYPE_GREEN:
        case XCPC_MONITOR_TYPE_GRAY:
        case XCPC_MONITOR_TYPE_CTM640:
        case XCPC_MONITOR_TYPE_CTM644:
        case XCPC_MONITOR_TYPE_GT64:
        case XCPC_MONITOR_TYPE_GT65:
        case XCPC_MONITOR_TYPE_CM14:
        case XCPC_MONITOR_TYPE_MM12:
            update();
            break;
        default:
            throw std::runtime_error("unsupported monitor type");
            break;
    }
}

auto Mainboard::set_refresh_rate(const std::string& string) -> void
{
    auto refresh_rate = Utils::refresh_rate_from_string(string);

    auto update = [&](const uint32_t frame_rate, const uint32_t frame_time, const uint8_t lnk_lk4) -> void
    {
        if(refresh_rate != _setup.refresh_rate) {
            _setup.refresh_rate = refresh_rate;
            _video.frame_rate   = frame_rate;
            _video.frame_time   = frame_time;
            _state.lnk_lk4      = lnk_lk4;
            if(_dpy != nullptr) {
                _dpy->set_refresh_rate(refresh_rate);
            }
            update_vga();
            reset();
        }
    };

    if(refresh_rate == XCPC_REFRESH_RATE_DEFAULT) {
        refresh_rate = XCPC_REFRESH_RATE_50HZ;
    }
    switch(refresh_rate) {
        case XCPC_REFRESH_RATE_50HZ:
            update(50, 20000, 1);
            break;
        case XCPC_REFRESH_RATE_60HZ:
            update(60, 16667, 0);
            break;
        default:
            throw std::runtime_error("unsupported refresh rate");
            break;
    }
}

auto Mainboard::set_keyboard_type(const std::string& string) -> void
{
    auto keyboard_type = Utils::keyboard_type_from_string(string);

    auto update = [&]() -> void
    {
        if(keyboard_type != _setup.keyboard_type) {
            _setup.keyboard_type = keyboard_type;
            if(_kbd != nullptr) {
                _kbd->set_keyboard_type(keyboard_type);
            }
        }
    };

    if(keyboard_type == XCPC_KEYBOARD_TYPE_DEFAULT) {
        keyboard_type = XCPC_KEYBOARD_TYPE_ENGLISH;
    }
    switch(keyboard_type) {
        case XCPC_KEYBOARD_TYPE_ENGLISH:
        case XCPC_KEYBOARD_TYPE_FRENCH:
        case XCPC_KEYBOARD_TYPE_GERMAN:
        case XCPC_KEYBOARD_TYPE_SPANISH:
        case XCPC_KEYBOARD_TYPE_DANISH:
            update();
            break;
        default:
            throw std::runtime_error("unsupported keyboard type");
            break;
    }
}

auto Mainboard::set_renderer_type(const std::string& string) -> void
{
    auto renderer_type = Utils::renderer_type_from_string(string);

    auto update = [&]() -> void
    {
        if(renderer_type != _setup.renderer_type) {
            _setup.renderer_type = renderer_type;
        }
    };

    if(renderer_type == XCPC_RENDERER_TYPE_DEFAULT) {
        renderer_type = XCPC_RENDERER_TYPE_OPENGL;
    }
    switch(renderer_type) {
        case XCPC_RENDERER_TYPE_XIMAGE:
        case XCPC_RENDERER_TYPE_OPENGL:
            update();
            break;
        default:
            throw std::runtime_error("unsupported renderer type");
            break;
    }
}

auto Mainboard::get_company_name() const -> std::string
{
    return Utils::company_name_to_string(_setup.company_name);
}

auto Mainboard::get_machine_type() const -> std::string
{
    return Utils::machine_type_to_string(_setup.machine_type);
}

auto Mainboard::get_memory_size() const -> std::string
{
    return Utils::memory_size_to_string(_setup.memory_size);
}

auto Mainboard::get_monitor_type() const -> std::string
{
    return Utils::monitor_type_to_string(_setup.monitor_type);
}

auto Mainboard::get_refresh_rate() const -> std::string
{
    return Utils::refresh_rate_to_string(_setup.refresh_rate);
}

auto Mainboard::get_keyboard_type() const -> std::string
{
    return Utils::keyboard_type_to_string(_setup.keyboard_type);
}

auto Mainboard::get_renderer_type() const -> std::string
{
    return Utils::renderer_type_to_string(_setup.renderer_type);
}

auto Mainboard::get_drive0_filename() const -> std::string
{
    if(_fdc != nullptr) {
        return _fdc->get_filename(fdc::FDC_DRIVE0);
    }
    return "";
}

auto Mainboard::get_drive1_filename() const -> std::string
{
    if(_fdc != nullptr) {
        return _fdc->get_filename(fdc::FDC_DRIVE1);
    }
    return "";
}

auto Mainboard::get_system_info() const -> std::string
{
    std::string system_info;

    system_info += get_company_name();
    system_info += ' ';
    system_info += get_machine_type();
    system_info += ' ';
    system_info += get_memory_size();
    system_info += ',';
    system_info += ' ';
    system_info += get_monitor_type();
    system_info += ' ';
    system_info += '@';
    system_info += ' ';
    system_info += get_refresh_rate();
    system_info += ',';
    system_info += ' ';
    system_info += get_keyboard_type();

    return system_info;
}

auto Mainboard::get_statistics() const -> std::string
{
    return _stats.buffer;
}

auto Mainboard::get_volume() const -> float
{
    return _audio.volume;
}

auto Mainboard::on_reset(Event& event) -> unsigned long
{
    /* reset the mainboard */ {
        reset();
    }
    return 0UL;
}

auto Mainboard::on_clock(Event& event) -> unsigned long
{
    unsigned long timeout    = 0UL;
    unsigned long timedrift  = 0UL;
    unsigned int  skip_frame = 0;
    const unsigned long frame_time = (_video.frame_time / _setup.speedup);

    /* clock the mainboard */ {
        clock();
    }
    /* compute the next deadline */ {
        if((_clock.deadline.tv_usec += frame_time) >= 1000000) {
            _clock.deadline.tv_usec -= 1000000;
            _clock.deadline.tv_sec  += 1;
        }
    }
    /* get the current time */ {
        Traits::gettimeofday(_clock.currtime);
    }
    /* compute the next deadline timeout in us */ {
        const long long currtime = XCPC_TIMESTAMP_OF(&_clock.currtime);
        const long long deadline = XCPC_TIMESTAMP_OF(&_clock.deadline);
        if(currtime <= deadline) {
            timeout     = static_cast<unsigned long>(deadline - currtime);
            skip_frame &= 0;
        }
        else {
            timedrift   = static_cast<unsigned long>(currtime - deadline);
            skip_frame |= 1;
        }
    }
    /* always force the first frame and skip frames if needed in speedup mode */ {
        if(_stats.frame_count == 0) {
            skip_frame &= 0;
        }
        else if(_setup.speedup >= 10) {
            skip_frame |= 1;
        }
    }
    /* draw the frame if needed */ {
        if(skip_frame == 0) {
            (*_funcs.render_func)(this);
            ++_stats.frame_drawn;
        }
    }
    /* compute stats */ {
        if(++_stats.frame_count == _video.frame_rate) {
            update_stats();
        }
    }
    /* check if the time has drifted for more than a second */ {
        if(timedrift >= 1000000UL) {
            timeout = frame_time;
            _clock.deadline = _clock.currtime;
            if((_clock.deadline.tv_usec += timeout) >= 1000000) {
                _clock.deadline.tv_usec -= 1000000;
                _clock.deadline.tv_sec  += 1;
            }
        }
    }
    /* schedule the next frame in ms */ {
        timeout /= 1000UL;
    }
    /* adjust timeout if needed and only for the first frame */ {
        if((timeout == 0UL) && (_stats.frame_count == 0)) {
            timeout = 1UL;
        }
    }
    return timeout;
}

auto Mainboard::on_create_window(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.create_window.x11_event;

    auto do_render_null = +[](Mainboard* self) -> void
    {
    };

    auto do_render_08bpp = +[](Mainboard* self) -> void
    {
        self->render_08bpp();
    };

    auto do_render_16bpp = +[](Mainboard* self) -> void
    {
        self->render_16bpp();
    };

    auto do_render_32bpp = +[](Mainboard* self) -> void
    {
        self->render_32bpp();
    };

    auto do_render_rgba = +[](Mainboard* self) -> void
    {
        self->render_rgba();
    };

    auto do_setup_ximage = [&]() -> void
    {
        switch(_dpy->get_image_bpp()) {
            case 8:
                _funcs.render_func = do_render_08bpp;
                break;
            case 16:
                _funcs.render_func = do_render_16bpp;
                break;
            case 32:
                _funcs.render_func = do_render_32bpp;
                break;
            default:
                _funcs.render_func = do_render_null;
                break;
        }
    };

    auto do_setup_opengl = [&]() -> void
    {
        switch(_dpy->get_image_bpp()) {
            case 32:
                _funcs.render_func = do_render_rgba;
                break;
            default:
                _funcs.render_func = do_render_null;
                break;
        }
    };

    auto do_setup_null = [&]() -> void
    {
        _funcs.render_func = do_render_null;
    };

    auto do_setup = [&]() -> void
    {
        switch(_setup.renderer_type) {
            case XCPC_RENDERER_TYPE_XIMAGE:
                do_setup_ximage();
                break;
            case XCPC_RENDERER_TYPE_OPENGL:
                do_setup_opengl();
                break;
            default:
                do_setup_null();
                break;
        }
    };

    /* realize display with renderer */ {
        _dpy->realize(_setup.renderer_type, x11_event->xany.display, x11_event->xany.window, _setup.xshm);
        _dpy->set_parameterb("video.crt_emulation", _setup.crt_emulation);
    }
    /* update gate-array */ {
        update_vga();
    }
    /* setup the render handler */ {
        do_setup();
    }
    return 0UL;
}

auto Mainboard::on_delete_window(Event& event) -> unsigned long
{
    if(_dpy != nullptr) {
        _dpy->unrealize();
    }
    return 0UL;
}

auto Mainboard::on_resize_window(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.resize_window.x11_event;

    if((_dpy != nullptr) && (x11_event != nullptr)) {
        _dpy->resize(x11_event->xconfigure.width, x11_event->xconfigure.height);
    }
    return 0UL;
}

auto Mainboard::on_expose_window(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.expose_window.x11_event;

    if((_dpy != nullptr) && (x11_event != nullptr)) {
        _dpy->expose(x11_event->xexpose);
    }
    return 0UL;
}

auto Mainboard::on_key_press(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.key_press.x11_event;

    if((_kbd != nullptr) && (x11_event != nullptr)) {
        _kbd->key_press(x11_event->xkey);
    }
    return 0UL;
}

auto Mainboard::on_key_release(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.key_release.x11_event;

    if((_kbd != nullptr) && (x11_event != nullptr)) {
        _kbd->key_release(x11_event->xkey);
    }
    return 0UL;
}

auto Mainboard::on_button_press(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.button_press.x11_event;

    if((_kbd != nullptr) && (x11_event != nullptr)) {
        _kbd->button_press(x11_event->xbutton);
    }
    return 0UL;
}

auto Mainboard::on_button_release(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.button_release.x11_event;

    if((_kbd != nullptr) && (x11_event != nullptr)) {
        _kbd->button_release(x11_event->xbutton);
    }
    return 0UL;
}

auto Mainboard::on_motion_notify(Event& event) -> unsigned long
{
    XEvent* x11_event = event.u.motion_notify.x11_event;

    if((_kbd != nullptr) && (x11_event != nullptr)) {
        _kbd->motion_notify(x11_event->xmotion);
    }
    return 0UL;
}

auto Mainboard::configure(const Settings& settings) -> void
{
    auto clamp_int = [](int value, const int min, const int max) -> int
    {
        if(value < min) {
            value = min;
        }
        if(value > max) {
            value = max;
        }
        return value;
    };

    auto is_set = [](const std::string& string) -> bool
    {
        if(string.size() == 0) {
            return false;
        }
        if(string == "{not-set}") {
            return false;
        }
        return true;
    };

    auto init_machine = [&]() -> void
    {
        set_company_name(settings.opt_company);
        set_machine_type(settings.opt_machine);
    //  set_memory_size(settings.opt_memory);
        set_monitor_type(settings.opt_monitor);
        set_refresh_rate(settings.opt_refresh);
        set_keyboard_type(settings.opt_keyboard);
        set_renderer_type(settings.opt_renderer);

        _setup.speedup       = clamp_int(::atoi(settings.opt_speedup.c_str()), 1, 100);
        _setup.xshm          = settings.opt_xshm;
        _setup.crt_emulation = settings.opt_crt_emulation;
        _state.snd_clock     = _device->sampleRate;
    };

    auto load_system_roms = [&]() -> void
    {
        const std::string firmware(settings.opt_sysrom);

        /* load lower rom */ {
            try {
                if(is_set(firmware)) {
                    load_lower_rom(firmware);
                }
            }
            catch(const std::exception& e) {
                ::xcpc_log_error("error while loading lower rom: %s", e.what());
            }
        }
        /* load upper rom */ {
            try {
                if(is_set(firmware)) {
                    load_upper_rom(firmware);
                }
            }
            catch(const std::exception& e) {
                ::xcpc_log_error("error while loading upper rom: %s", e.what());
            }
        }
    };

    auto load_expansions_roms = [&]() -> void
    {
        const std::string expansions[16] = {
            settings.opt_rom000,
            settings.opt_rom001,
            settings.opt_rom002,
            settings.opt_rom003,
            settings.opt_rom004,
            settings.opt_rom005,
            settings.opt_rom006,
            settings.opt_rom007,
            settings.opt_rom008,
            settings.opt_rom009,
            settings.opt_rom010,
            settings.opt_rom011,
            settings.opt_rom012,
            settings.opt_rom013,
            settings.opt_rom014,
            settings.opt_rom015,
        };

        /* load expansions */ {
            unsigned int index = 0;
            for(auto& exp : _exp) {
                std::string expansion;
                if(index < countof(expansions)) {
                    expansion = expansions[index];
                    if((index == 7) && !is_set(expansion)) {
                        continue;
                    }
                }
                if(is_set(expansion)) {
                    try {
                        load_expansion(expansion, index);
                    }
                    catch(const std::exception& e) {
                        ::xcpc_log_error("error while loading expansion: %s", e.what());
                    }
                }
                else {
                    if(exp != nullptr) {
                        exp = (delete exp, nullptr);
                    }
                }
                ++index;
            }
        }
    };

    auto load_initial_snapshot = [&]() -> void
    {
        try {
            if(is_set(settings.opt_snapshot)) {
                _machine.load_snapshot(settings.opt_snapshot);
            }
        }
        catch(const std::exception& e) {
            ::xcpc_log_error("error while loading initial snapshot: %s", e.what());
        }
    };

    auto load_initial_drive0 = [&]() -> void
    {
        try {
            if(is_set(settings.opt_drive0)) {
                _machine.insert_disk_into_drive0(settings.opt_drive0);
            }
        }
        catch(const std::exception& e) {
            ::xcpc_log_error("error while loading initial drive0: %s", e.what());
        }
    };

    auto load_initial_drive1 = [&]() -> void
    {
        try {
            if(is_set(settings.opt_drive1)) {
                _machine.insert_disk_into_drive1(settings.opt_drive1);
            }
        }
        catch(const std::exception& e) {
            ::xcpc_log_error("error while loading initial drive1: %s", e.what());
        }
    };

    auto initialize = [&]() -> void
    {
        try {
            init_machine();
            load_system_roms();
            load_expansions_roms();
            reset();
            load_initial_snapshot();
            load_initial_drive0();
            load_initial_drive1();
        }
        catch(const std::exception& e) {
            reset();
            throw;
        }
    };

    return initialize();
}

auto Mainboard::load_lower_rom(const std::string& filename) -> void
{
    constexpr int index = 0;
    auto*         rom   = _rom[index];

    if(rom == nullptr) {
        rom = _rom[index] = new mem::Instance(mem::ROM_BANK, *this);
    }
    if(rom != nullptr) {
        std::string path(filename);
        if((path.size() > 0) && (path[0] != '.') && (path[0] != '/')) {
            path = Utils::get_romdir() + '/' + path;
        }
        rom->load(path, 0x0000);
    }
}

auto Mainboard::load_upper_rom(const std::string& filename) -> void
{
    constexpr int index = 1;
    auto*         rom   = _rom[index];

    if(rom == nullptr) {
        rom = _rom[index] = new mem::Instance(mem::ROM_BANK, *this);
    }
    if(rom != nullptr) {
        std::string path(filename);
        if((path.size() > 0) && (path[0] != '.') && (path[0] != '/')) {
            path = Utils::get_romdir() + '/' + path;
        }
        rom->load(path, 0x4000);
    }
}

auto Mainboard::load_expansion(const std::string& filename, const int index) -> void
{
    auto* rom = _exp[index];

    if(rom == nullptr) {
        rom = _exp[index] = new mem::Instance(mem::ROM_BANK, *this);
    }
    if(rom != nullptr) {
        std::string path(filename);
        if((path.size() > 0) && (path[0] != '.') && (path[0] != '/')) {
            path = Utils::get_romdir() + '/' + path;
        }
        rom->load(path, 0x0000);
    }
}

auto Mainboard::load_cpc(sna::Snapshot& snapshot) -> void
{
    auto set_vdc = [&](const uint8_t index, const uint8_t value) -> void
    {
        auto& vdc(*_vdc);

        if(index == 0xff) {
            vdc.set_index(value);
        }
        else {
            vdc.set_index(index);
            vdc.set_value(value);
        }
    };

    auto set_psg = [&](const uint8_t index, const uint8_t value) -> void
    {
        auto& psg(*_psg);

        if(index == 0xff) {
            psg.set_index(value);
        }
        else {
            psg.set_index(index);
            psg.set_value(value);
        }
    };

    auto load_cpu = [&]() -> void
    {
        auto& cpu(*_cpu);

        cpu.set_af_l(snapshot->header.cpu_p_af_l);
        cpu.set_af_h(snapshot->header.cpu_p_af_h);
        cpu.set_bc_l(snapshot->header.cpu_p_bc_l);
        cpu.set_bc_h(snapshot->header.cpu_p_bc_h);
        cpu.set_de_l(snapshot->header.cpu_p_de_l);
        cpu.set_de_h(snapshot->header.cpu_p_de_h);
        cpu.set_hl_l(snapshot->header.cpu_p_hl_l);
        cpu.set_hl_h(snapshot->header.cpu_p_hl_h);
        cpu.set_ir_l(snapshot->header.cpu_p_ir_l);
        cpu.set_ir_h(snapshot->header.cpu_p_ir_h);
        cpu.set_iff1(snapshot->header.cpu_p_iff1);
        cpu.set_iff2(snapshot->header.cpu_p_iff2);
        cpu.set_ix_l(snapshot->header.cpu_p_ix_l);
        cpu.set_ix_h(snapshot->header.cpu_p_ix_h);
        cpu.set_iy_l(snapshot->header.cpu_p_iy_l);
        cpu.set_iy_h(snapshot->header.cpu_p_iy_h);
        cpu.set_sp_l(snapshot->header.cpu_p_sp_l);
        cpu.set_sp_h(snapshot->header.cpu_p_sp_h);
        cpu.set_pc_l(snapshot->header.cpu_p_pc_l);
        cpu.set_pc_h(snapshot->header.cpu_p_pc_h);
        cpu.set_im_l(snapshot->header.cpu_p_im_l);
        cpu.set_af_y(snapshot->header.cpu_a_af_l);
        cpu.set_af_x(snapshot->header.cpu_a_af_h);
        cpu.set_bc_y(snapshot->header.cpu_a_bc_l);
        cpu.set_bc_x(snapshot->header.cpu_a_bc_h);
        cpu.set_de_y(snapshot->header.cpu_a_de_l);
        cpu.set_de_x(snapshot->header.cpu_a_de_h);
        cpu.set_hl_y(snapshot->header.cpu_a_hl_l);
        cpu.set_hl_x(snapshot->header.cpu_a_hl_h);
    };

    auto load_vga = [&]() -> void
    {
        auto& vga(*_vga);

        vga->pen       = snapshot->header.vga_ink_ix;
        vga->ink[0x00] = snapshot->header.vga_ink_00;
        vga->ink[0x01] = snapshot->header.vga_ink_01;
        vga->ink[0x02] = snapshot->header.vga_ink_02;
        vga->ink[0x03] = snapshot->header.vga_ink_03;
        vga->ink[0x04] = snapshot->header.vga_ink_04;
        vga->ink[0x05] = snapshot->header.vga_ink_05;
        vga->ink[0x06] = snapshot->header.vga_ink_06;
        vga->ink[0x07] = snapshot->header.vga_ink_07;
        vga->ink[0x08] = snapshot->header.vga_ink_08;
        vga->ink[0x09] = snapshot->header.vga_ink_09;
        vga->ink[0x0a] = snapshot->header.vga_ink_10;
        vga->ink[0x0b] = snapshot->header.vga_ink_11;
        vga->ink[0x0c] = snapshot->header.vga_ink_12;
        vga->ink[0x0d] = snapshot->header.vga_ink_13;
        vga->ink[0x0e] = snapshot->header.vga_ink_14;
        vga->ink[0x0f] = snapshot->header.vga_ink_15;
        vga->ink[0x10] = snapshot->header.vga_ink_16;
        vga->rmr       = snapshot->header.vga_config;
    };

    auto load_vdc = [&]() -> void
    {
        set_vdc(0x00, snapshot->header.vdc_reg_00);
        set_vdc(0x01, snapshot->header.vdc_reg_01);
        set_vdc(0x02, snapshot->header.vdc_reg_02);
        set_vdc(0x03, snapshot->header.vdc_reg_03);
        set_vdc(0x04, snapshot->header.vdc_reg_04);
        set_vdc(0x05, snapshot->header.vdc_reg_05);
        set_vdc(0x06, snapshot->header.vdc_reg_06);
        set_vdc(0x07, snapshot->header.vdc_reg_07);
        set_vdc(0x08, snapshot->header.vdc_reg_08);
        set_vdc(0x09, snapshot->header.vdc_reg_09);
        set_vdc(0x0a, snapshot->header.vdc_reg_10);
        set_vdc(0x0b, snapshot->header.vdc_reg_11);
        set_vdc(0x0c, snapshot->header.vdc_reg_12);
        set_vdc(0x0d, snapshot->header.vdc_reg_13);
        set_vdc(0x0e, snapshot->header.vdc_reg_14);
        set_vdc(0x0f, snapshot->header.vdc_reg_15);
        set_vdc(0x10, snapshot->header.vdc_reg_16);
        set_vdc(0x11, snapshot->header.vdc_reg_17);
        set_vdc(0xff, snapshot->header.vdc_reg_ix);
    };

    auto load_ppi = [&]() -> void
    {
        auto& ppi(*_ppi);

        ppi.wr_port_a(snapshot->header.ppi_port_a);
        ppi.wr_port_b(snapshot->header.ppi_port_b);
        ppi.wr_port_c(snapshot->header.ppi_port_c);
        ppi.wr_ctrl_p(snapshot->header.ppi_ctrl_p);
    };

    auto load_psg = [&]() -> void
    {
        set_psg(0x00, snapshot->header.psg_reg_00);
        set_psg(0x01, snapshot->header.psg_reg_01);
        set_psg(0x02, snapshot->header.psg_reg_02);
        set_psg(0x03, snapshot->header.psg_reg_03);
        set_psg(0x04, snapshot->header.psg_reg_04);
        set_psg(0x05, snapshot->header.psg_reg_05);
        set_psg(0x06, snapshot->header.psg_reg_06);
        set_psg(0x07, snapshot->header.psg_reg_07);
        set_psg(0x08, snapshot->header.psg_reg_08);
        set_psg(0x09, snapshot->header.psg_reg_09);
        set_psg(0x0a, snapshot->header.psg_reg_10);
        set_psg(0x0b, snapshot->header.psg_reg_11);
        set_psg(0x0c, snapshot->header.psg_reg_12);
        set_psg(0x0d, snapshot->header.psg_reg_13);
        set_psg(0x0e, snapshot->header.psg_reg_14);
        set_psg(0x0f, snapshot->header.psg_reg_15);
        set_psg(0xff, snapshot->header.psg_reg_ix);
    };

    auto load_ram = [&]() -> void
    {
        _state.ram_conf = snapshot->header.ram_select;
    };

    auto load_rom = [&]() -> void
    {
        _state.rom_conf = snapshot->header.rom_select;
    };

    auto load_mem = [&]() -> void
    {
        size_t ram_size = 0;
        ram_size |= ((static_cast<uint32_t>(snapshot->header.ram_size_h)) << 18);
        ram_size |= ((static_cast<uint32_t>(snapshot->header.ram_size_l)) << 10);
        if(ram_size > static_cast<uint32_t>(_setup.memory_size)) {
            ram_size = static_cast<uint32_t>(_setup.memory_size);
        }
        uint32_t bank_index = 0;
        uint32_t bank_count = countof(_ram);
        for(auto& memory : snapshot->memory) {
            constexpr size_t memory_size = sizeof(memory.data);
            if((ram_size != 0) && (bank_index < bank_count)) {
                auto* bank = _ram[bank_index];
                if(bank != nullptr) {
                    bank->store(memory.data, memory_size);
                }
                ram_size -= memory_size;
            }
            else {
                break;
            }
            ++bank_index;
        }
        update_pal();
    };

    auto load_all = [&]() -> void
    {
        reset();
        load_cpu();
        load_vga();
        load_vdc();
        load_ppi();
        load_psg();
        load_ram();
        load_rom();
        load_mem();
    };

    return load_all();
}

auto Mainboard::save_cpc(sna::Snapshot& snapshot) -> void
{
    auto save_cpu = [&]() -> void
    {
        auto& cpu(*_cpu);

        snapshot->header.cpu_p_af_l = cpu.get_af_l();
        snapshot->header.cpu_p_af_h = cpu.get_af_h();
        snapshot->header.cpu_p_bc_l = cpu.get_bc_l();
        snapshot->header.cpu_p_bc_h = cpu.get_bc_h();
        snapshot->header.cpu_p_de_l = cpu.get_de_l();
        snapshot->header.cpu_p_de_h = cpu.get_de_h();
        snapshot->header.cpu_p_hl_l = cpu.get_hl_l();
        snapshot->header.cpu_p_hl_h = cpu.get_hl_h();
        snapshot->header.cpu_p_ir_l = cpu.get_ir_l();
        snapshot->header.cpu_p_ir_h = cpu.get_ir_h();
        snapshot->header.cpu_p_iff1 = cpu.get_iff1();
        snapshot->header.cpu_p_iff2 = cpu.get_iff2();
        snapshot->header.cpu_p_ix_l = cpu.get_ix_l();
        snapshot->header.cpu_p_ix_h = cpu.get_ix_h();
        snapshot->header.cpu_p_iy_l = cpu.get_iy_l();
        snapshot->header.cpu_p_iy_h = cpu.get_iy_h();
        snapshot->header.cpu_p_sp_l = cpu.get_sp_l();
        snapshot->header.cpu_p_sp_h = cpu.get_sp_h();
        snapshot->header.cpu_p_pc_l = cpu.get_pc_l();
        snapshot->header.cpu_p_pc_h = cpu.get_pc_h();
        snapshot->header.cpu_p_im_l = cpu.get_im_l();
        snapshot->header.cpu_a_af_l = cpu.get_af_y();
        snapshot->header.cpu_a_af_h = cpu.get_af_x();
        snapshot->header.cpu_a_bc_l = cpu.get_bc_y();
        snapshot->header.cpu_a_bc_h = cpu.get_bc_x();
        snapshot->header.cpu_a_de_l = cpu.get_de_y();
        snapshot->header.cpu_a_de_h = cpu.get_de_x();
        snapshot->header.cpu_a_hl_l = cpu.get_hl_y();
        snapshot->header.cpu_a_hl_h = cpu.get_hl_x();
    };

    auto save_vga = [&]() -> void
    {
        auto& vga(*_vga);

        snapshot->header.vga_ink_ix = vga->pen;
        snapshot->header.vga_ink_00 = vga->ink[0x00];
        snapshot->header.vga_ink_01 = vga->ink[0x01];
        snapshot->header.vga_ink_02 = vga->ink[0x02];
        snapshot->header.vga_ink_03 = vga->ink[0x03];
        snapshot->header.vga_ink_04 = vga->ink[0x04];
        snapshot->header.vga_ink_05 = vga->ink[0x05];
        snapshot->header.vga_ink_06 = vga->ink[0x06];
        snapshot->header.vga_ink_07 = vga->ink[0x07];
        snapshot->header.vga_ink_08 = vga->ink[0x08];
        snapshot->header.vga_ink_09 = vga->ink[0x09];
        snapshot->header.vga_ink_10 = vga->ink[0x0a];
        snapshot->header.vga_ink_11 = vga->ink[0x0b];
        snapshot->header.vga_ink_12 = vga->ink[0x0c];
        snapshot->header.vga_ink_13 = vga->ink[0x0d];
        snapshot->header.vga_ink_14 = vga->ink[0x0e];
        snapshot->header.vga_ink_15 = vga->ink[0x0f];
        snapshot->header.vga_ink_16 = vga->ink[0x10];
        snapshot->header.vga_config = vga->rmr;
    };

    auto save_vdc = [&]() -> void
    {
        auto& vdc(*_vdc);

        snapshot->header.vdc_reg_ix = vdc->regs.array.addr;
        snapshot->header.vdc_reg_00 = vdc->regs.array.data[0x00];
        snapshot->header.vdc_reg_01 = vdc->regs.array.data[0x01];
        snapshot->header.vdc_reg_02 = vdc->regs.array.data[0x02];
        snapshot->header.vdc_reg_03 = vdc->regs.array.data[0x03];
        snapshot->header.vdc_reg_04 = vdc->regs.array.data[0x04];
        snapshot->header.vdc_reg_05 = vdc->regs.array.data[0x05];
        snapshot->header.vdc_reg_06 = vdc->regs.array.data[0x06];
        snapshot->header.vdc_reg_07 = vdc->regs.array.data[0x07];
        snapshot->header.vdc_reg_08 = vdc->regs.array.data[0x08];
        snapshot->header.vdc_reg_09 = vdc->regs.array.data[0x09];
        snapshot->header.vdc_reg_10 = vdc->regs.array.data[0x0a];
        snapshot->header.vdc_reg_11 = vdc->regs.array.data[0x0b];
        snapshot->header.vdc_reg_12 = vdc->regs.array.data[0x0c];
        snapshot->header.vdc_reg_13 = vdc->regs.array.data[0x0d];
        snapshot->header.vdc_reg_14 = vdc->regs.array.data[0x0e];
        snapshot->header.vdc_reg_15 = vdc->regs.array.data[0x0f];
        snapshot->header.vdc_reg_16 = vdc->regs.array.data[0x10];
        snapshot->header.vdc_reg_17 = vdc->regs.array.data[0x11];
    };

    auto save_ppi = [&]() -> void
    {
        auto& ppi(*_ppi);

        snapshot->header.ppi_port_a = ppi->port_a;
        snapshot->header.ppi_port_b = ppi->port_b;
        snapshot->header.ppi_port_c = ppi->port_c;
        snapshot->header.ppi_ctrl_p = ppi->ctrl_p;
    };

    auto save_psg = [&]() -> void
    {
        auto& psg(*_psg);

        snapshot->header.psg_reg_ix = psg->index;
        snapshot->header.psg_reg_00 = psg->array[0x00];
        snapshot->header.psg_reg_01 = psg->array[0x01];
        snapshot->header.psg_reg_02 = psg->array[0x02];
        snapshot->header.psg_reg_03 = psg->array[0x03];
        snapshot->header.psg_reg_04 = psg->array[0x04];
        snapshot->header.psg_reg_05 = psg->array[0x05];
        snapshot->header.psg_reg_06 = psg->array[0x06];
        snapshot->header.psg_reg_07 = psg->array[0x07];
        snapshot->header.psg_reg_08 = psg->array[0x08];
        snapshot->header.psg_reg_09 = psg->array[0x09];
        snapshot->header.psg_reg_10 = psg->array[0x0a];
        snapshot->header.psg_reg_11 = psg->array[0x0b];
        snapshot->header.psg_reg_12 = psg->array[0x0c];
        snapshot->header.psg_reg_13 = psg->array[0x0d];
        snapshot->header.psg_reg_14 = psg->array[0x0e];
        snapshot->header.psg_reg_15 = psg->array[0x0f];
    };

    auto save_ram = [&]() -> void
    {
        snapshot->header.ram_select = _state.ram_conf;
    };

    auto save_rom = [&]() -> void
    {
        snapshot->header.rom_select = _state.rom_conf;
    };

    auto save_mem = [&]() -> void
    {
        size_t ram_size = static_cast<uint32_t>(_setup.memory_size);
        snapshot->header.ram_size_h = (ram_size >> 18);
        snapshot->header.ram_size_l = (ram_size >> 10);
        uint32_t bank_index = 0;
        uint32_t bank_count = countof(_ram);
        for(auto& memory : snapshot->memory) {
            constexpr size_t memory_size = sizeof(memory.data);
            if((ram_size != 0) && (bank_index < bank_count)) {
                auto* bank = _ram[bank_index];
                if(bank != nullptr) {
                    bank->fetch(memory.data, memory_size);
                }
                ram_size -= memory_size;
            }
            else {
                break;
            }
            ++bank_index;
        }
    };

    auto save_all = [&]() -> void
    {
        save_cpu();
        save_vga();
        save_vdc();
        save_ppi();
        save_psg();
        save_ram();
        save_rom();
        save_mem();
    };

    return save_all();
}

auto Mainboard::update_vga() -> void
{
    auto& dpy(*_dpy);
    auto& vga(*_vga);

    /* copy palette0 to gate-array */ {
        unsigned int index = 0;
        for(auto& pixel : vga->colormap.pixel0) {
            if(_setup.crt_emulation != false) {
                pixel = dpy->palette0[index];
            }
            else {
                pixel = dpy->palette0[index];
            }
            ++index;
        }
    }
    /* copy palette1 to gate-array */ {
        unsigned int index = 0;
        for(auto& pixel : vga->colormap.pixel1) {
            if(_setup.crt_emulation != false) {
                pixel = dpy->palette1[index];
            }
            else {
                pixel = dpy->palette0[index];
            }
            ++index;
        }
    }
}

auto Mainboard::update_pal() -> void
{
    if(_setup.memory_size >= XCPC_MEMORY_SIZE_128K) {
        switch(_state.ram_conf & 0x3f) {
            case 0x00:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[1])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[3])->data;
                }
                break;
            case 0x01:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[1])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[7])->data;
                }
                break;
            case 0x02:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[4])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[5])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[6])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[7])->data;
                }
                break;
            case 0x03:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[3])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[7])->data;
                }
                break;
            case 0x04:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[4])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[3])->data;
                }
                break;
            case 0x05:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[5])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[3])->data;
                }
                break;
            case 0x06:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[6])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[3])->data;
                }
                break;
            case 0x07:
                {
                    _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
                    _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[7])->data;
                    _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
                    _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[3])->data;
                }
                break;
            default:
                ::xcpc_log_alert("pal::update_pal() : unsupported ram configuration (%02x)", _state.ram_conf);
                break;
        }
    }
    else {
        _state.pal_rd[0] = _state.pal_wr[0] = (*_ram[0])->data;
        _state.pal_rd[1] = _state.pal_wr[1] = (*_ram[1])->data;
        _state.pal_rd[2] = _state.pal_wr[2] = (*_ram[2])->data;
        _state.pal_rd[3] = _state.pal_wr[3] = (*_ram[3])->data;
    }
    if(((*_vga)->rmr & 0x04) == 0) {
        if(_rom[0] != nullptr) {
            _state.pal_rd[0] = (*_rom[0])->data;
        }
    }
    if(((*_vga)->rmr & 0x08) == 0) {
        if(_rom[1] != nullptr) {
            _state.pal_rd[3] = (*_rom[1])->data;
        }
        if(_exp[_state.rom_conf] != nullptr) {
            _state.pal_rd[3] = (*_exp[_state.rom_conf])->data;
        }
    }
}

auto Mainboard::update_stats() -> void
{
    unsigned long elapsed_us = 0;

    /* get the current time */ {
        Traits::gettimeofday(_clock.currtime);
    }
    /* compute the elapsed time in us */ {
        const long long prevtime = XCPC_TIMESTAMP_OF(&_clock.proftime);
        const long long currtime = XCPC_TIMESTAMP_OF(&_clock.currtime);
        if(currtime >= prevtime) {
            elapsed_us = static_cast<unsigned long>(currtime - prevtime);
        }
        if(elapsed_us == 0UL) {
            elapsed_us = 1UL;
        }
    }
    /* compute and print the statistics */ {
        const float stats_frames  = static_cast<float>(_stats.frame_drawn * 1000000UL);
        const float stats_elapsed = static_cast<float>(elapsed_us);
        const float stats_fps     = ::rintf(stats_frames / stats_elapsed);
        const int rc = ::snprintf(_stats.buffer, sizeof(_stats.buffer), "%d fps", static_cast<int>(stats_fps));
        static_cast<void>(rc);
    }
    /* set the new reference */ {
        _clock.proftime = _clock.currtime;
        _stats.frame_count = 0;
        _stats.frame_drawn = 0;
    }
}

auto Mainboard::render_08bpp() -> void
{
    auto& vdc(*_vdc);
    auto& vga(*_vga);
    auto* scanline = &vga->scanline[0];
    const uint8_t* const mode0 = vga->mode0;
    const uint8_t* const mode1 = vga->mode1;
    const uint8_t* const mode2 = vga->mode2;
    const uint8_t* const ram[4] = {
        (*_ram[0])->data,
        (*_ram[1])->data,
        (*_ram[2])->data,
        (*_ram[3])->data,
    };
    const HorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc->regs.named.horizontal_total     < 63 ? vdc->regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc->regs.named.horizontal_displayed < 52 ? vdc->regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc->regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 0) & 0x0f)),
    };
    const VertProps v = {
        /* ch  : pixels */ (1 + (vdc->regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc->regs.named.vertical_total     < 40 ? vdc->regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc->regs.named.vertical_displayed < 40 ? vdc->regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc->regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 4) & 0x0f)),
    };
    const Borders b = {
        /* top : pixels */ ((v.vt - v.vsp) * v.ch) + vdc->regs.named.vertical_total_adjust,
        /* bot : pixels */ ((v.vsp - v.vd) * v.ch),
        /* lft : pixels */ ((h.ht - h.hsp) * h.cw),
        /* rgt : pixels */ ((h.hsp - h.hd) * h.cw),
    };
    unsigned int   address         = ((vdc->regs.named.start_address_high << 8) | (vdc->regs.named.start_address_low  << 0));
    const uint32_t bytes_per_line  = _dpy->get_image_bpl();
    int            remaining_lines = _dpy->get_image_height();
    uint8_t*       data_iter       = XCPC_BYTE_PTR(_dpy->get_image_data());
    uint8_t*       curr_line       = nullptr;
    uint8_t*       next_line       = nullptr;
    uint8_t        pixel0          = 0;
    uint8_t        pixel1          = 0;

    if(data_iter == nullptr) {
        return;
    }
    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel1;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(int row = 0; row < rows; ++row) {
            for(int ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    curr_line = data_iter;
                    data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    next_line = data_iter;
                    data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel1;
                    for(int col = 0; col < lfts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            ::xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel1;
                    for(int col = 0; col < rgts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_BYTE_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel1;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* put image */ {
        _dpy->render();
    }
}

auto Mainboard::render_16bpp() -> void
{
    auto& vdc(*_vdc);
    auto& vga(*_vga);
    auto* scanline = &vga->scanline[0];
    const uint8_t* const mode0 = vga->mode0;
    const uint8_t* const mode1 = vga->mode1;
    const uint8_t* const mode2 = vga->mode2;
    const uint8_t* const ram[4] = {
        (*_ram[0])->data,
        (*_ram[1])->data,
        (*_ram[2])->data,
        (*_ram[3])->data,
    };
    const HorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc->regs.named.horizontal_total     < 63 ? vdc->regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc->regs.named.horizontal_displayed < 52 ? vdc->regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc->regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 0) & 0x0f)),
    };
    const VertProps v = {
        /* ch  : pixels */ (1 + (vdc->regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc->regs.named.vertical_total     < 40 ? vdc->regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc->regs.named.vertical_displayed < 40 ? vdc->regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc->regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 4) & 0x0f)),
    };
    const Borders b = {
        /* top : pixels */ ((v.vt - v.vsp) * v.ch) + vdc->regs.named.vertical_total_adjust,
        /* bot : pixels */ ((v.vsp - v.vd) * v.ch),
        /* lft : pixels */ ((h.ht - h.hsp) * h.cw),
        /* rgt : pixels */ ((h.hsp - h.hd) * h.cw),
    };
    unsigned int   address         = ((vdc->regs.named.start_address_high << 8) | (vdc->regs.named.start_address_low  << 0));
    const uint32_t bytes_per_line  = _dpy->get_image_bpl();
    int            remaining_lines = _dpy->get_image_height();
    uint16_t*      data_iter       = XCPC_WORD_PTR(_dpy->get_image_data());
    uint16_t*      curr_line       = nullptr;
    uint16_t*      next_line       = nullptr;
    uint16_t       pixel0          = 0;
    uint16_t       pixel1          = 0;

    if(data_iter == nullptr) {
        return;
    }
    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel1;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(int row = 0; row < rows; ++row) {
            for(int ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    curr_line = data_iter;
                    data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    next_line = data_iter;
                    data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel1;
                    for(int col = 0; col < lfts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            ::xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel1;
                    for(int col = 0; col < rgts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_WORD_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel1;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* put image */ {
        _dpy->render();
    }
}

auto Mainboard::render_32bpp() -> void
{
    auto& vdc(*_vdc);
    auto& vga(*_vga);
    auto* scanline = &vga->scanline[0];
    const uint8_t* const mode0 = vga->mode0;
    const uint8_t* const mode1 = vga->mode1;
    const uint8_t* const mode2 = vga->mode2;
    const uint8_t* const ram[4] = {
        (*_ram[0])->data,
        (*_ram[1])->data,
        (*_ram[2])->data,
        (*_ram[3])->data,
    };
    const HorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc->regs.named.horizontal_total     < 63 ? vdc->regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc->regs.named.horizontal_displayed < 52 ? vdc->regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc->regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 0) & 0x0f)),
    };
    const VertProps v = {
        /* ch  : pixels */ (1 + (vdc->regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc->regs.named.vertical_total     < 40 ? vdc->regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc->regs.named.vertical_displayed < 40 ? vdc->regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc->regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 4) & 0x0f)),
    };
    const Borders b = {
        /* top : pixels */ ((v.vt - v.vsp) * v.ch) + vdc->regs.named.vertical_total_adjust,
        /* bot : pixels */ ((v.vsp - v.vd) * v.ch),
        /* lft : pixels */ ((h.ht - h.hsp) * h.cw),
        /* rgt : pixels */ ((h.hsp - h.hd) * h.cw),
    };
    unsigned int   address         = ((vdc->regs.named.start_address_high << 8) | (vdc->regs.named.start_address_low  << 0));
    const uint32_t bytes_per_line  = _dpy->get_image_bpl();
    int            remaining_lines = _dpy->get_image_height();
    uint32_t*      data_iter       = XCPC_LONG_PTR(_dpy->get_image_data());
    uint32_t*      curr_line       = nullptr;
    uint32_t*      next_line       = nullptr;
    uint32_t       pixel0          = 0;
    uint32_t       pixel1          = 0;

    if(data_iter == nullptr) {
        return;
    }
    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel1;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(int row = 0; row < rows; ++row) {
            for(int ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    curr_line = data_iter;
                    data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    next_line = data_iter;
                    data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel1;
                    for(int col = 0; col < lfts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel1;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel1;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            ::xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel1;
                    for(int col = 0; col < rgts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel1;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* put image */ {
        _dpy->render();
    }
}

auto Mainboard::render_rgba() -> void
{
    auto& vdc(*_vdc);
    auto& vga(*_vga);
    auto* scanline = &vga->scanline[0];
    const uint8_t* const mode0 = vga->mode0;
    const uint8_t* const mode1 = vga->mode1;
    const uint8_t* const mode2 = vga->mode2;
    const uint8_t* const ram[4] = {
        (*_ram[0])->data,
        (*_ram[1])->data,
        (*_ram[2])->data,
        (*_ram[3])->data,
    };
    const HorzProps h = {
        /* cw  : pixels */ (16),
        /* ht  : chars  */ (1 + (vdc->regs.named.horizontal_total     < 63 ? vdc->regs.named.horizontal_total     : 63)),
        /* hd  : chars  */ (0 + (vdc->regs.named.horizontal_displayed < 52 ? vdc->regs.named.horizontal_displayed : 52)),
        /* hsp : chars  */ (0 + (vdc->regs.named.horizontal_sync_position)),
        /* hsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 0) & 0x0f)),
    };
    const VertProps v = {
        /* ch  : pixels */ (1 + (vdc->regs.named.maximum_scanline_address)),
        /* vt  : chars  */ (1 + (vdc->regs.named.vertical_total     < 40 ? vdc->regs.named.vertical_total     : 40)),
        /* vd  : chars  */ (0 + (vdc->regs.named.vertical_displayed < 40 ? vdc->regs.named.vertical_displayed : 40)),
        /* vsp : chars  */ (0 + (vdc->regs.named.vertical_sync_position)),
        /* vsw : pixels */ (0 + ((vdc->regs.named.sync_width >> 4) & 0x0f)),
    };
    const Borders b = {
        /* top : pixels */ ((v.vt - v.vsp) * v.ch) + vdc->regs.named.vertical_total_adjust,
        /* bot : pixels */ ((v.vsp - v.vd) * v.ch),
        /* lft : pixels */ ((h.ht - h.hsp) * h.cw),
        /* rgt : pixels */ ((h.hsp - h.hd) * h.cw),
    };
    unsigned int   address         = ((vdc->regs.named.start_address_high << 8) | (vdc->regs.named.start_address_low  << 0));
    const uint32_t bytes_per_line  = _dpy->get_image_bpl();
    int            remaining_lines = _dpy->get_image_height();
    uint32_t*      data_iter       = XCPC_LONG_PTR(_dpy->get_image_data());
    uint32_t*      curr_line       = nullptr;
    uint32_t*      next_line       = nullptr;
    uint32_t       pixel0          = 0;
    uint32_t       pixel1          = 0;

    if(data_iter == nullptr) {
        return;
    }
    /* vertical top border */ {
        const int rows = b.top;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel0;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* vertical active display */ {
        const int rows = v.vd;
        const int cols = h.hd;
        const int rass = v.ch;
        const int lfts = b.lft;
        const int rgts = b.rgt;
        for(int row = 0; row < rows; ++row) {
            for(int ras = 0; ras < rass; ++ras) {
                if(remaining_lines >= 2) {
                    curr_line = data_iter;
                    data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    next_line = data_iter;
                    data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                    remaining_lines -= 2;
                }
                else {
                    break;
                }
                /* horizontal left border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel0;
                    for(int col = 0; col < lfts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                /* horizontal active display */ {
                    switch(scanline->mode) {
                        case 0x00: /* mode 0 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode0[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x0f].pixel0;
                                            pixel1 = scanline->color[byte & 0x0f].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 4;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x01: /* mode 1 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode1[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x03].pixel0;
                                            pixel1 = scanline->color[byte & 0x03].pixel0;
                                            *curr_line++ = pixel0; *curr_line++ = pixel0;
                                            *next_line++ = pixel1; *next_line++ = pixel1;
                                            byte >>= 2;
                                        }
                                    }
                                }
                            }
                            break;
                        case 0x02: /* mode 2 */
                            {
                                for(int col = 0; col < cols; ++col) {
                                    const uint16_t addr = ((address & 0x3000) << 2) | ((ras & 0x0007) << 11) | (((address + col) & 0x03ff) << 1);
                                    const uint16_t bank = ((addr >> 14) & 0x0003);
                                    const uint16_t disp = ((addr >>  0) & 0x3fff);
                                    if(col >= h.hsp) {
                                        break;
                                    }
                                    /* process 1st byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 0]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                    /* process 2nd byte */ {
                                        uint8_t byte = mode2[ram[bank][disp | 1]];
                                        /* render pixel 0 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 1 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 2 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 3 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 4 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 5 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 6 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                        /* render pixel 7 */ {
                                            pixel0 = scanline->color[byte & 0x01].pixel0;
                                            pixel1 = scanline->color[byte & 0x01].pixel0;
                                            *curr_line++ = pixel0;
                                            *next_line++ = pixel1;
                                            byte >>= 1;
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            ::xcpc_log_alert("mode %d is not supported", scanline->mode);
                            break;
                    }
                }
                /* horizontal right border */ {
                    pixel0 = scanline->color[16].pixel0;
                    pixel1 = scanline->color[16].pixel0;
                    for(int col = 0; col < rgts; ++col) {
                        *curr_line++ = pixel0;
                        *next_line++ = pixel1;
                    }
                }
                ++scanline;
            }
            address += h.hd;
        }
    }
    /* vertical bottom border */ {
        const int rows = b.bot;
        const int cols = h.ht * h.cw;
        for(int row = 0; row < rows; ++row) {
            if(remaining_lines >= 2) {
                curr_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                next_line = data_iter;
                data_iter = XCPC_LONG_PTR(XCPC_BYTE_PTR(data_iter) + bytes_per_line);
                pixel0 = scanline->color[16].pixel0;
                pixel1 = scanline->color[16].pixel0;
                remaining_lines -= 2;
            }
            else {
                break;
            }
            for(int col = 0; col < cols; ++col) {
                *curr_line++ = pixel0;
                *next_line++ = pixel1;
            }
            ++scanline;
        }
    }
    /* put image */ {
        _dpy->render();
    }
}

auto Mainboard::process(const void* input, void* output, const uint32_t count) -> void
{
    const MutexLock lock(_mutex);

    auto mix_mono = [&](MonoFrameFlt32& audio_frame) -> void
    {
        const auto index = _audio.rd_index;

        const float mono = (_audio.channel0[index] * 1.00f)
                         + (_audio.channel1[index] * 1.00f)
                         + (_audio.channel2[index] * 1.00f)
                         ;

        audio_frame.mono = ((mono / 3.0f) * _audio.volume);
    };

    auto mix_stereo = [&](StereoFrameFlt32& audio_frame) -> void
    {
        const auto index = _audio.rd_index;

        const float left  = (_audio.channel0[index] * 0.75f)
                          + (_audio.channel1[index] * 0.50f)
                          + (_audio.channel2[index] * 0.25f)
                          ;

        const float right = (_audio.channel0[index] * 0.25f)
                          + (_audio.channel1[index] * 0.50f)
                          + (_audio.channel2[index] * 0.75f)
                          ;

        audio_frame.left  = ((left  / 1.5f) * _audio.volume);
        audio_frame.right = ((right / 1.5f) * _audio.volume);
    };

    auto mix_surround40 = [&](Surround40FrameFlt32& audio_frame) -> void
    {
        const auto index = _audio.rd_index;

        const float left  = (_audio.channel0[index] * 0.75f)
                          + (_audio.channel1[index] * 0.50f)
                          + (_audio.channel2[index] * 0.25f)
                          ;

        const float right = (_audio.channel0[index] * 0.25f)
                          + (_audio.channel1[index] * 0.50f)
                          + (_audio.channel2[index] * 0.75f)
                          ;

        audio_frame.front_left  = ((left  / 1.5f) * _audio.volume);
        audio_frame.front_right = ((right / 1.5f) * _audio.volume);
        audio_frame.back_left   = ((left  / 1.5f) * _audio.volume);
        audio_frame.back_right  = ((right / 1.5f) * _audio.volume);
    };

    auto render_mono = [&]() -> void
    {
        for(uint32_t index = 0; index < count; ++index) {
            if(_audio.rd_index != _audio.wr_index) {
                mix_mono(reinterpret_cast<MonoFrameFlt32*>(output)[index]);
                _audio.rd_index = ((_audio.rd_index + 1) % SND_BUFSIZE);
            }
            else {
                break;
            }
        }
    };

    auto render_stereo = [&]() -> void
    {
        for(uint32_t index = 0; index < count; ++index) {
            if(_audio.rd_index != _audio.wr_index) {
                mix_stereo(reinterpret_cast<StereoFrameFlt32*>(output)[index]);
                _audio.rd_index = ((_audio.rd_index + 1) % SND_BUFSIZE);
            }
            else {
                break;
            }
        }
    };

    auto render_surround40 = [&]() -> void
    {
        for(uint32_t index = 0; index < count; ++index) {
            if(_audio.rd_index != _audio.wr_index) {
                mix_surround40(reinterpret_cast<Surround40FrameFlt32*>(output)[index]);
                _audio.rd_index = ((_audio.rd_index + 1) % SND_BUFSIZE);
            }
            else {
                break;
            }
        }
    };

    auto render = [&]() -> void
    {
        switch(_device->playback.channels) {
            case 1:
                render_mono();
                break;
            case 2:
                render_stereo();
                break;
            case 4:
                render_surround40();
                break;
            default:
                break;
        }
    };

    return render();
}

auto Mainboard::cpu_mreq_m1(cpu::Instance& instance, uint16_t addr, uint8_t data) -> uint8_t
{
    /* mreq m1 */ {
        const uint16_t bank   = ((addr >> 14) & 0x0003);
        const uint16_t offset = ((addr >>  0) & 0x3fff);
        data = _state.pal_rd[bank][offset];
    }
    /* adjust t-states */ {
        auto& cpu(*_cpu);
        const uint32_t old_t_states = cpu->t_states;
        const uint32_t new_t_states = ((old_t_states + 3) & (~ 3));
        const uint32_t add_t_states = (new_t_states - old_t_states);
        cpu->t_states  = new_t_states;
        cpu->i_period += add_t_states;
    }
    return data;
}

auto Mainboard::cpu_mreq_rd(cpu::Instance& instance, uint16_t addr, uint8_t data) -> uint8_t
{
    /* mreq rd */ {
        const uint16_t bank   = ((addr >> 14) & 0x0003);
        const uint16_t offset = ((addr >>  0) & 0x3fff);
        data = _state.pal_rd[bank][offset];
    }
    return data;
}

auto Mainboard::cpu_mreq_wr(cpu::Instance& instance, uint16_t addr, uint8_t data) -> uint8_t
{
    /* mreq wr */ {
        const uint16_t bank   = ((addr >> 14) & 0x0003);
        const uint16_t offset = ((addr >>  0) & 0x3fff);
        _state.pal_wr[bank][offset] = data;
    }
    return data;
}

auto Mainboard::cpu_iorq_m1(cpu::Instance& instance, uint16_t port, uint8_t data) -> uint8_t
{
    /* clear data */ {
        data = 0xff;
    }
    /* iorq m1 */ {
        _vga->ack_interrupt();
    }
    return data;
}

auto Mainboard::cpu_iorq_rd(cpu::Instance& instance, uint16_t port, uint8_t data) -> uint8_t
{
    /* clear data */ {
        data = 0x00;
    }
    /* vga-core [0-------xxxxxxxx] [0x7fxx] */ {
        if((port & 0x8000) == 0) {
            auto& vga(*(_vga));
            data = vga.get_value(data);
        }
    }
    /* vdc-6845 [-0------xxxxxxxx] [0xbfxx] */ {
        if((port & 0x4000) == 0) {
            auto& vdc(*(_vdc));
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [-0----00xxxxxxxx] [0xbcxx] */
                    {
                        data = 0xff; // illegal
                    }
                    break;
                case 1: /* [-0----01xxxxxxxx] [0xbdxx] */
                    {
                        data = 0xff; // illegal
                    }
                    break;
                case 2: /* [-0----10xxxxxxxx] [0xbexx] */
                    {
                        data = vdc.get_index(data);
                    }
                    break;
                case 3: /* [-0----11xxxxxxxx] [0xbfxx] */
                    {
                        data = vdc.get_value(data);
                    }
                    break;
            }
        }
    }
    /* rom-conf [--0-----xxxxxxxx] [0xdfxx] */ {
        if((port & 0x2000) == 0) {
            /* ignore */
        }
    }
    /* prt-port [---0----xxxxxxxx] [0xefxx] */ {
        if((port & 0x1000) == 0) {
            /* ignore */
        }
    }
    /* ppi-8255 [----0---xxxxxxxx] [0xf7xx] */ {
        if((port & 0x0800) == 0) {
            auto& ppi(*(_ppi));
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [----0-00xxxxxxxx] [0xf4xx] */
                    {
                        data = ppi.rd_port_a(data);
                        data &= 0xff;
                    }
                    break;
                case 1: /* [----0-01xxxxxxxx] [0xf5xx] */
                    {
                        data = ppi.rd_port_b(data);
                        data &= 0xff;
                    }
                    break;
                case 2: /* [----0-10xxxxxxxx] [0xf6xx] */
                    {
                        data = ppi.rd_port_c(data);
                        data &= 0xf0;
                    }
                    break;
                case 3: /* [----0-11xxxxxxxx] [0xf7xx] */
                    {
                        data |= 0xff; // illegal
                    }
                    break;
            }
        }
    }
    /* fdc-765a [-----0--0xxxxxxx] [0xfb7f] */ {
        if((port & 0x0480) == 0) {
            auto& fdc(*(_fdc));
            const uint8_t function = (((port >> 7) & 2) | (port & 1));
            switch(function) {
                case 0: /* [-----0-00xxxxxx0] [0xfa7e] */
                    {
                        data = 0xff;
                    }
                    break;
                case 1: /* [-----0-00xxxxxx1] [0xfa7f] */
                    {
                        data = 0xff;
                    }
                    break;
                case 2: /* [-----0-10xxxxxx0] [0xfb7e] */
                    {
                        data = fdc.rd_stat(data);
                    }
                    break;
                case 3: /* [-----0-10xxxxxx1] [0xfb7f] */
                    {
                        data = fdc.rd_data(data);
                    }
                    break;
            }
        }
    }
    return data;
}

auto Mainboard::cpu_iorq_wr(cpu::Instance& instance, uint16_t port, uint8_t data) -> uint8_t
{
    /* vga-core [0-------xxxxxxxx] [0x7fxx] */ {
        if((port & 0x8000) == 0) {
            auto& vga(*(_vga));
            static_cast<void>(vga.set_value(data));
        }
    }
    /* vdc-6845 [-0------xxxxxxxx] [0xbfxx] */ {
        if((port & 0x4000) == 0) {
            auto& vdc(*(_vdc));
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [-0----00xxxxxxxx] [0xbcxx] */
                    {
                        static_cast<void>(vdc.set_index(data));
                    }
                    break;
                case 1: /* [-0----01xxxxxxxx] [0xbdxx] */
                    {
                        static_cast<void>(vdc.set_value(data));
                    }
                    break;
                case 2: /* [-0----10xxxxxxxx] [0xbexx] */
                    {
                        /* illegal */
                    }
                    break;
                case 3: /* [-0----11xxxxxxxx] [0xbfxx] */
                    {
                        /* illegal */
                    }
                    break;
            }
        }
    }
    /* rom-conf [--0-----xxxxxxxx] [0xdfxx] */ {
        if((port & 0x2000) == 0) {
            auto& vga(*(_vga));
            static_cast<void>(vga_setup_rom(vga, data));
        }
    }
    /* prt-port [---0----xxxxxxxx] [0xefxx] */ {
        if((port & 0x1000) == 0) {
            /* ignore */
        }
    }
    /* ppi-8255 [----0---xxxxxxxx] [0xf7xx] */ {
        if((port & 0x0800) == 0) {
            auto& ppi(*(_ppi));
            const uint8_t function = ((port >> 8) & 3);
            switch(function) {
                case 0: /* [----0-00xxxxxxxx] [0xf4xx] */
                    {
                        static_cast<void>(ppi.wr_port_a(data));
                    }
                    break;
                case 1: /* [----0-01xxxxxxxx] [0xf5xx] */
                    {
                        static_cast<void>(ppi.wr_port_b(data));
                    }
                    break;
                case 2: /* [----0-10xxxxxxxx] [0xf6xx] */
                    {
                        static_cast<void>(ppi.wr_port_c(data));
                    }
                    break;
                case 3: /* [----0-11xxxxxxxx] [0xf7xx] */
                    {
                        static_cast<void>(ppi.wr_ctrl_p(data));
                    }
                    break;
            }
        }
    }
    /* fdc-765a [-----0--0xxxxxxx] [0xfb7f] */ {
        if((port & 0x0480) == 0) {
            auto& fdc(*(_fdc));
            const uint8_t function = (((port >> 7) & 2) | ((port >> 0) & 1));
            switch(function) {
                case 0: /* [-----0-00xxxxxx0] [0xfa7e] */
                    {
                        static_cast<void>(fdc.set_motor((((data & 1) << 1) | ((data & 1) << 0))));
                    }
                    break;
                case 1: /* [-----0-00xxxxxx1] [0xfa7f] */
                    {
                        static_cast<void>(fdc.set_motor((((data & 1) << 1) | ((data & 1) << 0))));
                    }
                    break;
                case 2: /* [-----0-10xxxxxx0] [0xfb7e] */
                    {
                        static_cast<void>(fdc.wr_stat(data));
                    }
                    break;
                case 3: /* [-----0-10xxxxxx1] [0xfb7f] */
                    {
                        static_cast<void>(fdc.wr_data(data));
                    }
                    break;
            }
        }
    }
    return data;
}

auto Mainboard::vga_raise_nmi(vga::Instance& instance, uint8_t value) -> uint8_t
{
    _cpu->pulse_nmi();

    return value;
}

auto Mainboard::vga_raise_int(vga::Instance& instance, uint8_t value) -> uint8_t
{
    _cpu->pulse_int();

    return value;
}

auto Mainboard::vga_setup_ram(vga::Instance& instance, uint8_t value) -> uint8_t
{
    _state.ram_conf = (value & 0x3f);

    update_pal();

    return value;
}

auto Mainboard::vga_setup_rom(vga::Instance& instance, uint8_t value) -> uint8_t
{
    _state.rom_conf = (value & 0xff);

    update_pal();

    return value;
}

auto Mainboard::vga_setup_rmr(vga::Instance& instance, uint8_t value) -> uint8_t
{
    update_pal();

    return value;
}

auto Mainboard::vdc_hsync(vdc::Instance& instance, uint8_t hsync) -> uint8_t
{
    auto& vga(*_vga);

    auto on_rising_edge = [&]() -> void
    {
        vga.assert_hsync(hsync);
    };

    auto on_falling_edge = [&]() -> void
    {
        vga.assert_hsync(hsync);
    };

    if((_state.vdc_hsync = hsync) != 0) {
        on_rising_edge();
    }
    else {
        on_falling_edge();
    }
    return 0x00;
}

auto Mainboard::vdc_vsync(vdc::Instance& instance, uint8_t vsync) -> uint8_t
{
    auto& vga(*_vga);

    auto on_rising_edge = [&]() -> void
    {
        vga.assert_vsync(vsync);
    };

    auto on_falling_edge = [&]() -> void
    {
        vga.assert_vsync(vsync);
    };

    if((_state.vdc_vsync = vsync) != 0) {
        on_rising_edge();
    }
    else {
        on_falling_edge();
    }
    return 0x00;
}

auto Mainboard::ppi_port_a_rd(ppi::Instance& instance, uint8_t data) -> uint8_t
{
    auto psg_get_value = [&]() -> uint8_t
    {
        return _psg->get_value(data);
    };

    auto psg_set_value = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    auto psg_set_index = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    auto psg_inative = [&]() -> uint8_t
    {
        return 0xff; // inactive
    };

    auto psg_update = [&]() -> uint8_t
    {
        const uint8_t psg_function = (_state.psg_bdir << 2)
                                   | (_state.psg_bc2  << 1)
                                   | (_state.psg_bc1  << 0)
                                   ;
        switch(psg_function & 0x07) {
            case 0x03:
                return psg_get_value();
            case 0x06:
                return psg_set_value();
            case 0x07:
                return psg_set_index();
            default:
                break;
        }
        return psg_inative();
    };

    auto process = [&]() -> uint8_t
    {
        /* update psg */ {
            data = psg_update();
        }
        return data;
    };

    return process();
}

auto Mainboard::ppi_port_a_wr(ppi::Instance& instance, uint8_t data) -> uint8_t
{
    auto psg_get_value = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    auto psg_set_value = [&]() -> uint8_t
    {
        return _psg->set_value(data);
    };

    auto psg_set_index = [&]() -> uint8_t
    {
        return _psg->set_index(data);
    };

    auto psg_inative = [&]() -> uint8_t
    {
        return 0xff; // inactive
    };

    auto psg_update = [&]() -> uint8_t
    {
        const uint8_t psg_function = (_state.psg_bdir << 2)
                                   | (_state.psg_bc2  << 1)
                                   | (_state.psg_bc1  << 0)
                                   ;
        switch(psg_function & 0x07) {
            case 0x03:
                return psg_get_value();
            case 0x06:
                return psg_set_value();
            case 0x07:
                return psg_set_index();
            default:
                break;
        }
        return psg_inative();
    };

    auto process = [&]() -> uint8_t
    {
        /* update state */ {
            _state.psg_data = data;
        }
        /* update psg */ {
            data = psg_update();
        }
        return data;
    };

    return process();
}

auto Mainboard::ppi_port_b_rd(ppi::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return static_cast<uint8_t>((_state.cas_rd_data & 0x01) << 7)
             | static_cast<uint8_t>((_state.prt_busy    & 0x01) << 6)
             | static_cast<uint8_t>((_state.exp_busy    & 0x01) << 5)
             | static_cast<uint8_t>((_state.lnk_lk4     & 0x01) << 4)
             | static_cast<uint8_t>((_state.lnk_lk3     & 0x01) << 3)
             | static_cast<uint8_t>((_state.lnk_lk2     & 0x01) << 2)
             | static_cast<uint8_t>((_state.lnk_lk1     & 0x01) << 1)
             | static_cast<uint8_t>((_state.vdc_vsync   & 0x01) << 0)
             ;
    };

    return process();
}

auto Mainboard::ppi_port_b_wr(ppi::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    return process();
}

auto Mainboard::ppi_port_c_rd(ppi::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    return process();
}

auto Mainboard::ppi_port_c_wr(ppi::Instance& instance, uint8_t data) -> uint8_t
{
    auto psg_get_value = [&]() -> uint8_t
    {
        return 0xff; // invalid
    };

    auto psg_set_value = [&]() -> uint8_t
    {
        return _psg->set_value(_state.psg_data);
    };

    auto psg_set_index = [&]() -> uint8_t
    {
        return _psg->set_index(_state.psg_data);
    };

    auto psg_inative = [&]() -> uint8_t
    {
        return 0xff; // inactive
    };

    auto psg_update = [&]() -> uint8_t
    {
        const uint8_t psg_function = (_state.psg_bdir << 2)
                                   | (_state.psg_bc2  << 1)
                                   | (_state.psg_bc1  << 0)
                                   ;
        switch(psg_function & 0x07) {
            case 0x03:
                return psg_get_value();
            case 0x06:
                return psg_set_value();
            case 0x07:
                return psg_set_index();
            default:
                break;
        }
        return psg_inative();
    };

    auto process = [&]() -> uint8_t
    {
        /* update state */ {
            _state.psg_bdir    = ((data & 0x80) >> 7);
            _state.psg_bc1     = ((data & 0x40) >> 6);
            _state.cas_wr_data = ((data & 0x20) >> 5);
            _state.cas_motor   = ((data & 0x10) >> 4);
        }
        /* update keyboard line */ {
            static_cast<void>(_kbd->set_line((data & 0x0f) >> 0));
        }
        /* update psg */ {
            data = psg_update();
        }
        return data;
    };

    return process();
}

auto Mainboard::psg_port_a_rd(psg::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return _kbd->get_data();
    };

    return process();
}

auto Mainboard::psg_port_a_wr(psg::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    return process();
}

auto Mainboard::psg_port_b_rd(psg::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    return process();
}

auto Mainboard::psg_port_b_wr(psg::Instance& instance, uint8_t data) -> uint8_t
{
    auto process = [&]() -> uint8_t
    {
        return 0xff; // illegal
    };

    return process();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
