/*
 * sna-format.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_SNA_FORMAT_H__
#define __XCPC_SNA_FORMAT_H__

// ---------------------------------------------------------------------------
// sna::Header
// ---------------------------------------------------------------------------

namespace sna {

struct Header
{
    uint8_t signature[8];
    uint8_t reserved[8];
    uint8_t version;
    uint8_t cpu_p_af_l;
    uint8_t cpu_p_af_h;
    uint8_t cpu_p_bc_l;
    uint8_t cpu_p_bc_h;
    uint8_t cpu_p_de_l;
    uint8_t cpu_p_de_h;
    uint8_t cpu_p_hl_l;
    uint8_t cpu_p_hl_h;
    uint8_t cpu_p_ir_l;
    uint8_t cpu_p_ir_h;
    uint8_t cpu_p_iff1;
    uint8_t cpu_p_iff2;
    uint8_t cpu_p_ix_l;
    uint8_t cpu_p_ix_h;
    uint8_t cpu_p_iy_l;
    uint8_t cpu_p_iy_h;
    uint8_t cpu_p_sp_l;
    uint8_t cpu_p_sp_h;
    uint8_t cpu_p_pc_l;
    uint8_t cpu_p_pc_h;
    uint8_t cpu_p_im_l;
    uint8_t cpu_a_af_l;
    uint8_t cpu_a_af_h;
    uint8_t cpu_a_bc_l;
    uint8_t cpu_a_bc_h;
    uint8_t cpu_a_de_l;
    uint8_t cpu_a_de_h;
    uint8_t cpu_a_hl_l;
    uint8_t cpu_a_hl_h;
    uint8_t vga_ink_ix;
    uint8_t vga_ink_00;
    uint8_t vga_ink_01;
    uint8_t vga_ink_02;
    uint8_t vga_ink_03;
    uint8_t vga_ink_04;
    uint8_t vga_ink_05;
    uint8_t vga_ink_06;
    uint8_t vga_ink_07;
    uint8_t vga_ink_08;
    uint8_t vga_ink_09;
    uint8_t vga_ink_10;
    uint8_t vga_ink_11;
    uint8_t vga_ink_12;
    uint8_t vga_ink_13;
    uint8_t vga_ink_14;
    uint8_t vga_ink_15;
    uint8_t vga_ink_16;
    uint8_t vga_config;
    uint8_t ram_select;
    uint8_t vdc_reg_ix;
    uint8_t vdc_reg_00;
    uint8_t vdc_reg_01;
    uint8_t vdc_reg_02;
    uint8_t vdc_reg_03;
    uint8_t vdc_reg_04;
    uint8_t vdc_reg_05;
    uint8_t vdc_reg_06;
    uint8_t vdc_reg_07;
    uint8_t vdc_reg_08;
    uint8_t vdc_reg_09;
    uint8_t vdc_reg_10;
    uint8_t vdc_reg_11;
    uint8_t vdc_reg_12;
    uint8_t vdc_reg_13;
    uint8_t vdc_reg_14;
    uint8_t vdc_reg_15;
    uint8_t vdc_reg_16;
    uint8_t vdc_reg_17;
    uint8_t rom_select;
    uint8_t ppi_port_a;
    uint8_t ppi_port_b;
    uint8_t ppi_port_c;
    uint8_t ppi_ctrl_p;
    uint8_t psg_reg_ix;
    uint8_t psg_reg_00;
    uint8_t psg_reg_01;
    uint8_t psg_reg_02;
    uint8_t psg_reg_03;
    uint8_t psg_reg_04;
    uint8_t psg_reg_05;
    uint8_t psg_reg_06;
    uint8_t psg_reg_07;
    uint8_t psg_reg_08;
    uint8_t psg_reg_09;
    uint8_t psg_reg_10;
    uint8_t psg_reg_11;
    uint8_t psg_reg_12;
    uint8_t psg_reg_13;
    uint8_t psg_reg_14;
    uint8_t psg_reg_15;
    uint8_t ram_size_l;
    uint8_t ram_size_h;
    uint8_t padding[147];
};

}

// ---------------------------------------------------------------------------
// sna::Memory
// ---------------------------------------------------------------------------

namespace sna {

struct Memory
{
    uint8_t data[16384];
};

}

// ---------------------------------------------------------------------------
// sna::State
// ---------------------------------------------------------------------------

namespace sna {

struct State
{
    Header header;
    Memory memory[32];
};

static_assert(sizeof(State::header) == 256UL,          "State::header is invalid");
static_assert(sizeof(State::memory) == 512UL * 1024UL, "State::memory is invalid");

}

// ---------------------------------------------------------------------------
// sna::Snapshot
// ---------------------------------------------------------------------------

namespace sna {

class Snapshot
{
public: // public interface
    Snapshot();

    Snapshot(const Snapshot&) = delete;

    Snapshot& operator=(const Snapshot&) = delete;

    virtual ~Snapshot() = default;

    auto load(const std::string& filename) -> void;

    auto save(const std::string& filename) -> void;

    auto operator->() -> State*
    {
        return &_state;
    }

private: // private data
    State _state;
};

}

// ---------------------------------------------------------------------------
// sna::SnapshotReader
// ---------------------------------------------------------------------------

namespace sna {

class SnapshotReader
{
public: // public interface
    SnapshotReader(const std::string& filename);

    SnapshotReader(const SnapshotReader&) = delete;

    SnapshotReader& operator=(const SnapshotReader&) = delete;

    virtual ~SnapshotReader();

    auto load(Snapshot& snapshot) -> void;

private: // private data
    FILE* _file;
};

}

// ---------------------------------------------------------------------------
// sna::SnapshotWriter
// ---------------------------------------------------------------------------

namespace sna {

class SnapshotWriter
{
public: // public interface
    SnapshotWriter(const std::string& filename);

    SnapshotWriter(const SnapshotWriter&) = delete;

    SnapshotWriter& operator=(const SnapshotWriter&) = delete;

    virtual ~SnapshotWriter();

    auto save(Snapshot& snapshot) -> void;

private: // private data
    FILE* _file;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_SNA_FORMAT_H__ */
