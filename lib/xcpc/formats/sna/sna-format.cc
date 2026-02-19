/*
 * sna-format.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <cstdarg>
#include <cstdint>
#include <climits>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "sna-format.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using State          = sna::State;
    using Header         = sna::Header;
    using Memory         = sna::Memory;
    using Snapshot       = sna::Snapshot;
    using SnapshotReader = sna::SnapshotReader;
    using SnapshotWriter = sna::SnapshotWriter;

    static constexpr uint8_t SNAPSHOT_VERSION_1 = 1;
    static constexpr uint8_t SNAPSHOT_VERSION_2 = 2;
    static constexpr uint8_t SNAPSHOT_VERSION_3 = 3;

    static const char signature[8];
    static const char reserved[8];
};

const char BasicTraits::signature[8] = {
    'M', 'V', ' ', '-', ' ', 'S', 'N', 'A'
};

const char BasicTraits::reserved[8] = {
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

}

// ---------------------------------------------------------------------------
// <anonymous>::StateTraits
// ---------------------------------------------------------------------------

namespace {

struct StateTraits final
    : public BasicTraits
{
    static auto construct(State& state) -> void
    {
        auto init_signature = [&]() -> void
        {
            static_cast<void>(::memcpy(state.header.signature, signature, sizeof(state.header.signature)));
        };

        auto init_reserved = [&]() -> void
        {
            static_cast<void>(::memcpy(state.header.reserved, reserved, sizeof(state.header.reserved)));
        };

        auto init_version = [&]() -> void
        {
            state.header.version = SNAPSHOT_VERSION_1;
        };

        init_signature();
        init_reserved();
        init_version();
    }

    static auto check(Header& header) -> void
    {
        if(::memcmp(header.signature, signature, sizeof(header.signature)) != 0) {
            throw std::runtime_error("bad signature");
        }
        if((header.version != SNAPSHOT_VERSION_1)
        && (header.version != SNAPSHOT_VERSION_2)
        && (header.version != SNAPSHOT_VERSION_3)) {
            throw std::runtime_error("bad version");
        }
    }
};

}

// ---------------------------------------------------------------------------
// sna::Snapshot
// ---------------------------------------------------------------------------

namespace sna {

Snapshot::Snapshot()
    : _state()
{
    StateTraits::construct(_state);
}

auto Snapshot::load(const std::string& filename) -> void
{
    SnapshotReader reader(filename);

    reader.load(*this);
}

auto Snapshot::save(const std::string& filename) -> void
{
    SnapshotWriter writer(filename);

    writer.save(*this);
}

}

// ---------------------------------------------------------------------------
// sna::SnapshotReader
// ---------------------------------------------------------------------------

namespace sna {

SnapshotReader::SnapshotReader(const std::string& filename)
    : _file(nullptr)
{
    if((_file = ::fopen(filename.c_str(), "r")) == nullptr) {
        throw std::runtime_error("unable to open snapshot for reading");
    }
}

SnapshotReader::~SnapshotReader()
{
    if(_file != nullptr) {
        _file = (::fclose(_file), nullptr);
    }
}

auto SnapshotReader::load(Snapshot& snapshot) -> void
{
    auto load_check = [&](Header& header) -> void
    {
        StateTraits::check(header);
    };

    auto load_header = [&](Header& header) -> void
    {
        constexpr size_t header_size = sizeof(header);
        const     size_t byte_count  = ::fread(&header, 1, header_size, _file);

        if(byte_count != header_size) {
            throw std::runtime_error("unable to load snapshot header");
        }
    };

    auto load_memory = [&](Memory& memory) -> void
    {
        constexpr size_t memory_size = sizeof(memory);
        const     size_t byte_count  = ::fread(&memory, 1, memory_size, _file);

        if(byte_count != memory_size) {
            throw std::runtime_error("unable to load snapshot memory");
        }
    };

    load_check(snapshot->header);
    load_header(snapshot->header);
    load_check(snapshot->header);

    size_t remaining_bytes = 0;
    remaining_bytes |= ((static_cast<size_t>(snapshot->header.ram_size_h)) << 18);
    remaining_bytes |= ((static_cast<size_t>(snapshot->header.ram_size_l)) << 10);
    for(auto& memory : snapshot->memory) {
        constexpr size_t memory_size = sizeof(memory.data);
        if(remaining_bytes >= memory_size) {
            load_memory(memory);
            remaining_bytes -= memory_size;
        }
        else {
            break;
        }
    }
}

}

// ---------------------------------------------------------------------------
// sna::SnapshotWriter
// ---------------------------------------------------------------------------

namespace sna {

SnapshotWriter::SnapshotWriter(const std::string& filename)
    : _file(nullptr)
{
    if((_file = ::fopen(filename.c_str(), "w")) == nullptr) {
        throw std::runtime_error("unable to open snapshot for writing");
    }
}

SnapshotWriter::~SnapshotWriter()
{
    if(_file != nullptr) {
        _file = (::fclose(_file), nullptr);
    }
}

auto SnapshotWriter::save(Snapshot& snapshot) -> void
{
    auto save_check = [&](Header& header) -> void
    {
        StateTraits::check(header);
    };

    auto save_header = [&](Header& header) -> void
    {
        constexpr size_t header_size = sizeof(header);
        const     size_t byte_count  = ::fwrite(&header, 1, header_size, _file);

        if(byte_count != header_size) {
            throw std::runtime_error("unable to save snapshot header");
        }
    };

    auto save_memory = [&](Memory& memory) -> void
    {
        constexpr size_t memory_size = sizeof(memory.data);
        const     size_t byte_count  = ::fwrite(&memory.data, 1, memory_size, _file);

        if(byte_count != memory_size) {
            throw std::runtime_error("unable to save snapshot memory");
        }
    };

    save_check(snapshot->header);
    save_header(snapshot->header);
    save_check(snapshot->header);

    size_t remaining_bytes = 0;
    remaining_bytes |= ((static_cast<size_t>(snapshot->header.ram_size_h)) << 18);
    remaining_bytes |= ((static_cast<size_t>(snapshot->header.ram_size_l)) << 10);
    for(auto& memory : snapshot->memory) {
        constexpr size_t memory_size = sizeof(memory.data);
        if(remaining_bytes >= memory_size) {
            save_memory(memory);
            remaining_bytes -= memory_size;
        }
        else {
            break;
        }
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
