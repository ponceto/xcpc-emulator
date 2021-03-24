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

static struct {
    int   initialized;
    int   loglevel;
    FILE* print_stream;
    FILE* error_stream;
} libxcpc = {
    0,                     /* initialized  */
    XCPC_LOGLEVEL_UNKNOWN, /* loglevel     */
    NULL,                  /* print_stream */
    NULL,                  /* error_stream */
};

static void init_loglevel(void)
{
    const char* XCPC_LOGLEVEL = getenv("XCPC_LOGLEVEL");

    if(libxcpc.loglevel <= XCPC_LOGLEVEL_UNKNOWN) {
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

static void init_print_stream(void)
{
    if(libxcpc.print_stream == NULL) {
        libxcpc.print_stream = stdout;
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
        libxcpc.error_stream = stderr;
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
        /* init */ {
            init_loglevel();
            init_print_stream();
            init_error_stream();
        }
        /* log */ {
            xcpc_trace ( "xcpc v%d.%d.%d : %s"
                       , PACKAGE_MAJOR_VERSION
                       , PACKAGE_MINOR_VERSION
                       , PACKAGE_MICRO_VERSION 
                       , "begin" );
        }
    }
}

void xcpc_end(void)
{
    if(--libxcpc.initialized == 0) {
        /* log */ {
            xcpc_trace ( "xcpc v%d.%d.%d : %s"
                       , PACKAGE_MAJOR_VERSION
                       , PACKAGE_MINOR_VERSION
                       , PACKAGE_MICRO_VERSION
                       , "end" );
        }
        /* fini */ {
            fini_error_stream();
            fini_print_stream();
            fini_loglevel();
        }
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

void xcpc_error(const char* format, ...)
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

void xcpc_print(const char* format, ...)
{
    FILE* stream = libxcpc.print_stream;

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

void xcpc_trace(const char* format, ...)
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

void xcpc_debug(const char* format, ...)
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
        xcpc_debug ( "%s has succeeded (type = %s, pointer = %p, size = %lu)"
                   , "malloc()"
                   , type
                   , pointer
                   , size );
    }
    else {
        xcpc_debug ( "%s has failed (type = %s, pointer = %p, size = %lu)"
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
        xcpc_debug ( "%s has succeeded (type = %s, pointer = %p, count = %lu, size = %lu)"
                   , "calloc()"
                   , type
                   , pointer
                   , count
                   , size );
    }
    else {
        xcpc_debug ( "%s has failed (type = %s, pointer = %p, count = %lu, size = %lu)"
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
        xcpc_debug ( "%s has succeeded (type = %s, old-pointer = %p, new-pointer = %p, size = %lu)"
                   , "realloc()"
                   , type
                   , old_pointer
                   , new_pointer
                   , size );
    }
    else {
        xcpc_debug ( "%s has failed (type = %s, old-pointer = %p, new-pointer = %p, size = %lu)"
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
        xcpc_debug ( "%s has succeeded (type = %s, pointer = %p)"
                   , "free()"
                   , type
                   , old_pointer );
    }
    else {
        xcpc_debug ( "%s has failed (type = %s, pointer = %p)"
                   , "free()"
                   , type
                   , old_pointer );
    }
    return new_pointer;
}

XcpcComputerModel xcpc_computer_model(const char* label, XcpcComputerModel value)
{
    struct {
        const char* label;
        int         value;
    } list[] = {
        { "cpc464" , XCPC_COMPUTER_MODEL_464  },
        { "cpc664" , XCPC_COMPUTER_MODEL_664  },
        { "cpc6128", XCPC_COMPUTER_MODEL_6128 },
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
    /* log */ {
        xcpc_debug("xcpc_computer_model() : %d", value);
    }
    return value;
}

XcpcMonitorModel xcpc_monitor_model(const char* label, XcpcMonitorModel value)
{
    struct {
        const char* label;
        int         value;
    } list[] = {
        { "color"     ,  XCPC_MONITOR_MODEL_COLOR      },
        { "green"     ,  XCPC_MONITOR_MODEL_GREEN      },
        { "monochrome",  XCPC_MONITOR_MODEL_MONOCHROME },
        { "ctm640"    ,  XCPC_MONITOR_MODEL_CTM640     },
        { "ctm644"    ,  XCPC_MONITOR_MODEL_CTM644     },
        { "gt64"      ,  XCPC_MONITOR_MODEL_GT64       },
        { "gt65"      ,  XCPC_MONITOR_MODEL_GT65       },
        { "cm14"      ,  XCPC_MONITOR_MODEL_CM14       },
        { "mm12"      ,  XCPC_MONITOR_MODEL_MM12       },
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
    /* log */ {
        xcpc_debug("xcpc_monitor_model() : %d", value);
    }
    return value;
}

XcpcRefreshRate xcpc_refresh_rate(const char* label, XcpcRefreshRate value)
{
    struct {
        const char* label;
        int         value;
    } list[] = {
        { "60Hz", XCPC_REFRESH_RATE_60HZ },
        { "50Hz", XCPC_REFRESH_RATE_50HZ },
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
    /* log */ {
        xcpc_debug("xcpc_refresh_rate() : %d", value);
    }
    return value;
}

XcpcKeyboardLayout xcpc_keyboard_layout(const char* label, XcpcKeyboardLayout value)
{
    struct {
        const char* label;
        int         value;
    } list[] = {
        { "qwerty", XCPC_KEYBOARD_LAYOUT_QWERTY },
        { "azerty", XCPC_KEYBOARD_LAYOUT_AZERTY },
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
    /* log */ {
        xcpc_debug("xcpc_keyboard_layout() : %d", value);
    }
    return value;
}

XcpcManufacturer xcpc_manufacturer(const char* label, XcpcManufacturer value)
{
    struct {
        const char* label;
        int         value;
    } list[] = {
        { "Isp"      , XCPC_MANUFACTURER_ISP       },
        { "Triumph"  , XCPC_MANUFACTURER_TRIUMPH   },
        { "Saisho"   , XCPC_MANUFACTURER_SAISHO    },
        { "Solavox"  , XCPC_MANUFACTURER_SOLAVOX   },
        { "Awa"      , XCPC_MANUFACTURER_AWA       },
        { "Schneider", XCPC_MANUFACTURER_SCHNEIDER },
        { "Orion"    , XCPC_MANUFACTURER_ORION     },
        { "Amstrad"  , XCPC_MANUFACTURER_AMSTRAD   },
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
    /* log */ {
        xcpc_debug("xcpc_manufacturer() : %d", value);
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

XcpcColor xcpc_color_get_values(XcpcMonitorModel monitor_model, XcpcColor color, unsigned short* r_out, unsigned short* g_out, unsigned short* b_out)
{
    if((color >= 0) && (color <= 31)) {
        const XcpcColorEntry* color_entry = &monitor_color_array[color];
        unsigned short r_value = color_entry->red;
        unsigned short g_value = color_entry->green;
        unsigned short b_value = color_entry->blue;
        unsigned short l_value = color_entry->luminance;
        switch(monitor_model) {
            case XCPC_MONITOR_MODEL_COLOR:
            case XCPC_MONITOR_MODEL_CTM640:
            case XCPC_MONITOR_MODEL_CTM644:
            case XCPC_MONITOR_MODEL_CM14:
                r_value = (r_value | 0);
                g_value = (g_value | 0);
                b_value = (b_value | 0);
                break;
            case XCPC_MONITOR_MODEL_GREEN:
            case XCPC_MONITOR_MODEL_GT64:
            case XCPC_MONITOR_MODEL_GT65:
                r_value = (l_value & 0);
                g_value = (l_value | 0);
                b_value = (l_value & 0);
                break;
            case XCPC_MONITOR_MODEL_MONOCHROME:
            case XCPC_MONITOR_MODEL_MM12:
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
