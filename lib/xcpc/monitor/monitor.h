/*
 * monitor.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_MONITOR_H__
#define __XCPC_MONITOR_H__

#include <xcpc/monitor/monitor-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcMonitor* xcpc_monitor_alloc       (void);
extern XcpcMonitor* xcpc_monitor_free        (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_construct   (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_destruct    (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_new         (void);
extern XcpcMonitor* xcpc_monitor_delete      (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_set_iface   (XcpcMonitor* monitor, const XcpcMonitorIface* monitor_iface);
extern XcpcMonitor* xcpc_monitor_reset       (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_realize     (XcpcMonitor* monitor, XcpcMonitorType monitor_type, Display* display, Window window, Bool try_xshm);
extern XcpcMonitor* xcpc_monitor_unrealize   (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_put_image   (XcpcMonitor* monitor);
extern XcpcMonitor* xcpc_monitor_expose      (XcpcMonitor* monitor, XExposeEvent* event);
extern XcpcMonitor* xcpc_monitor_resize      (XcpcMonitor* monitor, XConfigureEvent* event);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MONITOR_H__ */
