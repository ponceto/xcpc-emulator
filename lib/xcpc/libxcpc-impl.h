/*
 * libxcpc-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_LIBXCPC_IMPL_H__
#define __XCPC_LIBXCPC_IMPL_H__

#include <stdint.h>
#include <glib.h>
#include <xcpc/xlib/xlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef stringify_arg
#define stringify_arg(argument) #argument
#endif

#ifndef nameof
#define nameof(argument) stringify_arg(argument)
#endif

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

#ifndef xcpc_new
#define xcpc_new(type) ((type*)(xcpc_malloc(nameof(type), sizeof(type))))
#endif

#ifndef xcpc_delete
#define xcpc_delete(type, pointer) ((type*)(xcpc_free(nameof(type), (pointer))))
#endif

typedef enum _XcpcComputerModel  XcpcComputerModel;
typedef enum _XcpcMonitorModel   XcpcMonitorModel;
typedef enum _XcpcRefreshRate    XcpcRefreshRate;
typedef enum _XcpcKeyboardLayout XcpcKeyboardLayout;
typedef enum _XcpcManufacturer   XcpcManufacturer;
typedef enum _XcpcColor          XcpcColor;

enum _XcpcComputerModel
{
    XCPC_COMPUTER_MODEL_UNKNOWN   = -1,
    XCPC_COMPUTER_MODEL_464       =  0,
    XCPC_COMPUTER_MODEL_664       =  1,
    XCPC_COMPUTER_MODEL_6128      =  2,
};

enum _XcpcMonitorModel
{
    XCPC_MONITOR_MODEL_UNKNOWN    = -1,
    XCPC_MONITOR_MODEL_COLOR      =  0,
    XCPC_MONITOR_MODEL_GREEN      =  1,
    XCPC_MONITOR_MODEL_MONOCHROME =  2,
    XCPC_MONITOR_MODEL_CTM640     =  3,
    XCPC_MONITOR_MODEL_CTM644     =  4,
    XCPC_MONITOR_MODEL_GT64       =  5,
    XCPC_MONITOR_MODEL_GT65       =  6,
    XCPC_MONITOR_MODEL_CM14       =  7,
    XCPC_MONITOR_MODEL_MM12       =  8,
};

enum _XcpcRefreshRate
{
    XCPC_REFRESH_RATE_UNKNOWN     = -1,
    XCPC_REFRESH_RATE_60HZ        =  0,
    XCPC_REFRESH_RATE_50HZ        =  1,
};

enum _XcpcKeyboardLayout
{
    XCPC_KEYBOARD_LAYOUT_UNKNOWN  = -1,
    XCPC_KEYBOARD_LAYOUT_QWERTY   =  0,
    XCPC_KEYBOARD_LAYOUT_AZERTY   =  1,
};

enum _XcpcManufacturer
{
    XCPC_MANUFACTURER_UNKNOWN     = -1,
    XCPC_MANUFACTURER_ISP         =  0,
    XCPC_MANUFACTURER_TRIUMPH     =  1,
    XCPC_MANUFACTURER_SAISHO      =  2,
    XCPC_MANUFACTURER_SOLAVOX     =  3,
    XCPC_MANUFACTURER_AWA         =  4,
    XCPC_MANUFACTURER_SCHNEIDER   =  5,
    XCPC_MANUFACTURER_ORION       =  6,
    XCPC_MANUFACTURER_AMSTRAD     =  7,
};

enum _XcpcColor
{
    XCPC_COLOR_UNKNOWN                    = -1,
    XCPC_COLOR_WHITE                      =  0,
    XCPC_COLOR_WHITE_NOT_OFFICIAL         =  1,
    XCPC_COLOR_SEA_GREEN                  =  2,
    XCPC_COLOR_PASTEL_YELLOW              =  3,
    XCPC_COLOR_BLUE                       =  4,
    XCPC_COLOR_PURPLE                     =  5,
    XCPC_COLOR_CYAN                       =  6,
    XCPC_COLOR_PINK                       =  7,
    XCPC_COLOR_PURPLE_NOT_OFFICIAL        =  8,
    XCPC_COLOR_PASTEL_YELLOW_NOT_OFFICIAL =  9,
    XCPC_COLOR_BRIGHT_YELLOW              = 10,
    XCPC_COLOR_BRIGHT_WHITE               = 11,
    XCPC_COLOR_BRIGHT_RED                 = 12,
    XCPC_COLOR_BRIGHT_MAGENTA             = 13,
    XCPC_COLOR_ORANGE                     = 14,
    XCPC_COLOR_PASTEL_MAGENTA             = 15,
    XCPC_COLOR_BLUE_NOT_OFFICIAL          = 16,
    XCPC_COLOR_SEA_GREEN_NOT_OFFICIAL     = 17,
    XCPC_COLOR_BRIGHT_GREEN               = 18,
    XCPC_COLOR_BRIGHT_CYAN                = 19,
    XCPC_COLOR_BLACK                      = 20,
    XCPC_COLOR_BRIGHT_BLUE                = 21,
    XCPC_COLOR_GREEN                      = 22,
    XCPC_COLOR_SKY_BLUE                   = 23,
    XCPC_COLOR_MAGENTA                    = 24,
    XCPC_COLOR_PASTEL_GREEN               = 25,
    XCPC_COLOR_LIME                       = 26,
    XCPC_COLOR_PASTEL_CYAN                = 27,
    XCPC_COLOR_RED                        = 28,
    XCPC_COLOR_MAUVE                      = 29,
    XCPC_COLOR_YELLOW                     = 30,
    XCPC_COLOR_PASTEL_BLUE                = 31,
};

extern const char* XCPC_LOG_DOMAIN;

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_LIBXCPC_IMPL_H__ */
