/*
 * cpc-mainboard.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_CPC_MAINBOARD_H__
#define __XCPC_CPC_MAINBOARD_H__

#include <xcpc/amstrad/cpc/cpc-settings.h>
#include <xcpc/devices/dpy/dpy-device.h>
#include <xcpc/devices/kbd/kbd-device.h>
#include <xcpc/devices/cpu/cpu-device.h>
#include <xcpc/devices/vga/vga-device.h>
#include <xcpc/devices/vdc/vdc-device.h>
#include <xcpc/devices/ppi/ppi-device.h>
#include <xcpc/devices/psg/psg-device.h>
#include <xcpc/devices/fdc/fdc-device.h>
#include <xcpc/devices/mem/mem-device.h>
#include <xcpc/formats/cdt/cdt-format.h>
#include <xcpc/formats/dsk/dsk-format.h>
#include <xcpc/formats/sna/sna-format.h>

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace cpc {

using TimeVal   = struct timeval;
using PaintFunc = void (*)(Mainboard*);

}

// ---------------------------------------------------------------------------
// cpc::Mainboard
// ---------------------------------------------------------------------------

namespace cpc {

class Mainboard final
    : public xcpc::Mainboard
    , public AudioProcessor
    , private dpy::Interface
    , private kbd::Interface
    , private cpu::Interface
    , private vga::Interface
    , private vdc::Interface
    , private ppi::Interface
    , private psg::Interface
    , private fdc::Interface
    , private mem::Interface
{
public: // public interface
    Mainboard(Machine& machine);

    Mainboard(Machine& machine, const Settings& settings);

    Mainboard(const Mainboard&) = delete;

    Mainboard& operator=(const Mainboard&) = delete;

    virtual ~Mainboard();

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

public: // backend interface
    auto on_reset(Event& event) -> unsigned long;

    auto on_clock(Event& event) -> unsigned long;

    auto on_create_window(Event& event) -> unsigned long;

    auto on_delete_window(Event& event) -> unsigned long;

    auto on_resize_window(Event& event) -> unsigned long;

    auto on_expose_window(Event& event) -> unsigned long;

    auto on_key_press(Event& event) -> unsigned long;

    auto on_key_release(Event& event) -> unsigned long;

    auto on_button_press(Event& event) -> unsigned long;

    auto on_button_release(Event& event) -> unsigned long;

    auto on_motion_notify(Event& event) -> unsigned long;

public: // public types
    static constexpr uint32_t FLAG_RESET  = 0x01;
    static constexpr uint32_t FLAG_PAUSE  = 0x02;
    static constexpr uint32_t SND_BUFSIZE = 16384;

    struct Setup
    {
        CompanyName  company_name;
        MachineType  machine_type;
        MonitorType  monitor_type;
        RefreshRate  refresh_rate;
        KeyboardType keyboard_type;
        MemorySize   memory_size;
        uint32_t     speedup;
        bool         xshm;
        bool         scanlines;
    };

    struct Stats
    {
        unsigned int frame_count;
        unsigned int frame_drawn;
        char         buffer[256];
    };

    struct Clock
    {
        TimeVal currtime;
        TimeVal deadline;
        TimeVal proftime;
    };

    struct Funcs
    {
        PaintFunc paint_func;
    };

    struct State
    {
        uint32_t cpc_flags;   /* cpc flags                 */
        uint32_t cpc_clock;   /* cpc clock                 */
        uint32_t cpc_ticks;   /* cpc ticks                 */
        uint32_t cpu_clock;   /* cpu clock                 */
        uint32_t cpu_ticks;   /* cpu ticks                 */
        uint32_t vdc_clock;   /* vdc clock                 */
        uint32_t vdc_ticks;   /* vdc ticks                 */
        uint32_t psg_clock;   /* psg clock                 */
        uint32_t psg_ticks;   /* psg ticks                 */
        uint32_t snd_clock;   /* snd clock                 */
        uint32_t snd_ticks;   /* snd ticks                 */
        uint8_t  vdc_hsync;   /* display hsync signal      */
        uint8_t  vdc_vsync;   /* display vsync signal      */
        uint8_t  lnk_lk1;     /* manufacturer id bit1      */
        uint8_t  lnk_lk2;     /* manufacturer id bit2      */
        uint8_t  lnk_lk3;     /* manufacturer id bit3      */
        uint8_t  lnk_lk4;     /* screen refresh rate       */
        uint8_t  exp_busy;    /* expansion absent/present  */
        uint8_t  prt_busy;    /* printer busy/ready        */
        uint8_t  psg_data;    /* psg data latch            */
        uint8_t  psg_bdir;    /* psg bdir signal           */
        uint8_t  psg_bc1;     /* psg bc1 signal            */
        uint8_t  psg_bc2;     /* psg bc2 signal            */
        uint8_t  cas_motor;   /* cassette motor            */
        uint8_t  cas_sound;   /* cassette sound            */
        uint8_t  cas_rd_data; /* cassette read data        */
        uint8_t  cas_wr_data; /* cassette write data       */
        uint8_t  ram_conf;    /* ram configuration         */
        uint8_t  rom_conf;    /* rom configuration         */
        uint8_t* pal_rd[4];   /* pal ram/rom read banking  */
        uint8_t* pal_wr[4];   /* pal ram/rom write banking */
    };

    struct Audio
    {
        float    channel0[SND_BUFSIZE];
        float    channel1[SND_BUFSIZE];
        float    channel2[SND_BUFSIZE];
        float    volume;
        uint32_t rd_index;
        uint32_t wr_index;
    };

    struct Video
    {
        uint32_t frame_rate;
        uint32_t frame_duration;
    };

private: // private interface
    auto construct_dpy() -> void;
    auto construct_kbd() -> void;
    auto construct_cpu() -> void;
    auto construct_vga() -> void;
    auto construct_vdc() -> void;
    auto construct_ppi() -> void;
    auto construct_psg() -> void;
    auto construct_fdc() -> void;
    auto construct_ram() -> void;
    auto construct_rom() -> void;
    auto construct_exp() -> void;

    auto destruct_dpy() -> void;
    auto destruct_kbd() -> void;
    auto destruct_cpu() -> void;
    auto destruct_vga() -> void;
    auto destruct_vdc() -> void;
    auto destruct_ppi() -> void;
    auto destruct_psg() -> void;
    auto destruct_fdc() -> void;
    auto destruct_ram() -> void;
    auto destruct_rom() -> void;
    auto destruct_exp() -> void;

    auto reset_dpy() -> void;
    auto reset_kbd() -> void;
    auto reset_cpu() -> void;
    auto reset_vga() -> void;
    auto reset_vdc() -> void;
    auto reset_ppi() -> void;
    auto reset_psg() -> void;
    auto reset_fdc() -> void;
    auto reset_ram() -> void;
    auto reset_rom() -> void;
    auto reset_exp() -> void;

    auto configure(const Settings& settings) -> void;
    auto load_lower_rom(const std::string& filename) -> void;
    auto load_upper_rom(const std::string& filename) -> void;
    auto load_expansion(const std::string& filename, const int index) -> void;
    auto load_cpc(sna::Snapshot& snapshot) -> void;
    auto save_cpc(sna::Snapshot& snapshot) -> void;

    auto update_vga() -> void;
    auto update_pal() -> void;
    auto update_stats() -> void;
    auto paint_08bpp() -> void;
    auto paint_16bpp() -> void;
    auto paint_32bpp() -> void;

public: // audio interface
    virtual void process(const void* input, void* output, const uint32_t count) override final;

private: // cpu interface
    virtual auto cpu_mreq_m1(cpu::Device& device, uint16_t addr, uint8_t data) -> uint8_t override final;
    virtual auto cpu_mreq_rd(cpu::Device& device, uint16_t addr, uint8_t data) -> uint8_t override final;
    virtual auto cpu_mreq_wr(cpu::Device& device, uint16_t addr, uint8_t data) -> uint8_t override final;
    virtual auto cpu_iorq_m1(cpu::Device& device, uint16_t port, uint8_t data) -> uint8_t override final;
    virtual auto cpu_iorq_rd(cpu::Device& device, uint16_t port, uint8_t data) -> uint8_t override final;
    virtual auto cpu_iorq_wr(cpu::Device& device, uint16_t port, uint8_t data) -> uint8_t override final;

private: // vga interface
    virtual auto vga_raise_nmi(vga::Device& device, uint8_t value) -> uint8_t override final;
    virtual auto vga_raise_int(vga::Device& device, uint8_t value) -> uint8_t override final;
    virtual auto vga_setup_ram(vga::Device& device, uint8_t value) -> uint8_t override final;
    virtual auto vga_setup_rom(vga::Device& device, uint8_t value) -> uint8_t override final;
    virtual auto vga_setup_rmr(vga::Device& device, uint8_t value) -> uint8_t override final;

private: // vdc interface
    virtual auto vdc_hsync(vdc::Device& device, uint8_t hsync) -> uint8_t override final;
    virtual auto vdc_vsync(vdc::Device& device, uint8_t vsync) -> uint8_t override final;

private: // ppi interface
    virtual auto ppi_port_a_rd(ppi::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto ppi_port_a_wr(ppi::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto ppi_port_b_rd(ppi::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto ppi_port_b_wr(ppi::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto ppi_port_c_rd(ppi::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto ppi_port_c_wr(ppi::Device& device, uint8_t data) -> uint8_t override final;

private: // psg interface
    virtual auto psg_port_a_rd(psg::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto psg_port_a_wr(psg::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto psg_port_b_rd(psg::Device& device, uint8_t data) -> uint8_t override final;
    virtual auto psg_port_b_wr(psg::Device& device, uint8_t data) -> uint8_t override final;

private: // private data
    Machine&     _machine;
    Setup        _setup;
    Stats        _stats;
    Clock        _clock;
    Funcs        _funcs;
    State        _state;
    Audio        _audio;
    Video        _video;
    dpy::Device* _dpy;
    kbd::Device* _kbd;
    cpu::Device* _cpu;
    vga::Device* _vga;
    vdc::Device* _vdc;
    ppi::Device* _ppi;
    psg::Device* _psg;
    fdc::Device* _fdc;
    mem::Device* _ram[8];
    mem::Device* _rom[2];
    mem::Device* _exp[256];
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_CPC_MAINBOARD_H__ */
