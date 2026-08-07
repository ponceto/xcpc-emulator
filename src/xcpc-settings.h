/*
 * xcpc-settings.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <vector>
#include <memory>
#include <string>

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

    auto comment() const -> const std::string&;

    auto set_comment(const std::string& comment) -> void;

    auto get_bool(const bool fallback) const -> bool;

    auto get_long(const long fallback) const -> long;

    auto get_double(const double fallback) const -> double;

    auto get_string(const std::string& fallback) const -> std::string;

    auto set_bool(const bool value) -> void;

    auto set_long(const long value) -> void;

    auto set_double(const double value, const int precision = 6) -> void;

    auto set_string(const std::string& value) -> void;

    auto has_value() const -> bool;

private: // private data
    const std::string _name;
    std::string       _comment;
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

    auto comment() const -> const std::string&;

    auto set_comment(const std::string& comment) -> void;

    auto entry(const std::string& key) -> SettingsEntry&;

    auto entry(const std::string& key) const -> const SettingsEntry&;

    auto has_entry(const std::string& key) const -> bool;

    auto remove_entry(const std::string& key) -> bool;

    auto clear() -> void;

public: // public types
    using EntryList      = std::vector<std::unique_ptr<SettingsEntry>>;
    using EntryIndex     = std::map<std::string, SettingsEntry*>;
    using const_iterator = EntryList::const_iterator;

    auto begin() const -> const_iterator;

    auto end() const -> const_iterator;

private: // private data
    const std::string _name;
    std::string       _comment;
    EntryList         _entries;
    EntryIndex        _index;
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

    auto load() -> bool;

    auto save() -> bool;

    auto dirname() const -> const std::string&;

    auto filename() const -> const std::string&;

    auto fullpath() const -> const std::string&;

    auto table(const std::string& name) -> SettingsTable&;

    auto table(const std::string& name) const -> const SettingsTable&;

    auto has_table(const std::string& name) const -> bool;

    auto remove_table(const std::string& name) -> bool;

    auto clear() -> void;

public: // public types
    using TableList      = std::vector<std::unique_ptr<SettingsTable>>;
    using TableIndex     = std::map<std::string, SettingsTable*>;
    using const_iterator = TableList::const_iterator;

    auto begin() const -> const_iterator;

    auto end() const -> const_iterator;

private: // private data
    const std::string _dirname;
    const std::string _filename;
    const std::string _fullpath;
    TableList         _tables;
    TableIndex        _index;
    std::string       _epilog;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_SETTINGS_H__ */
