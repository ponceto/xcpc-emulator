/*
 * libxcpc.c - Copyright (c) 2001-2021 - Olivier Poncet
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

void xcpc_begin(void)
{
    if(libxcpc.initialized++ == 0) {
        init_loglevel();
        init_input_stream();
        init_print_stream();
        init_error_stream();
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

extern void* xcpc_malloc(const char* type, size_t size)
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

extern void* xcpc_calloc(const char* type, size_t count, size_t size)
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

extern void* xcpc_realloc(const char* type, void* pointer, size_t size)
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

extern void* xcpc_free(const char* type, void* pointer)
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
        "Amstrad CPC emulator - Copyright (c) 2001-2021 - Olivier Poncet\n\n"
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
    static struct {
        const char* label;
        int         value;
    } list[] = {
        { "default"  , XCPC_COMPANY_NAME_DEFAULT   },
        { "isp"      , XCPC_COMPANY_NAME_ISP       },
        { "triumph"  , XCPC_COMPANY_NAME_TRIUMPH   },
        { "saisho"   , XCPC_COMPANY_NAME_SAISHO    },
        { "solavox"  , XCPC_COMPANY_NAME_SOLAVOX   },
        { "awa"      , XCPC_COMPANY_NAME_AWA       },
        { "schneider", XCPC_COMPANY_NAME_SCHNEIDER },
        { "orion"    , XCPC_COMPANY_NAME_ORION     },
        { "amstrad"  , XCPC_COMPANY_NAME_AMSTRAD   },
    };

    if((label != NULL) && (*label != '\0')) {
        unsigned int index = 0;
        unsigned int count = countof(list);
        for(index = 0; index < count; ++index) {
            if(strcasecmp(list[index].label, label) == 0) {
                value = list[index].value;
                break;
            }
        }
    }
    return value;
}

XcpcMachineType xcpc_machine_type(const char* label, XcpcMachineType value)
{
    static struct {
        const char* label;
        int         value;
    } list[] = {
        { "default", XCPC_MACHINE_TYPE_DEFAULT },
        { "cpc464" , XCPC_MACHINE_TYPE_CPC464  },
        { "cpc664" , XCPC_MACHINE_TYPE_CPC664  },
        { "cpc6128", XCPC_MACHINE_TYPE_CPC6128 },
    };

    if((label != NULL) && (*label != '\0')) {
        unsigned int index = 0;
        unsigned int count = countof(list);
        for(index = 0; index < count; ++index) {
            if(strcasecmp(list[index].label, label) == 0) {
                value = list[index].value;
                break;
            }
        }
    }
    return value;
}

XcpcMonitorType xcpc_monitor_type(const char* label, XcpcMonitorType value)
{
    static struct {
        const char* label;
        int         value;
    } list[] = {
        { "default"   ,  XCPC_MONITOR_TYPE_DEFAULT    },
        { "color"     ,  XCPC_MONITOR_TYPE_COLOR      },
        { "green"     ,  XCPC_MONITOR_TYPE_GREEN      },
        { "monochrome",  XCPC_MONITOR_TYPE_MONOCHROME },
        { "ctm640"    ,  XCPC_MONITOR_TYPE_CTM640     },
        { "ctm644"    ,  XCPC_MONITOR_TYPE_CTM644     },
        { "gt64"      ,  XCPC_MONITOR_TYPE_GT64       },
        { "gt65"      ,  XCPC_MONITOR_TYPE_GT65       },
        { "cm14"      ,  XCPC_MONITOR_TYPE_CM14       },
        { "mm12"      ,  XCPC_MONITOR_TYPE_MM12       },
    };

    if((label != NULL) && (*label != '\0')) {
        unsigned int index = 0;
        unsigned int count = countof(list);
        for(index = 0; index < count; ++index) {
            if(strcasecmp(list[index].label, label) == 0) {
                value = list[index].value;
                break;
            }
        }
    }
    return value;
}

XcpcRefreshRate xcpc_refresh_rate(const char* label, XcpcRefreshRate value)
{
    static struct {
        const char* label;
        int         value;
    } list[] = {
        { "default", XCPC_REFRESH_RATE_DEFAULT },
        { "50hz"   , XCPC_REFRESH_RATE_50HZ    },
        { "60hz"   , XCPC_REFRESH_RATE_60HZ    },
    };

    if((label != NULL) && (*label != '\0')) {
        unsigned int index = 0;
        unsigned int count = countof(list);
        for(index = 0; index < count; ++index) {
            if(strcasecmp(list[index].label, label) == 0) {
                value = list[index].value;
                break;
            }
        }
    }
    return value;
}

XcpcKeyboardType xcpc_keyboard_type(const char* label, XcpcKeyboardType value)
{
    static struct {
        const char* label;
        int         value;
    } list[] = {
        { "default", XCPC_KEYBOARD_TYPE_DEFAULT },
        { "qwerty" , XCPC_KEYBOARD_TYPE_QWERTY  },
        { "azerty" , XCPC_KEYBOARD_TYPE_AZERTY  },
    };

    if((label != NULL) && (*label != '\0')) {
        unsigned int index = 0;
        unsigned int count = countof(list);
        for(index = 0; index < count; ++index) {
            if(strcasecmp(list[index].label, label) == 0) {
                value = list[index].value;
                break;
            }
        }
    }
    return value;
}

static const XcpcColorEntry monitor_color_array[] = {
    { XCPC_COLOR_WHITE                     , "white"                       , 0x8000, 0x8000, 0x8000, 0x8000 },
    { XCPC_COLOR_WHITE_NOT_OFFICIAL        , "white (not official)"        , 0x8000, 0x8000, 0x8000, 0x8000 },
    { XCPC_COLOR_SEA_GREEN                 , "sea green"                   , 0x0000, 0xffff, 0x8000, 0xa4dd },
    { XCPC_COLOR_PASTEL_YELLOW             , "pastel yellow"               , 0xffff, 0xffff, 0x8000, 0xf168 },
    { XCPC_COLOR_BLUE                      , "blue"                        , 0x0000, 0x0000, 0x8000, 0x0e97 },
    { XCPC_COLOR_PURPLE                    , "purple"                      , 0xffff, 0x0000, 0x8000, 0x5b22 },
    { XCPC_COLOR_CYAN                      , "cyan"                        , 0x0000, 0x8000, 0x8000, 0x59ba },
    { XCPC_COLOR_PINK                      , "pink"                        , 0xffff, 0x8000, 0x8000, 0xa645 },
    { XCPC_COLOR_PURPLE_NOT_OFFICIAL       , "purple (not official)"       , 0xffff, 0x0000, 0x8000, 0x5b22 },
    { XCPC_COLOR_PASTEL_YELLOW_NOT_OFFICIAL, "pastel yellow (not official)", 0xffff, 0xffff, 0x8000, 0xf168 },
    { XCPC_COLOR_BRIGHT_YELLOW             , "bright yellow"               , 0xffff, 0xffff, 0x0000, 0xe2d0 },
    { XCPC_COLOR_BRIGHT_WHITE              , "bright white"                , 0xffff, 0xffff, 0xffff, 0xffff },
    { XCPC_COLOR_BRIGHT_RED                , "bright red"                  , 0xffff, 0x0000, 0x0000, 0x4c8b },
    { XCPC_COLOR_BRIGHT_MAGENTA            , "bright magenta"              , 0xffff, 0x0000, 0xffff, 0x69ba },
    { XCPC_COLOR_ORANGE                    , "orange"                      , 0xffff, 0x8000, 0x0000, 0x97ad },
    { XCPC_COLOR_PASTEL_MAGENTA            , "pastel magenta"              , 0xffff, 0x8000, 0xffff, 0xb4dc },
    { XCPC_COLOR_BLUE_NOT_OFFICIAL         , "blue (not official)"         , 0x0000, 0x0000, 0x8000, 0x0e97 },
    { XCPC_COLOR_SEA_GREEN_NOT_OFFICIAL    , "sea green (not official)"    , 0x0000, 0xffff, 0x8000, 0xa4dd },
    { XCPC_COLOR_BRIGHT_GREEN              , "bright green"                , 0x0000, 0xffff, 0x0000, 0x9645 },
    { XCPC_COLOR_BRIGHT_CYAN               , "bright cyan"                 , 0x0000, 0xffff, 0xffff, 0xb374 },
    { XCPC_COLOR_BLACK                     , "black"                       , 0x0000, 0x0000, 0x0000, 0x0000 },
    { XCPC_COLOR_BRIGHT_BLUE               , "bright blue"                 , 0x0000, 0x0000, 0xffff, 0x1d2f },
    { XCPC_COLOR_GREEN                     , "green"                       , 0x0000, 0x8000, 0x0000, 0x4b23 },
    { XCPC_COLOR_SKY_BLUE                  , "sky blue"                    , 0x0000, 0x8000, 0xffff, 0x6852 },
    { XCPC_COLOR_MAGENTA                   , "magenta"                     , 0x8000, 0x0000, 0x8000, 0x34dd },
    { XCPC_COLOR_PASTEL_GREEN              , "pastel green"                , 0x8000, 0xffff, 0x8000, 0xcb22 },
    { XCPC_COLOR_LIME                      , "lime"                        , 0x8000, 0xffff, 0x0000, 0xbc8b },
    { XCPC_COLOR_PASTEL_CYAN               , "pastel cyan"                 , 0x8000, 0xffff, 0xffff, 0xd9ba },
    { XCPC_COLOR_RED                       , "red"                         , 0x8000, 0x0000, 0x0000, 0x2645 },
    { XCPC_COLOR_MAUVE                     , "mauve"                       , 0x8000, 0x0000, 0xffff, 0x4374 },
    { XCPC_COLOR_YELLOW                    , "yellow"                      , 0x8000, 0x8000, 0x0000, 0x7168 },
    { XCPC_COLOR_PASTEL_BLUE               , "pastel blue"                 , 0x8000, 0x8000, 0xffff, 0x8e97 } 
};

XcpcColor xcpc_monitor_color_get(const char* label)
{
    if(label != NULL) {
        unsigned int monitor_index = 0;
        unsigned int monitor_count = countof(monitor_color_array);
        for(monitor_index = 0; monitor_index < monitor_count; ++monitor_index) {
            const XcpcColorEntry* monitor_entry = &monitor_color_array[monitor_index];
            if(strcasecmp(monitor_entry->label, label) == 0) {
                return monitor_entry->color;
            }
        }
    }
    return XCPC_COLOR_UNKNOWN;
}

XcpcColor xcpc_color_get_values(XcpcMonitorType monitor_type, XcpcColor color, unsigned short* r_out, unsigned short* g_out, unsigned short* b_out)
{
    if((color >= 0) && (color <= 31)) {
        const XcpcColorEntry* color_entry = &monitor_color_array[color];
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
