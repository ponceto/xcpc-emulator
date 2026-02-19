/*
 * dsk-format.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_DSK_FORMAT_H__
#define __XCPC_DSK_FORMAT_H__

// ---------------------------------------------------------------------------
// dsk::DiskRecord
// ---------------------------------------------------------------------------

namespace dsk {

struct DiskRecord
{
    union {
        uint8_t raw[256];
        struct {
            uint8_t magic[23];
            uint8_t signature[11];
            uint8_t tool[14];
            uint8_t number_of_tracks;
            uint8_t number_of_sides;
            uint8_t track_size_lsb;
            uint8_t track_size_msb;
            uint8_t padding[204];
        } s;
    } info;
};

}

// ---------------------------------------------------------------------------
// dsk::TrackRecord
// ---------------------------------------------------------------------------

namespace dsk {

struct TrackRecord
{
    union {
        uint8_t raw[24];
        struct {
            uint8_t signature[12];
            uint8_t reserved1;
            uint8_t reserved2;
            uint8_t reserved3;
            uint8_t reserved4;
            uint8_t track_number;
            uint8_t side_number;
            uint8_t reserved5;
            uint8_t reserved6;
            uint8_t sector_size;
            uint8_t number_of_sectors;
            uint8_t gap3_length;
            uint8_t filler_byte;
        } s;
    } info;
};

}

// ---------------------------------------------------------------------------
// dsk::SectorRecord
// ---------------------------------------------------------------------------

namespace dsk {

struct SectorRecord
{
    union {
        uint8_t raw[8];
        struct {
            uint8_t fdc_c;    /* cylinder    */
            uint8_t fdc_h;    /* head        */
            uint8_t fdc_r;    /* sector id   */
            uint8_t fdc_n;    /* sector size */
            uint8_t fdc_st1;  /* status 1    */
            uint8_t fdc_st2;  /* status 2    */
            uint8_t size_lsb; /* size lsb    */
            uint8_t size_msb; /* size msb    */
        } s;
    } info;
    union {
        uint8_t raw0[128];
        uint8_t raw1[256];
        uint8_t raw2[512];
        uint8_t raw3[1024];
        uint8_t raw4[2048];
        uint8_t raw5[4096];
        uint8_t raw6[8192];
    } data;
};

}

// ---------------------------------------------------------------------------
// dsk::ImageRecord
// ---------------------------------------------------------------------------

namespace dsk {

struct ImageRecord
{
    DiskRecord   disk;
    TrackRecord  track;
    SectorRecord sector[29];
};

}

// ---------------------------------------------------------------------------
// dsk::BaseAdapter
// ---------------------------------------------------------------------------

namespace dsk {

class BaseAdapter
{
public: // public interface
    BaseAdapter(const int file);

    BaseAdapter(const BaseAdapter&) = delete;

    BaseAdapter& operator=(const BaseAdapter&) = delete;

    virtual ~BaseAdapter() = default;

    virtual void clear() = 0;

    virtual void check() = 0;

    virtual void fetch() = 0;

    virtual void store() = 0;

    virtual void print() = 0;

protected: // protected data
    const int _file;
};

}

// ---------------------------------------------------------------------------
// dsk::DiskInfoAdapter
// ---------------------------------------------------------------------------

namespace dsk {

class DiskInfoAdapter final
    : public BaseAdapter
{
public: // public interface
    DiskInfoAdapter(DiskRecord& disk, const int file);

    DiskInfoAdapter(const DiskInfoAdapter&) = delete;

    DiskInfoAdapter& operator=(const DiskInfoAdapter&) = delete;

    virtual ~DiskInfoAdapter() = default;

    virtual void clear() override final;

    virtual void check() override final;

    virtual void fetch() override final;

    virtual void store() override final;

    virtual void print() override final;

    void set_magic() const;

    void set_signature() const;

    void set_creator() const;

    void set_number_of_tracks(const uint8_t number_of_tracks) const;

    void set_number_of_sides(const uint8_t number_of_sides) const;

    void set_track_size(const uint16_t track_size) const;

    void set_padding(const uint8_t padding) const;

    auto get_magic() const -> std::string;

    auto get_signature() const -> std::string;

    auto get_creator() const -> std::string;

    auto get_number_of_tracks() const -> uint8_t;

    auto get_number_of_sides() const -> uint8_t;

    auto get_track_size() const -> uint16_t;

    auto get_side_size() const -> uint32_t;

private: // private data
    DiskRecord& _disk;
};

}

// ---------------------------------------------------------------------------
// dsk::TrackInfoAdapter
// ---------------------------------------------------------------------------

namespace dsk {

class TrackInfoAdapter final
    : public BaseAdapter
{
public: // public interface
    TrackInfoAdapter(DiskRecord& disk, TrackRecord& track, const int file);

    TrackInfoAdapter(const TrackInfoAdapter&) = delete;

    TrackInfoAdapter& operator=(const TrackInfoAdapter&) = delete;

    virtual ~TrackInfoAdapter() = default;

    virtual void clear() override final;

    virtual void check() override final;

    virtual void fetch() override final;

    virtual void store() override final;

    virtual void print() override final;

    void set_signature() const;

    void set_reserved1(const uint8_t reserved1) const;

    void set_reserved2(const uint8_t reserved2) const;

    void set_reserved3(const uint8_t reserved3) const;

    void set_reserved4(const uint8_t reserved4) const;

    void set_track_number(const uint8_t track_number) const;

    void set_side_number(const uint8_t side_number) const;

    void set_reserved5(const uint8_t reserved5) const;

    void set_reserved6(const uint8_t reserved6) const;

    void set_sector_size(const uint8_t sector_size) const;

    void set_number_of_sectors(const uint8_t number_of_sectors) const;

    void set_gap3_length(const uint8_t gap3_length) const;

    void set_filler_byte(const uint8_t filler_byte) const;

    auto get_signature() const -> std::string;

    auto get_reserved1() const -> uint8_t;

    auto get_reserved2() const -> uint8_t;

    auto get_reserved3() const -> uint8_t;

    auto get_reserved4() const -> uint8_t;

    auto get_track_number() const -> uint8_t;

    auto get_side_number() const -> uint8_t;

    auto get_reserved5() const -> uint8_t;

    auto get_reserved6() const -> uint8_t;

    auto get_sector_size() const -> uint8_t;

    auto get_number_of_sectors() const -> uint8_t;

    auto get_gap3_length() const -> uint8_t;

    auto get_filler_byte() const -> uint8_t;

private: // private data
    DiskRecord&  _disk;
    TrackRecord& _track;
};

}

// ---------------------------------------------------------------------------
// dsk::SectorInfoAdapter
// ---------------------------------------------------------------------------

namespace dsk {

class SectorInfoAdapter final
    : public BaseAdapter
{
public: // public interface
    SectorInfoAdapter(DiskRecord& disk, TrackRecord& track, SectorRecord& sector, const int file);

    SectorInfoAdapter(const SectorInfoAdapter&) = delete;

    SectorInfoAdapter& operator=(const SectorInfoAdapter&) = delete;

    virtual ~SectorInfoAdapter() = default;

    virtual void clear() override final;

    virtual void check() override final;

    virtual void fetch() override final;

    virtual void store() override final;

    virtual void print() override final;

    void set_fdc_c(const uint8_t fdc_c) const;

    void set_fdc_h(const uint8_t fdc_h) const;

    void set_fdc_r(const uint8_t fdc_r) const;

    void set_fdc_n(const uint8_t fdc_n) const;

    void set_fdc_st1(const uint8_t fdc_st1) const;

    void set_fdc_st2(const uint8_t fdc_st2) const;

    void set_size(const uint16_t sector_size) const;

    auto get_fdc_c() const -> uint8_t;

    auto get_fdc_h() const -> uint8_t;

    auto get_fdc_r() const -> uint8_t;

    auto get_fdc_n() const -> uint8_t;

    auto get_fdc_st1() const -> uint8_t;

    auto get_fdc_st2() const -> uint8_t;

    auto get_size() const -> uint16_t;

private: // private data
    DiskRecord&   _disk;
    TrackRecord&  _track;
    SectorRecord& _sector;
};

}

// ---------------------------------------------------------------------------
// dsk::SectorDataAdapter
// ---------------------------------------------------------------------------

namespace dsk {

class SectorDataAdapter final
    : public BaseAdapter
{
public: // public interface
    SectorDataAdapter(DiskRecord& disk, TrackRecord& track, SectorRecord& sector, const int file);

    SectorDataAdapter(const SectorDataAdapter&) = delete;

    SectorDataAdapter& operator=(const SectorDataAdapter&) = delete;

    virtual ~SectorDataAdapter() = default;

    virtual void clear() override final;

    virtual void check() override final;

    virtual void fetch() override final;

    virtual void store() override final;

    virtual void print() override final;

private: // private data
    DiskRecord&   _disk;
    TrackRecord&  _track;
    SectorRecord& _sector;
};

}

// ---------------------------------------------------------------------------
// dsk::ImageAdapter
// ---------------------------------------------------------------------------

namespace dsk {

class ImageAdapter final
    : public BaseAdapter
{
public: // public interface
    ImageAdapter(ImageRecord& image, const int file);

    ImageAdapter(const ImageAdapter&) = delete;

    ImageAdapter& operator=(const ImageAdapter&) = delete;

    virtual ~ImageAdapter() = default;

    virtual void clear() override final;

    virtual void check() override final;

    virtual void fetch() override final;

    virtual void store() override final;

    virtual void print() override final;

private: // private data
    ImageRecord& _image;
};

}

// ---------------------------------------------------------------------------
// dsk::Visitor
// ---------------------------------------------------------------------------

namespace dsk {

class Visitor
{
public: // public interface
    Visitor();

    Visitor(const Visitor&) = delete;

    Visitor& operator=(const Visitor&) = delete;

    virtual ~Visitor() = default;

    virtual void on_disk_info(DiskRecord&) = 0;

    virtual void on_track_info(TrackRecord&) = 0;

    virtual void on_sector_info(SectorRecord&) = 0;

    virtual void on_sector_data(SectorRecord&) = 0;
};

}

// ---------------------------------------------------------------------------
// dsk::Disk
// ---------------------------------------------------------------------------

namespace dsk {

class Disk
{
public: // public interface
    Disk(const std::string& filename);

    Disk(const Disk&) = delete;

    Disk& operator=(const Disk&) = delete;

    virtual ~Disk();

    virtual void dump();

    virtual void create();

    virtual void remove();

protected: // protected data
    dsk::ImageRecord _image;
    std::string      _filename;
    int              _file;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_DSK_FORMAT_H__ */
