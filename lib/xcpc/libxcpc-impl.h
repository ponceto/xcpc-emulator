/*
 * libxcpc-impl.h - Copyright (c) 2001-2024 - Olivier Poncet
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

typedef union _XcpcRegister     XcpcRegister;
typedef enum  _XcpcLogLevel     XcpcLogLevel;
typedef enum  _XcpcCompanyName  XcpcCompanyName;
typedef enum  _XcpcMachineType  XcpcMachineType;
typedef enum  _XcpcMonitorType  XcpcMonitorType;
typedef enum  _XcpcRefreshRate  XcpcRefreshRate;
typedef enum  _XcpcKeyboardType XcpcKeyboardType;
typedef enum  _XcpcMemorySize   XcpcMemorySize;
typedef enum  _XcpcColor        XcpcColor;

enum _XcpcLogLevel
{
    XCPC_LOGLEVEL_UNKNOWN = -1,
    XCPC_LOGLEVEL_QUIET   =  0,
    XCPC_LOGLEVEL_ERROR   =  1,
    XCPC_LOGLEVEL_ALERT   =  2,
    XCPC_LOGLEVEL_PRINT   =  3,
    XCPC_LOGLEVEL_TRACE   =  4,
    XCPC_LOGLEVEL_DEBUG   =  5,
};

enum _XcpcCompanyName
{
    XCPC_COMPANY_NAME_UNKNOWN   = -1,
    XCPC_COMPANY_NAME_DEFAULT   =  0,
    XCPC_COMPANY_NAME_ISP       =  1,
    XCPC_COMPANY_NAME_TRIUMPH   =  2,
    XCPC_COMPANY_NAME_SAISHO    =  3,
    XCPC_COMPANY_NAME_SOLAVOX   =  4,
    XCPC_COMPANY_NAME_AWA       =  5,
    XCPC_COMPANY_NAME_SCHNEIDER =  6,
    XCPC_COMPANY_NAME_ORION     =  7,
    XCPC_COMPANY_NAME_AMSTRAD   =  8,
};

enum _XcpcMachineType
{
    XCPC_MACHINE_TYPE_UNKNOWN = -1,
    XCPC_MACHINE_TYPE_DEFAULT =  0,
    XCPC_MACHINE_TYPE_CPC464  =  1,
    XCPC_MACHINE_TYPE_CPC664  =  2,
    XCPC_MACHINE_TYPE_CPC6128 =  3,
};

enum _XcpcMonitorType
{
    XCPC_MONITOR_TYPE_UNKNOWN    = -1,
    XCPC_MONITOR_TYPE_DEFAULT    =  0,
    XCPC_MONITOR_TYPE_COLOR      =  1,
    XCPC_MONITOR_TYPE_GREEN      =  2,
    XCPC_MONITOR_TYPE_MONOCHROME =  3,
    XCPC_MONITOR_TYPE_CTM640     =  4,
    XCPC_MONITOR_TYPE_CTM644     =  5,
    XCPC_MONITOR_TYPE_GT64       =  6,
    XCPC_MONITOR_TYPE_GT65       =  7,
    XCPC_MONITOR_TYPE_CM14       =  8,
    XCPC_MONITOR_TYPE_MM12       =  9,
};

enum _XcpcRefreshRate
{
    XCPC_REFRESH_RATE_UNKNOWN = -1,
    XCPC_REFRESH_RATE_DEFAULT =  0,
    XCPC_REFRESH_RATE_50HZ    =  1,
    XCPC_REFRESH_RATE_60HZ    =  2,
};

enum _XcpcKeyboardType
{
    XCPC_KEYBOARD_TYPE_UNKNOWN = -1,
    XCPC_KEYBOARD_TYPE_DEFAULT =  0,
    XCPC_KEYBOARD_TYPE_QWERTY  =  1,
    XCPC_KEYBOARD_TYPE_AZERTY  =  2,
};

enum _XcpcMemorySize
{
    XCPC_MEMORY_SIZE_UNKNOWN = -1,
    XCPC_MEMORY_SIZE_DEFAULT = (  0 * 1024),
    XCPC_MEMORY_SIZE_64K     = ( 64 * 1024),
    XCPC_MEMORY_SIZE_128K    = (128 * 1024),
    XCPC_MEMORY_SIZE_192K    = (192 * 1024),
    XCPC_MEMORY_SIZE_256K    = (256 * 1024),
    XCPC_MEMORY_SIZE_320K    = (320 * 1024),
    XCPC_MEMORY_SIZE_384K    = (384 * 1024),
    XCPC_MEMORY_SIZE_448K    = (448 * 1024),
    XCPC_MEMORY_SIZE_512K    = (512 * 1024),
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

union _XcpcRegister
{
    struct /* long */
    {
        uint32_t r;
    } l;
#if defined(MSB_FIRST) && !defined(LSB_FIRST)
    struct /* word */
    {
        uint16_t h;
        uint16_t l;
    } w;
    struct /* byte */
    {
        uint8_t  x;
        uint8_t  y;
        uint8_t  h;
        uint8_t  l;
    } b;
#endif
#if !defined(MSB_FIRST) && defined(LSB_FIRST)
    struct /* word */
    {
        uint16_t l;
        uint16_t h;
    } w;
    struct /* byte */
    {
        uint8_t  l;
        uint8_t  h;
        uint8_t  y;
        uint8_t  x;
    } b;
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_LIBXCPC_IMPL_H__ */
