/*
 * libxcpc.c - Copyright (c) 2001-2022 - Olivier Poncet
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include "libxcpc-priv.h"

static XcpcLibrary libxcpc = {
    0,                     /* initialized   */
    PACKAGE_MAJOR_VERSION, /* major version */
    PACKAGE_MINOR_VERSION, /* minor version */
    PACKAGE_MICRO_VERSION, /* micro version */
    XCPC_LOGLEVEL_UNKNOWN, /* loglevel      */
    NULL,                  /* input_stream  */
    NULL,                  /* print_stream  */
    NULL,                  /* error_stream  */
    NULL,                  /* bindir        */
    NULL,                  /* libdir        */
    NULL,                  /* datdir        */
    NULL,                  /* docdir        */
    NULL,                  /* resdir        */
    NULL,                  /* joystick0     */
    NULL,                  /* joystick1     */
};

static const XcpcCompanyNameEntry xcpc_company_name_table[] = {
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

static const XcpcMachineTypeEntry xcpc_machine_type_table[] = {
    { "default" , XCPC_MACHINE_TYPE_DEFAULT },
    { "CPC 464" , XCPC_MACHINE_TYPE_CPC464  },
    { "CPC 664" , XCPC_MACHINE_TYPE_CPC664  },
    { "CPC 6128", XCPC_MACHINE_TYPE_CPC6128 },
    { "CPC464"  , XCPC_MACHINE_TYPE_CPC464  },
    { "CPC664"  , XCPC_MACHINE_TYPE_CPC664  },
    { "CPC6128" , XCPC_MACHINE_TYPE_CPC6128 },
};

static const XcpcMonitorTypeEntry xcpc_monitor_type_table[] = {
    { "default"   , XCPC_MONITOR_TYPE_DEFAULT    },
    { "color"     , XCPC_MONITOR_TYPE_COLOR      },
    { "green"     , XCPC_MONITOR_TYPE_GREEN      },
    { "monochrome", XCPC_MONITOR_TYPE_MONOCHROME },
    { "CTM640"    , XCPC_MONITOR_TYPE_CTM640     },
    { "CTM644"    , XCPC_MONITOR_TYPE_CTM644     },
    { "GT64"      , XCPC_MONITOR_TYPE_GT64       },
    { "GT65"      , XCPC_MONITOR_TYPE_GT65       },
    { "CM14"      , XCPC_MONITOR_TYPE_CM14       },
    { "MM12"      , XCPC_MONITOR_TYPE_MM12       },
};

static const XcpcRefreshRateEntry xcpc_refresh_rate_table[] = {
    { "default", XCPC_REFRESH_RATE_DEFAULT },
    { "50Hz"   , XCPC_REFRESH_RATE_50HZ    },
    { "60Hz"   , XCPC_REFRESH_RATE_60HZ    },
};

static const XcpcKeyboardTypeEntry xcpc_keyboard_type_table[] = {
    { "default", XCPC_KEYBOARD_TYPE_DEFAULT },
    { "QWERTY" , XCPC_KEYBOARD_TYPE_QWERTY  },
    { "AZERTY" , XCPC_KEYBOARD_TYPE_AZERTY  },
};

static const XcpcMemorySizeEntry xcpc_memory_size_table[] = {
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

static const XcpcColorEntry xcpc_color_table[] = {
    { "white"                       , XCPC_COLOR_WHITE                     , 0x8000, 0x8000, 0x8000, 0x8000 },
    { "white (not official)"        , XCPC_COLOR_WHITE_NOT_OFFICIAL        , 0x8000, 0x8000, 0x8000, 0x8000 },
    { "sea green"                   , XCPC_COLOR_SEA_GREEN                 , 0x0000, 0xffff, 0x8000, 0xa4dd },
    { "pastel yellow"               , XCPC_COLOR_PASTEL_YELLOW             , 0xffff, 0xffff, 0x8000, 0xf168 },
    { "blue"                        , XCPC_COLOR_BLUE                      , 0x0000, 0x0000, 0x8000, 0x0e97 },
    { "purple"                      , XCPC_COLOR_PURPLE                    , 0xffff, 0x0000, 0x8000, 0x5b22 },
    { "cyan"                        , XCPC_COLOR_CYAN                      , 0x0000, 0x8000, 0x8000, 0x59ba },
    { "pink"                        , XCPC_COLOR_PINK                      , 0xffff, 0x8000, 0x8000, 0xa645 },
    { "purple (not official)"       , XCPC_COLOR_PURPLE_NOT_OFFICIAL       , 0xffff, 0x0000, 0x8000, 0x5b22 },
    { "pastel yellow (not official)", XCPC_COLOR_PASTEL_YELLOW_NOT_OFFICIAL, 0xffff, 0xffff, 0x8000, 0xf168 },
    { "bright yellow"               , XCPC_COLOR_BRIGHT_YELLOW             , 0xffff, 0xffff, 0x0000, 0xe2d0 },
    { "bright white"                , XCPC_COLOR_BRIGHT_WHITE              , 0xffff, 0xffff, 0xffff, 0xffff },
    { "bright red"                  , XCPC_COLOR_BRIGHT_RED                , 0xffff, 0x0000, 0x0000, 0x4c8b },
    { "bright magenta"              , XCPC_COLOR_BRIGHT_MAGENTA            , 0xffff, 0x0000, 0xffff, 0x69ba },
    { "orange"                      , XCPC_COLOR_ORANGE                    , 0xffff, 0x8000, 0x0000, 0x97ad },
    { "pastel magenta"              , XCPC_COLOR_PASTEL_MAGENTA            , 0xffff, 0x8000, 0xffff, 0xb4dc },
    { "blue (not official)"         , XCPC_COLOR_BLUE_NOT_OFFICIAL         , 0x0000, 0x0000, 0x8000, 0x0e97 },
    { "sea green (not official)"    , XCPC_COLOR_SEA_GREEN_NOT_OFFICIAL    , 0x0000, 0xffff, 0x8000, 0xa4dd },
    { "bright green"                , XCPC_COLOR_BRIGHT_GREEN              , 0x0000, 0xffff, 0x0000, 0x9645 },
    { "bright cyan"                 , XCPC_COLOR_BRIGHT_CYAN               , 0x0000, 0xffff, 0xffff, 0xb374 },
    { "black"                       , XCPC_COLOR_BLACK                     , 0x0000, 0x0000, 0x0000, 0x0000 },
    { "bright blue"                 , XCPC_COLOR_BRIGHT_BLUE               , 0x0000, 0x0000, 0xffff, 0x1d2f },
    { "green"                       , XCPC_COLOR_GREEN                     , 0x0000, 0x8000, 0x0000, 0x4b23 },
    { "sky blue"                    , XCPC_COLOR_SKY_BLUE                  , 0x0000, 0x8000, 0xffff, 0x6852 },
    { "magenta"                     , XCPC_COLOR_MAGENTA                   , 0x8000, 0x0000, 0x8000, 0x34dd },
    { "pastel green"                , XCPC_COLOR_PASTEL_GREEN              , 0x8000, 0xffff, 0x8000, 0xcb22 },
    { "lime"                        , XCPC_COLOR_LIME                      , 0x8000, 0xffff, 0x0000, 0xbc8b },
    { "pastel cyan"                 , XCPC_COLOR_PASTEL_CYAN               , 0x8000, 0xffff, 0xffff, 0xd9ba },
    { "red"                         , XCPC_COLOR_RED                       , 0x8000, 0x0000, 0x0000, 0x2645 },
    { "mauve"                       , XCPC_COLOR_MAUVE                     , 0x8000, 0x0000, 0xffff, 0x4374 },
    { "yellow"                      , XCPC_COLOR_YELLOW                    , 0x8000, 0x8000, 0x0000, 0x7168 },
    { "pastel blue"                 , XCPC_COLOR_PASTEL_BLUE               , 0x8000, 0x8000, 0xffff, 0x8e97 } 
};

static void init_loglevel(void)
{
    if(libxcpc.loglevel <= XCPC_LOGLEVEL_UNKNOWN) {
        const char* XCPC_LOGLEVEL = getenv("XCPC_LOGLEVEL");
        if((XCPC_LOGLEVEL != NULL) && (*XCPC_LOGLEVEL != '\0')) {
            (void) xcpc_set_loglevel(atoi(XCPC_LOGLEVEL));
        }
        else {
            (void) xcpc_set_loglevel(XCPC_LOGLEVEL_PRINT);
        }
    }
}

static void fini_loglevel(void)
{
    if(libxcpc.loglevel != XCPC_LOGLEVEL_UNKNOWN) {
        libxcpc.loglevel = XCPC_LOGLEVEL_UNKNOWN;
    }
}

static void init_input_stream(void)
{
    if(libxcpc.input_stream == NULL) {
        libxcpc.input_stream = XCPC_DEFAULT_INPUT_STREAM;
    }
}

static void fini_input_stream(void)
{
    if(libxcpc.input_stream != NULL) {
        libxcpc.input_stream = NULL;
    }
}

static void init_print_stream(void)
{
    if(libxcpc.print_stream == NULL) {
        libxcpc.print_stream = XCPC_DEFAULT_PRINT_STREAM;
    }
}

static void fini_print_stream(void)
{
    if(libxcpc.print_stream != NULL) {
        libxcpc.print_stream = NULL;
    }
}

static void init_error_stream(void)
{
    if(libxcpc.error_stream == NULL) {
        libxcpc.error_stream = XCPC_DEFAULT_ERROR_STREAM;
    }
}

static void fini_error_stream(void)
{
    if(libxcpc.error_stream != NULL) {
        libxcpc.error_stream = NULL;
    }
}

static void init_directories(void)
{
    const char* bindir = getenv("XCPC_BINDIR");
    const char* libdir = getenv("XCPC_LIBDIR");
    const char* datdir = getenv("XCPC_DATDIR");
    const char* docdir = getenv("XCPC_DOCDIR");
    const char* resdir = getenv("XCPC_RESDIR");
    const char* romdir = getenv("XCPC_ROMDIR");
    const char* dskdir = getenv("XCPC_DSKDIR");
    const char* snadir = getenv("XCPC_SNADIR");

    if(libxcpc.bindir == NULL) { libxcpc.bindir = strdup(bindir != NULL ? bindir : XCPC_BINDIR); }
    if(libxcpc.libdir == NULL) { libxcpc.libdir = strdup(libdir != NULL ? libdir : XCPC_LIBDIR); }
    if(libxcpc.datdir == NULL) { libxcpc.datdir = strdup(datdir != NULL ? datdir : XCPC_DATDIR); }
    if(libxcpc.docdir == NULL) { libxcpc.docdir = strdup(docdir != NULL ? docdir : XCPC_DOCDIR); }
    if(libxcpc.resdir == NULL) { libxcpc.resdir = strdup(resdir != NULL ? resdir : XCPC_RESDIR); }
    if(libxcpc.romdir == NULL) { libxcpc.romdir = strdup(romdir != NULL ? romdir : XCPC_ROMDIR); }
    if(libxcpc.dskdir == NULL) { libxcpc.dskdir = strdup(dskdir != NULL ? dskdir : XCPC_DSKDIR); }
    if(libxcpc.snadir == NULL) { libxcpc.snadir = strdup(snadir != NULL ? snadir : XCPC_SNADIR); }
}

static void fini_directories(void)
{
    if(libxcpc.bindir != NULL) { libxcpc.bindir = (free(libxcpc.bindir), NULL); }
    if(libxcpc.libdir != NULL) { libxcpc.libdir = (free(libxcpc.libdir), NULL); }
    if(libxcpc.datdir != NULL) { libxcpc.datdir = (free(libxcpc.datdir), NULL); }
    if(libxcpc.docdir != NULL) { libxcpc.docdir = (free(libxcpc.docdir), NULL); }
    if(libxcpc.resdir != NULL) { libxcpc.resdir = (free(libxcpc.resdir), NULL); }
    if(libxcpc.romdir != NULL) { libxcpc.romdir = (free(libxcpc.romdir), NULL); }
    if(libxcpc.dskdir != NULL) { libxcpc.dskdir = (free(libxcpc.dskdir), NULL); }
    if(libxcpc.snadir != NULL) { libxcpc.snadir = (free(libxcpc.snadir), NULL); }
}

static void init_joystick0(void)
{
    if(libxcpc.joystick0 == NULL) {
        libxcpc.joystick0 = getenv("XCPC_JOYSTICK0");
    }
    if(libxcpc.joystick0 == NULL) {
        libxcpc.joystick0 = XCPC_DEFAULT_JOYSTICK0;
    }
}

static void fini_joystick0(void)
{
    if(libxcpc.joystick0 != NULL) {
        libxcpc.joystick0 = NULL;
    }
}

static void init_joystick1(void)
{
    if(libxcpc.joystick1 == NULL) {
        libxcpc.joystick1 = getenv("XCPC_JOYSTICK1");
    }
    if(libxcpc.joystick1 == NULL) {
        libxcpc.joystick1 = XCPC_DEFAULT_JOYSTICK1;
    }
}

static void fini_joystick1(void)
{
    if(libxcpc.joystick1 != NULL) {
        libxcpc.joystick1 = NULL;
    }
}

void xcpc_begin(void)
{
    if(libxcpc.initialized++ == 0) {
        init_loglevel();
        init_input_stream();
        init_print_stream();
        init_error_stream();
        init_directories();
        init_joystick0();
        init_joystick1();
    }
    /* log */ {
        xcpc_log_trace ( "xcpc %d.%d.%d - begin"
                       , xcpc_major_version()
                       , xcpc_minor_version()
                       , xcpc_micro_version()
                       , "begin" );
    }
}

void xcpc_end(void)
{
    /* log */ {
        xcpc_log_trace ( "xcpc %d.%d.%d - end"
                       , xcpc_major_version()
                       , xcpc_minor_version()
                       , xcpc_micro_version()
                       , "end" );
    }
    if(--libxcpc.initialized == 0) {
        fini_joystick1();
        fini_joystick0();
        fini_directories();
        fini_error_stream();
        fini_print_stream();
        fini_input_stream();
        fini_loglevel();
    }
}

int xcpc_major_version(void)
{
    return libxcpc.major_version;
}

int xcpc_minor_version(void)
{
    return libxcpc.minor_version;
}

int xcpc_micro_version(void)
{
    return libxcpc.micro_version;
}

void xcpc_println(const char* format, ...)
{
    FILE* stream = stdout;

    if(stream != NULL) {
        va_list arguments;
        va_start(arguments, format);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

void xcpc_errorln(const char* format, ...)
{
    FILE* stream = stderr;

    if(stream != NULL) {
        va_list arguments;
        va_start(arguments, format);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

int xcpc_get_loglevel(void)
{
    return libxcpc.loglevel;
}

int xcpc_set_loglevel(const int loglevel)
{
    if(loglevel > XCPC_LOGLEVEL_UNKNOWN) {
        libxcpc.loglevel = loglevel;
    }
    if(libxcpc.loglevel <= XCPC_LOGLEVEL_UNKNOWN) {
        libxcpc.loglevel = XCPC_LOGLEVEL_PRINT;
    }
    return libxcpc.loglevel;
}

void xcpc_log_error(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != NULL) && (libxcpc.loglevel >= XCPC_LOGLEVEL_ERROR)) {
        va_list arguments;
        va_start(arguments, format);
        (void) fputc('E', stream);
        (void) fputc('\t', stream);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_alert(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != NULL) && (libxcpc.loglevel >= XCPC_LOGLEVEL_ALERT)) {
        va_list arguments;
        va_start(arguments, format);
        (void) fputc('W', stream);
        (void) fputc('\t', stream);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_print(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != NULL) && (libxcpc.loglevel >= XCPC_LOGLEVEL_PRINT)) {
        va_list arguments;
        va_start(arguments, format);
        (void) fputc('I', stream);
        (void) fputc('\t', stream);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_trace(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != NULL) && (libxcpc.loglevel >= XCPC_LOGLEVEL_TRACE)) {
        va_list arguments;
        va_start(arguments, format);
        (void) fputc('T', stream);
        (void) fputc('\t', stream);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

void xcpc_log_debug(const char* format, ...)
{
    FILE* stream = libxcpc.error_stream;

    if((stream != NULL) && (libxcpc.loglevel >= XCPC_LOGLEVEL_DEBUG)) {
        va_list arguments;
        va_start(arguments, format);
        (void) fputc('D', stream);
        (void) fputc('\t', stream);
        (void) vfprintf(stream, format, arguments);
        (void) fputc('\n', stream);
        (void) fflush(stream);
        va_end(arguments);
    }
}

void* xcpc_malloc(const char* type, size_t size)
{
    void* pointer = malloc(size);

    if(pointer != NULL) {
        xcpc_log_debug ( "%s has succeeded (type = %s, pointer = %p, size = %lu)"
                       , "malloc()"
                       , type
                       , pointer
                       , size );
    }
    else {
        xcpc_log_error ( "%s has failed (type = %s, pointer = %p, size = %lu)"
                       , "malloc()"
                       , type
                       , pointer
                       , size );
    }
    return pointer;
}

void* xcpc_calloc(const char* type, size_t count, size_t size)
{
    void* pointer = calloc(count, size);

    if(pointer != NULL) {
        xcpc_log_debug ( "%s has succeeded (type = %s, pointer = %p, count = %lu, size = %lu)"
                       , "calloc()"
                       , type
                       , pointer
                       , count
                   , size );
    }
    else {
        xcpc_log_error ( "%s has failed (type = %s, pointer = %p, count = %lu, size = %lu)"
                       , "calloc()"
                       , type
                       , pointer
                       , count
                   , size );
    }
    return pointer;
}

void* xcpc_realloc(const char* type, void* pointer, size_t size)
{
    void* old_pointer = pointer;
    void* new_pointer = realloc(pointer, size);

    if(new_pointer != NULL) {
        xcpc_log_debug ( "%s has succeeded (type = %s, old-pointer = %p, new-pointer = %p, size = %lu)"
                       , "realloc()"
                       , type
                       , old_pointer
                       , new_pointer
                       , size );
    }
    else {
        xcpc_log_error ( "%s has failed (type = %s, old-pointer = %p, new-pointer = %p, size = %lu)"
                       , "realloc()"
                       , type
                       , old_pointer
                       , new_pointer
                       , size );
    }
    return new_pointer;
}

void* xcpc_free(const char* type, void* pointer)
{
    void* old_pointer = pointer;
    void* new_pointer = (free(pointer), NULL);

    if(old_pointer != NULL) {
        xcpc_log_debug ( "%s has succeeded (type = %s, pointer = %p)"
                       , "free()"
                       , type
                       , old_pointer );
    }
    else {
        xcpc_log_error ( "%s has failed (type = %s, pointer = %p)"
                       , "free()"
                       , type
                       , old_pointer );
    }
    return new_pointer;
}

const char* xcpc_get_bindir(void)
{
    return libxcpc.bindir;
}

const char* xcpc_get_libdir(void)
{
    return libxcpc.libdir;
}

const char* xcpc_get_datdir(void)
{
    return libxcpc.datdir;
}

const char* xcpc_get_docdir(void)
{
    return libxcpc.docdir;
}

const char* xcpc_get_resdir(void)
{
    return libxcpc.resdir;
}

const char* xcpc_get_romdir(void)
{
    return libxcpc.romdir;
}

const char* xcpc_get_dskdir(void)
{
    return libxcpc.dskdir;
}

const char* xcpc_get_snadir(void)
{
    return libxcpc.snadir;
}

const char* xcpc_get_joystick0(void)
{
    return libxcpc.joystick0;
}

const char* xcpc_get_joystick1(void)
{
    return libxcpc.joystick1;
}

const char* xcpc_legal_text(void)
{
    static const char text[] = ""
        "Amstrad has kindly given it's permission for it's copyrighted\n"
        "material to be redistributed but Amstrad retains it's copyright.\n\n"
        "Some of the Amstrad CPC ROM code is copyright Locomotive Software.\n\n"
        "ROM and DISK images are protected under the copyrights of their authors,\n"
        "and cannot be distributed in this package. You can download and/or use\n"
        "ROM and DISK images at your own risk and responsibility."
        ;
    return text;
}

const char* xcpc_about_text(void)
{
    static const char text[] = PACKAGE_STRING " - "
        "Amstrad CPC emulator - Copyright (c) 2001-2022 - Olivier Poncet\n\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 2 of the License, or\n"
        "(at your option) any later version.\n\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.  If not, see <http://www.gnu.org/licenses/>"
        ;
    return text;
}

XcpcCompanyName xcpc_company_name(const char* label, XcpcCompanyName value)
{
    if((label != NULL) && (*label != '\0')) {
        unsigned int       index = 0;
        const unsigned int count = countof(xcpc_company_name_table);
        do {
            const XcpcCompanyNameEntry* entry = &xcpc_company_name_table[index];
            if(strcasecmp(entry->label, label) == 0) {
                value = entry->value;
                break;
            }
        } while(++index < count);
    }
    return value;
}

XcpcMachineType xcpc_machine_type(const char* label, XcpcMachineType value)
{
    if((label != NULL) && (*label != '\0')) {
        unsigned int       index = 0;
        const unsigned int count = countof(xcpc_machine_type_table);
        do {
            const XcpcMachineTypeEntry* entry = &xcpc_machine_type_table[index];
            if(strcasecmp(entry->label, label) == 0) {
                value = entry->value;
                break;
            }
        } while(++index < count);
    }
    return value;
}

XcpcMonitorType xcpc_monitor_type(const char* label, XcpcMonitorType value)
{
    if((label != NULL) && (*label != '\0')) {
        unsigned int       index = 0;
        const unsigned int count = countof(xcpc_monitor_type_table);
        do {
            const XcpcMonitorTypeEntry* entry = &xcpc_monitor_type_table[index];
            if(strcasecmp(entry->label, label) == 0) {
                value = entry->value;
                break;
            }
        } while(++index < count);
    }
    return value;
}

XcpcRefreshRate xcpc_refresh_rate(const char* label, XcpcRefreshRate value)
{
    if((label != NULL) && (*label != '\0')) {
        unsigned int       index = 0;
        const unsigned int count = countof(xcpc_refresh_rate_table);
        do {
            const XcpcRefreshRateEntry* entry = &xcpc_refresh_rate_table[index];
            if(strcasecmp(entry->label, label) == 0) {
                value = entry->value;
                break;
            }
        } while(++index < count);
    }
    return value;
}

XcpcKeyboardType xcpc_keyboard_type(const char* label, XcpcKeyboardType value)
{
    if((label != NULL) && (*label != '\0')) {
        unsigned int       index = 0;
        const unsigned int count = countof(xcpc_keyboard_type_table);
        do {
            const XcpcKeyboardTypeEntry* entry = &xcpc_keyboard_type_table[index];
            if(strcasecmp(entry->label, label) == 0) {
                value = entry->value;
                break;
            }
        } while(++index < count);
    }
    return value;
}

XcpcMemorySize xcpc_memory_size(const char* label, XcpcMemorySize value)
{
    if((label != NULL) && (*label != '\0')) {
        unsigned int       index = 0;
        const unsigned int count = countof(xcpc_memory_size_table);
        do {
            const XcpcMemorySizeEntry* entry = &xcpc_memory_size_table[index];
            if(strcasecmp(entry->label, label) == 0) {
                value = entry->value;
                break;
            }
        } while(++index < count);
    }
    return value;
}

XcpcColor xcpc_color_get_values(XcpcMonitorType monitor_type, XcpcColor color, unsigned short* r_out, unsigned short* g_out, unsigned short* b_out)
{
    if((color >= 0) && (color <= 31)) {
        const XcpcColorEntry* color_entry = &xcpc_color_table[color];
        unsigned short r_value = color_entry->red;
        unsigned short g_value = color_entry->green;
        unsigned short b_value = color_entry->blue;
        unsigned short l_value = color_entry->luminance;
        switch(monitor_type) {
            case XCPC_MONITOR_TYPE_COLOR:
            case XCPC_MONITOR_TYPE_CTM640:
            case XCPC_MONITOR_TYPE_CTM644:
            case XCPC_MONITOR_TYPE_CM14:
                r_value = (r_value | 0);
                g_value = (g_value | 0);
                b_value = (b_value | 0);
                break;
            case XCPC_MONITOR_TYPE_GREEN:
            case XCPC_MONITOR_TYPE_GT64:
            case XCPC_MONITOR_TYPE_GT65:
                r_value = (l_value & 0);
                g_value = (l_value | 0);
                b_value = (l_value & 0);
                break;
            case XCPC_MONITOR_TYPE_MONOCHROME:
            case XCPC_MONITOR_TYPE_MM12:
                r_value = (l_value | 0);
                g_value = (l_value | 0);
                b_value = (l_value | 0);
                break;
            default:
                r_value = (r_value | 0);
                g_value = (g_value | 0);
                b_value = (b_value | 0);
                break;
        }
        if(r_out != NULL) {
            *r_out = r_value;
        }
        if(g_out != NULL) {
            *g_out = g_value;
        }
        if(b_out != NULL) {
            *b_out = b_value;
        }
        return color;
    }
    return XCPC_COLOR_UNKNOWN;
}

const char* xcpc_company_name_to_string(XcpcCompanyName value)
{
    unsigned int       index = 0;
    const unsigned int count = countof(xcpc_company_name_table);

    do {
        const XcpcCompanyNameEntry* entry = &xcpc_company_name_table[index];
        if(entry->value == value) {
            return entry->label;
        }
    } while(++index < count);

    return "unknown";
}

const char* xcpc_machine_type_to_string(XcpcMachineType value)
{
    unsigned int       index = 0;
    const unsigned int count = countof(xcpc_machine_type_table);

    do {
        const XcpcMachineTypeEntry* entry = &xcpc_machine_type_table[index];
        if(entry->value == value) {
            return entry->label;
        }
    } while(++index < count);

    return "unknown";
}

const char* xcpc_monitor_type_to_string(XcpcMonitorType value)
{
    unsigned int       index = 0;
    const unsigned int count = countof(xcpc_monitor_type_table);

    do {
        const XcpcMonitorTypeEntry* entry = &xcpc_monitor_type_table[index];
        if(entry->value == value) {
            return entry->label;
        }
    } while(++index < count);

    return "unknown";
}

const char* xcpc_refresh_rate_to_string(XcpcRefreshRate value)
{
    unsigned int       index = 0;
    const unsigned int count = countof(xcpc_refresh_rate_table);

    do {
        const XcpcRefreshRateEntry* entry = &xcpc_refresh_rate_table[index];
        if(entry->value == value) {
            return entry->label;
        }
    } while(++index < count);

    return "unknown";
}

const char* xcpc_keyboard_type_to_string(XcpcKeyboardType value)
{
    unsigned int       index = 0;
    const unsigned int count = countof(xcpc_keyboard_type_table);

    do {
        const XcpcKeyboardTypeEntry* entry = &xcpc_keyboard_type_table[index];
        if(entry->value == value) {
            return entry->label;
        }
    } while(++index < count);

    return "unknown";
}

const char* xcpc_memory_size_to_string(XcpcMemorySize value)
{
    unsigned int       index = 0;
    const unsigned int count = countof(xcpc_memory_size_table);

    do {
        const XcpcMemorySizeEntry* entry = &xcpc_memory_size_table[index];
        if(entry->value == value) {
            return entry->label;
        }
    } while(++index < count);

    return "unknown";
}
