/*
 * xcpc-settings.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
    static constexpr char COMMENT_PREFIX = ';';

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

    static auto is_comment(const std::string& line) -> bool
    {
        if(line.empty() == false) {
            return (line[0] == '#') || (line[0] == COMMENT_PREFIX);
        }
        return false;
    }

    static auto read_line(FILE* file, std::string& line) -> bool
    {
        char         buffer[256];
        const size_t buflen = sizeof(buffer);

        line.clear();
        while(::fgets(buffer, buflen, file) != nullptr) {
            line.append(buffer);
            if(line.back() == '\n') {
                return true;
            }
        }
        return (line.empty() == false);
    }

    static auto normalize_comment(const std::string& comment) -> std::string
    {
        std::string result;
        std::string line;

        auto flush_line = [&]() -> void
        {
            const std::string trimmed(trim(line));
            if(trimmed.empty() == false) {
                if(is_comment(trimmed) == false) {
                    result.append(1, COMMENT_PREFIX).append(1, ' ');
                }
                result.append(trimmed).append(1, '\n');
            }
            line.clear();
        };

        for(const char character : comment) {
            if((character == '\r') || (character == '\n')) {
                flush_line();
            }
            else {
                line.append(1, character);
            }
        }
        flush_line();

        return result;
    }

    static auto sanitize_value(const std::string& value) -> std::string
    {
        std::string result;

        result.reserve(value.size());
        for(const char character : value) {
            if((character == '\r') || (character == '\n')) {
                continue;
            }
            result.append(1, character);
        }
        return result;
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

    static auto ensure_directory(const std::string& directory) -> bool
    {
        if((directory.empty() != false) || (directory == "/") || (directory == ".")) {
            return true;
        }
        const auto separator = directory.rfind('/');
        if((separator != std::string::npos) && (separator > 0)) {
            if(ensure_directory(directory.substr(0, separator)) == false) {
                return false;
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
    , _comment()
    , _value()
    , _has_value(false)
{
}

auto SettingsEntry::name() const -> const std::string&
{
    return _name;
}

auto SettingsEntry::comment() const -> const std::string&
{
    return _comment;
}

auto SettingsEntry::set_comment(const std::string& comment) -> void
{
    _comment = SettingsTraits::normalize_comment(comment);
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
        ::xcpc_log_alert("settings: not a boolean value (%s = %s)", _name.c_str(), _value.c_str());
    }
    return fallback;
}

auto SettingsEntry::get_long(const long fallback) const -> long
{
    if(_has_value != false) {
        const ScopedLocale locale(LC_NUMERIC, "C");
        const char* const  string = _value.c_str();
        char*              endptr = nullptr;

        errno = 0;
        const long result = ::strtol(string, &endptr, 10);
        if((*string != '\0') && (endptr != nullptr) && (*endptr == '\0') && (errno != ERANGE)) {
            return result;
        }
        ::xcpc_log_alert("settings: not an integer value (%s = %s)", _name.c_str(), _value.c_str());
    }
    return fallback;
}

auto SettingsEntry::get_double(const double fallback) const -> double
{
    if(_has_value != false) {
        const ScopedLocale locale(LC_NUMERIC, "C");
        const char* const  string = _value.c_str();
        char*              endptr = nullptr;

        errno = 0;
        const double result = ::strtod(string, &endptr);
        if((*string != '\0') && (endptr != nullptr) && (*endptr == '\0') && (errno != ERANGE)) {
            return result;
        }
        ::xcpc_log_alert("settings: not a real value (%s = %s)", _name.c_str(), _value.c_str());
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

    char         buffer[512];
    const size_t buflen = sizeof(buffer);
    const int    result = ::snprintf(buffer, buflen, "%ld", value);

    if((result > 0) && (result < static_cast<int>(buflen))) {
        _value     = buffer;
        _has_value = true;
    }
    else {
        ::xcpc_log_alert("settings: cannot format integer value (%s)", _name.c_str());
    }
}

auto SettingsEntry::set_double(const double value, const int precision) -> void
{
    const ScopedLocale locale(LC_NUMERIC, "C");

    char         buffer[512];
    const size_t buflen = sizeof(buffer);
    const int    result = ::snprintf(buffer, buflen, "%.*g", precision, value);

    if((result > 0) && (result < static_cast<int>(buflen))) {
        _value     = buffer;
        _has_value = true;
    }
    else {
        ::xcpc_log_alert("settings: cannot format real value (%s)", _name.c_str());
    }
}

auto SettingsEntry::set_string(const std::string& value) -> void
{
    const std::string sanitized(SettingsTraits::sanitize_value(value));

    if(sanitized.size() != value.size()) {
        ::xcpc_log_alert("settings: end-of-line characters stripped (%s)", _name.c_str());
    }
    _value     = sanitized;
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
    , _comment()
    , _entries()
    , _index()
{
}

auto SettingsTable::name() const -> const std::string&
{
    return _name;
}

auto SettingsTable::comment() const -> const std::string&
{
    return _comment;
}

auto SettingsTable::set_comment(const std::string& comment) -> void
{
    _comment = SettingsTraits::normalize_comment(comment);
}

auto SettingsTable::entry(const std::string& key) -> SettingsEntry&
{
    auto it = _index.find(key);
    if(it != _index.end()) {
        return *it->second;
    }
    _entries.emplace_back(std::make_unique<SettingsEntry>(key));

    SettingsEntry* const settings_entry = _entries.back().get();

    _index.emplace(key, settings_entry);

    return *settings_entry;
}

auto SettingsTable::entry(const std::string& key) const -> const SettingsEntry&
{
    static const SettingsEntry empty_entry("");

    auto it = _index.find(key);
    if(it != _index.end()) {
        return *it->second;
    }
    return empty_entry;
}

auto SettingsTable::has_entry(const std::string& key) const -> bool
{
    return _index.find(key) != _index.end();
}

auto SettingsTable::remove_entry(const std::string& key) -> bool
{
    auto it = _index.find(key);
    if(it == _index.end()) {
        return false;
    }
    const SettingsEntry* const settings_entry = it->second;

    _index.erase(it);
    for(auto entry_it = _entries.begin(); entry_it != _entries.end(); ++entry_it) {
        if(entry_it->get() == settings_entry) {
            _entries.erase(entry_it);
            break;
        }
    }
    return true;
}

auto SettingsTable::clear() -> void
{
    _index.clear();
    _entries.clear();
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
    , _index()
    , _epilog()
{
    if(SettingsTraits::ensure_directory(_dirname) == false) {
        ::xcpc_log_error("settings: cannot create directory (%s) : %s", _dirname.c_str(), ::strerror(errno));
    }
}

auto SettingsFile::load() -> bool
{
    const FileUniquePtr file(::fopen(_fullpath.c_str(), "r"));

    if(bool(file) == false) {
        if(errno == ENOENT) {
            ::xcpc_log_trace("settings: file not found (%s)", _fullpath.c_str());
        }
        else {
            ::xcpc_log_error("settings: cannot read file (%s) : %s", _fullpath.c_str(), ::strerror(errno));
        }
        return false;
    }

    SettingsTable* current_table = nullptr;
    std::string    pending;
    std::string    input;

    auto parse_comment = [&](const std::string& line) -> void
    {
        pending.append(line).append(1, '\n');
    };

    auto parse_section = [&](const std::string& line) -> void
    {
        const auto end = line.find(']');
        if(end == std::string::npos) {
            ::xcpc_log_alert("settings: unterminated section (%s)", line.c_str());
            return;
        }
        const std::string name(SettingsTraits::trim(line.substr(1, (end - 1))));
        if(name.empty() != false) {
            ::xcpc_log_alert("settings: empty section name (%s)", line.c_str());
            return;
        }
        current_table = &table(name);
        if(pending.empty() == false) {
            current_table->set_comment(pending);
            pending.clear();
        }
    };

    auto parse_entry = [&](const std::string& line) -> void
    {
        const auto separator = line.find('=');
        if(separator == std::string::npos) {
            ::xcpc_log_alert("settings: malformed entry (%s)", line.c_str());
            return;
        }
        if(current_table == nullptr) {
            ::xcpc_log_alert("settings: entry outside of a section (%s)", line.c_str());
            return;
        }
        const std::string key(SettingsTraits::trim(line.substr(0, separator)));
        const std::string val(SettingsTraits::trim(line.substr(separator + 1)));
        if(key.empty() != false) {
            ::xcpc_log_alert("settings: empty key (%s)", line.c_str());
            return;
        }
        SettingsEntry& settings_entry(current_table->entry(key));
        settings_entry.set_string(val);
        if(pending.empty() == false) {
            settings_entry.set_comment(pending);
            pending.clear();
        }
    };

    while(SettingsTraits::read_line(file.get(), input) != false) {
        const std::string line(SettingsTraits::trim(input));
        if(line.empty() != false) {
            continue;
        }
        else if(SettingsTraits::is_comment(line) != false) {
            parse_comment(line);
        }
        else if(line[0] == '[') {
            parse_section(line);
        }
        else {
            parse_entry(line);
        }
    }
    _epilog = SettingsTraits::normalize_comment(pending);

    ::xcpc_log_trace("settings: settings loaded from (%s)", _fullpath.c_str());

    return true;
}

auto SettingsFile::save() -> bool
{
    const std::string tmppath(_fullpath + ".tmp");

    auto write_all = [&](FILE* file) -> void
    {
        auto write_comment = [&](const std::string& comment) -> void
        {
            if(comment.empty() == false) {
                static_cast<void>(::fputs(comment.c_str(), file));
            }
        };

        bool first_table = true;
        for(const auto& settings_table : _tables) {
            if(first_table != false) {
                first_table = false;
            }
            else {
                static_cast<void>(::fputc('\n', file));
            }
            write_comment(settings_table->comment());
            static_cast<void>(::fprintf(file, "[%s]\n", settings_table->name().c_str()));
            for(const auto& settings_entry : *settings_table) {
                if(settings_entry->has_value() != false) {
                    write_comment(settings_entry->comment());
                    static_cast<void>(::fprintf(file, "%s = %s\n", settings_entry->name().c_str(), settings_entry->get_string("").c_str()));
                }
            }
        }
        write_comment(_epilog);
    };

    auto abort_save = [&](const char* reason, const int error) -> bool
    {
        ::xcpc_log_error("settings: cannot write file (%s) : %s : %s", _fullpath.c_str(), reason, ::strerror(error));
        static_cast<void>(::unlink(tmppath.c_str()));
        return false;
    };

    auto write_file = [&]() -> bool
    {
        FILE* const file = ::fopen(tmppath.c_str(), "w");

        if(file == nullptr) {
            ::xcpc_log_error("settings: cannot create file (%s) : %s", tmppath.c_str(), ::strerror(errno));
            return false;
        }
        /* write the contents, then flush and sync before closing */ {
            const char* reason = nullptr;
            int         error  = 0;

            write_all(file);
            if((reason == nullptr) && (::ferror(file) != 0)) {
                reason = "fprintf() has failed";
                error  = errno;
            }
            if((reason == nullptr) && (::fflush(file) != 0)) {
                reason = "fflush() has failed";
                error  = errno;
            }
            if((reason == nullptr) && (::fsync(::fileno(file)) != 0)) {
                /* a filesystem which does not support fsync() is not an error */
                if((errno != EINVAL) && (errno != ENOSYS) && (errno != ENOTSUP)) {
                    reason = "fsync() has failed";
                    error  = errno;
                }
            }
            if(::fclose(file) != 0) {
                if(reason == nullptr) {
                    reason = "fclose() has failed";
                    error  = errno;
                }
            }
            if(reason != nullptr) {
                return abort_save(reason, error);
            }
        }
        /* atomically replace the target file */ {
            if(::rename(tmppath.c_str(), _fullpath.c_str()) != 0) {
                return abort_save("rename() has failed", errno);
            }
        }
        return true;
    };

    if(write_file() == false) {
        return false;
    }
    ::xcpc_log_trace("settings: settings saved to (%s)", _fullpath.c_str());

    return true;
}

auto SettingsFile::dirname() const -> const std::string&
{
    return _dirname;
}

auto SettingsFile::filename() const -> const std::string&
{
    return _filename;
}

auto SettingsFile::fullpath() const -> const std::string&
{
    return _fullpath;
}

auto SettingsFile::table(const std::string& name) -> SettingsTable&
{
    auto it = _index.find(name);
    if(it != _index.end()) {
        return *it->second;
    }
    _tables.emplace_back(std::make_unique<SettingsTable>(name));

    SettingsTable* const settings_table = _tables.back().get();

    _index.emplace(name, settings_table);

    return *settings_table;
}

auto SettingsFile::table(const std::string& name) const -> const SettingsTable&
{
    static const SettingsTable empty_table("");

    auto it = _index.find(name);
    if(it != _index.end()) {
        return *it->second;
    }
    return empty_table;
}

auto SettingsFile::has_table(const std::string& name) const -> bool
{
    return _index.find(name) != _index.end();
}

auto SettingsFile::remove_table(const std::string& name) -> bool
{
    auto it = _index.find(name);
    if(it == _index.end()) {
        return false;
    }
    const SettingsTable* const settings_table = it->second;

    _index.erase(it);
    for(auto table_it = _tables.begin(); table_it != _tables.end(); ++table_it) {
        if(table_it->get() == settings_table) {
            _tables.erase(table_it);
            break;
        }
    }
    return true;
}

auto SettingsFile::clear() -> void
{
    _index.clear();
    _tables.clear();
    _epilog.clear();
}

auto SettingsFile::begin() const -> const_iterator
{
    return _tables.begin();
}

auto SettingsFile::end() const -> const_iterator
{
    return _tables.end();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
