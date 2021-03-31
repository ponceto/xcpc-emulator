/*
 * libxcpc.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_LIBXCPC_H__
#define __XCPC_LIBXCPC_H__

#include <xcpc/libxcpc-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void  xcpc_begin         (void);
extern int   xcpc_main          (int* argc, char*** argv);
extern void  xcpc_end           (void);

extern int   xcpc_major_version (void);
extern int   xcpc_minor_version (void);
extern int   xcpc_micro_version (void);

extern void  xcpc_println       (const char* format, ...);
extern void  xcpc_errorln       (const char* format, ...);

extern int   xcpc_get_loglevel  (void);
extern int   xcpc_set_loglevel  (const int loglevel);
extern void  xcpc_log_error     (const char* format, ...);
extern void  xcpc_log_alert     (const char* format, ...);
extern void  xcpc_log_print     (const char* format, ...);
extern void  xcpc_log_trace     (const char* format, ...);
extern void  xcpc_log_debug     (const char* format, ...);

extern void* xcpc_malloc        (const char* type, size_t size);
extern void* xcpc_calloc        (const char* type, size_t count, size_t size);
extern void* xcpc_realloc       (const char* type, void* pointer, size_t size);
extern void* xcpc_free          (const char* type, void* pointer);

extern XcpcCompanyName  xcpc_company_name     (const char* label, XcpcCompanyName  value);
extern XcpcMachineType  xcpc_machine_type     (const char* label, XcpcMachineType  value);
extern XcpcMonitorType  xcpc_monitor_type     (const char* label, XcpcMonitorType  value);
extern XcpcRefreshRate  xcpc_refresh_rate     (const char* label, XcpcRefreshRate  value);
extern XcpcKeyboardType xcpc_keyboard_type    (const char* label, XcpcKeyboardType value);
extern XcpcColor        xcpc_color            (const char* label);
extern XcpcColor        xcpc_color_get_values (XcpcMonitorType monitor_type, XcpcColor color, unsigned short* r, unsigned short* g, unsigned short* b);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_LIBXCPC_H__ */
