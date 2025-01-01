/*
 * console.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include "console.h"

// ---------------------------------------------------------------------------
// <anonymous>::ConsoleTraits
// ---------------------------------------------------------------------------

namespace {

struct ConsoleTraits
{
    static void println(std::ostream& stream, const char* format, va_list arguments)
    {
        if(stream.good()) {
            char* message = nullptr;
            const int rc = ::vasprintf(&message, format, arguments);
            if((rc != -1) && (message != nullptr)) {
                stream << message << std::endl;
                message = (::free(message), nullptr);
            }
        }
    }
};

}

// ---------------------------------------------------------------------------
// base::Console
// ---------------------------------------------------------------------------

namespace base {

Console::Console ( std::istream& istream
                 , std::ostream& ostream
                 , std::ostream& estream )
    : _istream(istream)
    , _ostream(ostream)
    , _estream(estream)
{
}

void Console::println(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    ConsoleTraits::println(_ostream, format, arguments);
    va_end(arguments);
}

void Console::errorln(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    ConsoleTraits::println(_estream, format, arguments);
    va_end(arguments);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
