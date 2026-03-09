/*
 * xcpc-settings-file.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_SETTINGS_H__
#define __XCPC_SETTINGS_H__

#include <map>

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace base {

class SettingsEntry;
class SettingsTable;
class SettingsFile;

}

// ---------------------------------------------------------------------------
// base::SettingsEntry
// ---------------------------------------------------------------------------

namespace base {

class SettingsEntry
{
public: // public interface
    SettingsEntry(const std::string& name);

    SettingsEntry(SettingsEntry&&) = delete;

    SettingsEntry(const SettingsEntry&) = delete;

    SettingsEntry& operator=(SettingsEntry&&) = delete;

    SettingsEntry& operator=(const SettingsEntry&) = delete;

    virtual ~SettingsEntry() = default;

    auto name() const -> const std::string&;

    auto get_bool(const bool fallback) const -> bool;

    auto get_long(const long fallback) const -> long;

    auto get_double(const double fallback) const -> double;

    auto get_string(const std::string& fallback) const -> std::string;

    auto set_bool(const bool value) -> void;

    auto set_long(const long value) -> void;

    auto set_double(const double value) -> void;

    auto set_string(const std::string& value) -> void;

    auto has_value() const -> bool;

private: // private data
    const std::string _name;
    std::string       _value;
    bool              _has_value;
};

}

// ---------------------------------------------------------------------------
// base::SettingsTable
// ---------------------------------------------------------------------------

namespace base {

class SettingsTable
{
public: // public interface
    SettingsTable(const std::string& name);

    SettingsTable(SettingsTable&&) = delete;

    SettingsTable(const SettingsTable&) = delete;

    SettingsTable& operator=(SettingsTable&&) = delete;

    SettingsTable& operator=(const SettingsTable&) = delete;

    virtual ~SettingsTable() = default;

    auto name() const -> const std::string&;

    auto entry(const std::string& key) -> SettingsEntry&;

    auto entry(const std::string& key) const -> const SettingsEntry&;

    auto has_entry(const std::string& key) const -> bool;

public: // public types
    using EntryMap = std::map<std::string, std::unique_ptr<SettingsEntry>>;
    using const_iterator = EntryMap::const_iterator;

    auto begin() const -> const_iterator;

    auto end() const -> const_iterator;

private: // private data
    const std::string _name;
    EntryMap          _entries;
};

}

// ---------------------------------------------------------------------------
// base::SettingsFile
// ---------------------------------------------------------------------------

namespace base {

class SettingsFile
{
public: // public interface
    SettingsFile(const std::string& filename);

    SettingsFile(SettingsFile&&) = delete;

    SettingsFile(const SettingsFile&) = delete;

    SettingsFile& operator=(SettingsFile&&) = delete;

    SettingsFile& operator=(const SettingsFile&) = delete;

    virtual ~SettingsFile() = default;

    auto load() -> void;

    auto save() -> void;

    auto table(const std::string& name) -> SettingsTable&;

    auto table(const std::string& name) const -> const SettingsTable&;

private: // private data
    using TableMap = std::map<std::string, std::unique_ptr<SettingsTable>>;

private: // private data
    const std::string _dirname;
    const std::string _filename;
    const std::string _fullpath;
    TableMap          _tables;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_SETTINGS_H__ */
