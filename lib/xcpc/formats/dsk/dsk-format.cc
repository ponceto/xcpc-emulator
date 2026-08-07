/*
 * dsk-format.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "dsk-format.h"

// ---------------------------------------------------------------------------
// some useful macros
// ---------------------------------------------------------------------------

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// ---------------------------------------------------------------------------
//  <anonymous>::utils
// ---------------------------------------------------------------------------

namespace {

struct utils
{
    static void println(const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        static_cast<void>(::vfprintf(stdout, format, arguments));
        static_cast<void>(::fputc('\n', stdout));
        va_end(arguments);
    }

    static void errorln(const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        static_cast<void>(::vfprintf(stderr, format, arguments));
        static_cast<void>(::fputc('\n', stderr));
        va_end(arguments);
    }

    static int open(const std::string& filename, const int flags, const int mode = 0)
    {
        const int rc = ::open(filename.c_str(), flags, mode);
        if(rc < 0) {
            throw std::runtime_error("open() has failed");
        }
        return rc;
    }

    static ssize_t fetch(int fd, void* buffer, size_t length)
    {
        uint8_t* bufptr = reinterpret_cast<uint8_t*>(buffer);
        ssize_t  buflen = static_cast<ssize_t>(length);

        length = 0;
        while(buflen > 0) {
            const ssize_t rc = ::read(fd, bufptr, buflen);
            if(rc < 0) {
                throw std::runtime_error("fetch() has failed");
            }
            if(rc == 0) {
                break;
            }
            length += rc;
            bufptr += rc;
            buflen -= rc;
        }
        return length;
    }

    static ssize_t store(int fd, const void* buffer, size_t length)
    {
        const uint8_t* bufptr = reinterpret_cast<const uint8_t*>(buffer);
        ssize_t        buflen = static_cast<ssize_t>(length);

        length = 0;
        while(buflen > 0) {
            const ssize_t rc = ::write(fd, bufptr, buflen);
            if(rc < 0) {
                throw std::runtime_error("store() has failed");
            }
            if(rc == 0) {
                break;
            }
            length += rc;
            bufptr += rc;
            buflen -= rc;
        }
        return length;
    }

    static void close(int fd)
    {
        const int rc = ::close(fd);
        if(rc != 0) {
            throw std::runtime_error("close() has failed");
        }
    }

    static void unlink(const std::string& filename)
    {
        const int rc = ::unlink(filename.c_str());
        if(rc < 0) {
            throw std::runtime_error("unlink() has failed");
        }
    }

    template <typename T>
    static void clear_data(T& data, const uint8_t byte)
    {
        static_cast<void>(::memset(&data, byte, sizeof(T)));
    }

    template <typename T>
    static auto fetch_data(int fd, T& data) -> void
    {
        const size_t size = sizeof(T);
        const ssize_t rc = fetch(fd, &data, size);
        if(rc != size) {
            throw std::runtime_error("fetch_data() has failed");
        }
    }

    template <typename T>
    static auto store_data(int fd, const T& data) -> void
    {
        const size_t size = sizeof(T);
        const ssize_t rc = store(fd, &data, size);
        if(rc != size) {
            throw std::runtime_error("store_data() has failed");
        }
    }
};

}

// ---------------------------------------------------------------------------
// dsk::internal
// ---------------------------------------------------------------------------

namespace dsk {

namespace internal {

constexpr char std_magic[] = {
    'M', 'V', ' ', '-', ' ', 'C', 'P', 'C', 'E', 'M', 'U', ' ', 'D', 'i', 's', 'k', '-', 'F', 'i', 'l', 'e', '\r', '\n'
};

constexpr char ext_magic[] = {
    'E', 'X', 'T', 'E', 'N', 'D', 'E', 'D', ' ', 'C', 'P', 'C', ' ', 'D', 'S', 'K', ' ', 'F', 'i', 'l', 'e', '\r', '\n'
};

constexpr char dsk_info[] = {
    'D', 'i', 's', 'k', '-', 'I', 'n', 'f', 'o', '\r', '\n'
};

constexpr char dsk_tool[] = {
    'D', 'i', 's', 'k', '-', 'U', 't', 'i', 'l', 'i', 't', 'i', 'e', 's'
};

constexpr char trk_info[] = {
    'T', 'r', 'a', 'c', 'k', '-', 'I', 'n', 'f', 'o', '\r', '\n'
};

struct traits
{
    static constexpr uint8_t MIN_NUMBER_OF_SIDES   = 1;
    static constexpr uint8_t MAX_NUMBER_OF_SIDES   = 2;
    static constexpr uint8_t MIN_NUMBER_OF_TRACKS  = 1;
    static constexpr uint8_t MAX_NUMBER_OF_TRACKS  = 80;
    static constexpr uint8_t MIN_NUMBER_OF_SECTORS = 1;
    static constexpr uint8_t MAX_NUMBER_OF_SECTORS = 29;
    static constexpr uint8_t MIN_SECTOR_SIZE       = 0;
    static constexpr uint8_t MAX_SECTOR_SIZE       = 7;
};

}

}

// ---------------------------------------------------------------------------
// dsk::BaseAdapter
// ---------------------------------------------------------------------------

namespace dsk {

BaseAdapter::BaseAdapter(const int file)
    : _file(file)
{
}

}

// ---------------------------------------------------------------------------
// dsk::DiskInfoAdapter
// ---------------------------------------------------------------------------

namespace dsk {

DiskInfoAdapter::DiskInfoAdapter(DiskRecord& disk, const int file)
    : BaseAdapter(file)
    , _disk(disk)
{
}

void DiskInfoAdapter::clear()
{
    utils::clear_data(_disk.info, 0x00);
}

void DiskInfoAdapter::check()
{
    bool relaxed = false;

    auto check_standard_magic = [&]() -> bool
    {
        int       index = 0;
        int const min_count = 8;
        int const max_count = countof(internal::std_magic);
        for(auto byte : _disk.info.s.magic) {
            if(byte != internal::std_magic[index]) {
                if(index >= min_count) {
                    relaxed = true;
                    return true;
                }
                return false;
            }
            if(++index >= max_count) {
                break;
            }
        }
        return true;
    };

    auto check_extended_magic = [&]() -> bool
    {
        int       index = 0;
        int const min_count = 8;
        int const max_count = countof(internal::ext_magic);
        for(auto byte : _disk.info.s.magic) {
            if(byte != internal::ext_magic[index]) {
                if(index >= min_count) {
                    relaxed = true;
                    return true;
                }
                return false;
            }
            if(++index >= max_count) {
                break;
            }
        }
        return true;
    };

    auto check_disk_signature = [&]() -> bool
    {
        int       index = 0;
        int const min_count = 9;
        int const max_count = countof(internal::dsk_info);
        for(auto byte : _disk.info.s.signature) {
            if(byte != internal::dsk_info[index]) {
                if(relaxed == false) {
                    if(index >= min_count) {
                        relaxed = true;
                        return true;
                    }
                    return false;
                }
            }
            if(++index >= max_count) {
                break;
            }
        }
        return true;
    };

    auto check_number_of_tracks = [&]() -> bool
    {
        if((_disk.info.s.number_of_tracks < internal::traits::MIN_NUMBER_OF_TRACKS)
        || (_disk.info.s.number_of_tracks > internal::traits::MAX_NUMBER_OF_TRACKS)) {
            return false;
        }
        return true;
    };

    auto check_number_of_sides = [&]() -> bool
    {
        if((_disk.info.s.number_of_sides < internal::traits::MIN_NUMBER_OF_SIDES)
        || (_disk.info.s.number_of_sides > internal::traits::MAX_NUMBER_OF_SIDES)) {
            return false;
        }
        return true;
    };

    if((check_standard_magic() == false) && (check_extended_magic() == false)) {
        throw std::runtime_error("disk has a bad magic");
    }
    if(check_disk_signature() == false) {
        throw std::runtime_error("disk has a bad signature");
    }
    if(check_number_of_tracks() == false) {
        throw std::runtime_error("disk has a bad number of tracks");
    }
    if(check_number_of_sides() == false) {
        throw std::runtime_error("disk has a bad number of sides");
    }
}

void DiskInfoAdapter::fetch()
{
    utils::fetch_data(_file, _disk.info);
}

void DiskInfoAdapter::store()
{
    utils::store_data(_file, _disk.info);
}

void DiskInfoAdapter::print()
{
    utils::println("[disk]");
    utils::println("disk.info.magic ................ %s", get_magic().c_str());
    utils::println("disk.info.signature ............ %s", get_signature().c_str());
    utils::println("disk.info.tool ................. %s", get_creator().c_str());
    utils::println("disk.info.number_of_tracks ..... %d", get_number_of_tracks());
    utils::println("disk.info.number_of_sides ...... %d", get_number_of_sides());
    utils::println("disk.info.track_size ........... %d", get_track_size());
    utils::println("disk.info.side_size ............ %d", get_side_size());
    utils::println("");
}

void DiskInfoAdapter::set_magic() const
{
    int       index = 0;
    int const count = countof(internal::std_magic);
    for(auto& byte : _disk.info.s.magic) {
        byte = internal::std_magic[index];
        if(++index >= count) {
            break;
        }
    }
}

void DiskInfoAdapter::set_signature() const
{
    int       index = 0;
    int const count = countof(internal::dsk_info);
    for(auto& byte : _disk.info.s.signature) {
        byte = internal::dsk_info[index];
        if(++index >= count) {
            break;
        }
    }
}

void DiskInfoAdapter::set_creator() const
{
    int       index = 0;
    int const count = countof(internal::dsk_tool);
    for(auto& byte : _disk.info.s.tool) {
        byte = internal::dsk_tool[index];
        if(++index >= count) {
            break;
        }
    }
}

void DiskInfoAdapter::set_number_of_tracks(const uint8_t number_of_tracks) const
{
    _disk.info.s.number_of_tracks = number_of_tracks;
}

void DiskInfoAdapter::set_number_of_sides(const uint8_t number_of_sides) const
{
    _disk.info.s.number_of_sides = number_of_sides;
}

void DiskInfoAdapter::set_track_size(const uint16_t track_size) const
{
    _disk.info.s.track_size_lsb = ((track_size >> 0) & 0xff);
    _disk.info.s.track_size_msb = ((track_size >> 8) & 0xff);
}

void DiskInfoAdapter::set_padding(const uint8_t padding) const
{
    for(auto& byte : _disk.info.s.padding) {
        byte = padding;
    }
}

auto DiskInfoAdapter::get_magic() const -> std::string
{
    std::string string;

    for(auto character : _disk.info.s.magic) {
        if(character >= ' ') {
            string += character;
        }
        else if(character == '\0') {
            string += '\\';
            string += '0';
        }
        else if(character == '\r') {
            string += '\\';
            string += 'r';
        }
        else if(character == '\n') {
            string += '\\';
            string += 'n';
        }
        else {
            string += '?';
        }
    }
    return string;
}

auto DiskInfoAdapter::get_signature() const -> std::string
{
    std::string string;

    for(auto character : _disk.info.s.signature) {
        if(character >= ' ') {
            string += character;
        }
        else if(character == '\0') {
            string += '\\';
            string += '0';
        }
        else if(character == '\r') {
            string += '\\';
            string += 'r';
        }
        else if(character == '\n') {
            string += '\\';
            string += 'n';
        }
        else {
            string += '?';
        }
    }
    return string;
}

auto DiskInfoAdapter::get_creator() const -> std::string
{
    std::string string;

    for(auto character : _disk.info.s.tool) {
        if(character >= ' ') {
            string += character;
        }
        else if(character == '\0') {
            string += '\\';
            string += '0';
        }
        else if(character == '\r') {
            string += '\\';
            string += 'r';
        }
        else if(character == '\n') {
            string += '\\';
            string += 'n';
        }
        else {
            string += '?';
        }
    }
    return string;
}

auto DiskInfoAdapter::get_number_of_tracks() const -> uint8_t
{
    return _disk.info.s.number_of_tracks;
}

auto DiskInfoAdapter::get_number_of_sides() const -> uint8_t
{
    return _disk.info.s.number_of_sides;
}

auto DiskInfoAdapter::get_track_size() const -> uint16_t
{
    return (static_cast<uint16_t>(_disk.info.s.track_size_msb) << 8)
         | (static_cast<uint16_t>(_disk.info.s.track_size_lsb) << 0)
         ;
}

auto DiskInfoAdapter::get_side_size() const -> uint32_t
{
    const uint32_t number_of_tracks = get_number_of_tracks();
    const uint32_t number_of_sides  = get_number_of_sides();
    const uint32_t track_size       = get_track_size();
    const uint32_t side_size        = (number_of_sides * (number_of_tracks * track_size));

    return side_size;
}

}

// ---------------------------------------------------------------------------
// dsk::TrackInfoAdapter
// ---------------------------------------------------------------------------

namespace dsk {

TrackInfoAdapter::TrackInfoAdapter(DiskRecord& disk, TrackRecord& track, const int file)
    : BaseAdapter(file)
    , _disk(disk)
    , _track(track)
{
}

void TrackInfoAdapter::clear()
{
    utils::clear_data(_track.info, 0x00);
}

void TrackInfoAdapter::check()
{
    bool relaxed = false;

    auto check_track_signature = [&]() -> bool
    {
        int       index = 0;
        int const min_count = 10;
        int const max_count = countof(internal::trk_info);
        for(auto& byte : _track.info.s.signature) {
            if(byte != internal::trk_info[index]) {
                if(index >= min_count) {
                    relaxed = true;
                    return true;
                }
                return false;
            }
            if(++index >= max_count) {
                break;
            }
        }
        return true;
    };

    auto check_track_number = [&]() -> bool
    {
        if(_track.info.s.track_number >= internal::traits::MAX_NUMBER_OF_TRACKS) {
            return false;
        }
        return true;
    };

    auto check_side_number = [&]() -> bool
    {
        if(_track.info.s.side_number >= internal::traits::MAX_NUMBER_OF_SIDES) {
            return false;
        }
        return true;
    };

    auto check_sector_size = [&]() -> bool
    {
        if(_track.info.s.sector_size >= internal::traits::MAX_SECTOR_SIZE) {
            return false;
        }
        return true;
    };

    auto check_number_of_sectors = [&]() -> bool
    {
        if(_track.info.s.number_of_sectors >= internal::traits::MAX_NUMBER_OF_SECTORS) {
            return false;
        }
        return true;
    };

    if(check_track_signature() == false) {
        throw std::runtime_error("track has a bad signature");
    }
    if(check_track_number() == false) {
        throw std::runtime_error("track has a bad track number");
    }
    if(check_side_number() == false) {
        throw std::runtime_error("track has a bad side number");
    }
    if(check_sector_size() == false) {
        throw std::runtime_error("track has a bad sector size");
    }
    if(check_number_of_sectors() == false) {
        throw std::runtime_error("track has a bad number of sectors");
    }
}

void TrackInfoAdapter::fetch()
{
    utils::fetch_data(_file, _track.info);
}

void TrackInfoAdapter::store()
{
    utils::store_data(_file, _track.info);
}

void TrackInfoAdapter::print()
{
    utils::println("[track#%02x]", get_track_number());
    utils::println("track.info.signature ........... %s"    , get_signature().c_str());
    utils::println("track.info.reserved1 ........... 0x%02x", get_reserved1());
    utils::println("track.info.reserved2 ........... 0x%02x", get_reserved2());
    utils::println("track.info.reserved3 ........... 0x%02x", get_reserved3());
    utils::println("track.info.reserved4 ........... 0x%02x", get_reserved4());
    utils::println("track.info.track_number ........ 0x%02x", get_track_number());
    utils::println("track.info.side_number ......... 0x%02x", get_side_number());
    utils::println("track.info.reserved5 ........... 0x%02x", get_reserved5());
    utils::println("track.info.reserved6 ........... 0x%02x", get_reserved6());
    utils::println("track.info.sector_size ......... 0x%02x", get_sector_size());
    utils::println("track.info.number_of_sectors ... 0x%02x", get_number_of_sectors());
    utils::println("track.info.gap3_length ......... 0x%02x", get_gap3_length());
    utils::println("track.info.filler_byte ......... 0x%02x", get_filler_byte());
    utils::println("");
}

void TrackInfoAdapter::set_signature() const
{
    int       index = 0;
    int const count = countof(internal::trk_info);
    for(auto& byte : _track.info.s.signature) {
        byte = internal::trk_info[index];
        if(++index >= count) {
            break;
        }
    }
}

void TrackInfoAdapter::set_reserved1(const uint8_t reserved1) const
{
    _track.info.s.reserved1 = reserved1;
}

void TrackInfoAdapter::set_reserved2(const uint8_t reserved2) const
{
    _track.info.s.reserved2 = reserved2;
}

void TrackInfoAdapter::set_reserved3(const uint8_t reserved3) const
{
    _track.info.s.reserved3 = reserved3;
}

void TrackInfoAdapter::set_reserved4(const uint8_t reserved4) const
{
    _track.info.s.reserved4 = reserved4;
}

void TrackInfoAdapter::set_track_number(const uint8_t track_number) const
{
    _track.info.s.track_number = track_number;
}

void TrackInfoAdapter::set_side_number(const uint8_t side_number) const
{
    _track.info.s.side_number = side_number;
}

void TrackInfoAdapter::set_reserved5(const uint8_t reserved5) const
{
    _track.info.s.reserved5 = reserved5;
}

void TrackInfoAdapter::set_reserved6(const uint8_t reserved6) const
{
    _track.info.s.reserved6 = reserved6;
}

void TrackInfoAdapter::set_sector_size(const uint8_t sector_size) const
{
    _track.info.s.sector_size = sector_size;
}

void TrackInfoAdapter::set_number_of_sectors(const uint8_t number_of_sectors) const
{
    _track.info.s.number_of_sectors = number_of_sectors;
}

void TrackInfoAdapter::set_gap3_length(const uint8_t gap3_length) const
{
    _track.info.s.gap3_length = gap3_length;
}

void TrackInfoAdapter::set_filler_byte(const uint8_t filler_byte) const
{
    _track.info.s.filler_byte = filler_byte;
}

auto TrackInfoAdapter::get_signature() const -> std::string
{
    std::string string;

    for(auto character : _track.info.s.signature) {
        if(character >= ' ') {
            string += character;
        }
        else if(character == '\0') {
            string += '\\';
            string += '0';
        }
        else if(character == '\r') {
            string += '\\';
            string += 'r';
        }
        else if(character == '\n') {
            string += '\\';
            string += 'n';
        }
        else {
            string += '?';
        }
    }
    return string;
}

auto TrackInfoAdapter::get_reserved1() const -> uint8_t
{
    return _track.info.s.reserved1;
}

auto TrackInfoAdapter::get_reserved2() const -> uint8_t
{
    return _track.info.s.reserved2;
}

auto TrackInfoAdapter::get_reserved3() const -> uint8_t
{
    return _track.info.s.reserved3;
}

auto TrackInfoAdapter::get_reserved4() const -> uint8_t
{
    return _track.info.s.reserved4;
}

auto TrackInfoAdapter::get_track_number() const -> uint8_t
{
    return _track.info.s.track_number;
}

auto TrackInfoAdapter::get_side_number() const -> uint8_t
{
    return _track.info.s.side_number;
}

auto TrackInfoAdapter::get_reserved5() const -> uint8_t
{
    return _track.info.s.reserved5;
}

auto TrackInfoAdapter::get_reserved6() const -> uint8_t
{
    return _track.info.s.reserved6;
}

auto TrackInfoAdapter::get_sector_size() const -> uint8_t
{
    return _track.info.s.sector_size;
}

auto TrackInfoAdapter::get_number_of_sectors() const -> uint8_t
{
    return _track.info.s.number_of_sectors;
}

auto TrackInfoAdapter::get_gap3_length() const -> uint8_t
{
    return _track.info.s.gap3_length;
}

auto TrackInfoAdapter::get_filler_byte() const -> uint8_t
{
    return _track.info.s.filler_byte;
}

}

// ---------------------------------------------------------------------------
// dsk::SectorInfoAdapter
// ---------------------------------------------------------------------------

namespace dsk {

SectorInfoAdapter::SectorInfoAdapter(DiskRecord& disk, TrackRecord& track, SectorRecord& sector, const int file)
    : BaseAdapter(file)
    , _disk(disk)
    , _track(track)
    , _sector(sector)
{
}

void SectorInfoAdapter::clear()
{
    utils::clear_data(_sector.info, 0x00);
}

void SectorInfoAdapter::check()
{
    if(_sector.info.s.fdc_c >= internal::traits::MAX_NUMBER_OF_TRACKS) {
        throw std::runtime_error("sector has a bad track number");
    }
    if(_sector.info.s.fdc_h >= internal::traits::MAX_NUMBER_OF_SIDES) {
        throw std::runtime_error("sector has a bad side number");
    }
    if(_sector.info.s.fdc_n >= internal::traits::MAX_SECTOR_SIZE) {
        throw std::runtime_error("sector has a bad sector size");
    }
}

void SectorInfoAdapter::fetch()
{
    utils::fetch_data(_file, _sector.info);
}

void SectorInfoAdapter::store()
{
    utils::store_data(_file, _sector.info);
}

void SectorInfoAdapter::print()
{
    utils::println("[sector#%02x]", get_fdc_r());
    utils::println("sector.info.fdc_c .............. 0x%02x", get_fdc_c());
    utils::println("sector.info.fdc_h .............. 0x%02x", get_fdc_h());
    utils::println("sector.info.fdc_r .............. 0x%02x", get_fdc_r());
    utils::println("sector.info.fdc_n .............. 0x%02x", get_fdc_n());
    utils::println("sector.info.fdc_st1 ............ 0x%02x", get_fdc_st1());
    utils::println("sector.info.fdc_st2 ............ 0x%02x", get_fdc_st2());
    utils::println("sector.info.size ............... %d"    , get_size());
    utils::println("");
}

void SectorInfoAdapter::set_fdc_c(const uint8_t fdc_c) const
{
    _sector.info.s.fdc_c = fdc_c;
}

void SectorInfoAdapter::set_fdc_h(const uint8_t fdc_h) const
{
    _sector.info.s.fdc_h = fdc_h;
}

void SectorInfoAdapter::set_fdc_r(const uint8_t fdc_r) const
{
    _sector.info.s.fdc_r = fdc_r;
}

void SectorInfoAdapter::set_fdc_n(const uint8_t fdc_n) const
{
    _sector.info.s.fdc_n = fdc_n;
}

void SectorInfoAdapter::set_fdc_st1(const uint8_t fdc_st1) const
{
    _sector.info.s.fdc_st1 = fdc_st1;
}

void SectorInfoAdapter::set_fdc_st2(const uint8_t fdc_st2) const
{
    _sector.info.s.fdc_st2 = fdc_st2;
}

void SectorInfoAdapter::set_size(const uint16_t sector_size) const
{
    _sector.info.s.size_lsb = ((sector_size >> 0) & 0xff);
    _sector.info.s.size_msb = ((sector_size >> 8) & 0xff);
}

auto SectorInfoAdapter::get_fdc_c() const -> uint8_t
{
    return _sector.info.s.fdc_c;
}

auto SectorInfoAdapter::get_fdc_h() const -> uint8_t
{
    return _sector.info.s.fdc_h;
}

auto SectorInfoAdapter::get_fdc_r() const -> uint8_t
{
    return _sector.info.s.fdc_r;
}

auto SectorInfoAdapter::get_fdc_n() const -> uint8_t
{
    return _sector.info.s.fdc_n;
}

auto SectorInfoAdapter::get_fdc_st1() const -> uint8_t
{
    return _sector.info.s.fdc_st1;
}

auto SectorInfoAdapter::get_fdc_st2() const -> uint8_t
{
    return _sector.info.s.fdc_st2;
}

auto SectorInfoAdapter::get_size() const -> uint16_t
{
    return (static_cast<uint16_t>(_sector.info.s.size_msb) << 8)
         | (static_cast<uint16_t>(_sector.info.s.size_lsb) << 0)
         ;
}

}

// ---------------------------------------------------------------------------
// dsk::SectorDataAdapter
// ---------------------------------------------------------------------------

namespace dsk {

SectorDataAdapter::SectorDataAdapter(DiskRecord& disk, TrackRecord& track, SectorRecord& sector, const int file)
    : BaseAdapter(file)
    , _disk(disk)
    , _track(track)
    , _sector(sector)
{
}

void SectorDataAdapter::clear()
{
    utils::clear_data(_sector.data, _track.info.s.filler_byte);
}

void SectorDataAdapter::check()
{
}

void SectorDataAdapter::fetch()
{
    switch(_sector.info.s.fdc_n) {
        case 0:
            utils::fetch_data(_file, _sector.data.raw0);
            break;
        case 1:
            utils::fetch_data(_file, _sector.data.raw1);
            break;
        case 2:
            utils::fetch_data(_file, _sector.data.raw2);
            break;
        case 3:
            utils::fetch_data(_file, _sector.data.raw3);
            break;
        case 4:
            utils::fetch_data(_file, _sector.data.raw4);
            break;
        case 5:
            utils::fetch_data(_file, _sector.data.raw5);
            break;
        case 6:
            utils::fetch_data(_file, _sector.data.raw6);
            break;
        default:
            throw std::runtime_error("unsupported sector size");
            break;
    }
}

void SectorDataAdapter::store()
{
    switch(_sector.info.s.fdc_n) {
        case 0:
            utils::store_data(_file, _sector.data.raw0);
            break;
        case 1:
            utils::store_data(_file, _sector.data.raw1);
            break;
        case 2:
            utils::store_data(_file, _sector.data.raw2);
            break;
        case 3:
            utils::store_data(_file, _sector.data.raw3);
            break;
        case 4:
            utils::store_data(_file, _sector.data.raw4);
            break;
        case 5:
            utils::store_data(_file, _sector.data.raw5);
            break;
        case 6:
            utils::store_data(_file, _sector.data.raw6);
            break;
        default:
            throw std::runtime_error("unsupported sector size");
            break;
    }
}

void SectorDataAdapter::print()
{
}

}

// ---------------------------------------------------------------------------
// dsk::ImageAdapter
// ---------------------------------------------------------------------------

namespace dsk {

ImageAdapter::ImageAdapter(ImageRecord& image, const int file)
    : BaseAdapter(file)
    , _image(image)
{
}

void ImageAdapter::clear()
{
    auto clear_disk_info = [&]() -> void
    {
        DiskInfoAdapter disk_info(_image.disk, _file);

        disk_info.clear();
    };

    auto clear_track_info = [&]() -> void
    {
        TrackInfoAdapter track_info(_image.disk, _image.track, _file);

        track_info.clear();
    };

    auto clear_sector_info = [&]() -> void
    {
        for(auto& sector : _image.sector) {
            SectorInfoAdapter sector_info(_image.disk, _image.track, sector, _file);

            sector_info.clear();
        }
    };

    auto clear_sector_data = [&]() -> void
    {
        for(auto& sector : _image.sector) {
            SectorDataAdapter sector_data(_image.disk, _image.track, sector, _file);

            sector_data.clear();
        }
    };

    auto clear_all = [&]() -> void
    {
        clear_disk_info();
        clear_track_info();
        clear_sector_info();
        clear_sector_data();
    };

    return clear_all();
}

void ImageAdapter::check()
{
}

void ImageAdapter::fetch()
{
    uint8_t  number_of_sides   = 0;
    uint8_t  number_of_tracks  = 0;
    uint8_t  number_of_sectors = 0;
    uint8_t  current_side      = 0;
    uint8_t  current_track     = 0;
    uint8_t  current_sector    = 0;
    uint16_t track_size        = 0;
    uint8_t  sector_size       = 0;

    auto fetch_track_info = [&]() -> void
    {
        TrackInfoAdapter track_info(_image.disk, _image.track, _file);
        track_info.fetch();
        track_info.print();
        track_info.check();
        number_of_sectors = track_info.get_number_of_sectors();
        sector_size       = track_info.get_sector_size();
    };

    auto fetch_sector_info = [&]() -> void
    {
        current_sector = 0;
        for(auto& sector : _image.sector) {
            SectorInfoAdapter sector_info(_image.disk, _image.track, sector, _file);
            if(current_sector < number_of_sectors) {
                sector_info.fetch();
                sector_info.print();
                sector_info.check();
            }
            else {
                sector_info.fetch();
            }
            ++current_sector;
        }
    };

    auto fetch_sector_data = [&]() -> void
    {
        current_sector = 0;
        for(auto& sector : _image.sector) {
            SectorDataAdapter sector_data(_image.disk, _image.track, sector, _file);
            if(current_sector < number_of_sectors) {
                sector_data.clear();
                sector_data.fetch();
                sector_data.print();
            }
            else {
                sector_data.clear();
            }
            ++current_sector;
        }
    };

    auto fetch_tracks = [&]() -> void
    {
        for(current_track = 0; current_track < number_of_tracks; ++current_track) {
            for(current_side = 0; current_side < number_of_sides; ++current_side) {
                fetch_track_info();
                fetch_sector_info();
                fetch_sector_data();
            }
        }
    };

    auto fetch_header = [&]() -> void
    {
        DiskInfoAdapter disk_info(_image.disk, _file);
        disk_info.fetch();
        disk_info.print();
        disk_info.check();
        number_of_sides  = disk_info.get_number_of_sides();
        number_of_tracks = disk_info.get_number_of_tracks();
        track_size       = disk_info.get_track_size();
    };

    auto do_fetch = [&]() -> void
    {
        if(_file != -1) {
            fetch_header();
            fetch_tracks();
        }
        else {
            throw std::runtime_error("file is not opened");
        }
    };

    return do_fetch();
}

void ImageAdapter::store()
{
    uint8_t  constexpr unused_byte       = 0x00;
    uint8_t  constexpr number_of_sides   = 1;
    uint8_t  constexpr number_of_tracks  = 40;
    uint8_t  constexpr number_of_sectors = 9;
    uint8_t            current_side      = 0;
    uint8_t            current_track     = 0;
    uint8_t            current_sector    = 0;
    uint8_t  constexpr sector_size       = 2;
    uint16_t constexpr track_size        = (24 + (29 * 8)) + (number_of_sectors * (128 * (1 << sector_size)));
    uint8_t  constexpr sector_base       = 0xc0;
    uint8_t  constexpr gap3_length       = 0x2a;
    uint8_t  constexpr filler_byte       = 0xe5;

    auto store_track_info = [&]() -> void
    {
        TrackInfoAdapter track_info(_image.disk, _image.track, _file);
        track_info.clear();
        track_info.set_signature();
        track_info.set_reserved1(unused_byte);
        track_info.set_reserved2(unused_byte);
        track_info.set_reserved3(unused_byte);
        track_info.set_reserved4(unused_byte);
        track_info.set_track_number(current_track);
        track_info.set_side_number(current_side);
        track_info.set_reserved5(unused_byte);
        track_info.set_reserved6(unused_byte);
        track_info.set_sector_size(sector_size);
        track_info.set_number_of_sectors(number_of_sectors);
        track_info.set_gap3_length(gap3_length);
        track_info.set_filler_byte(filler_byte);
        track_info.check();
        track_info.store();
    };

    auto store_sector_info = [&]() -> void
    {
        current_sector = 0;
        for(auto& sector : _image.sector) {
            SectorInfoAdapter sector_info(_image.disk, _image.track, sector, _file);
            if(current_sector < number_of_sectors) {
                sector_info.clear();
                sector_info.set_fdc_c(current_track);
                sector_info.set_fdc_h(current_side);
                sector_info.set_fdc_r(sector_base + (current_sector + 1));
                sector_info.set_fdc_n(sector_size);
                sector_info.set_fdc_st1(0x00);
                sector_info.set_fdc_st2(0x00);
                sector_info.set_size(0);
                sector_info.check();
                sector_info.store();
            }
            else {
                sector_info.clear();
                sector_info.check();
                sector_info.store();
            }
            ++current_sector;
        }
    };

    auto store_sector_data = [&]() -> void
    {
        current_sector = 0;
        for(auto& sector : _image.sector) {
            SectorDataAdapter sector_data(_image.disk, _image.track, sector, _file);
            if(current_sector < number_of_sectors) {
                sector_data.clear();
                sector_data.check();
                sector_data.store();
            }
            else {
                sector_data.clear();
            }
            ++current_sector;
        }
    };

    auto store_tracks = [&]() -> void
    {
        for(current_track = 0; current_track < number_of_tracks; ++current_track) {
            for(current_side = 0; current_side < number_of_sides; ++current_side) {
                store_track_info();
                store_sector_info();
                store_sector_data();
            }
        }
    };

    auto store_header = [&]() -> void
    {
        DiskInfoAdapter disk_info(_image.disk, _file);
        disk_info.clear();
        disk_info.set_magic();
        disk_info.set_signature();
        disk_info.set_creator();
        disk_info.set_number_of_tracks(number_of_tracks);
        disk_info.set_number_of_sides(number_of_sides);
        disk_info.set_track_size(track_size);
        disk_info.set_padding(unused_byte);
        disk_info.check();
        disk_info.store();
    };

    auto do_store = [&]() -> void
    {
        if(_file != -1) {
            store_header();
            store_tracks();
        }
        else {
            throw std::runtime_error("file is not opened");
        }
    };

    return do_store();
}

void ImageAdapter::print()
{
}

}

// ---------------------------------------------------------------------------
// dsk::Disk
// ---------------------------------------------------------------------------

namespace dsk {

Disk::Disk(const std::string& filename)
    : _image()
    , _filename(filename)
    , _file(-1)
{
}

Disk::~Disk()
{
    if(_file != -1) {
        _file = (utils::close(_file), -1);
    }
}

void Disk::dump()
{
    constexpr int flags = (O_RDONLY);
    constexpr int mode  = 0;

    auto do_open = [&]() -> void
    {
        if(_file == -1) {
            _file = utils::open(_filename, flags, mode);
        }
        else {
            throw std::runtime_error(std::string() + '<' + _filename + '>' + ' ' + "is already opened");
        }
    };

    auto do_close = [&]() -> void
    {
        if(_file != -1) {
            _file = (utils::close(_file), -1);
        }
        else {
            throw std::runtime_error(std::string() + '<' + _filename + '>' + ' ' + "is already closed");
        }
    };

    auto do_fetch = [&]() -> void
    {
        ImageAdapter image(_image, _file);

        image.clear();
        image.fetch();
    };

    auto do_dump = [&]() -> void
    {
        do_open();
        do_fetch();
        do_close();
    };

    return do_dump();
}

void Disk::create()
{
    constexpr int flags = (O_CREAT | O_TRUNC | O_RDWR);
    constexpr int mode  = 0644;

    auto do_open = [&]() -> void
    {
        if(_file == -1) {
            _file = utils::open(_filename, flags, mode);
        }
        else {
            throw std::runtime_error(std::string() + '<' + _filename + '>' + ' ' + "is already opened");
        }
    };

    auto do_close = [&]() -> void
    {
        if(_file != -1) {
            _file = (utils::close(_file), -1);
        }
        else {
            throw std::runtime_error(std::string() + '<' + _filename + '>' + ' ' + "is already closed");
        }
    };

    auto do_store = [&]() -> void
    {
        ImageAdapter image(_image, _file);

        image.clear();
        image.store();
    };

    auto do_create = [&]() -> void
    {
        do_open();
        do_store();
        do_close();
    };

    return do_create();
}

void Disk::remove()
{
    auto do_remove = [&]() -> void
    {
        utils::unlink(_filename);
    };

    return do_remove();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
