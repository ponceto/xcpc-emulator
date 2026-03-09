/*
 * xcpc-settings-file.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include <climits>
#include <cassert>
#include <clocale>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include <xcpc/libxcpc-cxx.h>
#include "xcpc-settings.h"

// ---------------------------------------------------------------------------
// <anonymous>::FileTraits
// ---------------------------------------------------------------------------

namespace {

struct FileDeleter
{
    auto operator()(FILE* file) -> void
    {
        if(file != nullptr) {
            file = (::fclose(file), nullptr);
        }
    }
};

using FileUniquePtr = std::unique_ptr<FILE, FileDeleter>;

}

// ---------------------------------------------------------------------------
// <anonymous>::SettingsTraits
// ---------------------------------------------------------------------------

namespace {

struct SettingsTraits
{
    static auto trim(const std::string& string) -> std::string
    {
        const char* spaces = " \t\v\f\r\n";
        const auto  first  = string.find_first_not_of(spaces);
        const auto  last   = string.find_last_not_of(spaces);
        if((first != std::string::npos) && (last != std::string::npos)) {
            return string.substr(first, ((last - first) + 1));
        }
        return std::string();
    }

    static auto get_config_directory(const std::string& subdir) -> std::string
    {
        /* check ${XDG_CONFIG_HOME} */ {
            const char* const directory = ::getenv("XDG_CONFIG_HOME");
            if((directory != nullptr) && (*directory != '\0')) {
                return std::string(directory) + '/' + subdir;
            }
        }
        /* check ${HOME} */ {
            const char* const directory = ::getenv("HOME");
            if((directory != nullptr) && (*directory != '\0')) {
                return std::string(directory) + '/' + ".config" + '/' + subdir;
            }
        }
        return std::string(".config") + '/' + subdir;
    }

    static auto create_directory(const std::string& directory) -> bool
    {
        const int rc = ::mkdir(directory.c_str(), 0755);

        if(rc == 0) {
            return true;
        }
        else if(errno == EEXIST) {
            return true;
        }
        return false;
    }

    static auto ensure_directory(const std::string& directory, int level = 1) -> bool
    {
        if(level > 0) {
            const auto separator = directory.rfind('/');
            if(separator != std::string::npos) {
                ensure_directory(directory.substr(0, separator), (level - 1));
            }
        }
        return create_directory(directory);
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::LocaleTraits
// ---------------------------------------------------------------------------

namespace {

struct LocaleTraits
{
    static auto get_locale(int category) -> std::string
    {
        const char* value = ::setlocale(category, nullptr);

        if(value == nullptr) {
            value = "";
        }
        return value;
    }

    static auto set_locale(int category, const std::string& locale) -> std::string
    {
        const char* value = ::setlocale(category, locale.c_str());

        if(value == nullptr) {
            value = "";
        }
        return value;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::ScopedLocale
// ---------------------------------------------------------------------------

namespace {

class ScopedLocale
{
public: // public interface
    ScopedLocale(int category, const std::string& locale)
        : _category(category)
        , _old_locale(LocaleTraits::get_locale(_category))
        , _new_locale(LocaleTraits::set_locale(_category, locale))
    {
    }

    virtual ~ScopedLocale()
    {
        LocaleTraits::set_locale(_category, _old_locale);
    }

    ScopedLocale(ScopedLocale&&) = delete;

    ScopedLocale(const ScopedLocale&) = delete;

    ScopedLocale& operator=(ScopedLocale&&) = delete;

    ScopedLocale& operator=(const ScopedLocale&) = delete;

private: // private data
    const int         _category;
    const std::string _old_locale;
    const std::string _new_locale;
};

}

// ---------------------------------------------------------------------------
// base::SettingsEntry
// ---------------------------------------------------------------------------

namespace base {

SettingsEntry::SettingsEntry(const std::string& name)
    : _name(name)
    , _value()
    , _has_value(false)
{
}

auto SettingsEntry::name() const -> const std::string&
{
    return _name;
}

auto SettingsEntry::get_bool(const bool fallback) const -> bool
{
    if(_has_value != false) {
        if(_value == "true") {
            return true;
        }
        if(_value == "false") {
            return false;
        }
    }
    return fallback;
}

auto SettingsEntry::get_long(const long fallback) const -> long
{
    const ScopedLocale locale(LC_NUMERIC, "C");

    if(_has_value != false) {
        char*       endptr = nullptr;
        const long  result = ::strtol(_value.c_str(), &endptr, 10);
        if(endptr != _value.c_str()) {
            return result;
        }
    }
    return fallback;
}

auto SettingsEntry::get_double(const double fallback) const -> double
{
    const ScopedLocale locale(LC_NUMERIC, "C");

    if(_has_value != false) {
        char*        endptr = nullptr;
        const double result = ::strtod(_value.c_str(), &endptr);
        if(endptr != _value.c_str()) {
            return result;
        }
    }
    return fallback;
}

auto SettingsEntry::get_string(const std::string& fallback) const -> std::string
{
    if(_has_value != false) {
        return _value;
    }
    return fallback;
}

auto SettingsEntry::set_bool(const bool value) -> void
{
    _value     = (value != false ? "true" : "false");
    _has_value = true;
}

auto SettingsEntry::set_long(const long value) -> void
{
    const ScopedLocale locale(LC_NUMERIC, "C");

    char         buffer[64];
    const size_t buflen = sizeof(buffer);
    const int    result = ::snprintf(buffer, buflen, "%ld", value);

    if(result > 0) {
        _value     = buffer;
        _has_value = true;
    }
}

auto SettingsEntry::set_double(const double value) -> void
{
    const ScopedLocale locale(LC_NUMERIC, "C");

    char         buffer[64];
    const size_t buflen = sizeof(buffer);
    const int    result = ::snprintf(buffer, buflen, "%f", value);

    if(result > 0) {
        _value     = buffer;
        _has_value = true;
    }
}

auto SettingsEntry::set_string(const std::string& value) -> void
{
    _value     = value;
    _has_value = true;
}

auto SettingsEntry::has_value() const -> bool
{
    return _has_value;
}

}

// ---------------------------------------------------------------------------
// base::SettingsTable
// ---------------------------------------------------------------------------

namespace base {

SettingsTable::SettingsTable(const std::string& name)
    : _name(name)
    , _entries()
{
}

auto SettingsTable::name() const -> const std::string&
{
    return _name;
}

auto SettingsTable::entry(const std::string& key) -> SettingsEntry&
{
    auto it = _entries.find(key);
    if(it == _entries.end()) {
        auto result = _entries.emplace(key, std::make_unique<SettingsEntry>(key));
        it = result.first;
    }
    return *it->second;
}

auto SettingsTable::entry(const std::string& key) const -> const SettingsEntry&
{
    static const SettingsEntry empty_entry("");

    auto it = _entries.find(key);
    if(it != _entries.end()) {
        return *it->second;
    }
    return empty_entry;
}

auto SettingsTable::has_entry(const std::string& key) const -> bool
{
    return _entries.find(key) != _entries.end();
}

auto SettingsTable::begin() const -> const_iterator
{
    return _entries.begin();
}

auto SettingsTable::end() const -> const_iterator
{
    return _entries.end();
}

}

// ---------------------------------------------------------------------------
// base::SettingsFile
// ---------------------------------------------------------------------------

namespace base {

SettingsFile::SettingsFile(const std::string& filename)
    : _dirname(SettingsTraits::get_config_directory("xcpc"))
    , _filename(filename)
    , _fullpath(_dirname + "/" + _filename)
    , _tables()
{
    if(SettingsTraits::ensure_directory(_dirname) == false) {
        ::xcpc_log_error("settings: cannot create directory (%s)", _dirname.c_str());
    }
}

auto SettingsFile::load() -> void
{
    const FileUniquePtr file(::fopen(_fullpath.c_str(), "r"));

    if(bool(file) != false) {
        char         buffer[1024];
        const size_t buflen = sizeof(buffer);
        SettingsTable* current_table = nullptr;
        while(::fgets(buffer, buflen, file.get()) != nullptr) {
            const std::string line = SettingsTraits::trim(buffer);
            switch(line[0]) {
                case '\0':
                    break;
                case '#':
                    break;
                case ';':
                    break;
                case '[':
                    {
                        const auto end = line.find(']');
                        if(end != std::string::npos) {
                            const std::string name = SettingsTraits::trim(line.substr(1, (end - 1)));
                            if(name.empty() == false) {
                                current_table = &table(name);
                            }
                        }
                    }
                    break;
                case '_':
                case 'a'...'z':
                case 'A'...'Z':
                    {
                        if(current_table != nullptr) {
                            const auto separator = line.find('=');
                            if(separator != std::string::npos) {
                                const std::string key = SettingsTraits::trim(line.substr(0, separator));
                                const std::string val = SettingsTraits::trim(line.substr(separator + 1));
                                if(key.empty() == false) {
                                    current_table->entry(key).set_string(val);
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        ::xcpc_log_trace("load_settings() : settings loaded from (%s)", _fullpath.c_str());
    }
    else {
        ::xcpc_log_trace("load_settings() : file not found (%s)", _fullpath.c_str());
    }
}

auto SettingsFile::save() -> void
{
    const FileUniquePtr file(::fopen(_fullpath.c_str(), "w"));

    if(bool(file) != false) {
        for(const auto& table_pair : _tables) {
            const auto& tbl = *table_pair.second;
            ::fprintf(file.get(), "\n[%s]\n", tbl.name().c_str());
            for(const auto& entry_pair : tbl) {
                const auto& ent = *entry_pair.second;
                if(ent.has_value()) {
                    ::fprintf(file.get(), "%s = %s\n", ent.name().c_str(), ent.get_string("").c_str());
                }
            }
        }
        ::xcpc_log_trace("save_settings() : settings saved to (%s)", _fullpath.c_str());
    }
    else {
        ::xcpc_log_trace("save_settings() : file not found (%s)", _fullpath.c_str());
    }
}

auto SettingsFile::table(const std::string& name) -> SettingsTable&
{
    auto it = _tables.find(name);
    if(it == _tables.end()) {
        auto result = _tables.emplace(name, std::unique_ptr<SettingsTable>(new SettingsTable(name)));
        it = result.first;
    }
    return *it->second;
}

auto SettingsFile::table(const std::string& name) const -> const SettingsTable&
{
    static const SettingsTable empty_table("");

    auto it = _tables.find(name);
    if(it != _tables.end()) {
        return *it->second;
    }
    return empty_table;
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
