/*
 * libxcpc.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include "libxcpc-priv.h"

// ---------------------------------------------------------------------------
// some useful macros
// ---------------------------------------------------------------------------

#ifndef XCPC_INPUT_STREAM
#define XCPC_INPUT_STREAM stdin
#endif

#ifndef XCPC_PRINT_STREAM
#define XCPC_PRINT_STREAM stdout
#endif

#ifndef XCPC_ERROR_STREAM
#define XCPC_ERROR_STREAM stderr
#endif

#ifndef XCPC_JOYSTICK0
#define XCPC_JOYSTICK0 "/dev/input/js0"
#endif

#ifndef XCPC_JOYSTICK1
#define XCPC_JOYSTICK1 "/dev/input/js1"
#endif

// ---------------------------------------------------------------------------
// xcpc::internal::Library
// ---------------------------------------------------------------------------

namespace xcpc {

namespace internal {

struct Library
{
    int   initialized;
    int   loglevel;
    FILE* input_stream;
    FILE* print_stream;
    FILE* error_stream;
    char* bindir;
    char* libdir;
    char* datdir;
    char* docdir;
    char* resdir;
    char* romdir;
    char* dskdir;
    char* snadir;
    char* joystick0;
    char* joystick1;
    void* reserved;
};

}

}

// ---------------------------------------------------------------------------
// xcpc::internal::ValueEntry
// ---------------------------------------------------------------------------

namespace xcpc {

namespace internal {

struct ValueEntry
{
    const char* label;
    int         value;
};

}

}

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using XcpcLibrary    = xcpc::internal::Library;
using XcpcValueEntry = xcpc::internal::ValueEntry;

// ---------------------------------------------------------------------------
// <anonymous>::libxcpc
// ---------------------------------------------------------------------------

namespace {

XcpcLibrary libxcpc = {
    0,                     /* initialized   */
    XCPC_LOGLEVEL_UNKNOWN, /* loglevel      */
    nullptr,               /* input_stream  */
    nullptr,               /* print_stream  */
    nullptr,               /* error_stream  */
    nullptr,               /* bindir        */
    nullptr,               /* libdir        */
    nullptr,               /* datdir        */
    nullptr,               /* docdir        */
    nullptr,               /* resdir        */
    nullptr,               /* joystick0     */
    nullptr,               /* joystick1     */
    nullptr                /* reserved      */
};

}

// ---------------------------------------------------------------------------
// <anonymous>::company_name_table
// ---------------------------------------------------------------------------

namespace {

const XcpcValueEntry company_name_table[] = {
    { "default"  , XCPC_COMPANY_NAME_DEFAULT   },
    { "Isp"      , XCPC_COMPANY_NAME_ISP       },
    { "Triumph"  , XCPC_COMPANY_NAME_TRIUMPH   },
    { "Saisho"   , XCPC_COMPANY_NAME_SAISHO    },
    { "Solavox"  , XCPC_COMPANY_NAME_SOLAVOX   },
    { "Awa"      , XCPC_COMPANY_NAME_AWA       },
    { "Schneider", XCPC_COMPANY_NAME_SCHNEIDER },
    { "Orion"    , XCPC_COMPANY_NAME_ORION     },
    { "Amstrad"  , XCPC_COMPANY_NAME_AMSTRAD   },
};

}

// ---------------------------------------------------------------------------
// <anonymous>::machine_type_table
// ---------------------------------------------------------------------------

namespace {

const XcpcValueEntry machine_type_table[] = {
    { "default" , XCPC_MACHINE_TYPE_DEFAULT },
    { "CPC 464" , XCPC_MACHINE_TYPE_CPC464  },
    { "CPC 664" , XCPC_MACHINE_TYPE_CPC664  },
    { "CPC 6128", XCPC_MACHINE_TYPE_CPC6128 },
    { "CPC464"  , XCPC_MACHINE_TYPE_CPC464  },
    { "CPC664"  , XCPC_MACHINE_TYPE_CPC664  },
    { "CPC6128" , XCPC_MACHINE_TYPE_CPC6128 },
    { "v1"      , XCPC_MACHINE_TYPE_CPC464  },
    { "v2"      , XCPC_MACHINE_TYPE_CPC664  },
    { "v3"      , XCPC_MACHINE_TYPE_CPC6128 },
};

}

// ---------------------------------------------------------------------------
// <anonymous>::monitor_type_table
// ---------------------------------------------------------------------------

namespace {

const XcpcValueEntry monitor_type_table[] = {
    { "default", XCPC_MONITOR_TYPE_DEFAULT },
    { "color"  , XCPC_MONITOR_TYPE_COLOR   },
    { "green"  , XCPC_MONITOR_TYPE_GREEN   },
    { "gray"   , XCPC_MONITOR_TYPE_GRAY    },
    { "CTM640" , XCPC_MONITOR_TYPE_CTM640  },
    { "CTM644" , XCPC_MONITOR_TYPE_CTM644  },
    { "GT64"   , XCPC_MONITOR_TYPE_GT64    },
    { "GT65"   , XCPC_MONITOR_TYPE_GT65    },
    { "CM14"   , XCPC_MONITOR_TYPE_CM14    },
    { "MM12"   , XCPC_MONITOR_TYPE_MM12    },
};

}

// ---------------------------------------------------------------------------
// <anonymous>::refresh_rate_table
// ---------------------------------------------------------------------------

namespace {

const XcpcValueEntry refresh_rate_table[] = {
    { "default", XCPC_REFRESH_RATE_DEFAULT },
    { "50Hz"   , XCPC_REFRESH_RATE_50HZ    },
    { "60Hz"   , XCPC_REFRESH_RATE_60HZ    },
};

}

// ---------------------------------------------------------------------------
// <anonymous>::keyboard_type_table
// ---------------------------------------------------------------------------

namespace {

const XcpcValueEntry keyboard_type_table[] = {
    { "default", XCPC_KEYBOARD_TYPE_DEFAULT },
    { "english", XCPC_KEYBOARD_TYPE_ENGLISH },
    { "french" , XCPC_KEYBOARD_TYPE_FRENCH  },
    { "german" , XCPC_KEYBOARD_TYPE_GERMAN  },
    { "spanish", XCPC_KEYBOARD_TYPE_SPANISH },
    { "danish" , XCPC_KEYBOARD_TYPE_DANISH  },
};

}

// ---------------------------------------------------------------------------
// <anonymous>::memory_size_table
// ---------------------------------------------------------------------------

namespace {

const XcpcValueEntry memory_size_table[] = {
    { "default", XCPC_MEMORY_SIZE_DEFAULT },
    { "64Kb"   , XCPC_MEMORY_SIZE_64K     },
    { "128Kb"  , XCPC_MEMORY_SIZE_128K    },
    { "192Kb"  , XCPC_MEMORY_SIZE_192K    },
    { "256Kb"  , XCPC_MEMORY_SIZE_256K    },
    { "320Kb"  , XCPC_MEMORY_SIZE_320K    },
    { "384Kb"  , XCPC_MEMORY_SIZE_384K    },
    { "448Kb"  , XCPC_MEMORY_SIZE_448K    },
    { "512Kb"  , XCPC_MEMORY_SIZE_512K    },
};

}

// ---------------------------------------------------------------------------
// <anonymous>::constants
// ---------------------------------------------------------------------------

namespace {

constexpr int libxcpc_major_version = PACKAGE_MAJOR_VERSION;
constexpr int libxcpc_minor_version = PACKAGE_MINOR_VERSION;
constexpr int libxcpc_micro_version = PACKAGE_MICRO_VERSION;

constexpr char libxcpc_version[] = ""
    PACKAGE_STRING;
    ;

constexpr char libxcpc_copyright[] = ""
    "Copyright (c) 2001-2025 - Olivier Poncet";
    ;

constexpr char libxcpc_comments[] = ""
    "Xcpc, an Amstrad CPC emulator for Linux, BSD and Unix"                    "\n"
    ""                                                                         "\n"
    "Amstrad has kindly given it's permission for it's copyrighted"            "\n"
    "material to be redistributed but Amstrad retains it's copyright."         "\n"
    ""                                                                         "\n"
    "Some of the Amstrad CPC ROM code is copyright Locomotive Software."       "\n"
    ""                                                                         "\n"
    "ROM and DISK images are protected under the copyrights of their authors," "\n"
    "and cannot be distributed in this package. You can download and/or use"   "\n"
    "ROM and DISK images at your own risk and responsibility."                 "\n"
    ;

constexpr char libxcpc_website[] = ""
    "https://www.xcpc-emulator.net/"
    ;

constexpr char libxcpc_license[] = ""
    "Xcpc, an Amstrad CPC emulator for Linux, BSD and Unix"                              "\n"
    ""                                                                                   "\n"
    "This program is free software: you can redistribute it and/or modify"               "\n"
    "it under the terms of the GNU General Public License as published by"               "\n"
    "the Free Software Foundation, either version 2 of the License, or"                  "\n"
    "(at your option) any later version."                                                "\n"
    ""                                                                                   "\n"
    "This program is distributed in the hope that it will be useful,"                    "\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of"                     "\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"                      "\n"
    "GNU General Public License for more details."                                       "\n"
    ""                                                                                   "\n"
    "You should have received a copy of the GNU General Public License"                  "\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>"               "\n"
    ""                                                                                   "\n"
    "----------------------------------------------------------------------------------" "\n"
    ""                                                                                   "\n"
    "miniaudio v0.11.24, a single file audio playback and capture library written in C." "\n"
    ""                                                                                   "\n"
    "Copyright 2025 David Reid"                                                          "\n"
    ""                                                                                   "\n"
    "Permission is hereby granted, free of charge, to any person obtaining a copy of"    "\n"
    "this software and associated documentation files (the \"Software\"), to deal in"    "\n"
    "the Software without restriction, including without limitation the rights to"       "\n"
    "use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies"      "\n"
    "of the Software, and to permit persons to whom the Software is furnished to do"     "\n"
    "so."                                                                                "\n"
    ""                                                                                   "\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"       "\n"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"           "\n"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"        "\n"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"             "\n"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"      "\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"      "\n"
    "SOFTWARE."                                                                          "\n"
    ""                                                                                   "\n"
    "----------------------------------------------------------------------------------" "\n"
    ""                                                                                   "\n"
    "libdsk v1.4.2, a library for accessing discs and disc image files."                 "\n"
    ""                                                                                   "\n"
    "Copyright (C) 2001-2015 John Elliott <seasip.webmaster@gmail.com>"                  "\n"
    ""                                                                                   "\n"
    "Modifications to add dsk_dirty()"                                                   "\n"
    "(c) 2005 Philip Kendall <pak21-spectrum@srcf.ucam.org>"                             "\n"
    ""                                                                                   "\n"
    "This library is free software; you can redistribute it and/or"                      "\n"
    "modify it under the terms of the GNU Library General Public"                        "\n"
    "License as published by the Free Software Foundation; either"                       "\n"
    "version 2 of the License, or (at your option) any later version."                   "\n"
    ""                                                                                   "\n"
    "This library is distributed in the hope that it will be useful,"                    "\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of"                     "\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU"                  "\n"
    "Library General Public License for more details."                                   "\n"
    ""                                                                                   "\n"
    "You should have received a copy of the GNU Library General Public"                  "\n"
    "License along with this library; if not, write to the Free"                         "\n"
    "Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,"                    "\n"
    "MA 02111-1307, USA"                                                                 "\n"
    ""                                                                                   "\n"
    "----------------------------------------------------------------------------------" "\n"
    ""                                                                                   "\n"
    "lib765 v0.4.2, a library to emulate the uPD765a floppy controller."                 "\n"
    ""                                                                                   "\n"
    "Copyright (C) 2002,2003,2004  John Elliott <jce@seasip.demon.co.uk>"                "\n"
    ""                                                                                   "\n"
    "Modifications to add dirty flags"                                                   "\n"
    "(c) 2005 Philip Kendall <pak21-spectrum@srcf.ucam.org>"                             "\n"
    ""                                                                                   "\n"
    "This library is free software; you can redistribute it and/or"                      "\n"
    "modify it under the terms of the GNU Library General Public"                        "\n"
    "License as published by the Free Software Foundation; either"                       "\n"
    "version 2 of the License, or (at your option) any later version."                   "\n"
    ""                                                                                   "\n"
    "This library is distributed in the hope that it will be useful,"                    "\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of"                     "\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU"                  "\n"
    "Library General Public License for more details."                                   "\n"
    ""                                                                                   "\n"
    "You should have received a copy of the GNU Library General Public"                  "\n"
    "License along with this library; if not, write to the Free"                         "\n"
    "Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA."                 "\n"
    ;

constexpr char libxcpc_bindir[]    = XCPC_BINDIR;
constexpr char libxcpc_libdir[]    = XCPC_LIBDIR;
constexpr char libxcpc_datdir[]    = XCPC_DATDIR;
constexpr char libxcpc_docdir[]    = XCPC_DOCDIR;
constexpr char libxcpc_resdir[]    = XCPC_RESDIR;
constexpr char libxcpc_romdir[]    = XCPC_ROMDIR;
constexpr char libxcpc_dskdir[]    = XCPC_DSKDIR;
constexpr char libxcpc_snadir[]    = XCPC_SNADIR;
constexpr char libxcpc_joystick0[] = XCPC_JOYSTICK0;
constexpr char libxcpc_joystick1[] = XCPC_JOYSTICK1;

}

// ---------------------------------------------------------------------------
// <anonymous>::libxcpc_traits
// ---------------------------------------------------------------------------

namespace {

struct libxcpc_traits
{
    static char* getenv(const char* variable, const char* fallback)
    {
        const char* value = ::getenv(variable);

        if(value == nullptr) {
            value = fallback;
        }
        return ::strdup(value);
    }

    static auto init_loglevel(XcpcLibrary& library) -> void
    {
        if(library.loglevel <= XCPC_LOGLEVEL_UNKNOWN) {
            const char* XCPC_LOGLEVEL = ::getenv("XCPC_LOGLEVEL");
            if((XCPC_LOGLEVEL != nullptr) && (*XCPC_LOGLEVEL != '\0')) {
                set_loglevel(library, ::atoi(XCPC_LOGLEVEL));
            }
            else {
                set_loglevel(library, XCPC_LOGLEVEL_PRINT);
            }
        }
    }

    static auto fini_loglevel(XcpcLibrary& library) -> void
    {
        if(library.loglevel != XCPC_LOGLEVEL_UNKNOWN) {
            library.loglevel = XCPC_LOGLEVEL_UNKNOWN;
        }
    }

    static auto init_streams(XcpcLibrary& library) -> void
    {
        if(library.input_stream == nullptr) {
            library.input_stream = XCPC_INPUT_STREAM;
        }
        if(library.print_stream == nullptr) {
            library.print_stream = XCPC_PRINT_STREAM;
        }
        if(library.error_stream == nullptr) {
            library.error_stream = XCPC_ERROR_STREAM;
        }
    }

    static auto fini_streams(XcpcLibrary& library) -> void
    {
        if(library.input_stream != nullptr) {
            library.input_stream = nullptr;
        }
        if(library.print_stream != nullptr) {
            library.print_stream = nullptr;
        }
        if(library.error_stream != nullptr) {
            library.error_stream = nullptr;
        }
    }

    static auto init_directories(XcpcLibrary& library) -> void
    {
        if(library.bindir == nullptr) { library.bindir = getenv("XCPC_BINDIR", libxcpc_bindir); }
        if(library.libdir == nullptr) { library.libdir = getenv("XCPC_LIBDIR", libxcpc_libdir); }
        if(library.datdir == nullptr) { library.datdir = getenv("XCPC_DATDIR", libxcpc_datdir); }
        if(library.docdir == nullptr) { library.docdir = getenv("XCPC_DOCDIR", libxcpc_docdir); }
        if(library.resdir == nullptr) { library.resdir = getenv("XCPC_RESDIR", libxcpc_resdir); }
        if(library.romdir == nullptr) { library.romdir = getenv("XCPC_ROMDIR", libxcpc_romdir); }
        if(library.dskdir == nullptr) { library.dskdir = getenv("XCPC_DSKDIR", libxcpc_dskdir); }
        if(library.snadir == nullptr) { library.snadir = getenv("XCPC_SNADIR", libxcpc_snadir); }
    }

    static auto fini_directories(XcpcLibrary& library) -> void
    {
        if(library.bindir != nullptr) { library.bindir = (::free(library.bindir), nullptr); }
        if(library.libdir != nullptr) { library.libdir = (::free(library.libdir), nullptr); }
        if(library.datdir != nullptr) { library.datdir = (::free(library.datdir), nullptr); }
        if(library.docdir != nullptr) { library.docdir = (::free(library.docdir), nullptr); }
        if(library.resdir != nullptr) { library.resdir = (::free(library.resdir), nullptr); }
        if(library.romdir != nullptr) { library.romdir = (::free(library.romdir), nullptr); }
        if(library.dskdir != nullptr) { library.dskdir = (::free(library.dskdir), nullptr); }
        if(library.snadir != nullptr) { library.snadir = (::free(library.snadir), nullptr); }
    }

    static auto init_joysticks(XcpcLibrary& library) -> void
    {
        if(library.joystick0 == nullptr) { library.joystick0 = getenv("XCPC_JOYSTICK0", libxcpc_joystick0); }
        if(library.joystick1 == nullptr) { library.joystick1 = getenv("XCPC_JOYSTICK1", libxcpc_joystick1); }
    }

    static auto fini_joysticks(XcpcLibrary& library) -> void
    {
        if(library.joystick0 != nullptr) { library.joystick0 = (::free(library.joystick0), nullptr); }
        if(library.joystick1 != nullptr) { library.joystick1 = (::free(library.joystick1), nullptr); }
    }

    static auto begin(XcpcLibrary& library) -> void
    {
        if(library.initialized++ == 0) {
            init_loglevel(library);
            init_streams(library);
            init_directories(library);
            init_joysticks(library);
        }
    }

    static auto end(XcpcLibrary& library) -> void
    {
        if(--library.initialized == 0) {
            fini_joysticks(library);
            fini_directories(library);
            fini_streams(library);
            fini_loglevel(library);
        }
    }

    static auto get_major_version() -> int
    {
        return libxcpc_major_version;
    }

    static auto get_minor_version() -> int
    {
        return libxcpc_minor_version;
    }

    static auto get_micro_version() -> int
    {
        return libxcpc_micro_version;
    }

    static auto get_version() -> const char*
    {
        return libxcpc_version;
    }

    static auto get_copyright() -> const char*
    {
        return libxcpc_copyright;
    }

    static auto get_comments() -> const char*
    {
        return libxcpc_comments;
    }

    static auto get_website() -> const char*
    {
        return libxcpc_website;
    }

    static auto get_license() -> const char*
    {
        return libxcpc_license;
    }

    static auto get_bindir(XcpcLibrary& library) -> const char*
    {
        return library.bindir;
    }

    static auto get_libdir(XcpcLibrary& library) -> const char*
    {
        return library.libdir;
    }

    static auto get_datdir(XcpcLibrary& library) -> const char*
    {
        return library.datdir;
    }

    static auto get_docdir(XcpcLibrary& library) -> const char*
    {
        return library.docdir;
    }

    static auto get_resdir(XcpcLibrary& library) -> const char*
    {
        return library.resdir;
    }

    static auto get_romdir(XcpcLibrary& library) -> const char*
    {
        return library.romdir;
    }

    static auto get_dskdir(XcpcLibrary& library) -> const char*
    {
        return library.dskdir;
    }

    static auto get_snadir(XcpcLibrary& library) -> const char*
    {
        return library.snadir;
    }

    static auto get_joystick0(XcpcLibrary& library) -> const char*
    {
        return library.joystick0;
    }

    static auto get_joystick1(XcpcLibrary& library) -> const char*
    {
        return library.joystick1;
    }

    static auto get_loglevel(XcpcLibrary& library) -> int
    {
        return library.loglevel;
    }

    static auto set_loglevel(XcpcLibrary& library, const int loglevel) -> void
    {
        if(loglevel > XCPC_LOGLEVEL_UNKNOWN) {
            library.loglevel = loglevel;
        }
        if(library.loglevel <= XCPC_LOGLEVEL_UNKNOWN) {
            library.loglevel = XCPC_LOGLEVEL_PRINT;
        }
    }

    static auto company_name_from_string(const char* label) -> XcpcCompanyName
    {
        if((label != nullptr) && (*label != '\0')) {
            for(auto& entry : company_name_table) {
                if(::strcasecmp(entry.label, label) == 0) {
                    return static_cast<XcpcCompanyName>(entry.value);
                }
            }
        }
        return XCPC_COMPANY_NAME_UNKNOWN;
    }

    static auto machine_type_from_string(const char* label) -> XcpcMachineType
    {
        if((label != nullptr) && (*label != '\0')) {
            for(auto& entry : machine_type_table) {
                if(::strcasecmp(entry.label, label) == 0) {
                    return static_cast<XcpcMachineType>(entry.value);
                }
            }
        }
        return XCPC_MACHINE_TYPE_UNKNOWN;
    }

    static auto monitor_type_from_string(const char* label) -> XcpcMonitorType
    {
        if((label != nullptr) && (*label != '\0')) {
            for(auto& entry : monitor_type_table) {
                if(::strcasecmp(entry.label, label) == 0) {
                    return static_cast<XcpcMonitorType>(entry.value);
                }
            }
        }
        return XCPC_MONITOR_TYPE_UNKNOWN;
    }

    static auto refresh_rate_from_string(const char* label) -> XcpcRefreshRate
    {
        if((label != nullptr) && (*label != '\0')) {
            for(auto& entry : refresh_rate_table) {
                if(::strcasecmp(entry.label, label) == 0) {
                    return static_cast<XcpcRefreshRate>(entry.value);
                }
            }
        }
        return XCPC_REFRESH_RATE_UNKNOWN;
    }

    static auto keyboard_type_from_string(const char* label) -> XcpcKeyboardType
    {
        if((label != nullptr) && (*label != '\0')) {
            for(auto& entry : keyboard_type_table) {
                if(::strcasecmp(entry.label, label) == 0) {
                    return static_cast<XcpcKeyboardType>(entry.value);
                }
            }
        }
        return XCPC_KEYBOARD_TYPE_UNKNOWN;
    }

    static auto memory_size_from_string(const char* label) -> XcpcMemorySize
    {
        if((label != nullptr) && (*label != '\0')) {
            for(auto& entry : memory_size_table) {
                if(::strcasecmp(entry.label, label) == 0) {
                    return static_cast<XcpcMemorySize>(entry.value);
                }
            }
        }
        return XCPC_MEMORY_SIZE_UNKNOWN;
    }

    static auto company_name_to_string(const XcpcCompanyName value) -> const char*
    {
        for(auto& entry : company_name_table) {
            if(static_cast<XcpcCompanyName>(entry.value) == value) {
                return entry.label;
            }
        }
        return "unknown";
    }

    static auto machine_type_to_string(const XcpcMachineType value) -> const char*
    {
        for(auto& entry : machine_type_table) {
            if(static_cast<XcpcMachineType>(entry.value) == value) {
                return entry.label;
            }
        }
        return "unknown";
    }

    static auto monitor_type_to_string(const XcpcMonitorType value) -> const char*
    {
        for(auto& entry : monitor_type_table) {
            if(static_cast<XcpcMonitorType>(entry.value) == value) {
                return entry.label;
            }
        }
        return "unknown";
    }

    static auto refresh_rate_to_string(const XcpcRefreshRate value) -> const char*
    {
        for(auto& entry : refresh_rate_table) {
            if(static_cast<XcpcRefreshRate>(entry.value) == value) {
                return entry.label;
            }
        }
        return "unknown";
    }

    static auto keyboard_type_to_string(const XcpcKeyboardType value) -> const char*
    {
        for(auto& entry : keyboard_type_table) {
            if(static_cast<XcpcKeyboardType>(entry.value) == value) {
                return entry.label;
            }
        }
        return "unknown";
    }

    static auto memory_size_to_string(const XcpcMemorySize value) -> const char*
    {
        for(auto& entry : memory_size_table) {
            if(static_cast<XcpcMemorySize>(entry.value) == value) {
                return entry.label;
            }
        }
        return "unknown";
    }
};

}

// ---------------------------------------------------------------------------
// libxcpc public interface
// ---------------------------------------------------------------------------

void xcpc_begin(void)
{
    return libxcpc_traits::begin(libxcpc);
}

void xcpc_end(void)
{
    return libxcpc_traits::end(libxcpc);
}

int xcpc_get_major_version(void)
{
    return libxcpc_traits::get_major_version();
}

int xcpc_get_minor_version(void)
{
    return libxcpc_traits::get_minor_version();
}

int xcpc_get_micro_version(void)
{
    return libxcpc_traits::get_micro_version();
}

const char* xcpc_get_version(void)
{
    return libxcpc_traits::get_version();
}

const char* xcpc_get_copyright(void)
{
    return libxcpc_traits::get_copyright();
}

const char* xcpc_get_comments(void)
{
    return libxcpc_traits::get_comments();
}

const char* xcpc_get_website(void)
{
    return libxcpc_traits::get_website();
}

const char* xcpc_get_license(void)
{
    return libxcpc_traits::get_license();
}

const char* xcpc_get_bindir(void)
{
    return libxcpc_traits::get_bindir(libxcpc);
}

const char* xcpc_get_libdir(void)
{
    return libxcpc_traits::get_libdir(libxcpc);
}

const char* xcpc_get_datdir(void)
{
    return libxcpc_traits::get_datdir(libxcpc);
}

const char* xcpc_get_docdir(void)
{
    return libxcpc_traits::get_docdir(libxcpc);
}

const char* xcpc_get_resdir(void)
{
    return libxcpc_traits::get_resdir(libxcpc);
}

const char* xcpc_get_romdir(void)
{
    return libxcpc_traits::get_romdir(libxcpc);
}

const char* xcpc_get_dskdir(void)
{
    return libxcpc_traits::get_dskdir(libxcpc);
}

const char* xcpc_get_snadir(void)
{
    return libxcpc_traits::get_snadir(libxcpc);
}

const char* xcpc_get_joystick0(void)
{
    return libxcpc_traits::get_joystick0(libxcpc);
}

const char* xcpc_get_joystick1(void)
{
    return libxcpc_traits::get_joystick1(libxcpc);
}

int xcpc_get_loglevel(void)
{
    return libxcpc_traits::get_loglevel(libxcpc);
}

int xcpc_set_loglevel(const int loglevel)
{
    libxcpc_traits::set_loglevel(libxcpc, loglevel);

    return libxcpc_traits::get_loglevel(libxcpc);
}

void xcpc_println(const char* format, ...)
{
    FILE* stream = libxcpc.print_stream;

    if(stream != nullptr) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

void xcpc_errorln(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if(stream != nullptr) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_error(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != nullptr) && (libxcpc.loglevel >= XCPC_LOGLEVEL_ERROR)) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::fputc('E', stream);
        (void) ::fputc('\t', stream);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_alert(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != nullptr) && (libxcpc.loglevel >= XCPC_LOGLEVEL_ALERT)) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::fputc('W', stream);
        (void) ::fputc('\t', stream);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_print(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != nullptr) && (libxcpc.loglevel >= XCPC_LOGLEVEL_PRINT)) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::fputc('I', stream);
        (void) ::fputc('\t', stream);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_trace(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != nullptr) && (libxcpc.loglevel >= XCPC_LOGLEVEL_TRACE)) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::fputc('T', stream);
        (void) ::fputc('\t', stream);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_debug(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != nullptr) && (libxcpc.loglevel >= XCPC_LOGLEVEL_DEBUG)) {
        va_list arguments;
        va_start(arguments, format);
        (void) ::fputc('D', stream);
        (void) ::fputc('\t', stream);
        (void) ::vfprintf(stream, format, arguments);
        (void) ::fputc('\n', stream);
        (void) ::fflush(stream);
        va_end(arguments);
    }
}

// ---------------------------------------------------------------------------
// enums from string
// ---------------------------------------------------------------------------

XcpcCompanyName xcpc_company_name_from_string(const char* label)
{
    return libxcpc_traits::company_name_from_string(label);
}

XcpcMachineType xcpc_machine_type_from_string(const char* label)
{
    return libxcpc_traits::machine_type_from_string(label);
}

XcpcMonitorType xcpc_monitor_type_from_string(const char* label)
{
    return libxcpc_traits::monitor_type_from_string(label);
}

XcpcRefreshRate xcpc_refresh_rate_from_string(const char* label)
{
    return libxcpc_traits::refresh_rate_from_string(label);
}

XcpcKeyboardType xcpc_keyboard_type_from_string(const char* label)
{
    return libxcpc_traits::keyboard_type_from_string(label);
}

XcpcMemorySize xcpc_memory_size_from_string(const char* label)
{
    return libxcpc_traits::memory_size_from_string(label);
}

// ---------------------------------------------------------------------------
// enums to string
// ---------------------------------------------------------------------------

const char* xcpc_company_name_to_string(XcpcCompanyName value)
{
    return libxcpc_traits::company_name_to_string(value);
}

const char* xcpc_machine_type_to_string(XcpcMachineType value)
{
    return libxcpc_traits::machine_type_to_string(value);
}

const char* xcpc_monitor_type_to_string(XcpcMonitorType value)
{
    return libxcpc_traits::monitor_type_to_string(value);
}

const char* xcpc_refresh_rate_to_string(XcpcRefreshRate value)
{
    return libxcpc_traits::refresh_rate_to_string(value);
}

const char* xcpc_keyboard_type_to_string(XcpcKeyboardType value)
{
    return libxcpc_traits::keyboard_type_to_string(value);
}

const char* xcpc_memory_size_to_string(XcpcMemorySize value)
{
    return libxcpc_traits::memory_size_to_string(value);
}

// ---------------------------------------------------------------------------
// xcpc::Utils
// ---------------------------------------------------------------------------

namespace xcpc {

auto Utils::get_major_version() -> int
{
    return ::xcpc_get_major_version();
}

auto Utils::get_minor_version() -> int
{
    return ::xcpc_get_minor_version();
}

auto Utils::get_micro_version() -> int
{
    return ::xcpc_get_micro_version();
}

auto Utils::get_version() -> std::string
{
    return ::xcpc_get_version();
}

auto Utils::get_copyright() -> std::string
{
    return ::xcpc_get_copyright();
}

auto Utils::get_comments() -> std::string
{
    return ::xcpc_get_comments();
}

auto Utils::get_website() -> std::string
{
    return ::xcpc_get_website();
}

auto Utils::get_bindir() -> std::string
{
    return ::xcpc_get_bindir();
}

auto Utils::get_libdir() -> std::string
{
    return ::xcpc_get_libdir();
}

auto Utils::get_datdir() -> std::string
{
    return ::xcpc_get_datdir();
}

auto Utils::get_docdir() -> std::string
{
    return ::xcpc_get_docdir();
}

auto Utils::get_resdir() -> std::string
{
    return ::xcpc_get_resdir();
}

auto Utils::get_romdir() -> std::string
{
    return ::xcpc_get_romdir();
}

auto Utils::get_dskdir() -> std::string
{
    return ::xcpc_get_dskdir();
}

auto Utils::get_snadir() -> std::string
{
    return ::xcpc_get_snadir();
}

auto Utils::get_joystick0() -> std::string
{
    return ::xcpc_get_joystick0();
}

auto Utils::get_joystick1() -> std::string
{
    return ::xcpc_get_joystick1();
}

auto Utils::get_license() -> std::string
{
    return ::xcpc_get_license();
}

auto Utils::get_loglevel() -> int
{
    return ::xcpc_get_loglevel();
}

auto Utils::set_loglevel(const int loglevel) -> int
{
    return ::xcpc_set_loglevel(loglevel);
}

auto Utils::company_name_from_string(const std::string& string) -> CompanyName
{
    return ::xcpc_company_name_from_string(string.c_str());
}

auto Utils::machine_type_from_string(const std::string& string) -> MachineType
{
    return ::xcpc_machine_type_from_string(string.c_str());
}

auto Utils::monitor_type_from_string(const std::string& string) -> MonitorType
{
    return ::xcpc_monitor_type_from_string(string.c_str());
}

auto Utils::refresh_rate_from_string(const std::string& string) -> RefreshRate
{
    return ::xcpc_refresh_rate_from_string(string.c_str());
}

auto Utils::keyboard_type_from_string(const std::string& string) -> KeyboardType
{
    return ::xcpc_keyboard_type_from_string(string.c_str());
}

auto Utils::memory_size_from_string(const std::string& string) -> MemorySize
{
    return ::xcpc_memory_size_from_string(string.c_str());
}

auto Utils::company_name_to_string(const CompanyName value) -> std::string
{
    return ::xcpc_company_name_to_string(value);
}

auto Utils::machine_type_to_string(const MachineType value) -> std::string
{
    return ::xcpc_machine_type_to_string(value);
}

auto Utils::monitor_type_to_string(const MonitorType value) -> std::string
{
    return ::xcpc_monitor_type_to_string(value);
}

auto Utils::refresh_rate_to_string(const RefreshRate value) -> std::string
{
    return ::xcpc_refresh_rate_to_string(value);
}

auto Utils::keyboard_type_to_string(const KeyboardType value) -> std::string
{
    return ::xcpc_keyboard_type_to_string(value);
}

auto Utils::memory_size_to_string(const MemorySize value) -> std::string
{
    return ::xcpc_memory_size_to_string(value);
}

}

// ---------------------------------------------------------------------------
// <anonymous>::AudioTraits
// ---------------------------------------------------------------------------

namespace {

struct AudioTraits
{
    using AudioDeviceType = xcpc::AudioDeviceType;
    using MiniAudioConfig = xcpc::MiniAudioConfig;
    using MiniAudioDevice = xcpc::MiniAudioDevice;
    using AudioConfig     = xcpc::AudioConfig;
    using AudioDevice     = xcpc::AudioDevice;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::MiniAudioConfigTraits
// ---------------------------------------------------------------------------

namespace {

struct MiniAudioConfigTraits final
    : public AudioTraits
{
    static auto init(MiniAudioConfig& config, const AudioDeviceType type) -> void
    {
        config = ::ma_device_config_init(type);
    }

    static auto get_audio_config() -> AudioConfig
    {
        AudioConfig config(ma_device_type_playback);

        auto get_channels = [](const uint32_t default_value) -> uint32_t
        {
            const char* value = ::getenv("XCPC_AUDIO_CHANNELS");

            if(value != nullptr) {
                return ::atoi(value);
            }
            return default_value;
        };

        auto get_sampleRate = [](const uint32_t default_value) -> uint32_t
        {
            const char* value = ::getenv("XCPC_AUDIO_SAMPLERATE");

            if(value != nullptr) {
                return ::atoi(value);
            }
            return default_value;
        };

        auto get_periodSizeInMilliseconds = [](const uint32_t default_value) -> uint32_t
        {
            const char* value = ::getenv("XCPC_AUDIO_PERIODSIZEINMILLISECONDS");

            if(value != nullptr) {
                return ::atoi(value);
            }
            return default_value;
        };

        config->sampleRate               = get_sampleRate(0);
        config->periodSizeInMilliseconds = get_periodSizeInMilliseconds(0);
        config->playback.format          = ma_format_f32;
        config->playback.channels        = get_channels(0);

        return config;
    }

};

}

// ---------------------------------------------------------------------------
// <anonymous>::MiniAudioDeviceTraits
// ---------------------------------------------------------------------------

namespace {

struct MiniAudioDeviceTraits final
    : public AudioTraits
{
    static void init(MiniAudioDevice& device, MiniAudioConfig* config)
    {
        if(::ma_device_init(nullptr, config, &device) != MA_SUCCESS) {
            throw std::runtime_error("ma_device_init() has failed");
        }
    }

    static void uninit(MiniAudioDevice& device)
    {
        ::ma_device_uninit(&device);
    }

    static void start(MiniAudioDevice& device)
    {
        if(::ma_device_start(&device) != MA_SUCCESS) {
            throw std::runtime_error("ma_device_start() has failed");
        }
    }

    static void stop(MiniAudioDevice& device)
    {
        if(::ma_device_stop(&device) != MA_SUCCESS) {
            throw std::runtime_error("ma_device_stop() has failed");
        }
    }
};

}

// ---------------------------------------------------------------------------
// xcpc::AudioConfig
// ---------------------------------------------------------------------------

namespace xcpc {

AudioConfig::AudioConfig(const AudioDeviceType type)
    : _impl()
{
    MiniAudioConfigTraits::init(_impl, type);
}

}

// ---------------------------------------------------------------------------
// xcpc::AudioDevice
// ---------------------------------------------------------------------------

namespace xcpc {

AudioDevice::AudioDevice()
    : AudioDevice(MiniAudioConfigTraits::get_audio_config())
{
}

AudioDevice::AudioDevice(const AudioConfig& config)
    : _impl()
    , _processor(nullptr)
{
    AudioConfig settings(config);

    auto callback = [](MiniAudioDevice* device_impl, void* output, const void* input, ma_uint32 count) -> void
    {
        AudioDevice* device = reinterpret_cast<AudioDevice*>(device_impl->pUserData);

        if(device != nullptr) {
            AudioProcessor* processor = device->_processor;
            if(processor != nullptr) {
                processor->process(input, output, count);
            }
        }
    };

    auto get_config = [&]() -> MiniAudioConfig*
    {
        settings->pUserData    = this;
        settings->dataCallback = callback;
        return settings.get();
    };

    MiniAudioDeviceTraits::init(_impl, get_config());
}

AudioDevice::~AudioDevice()
{
    MiniAudioDeviceTraits::uninit(_impl);
}

void AudioDevice::start()
{
    MiniAudioDeviceTraits::start(_impl);
}

void AudioDevice::stop()
{
    MiniAudioDeviceTraits::stop(_impl);
}

void AudioDevice::attach(AudioProcessor& processor)
{
    if(_processor == nullptr) {
        _processor = &processor;
    }
}

void AudioDevice::detach(AudioProcessor& processor)
{
    if(_processor == &processor) {
        _processor = nullptr;
    }
}

}

// ---------------------------------------------------------------------------
// xcpc::AudioProcessor
// ---------------------------------------------------------------------------

namespace xcpc {

AudioProcessor::AudioProcessor(AudioDevice& device)
    : _device(device)
    , _mutex()
{
    _device.attach(*this);
}

AudioProcessor::~AudioProcessor()
{
    _device.detach(*this);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
