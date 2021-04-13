/*
 * monitor-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_MONITOR_IMPL_H__
#define __XCPC_MONITOR_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_MONITOR_IFACE(instance) (&(instance)->iface)
#define XCPC_MONITOR_SETUP(instance) (&(instance)->setup)
#define XCPC_MONITOR_STATE(instance) (&(instance)->state)

typedef struct _XcpcMonitorIface XcpcMonitorIface;
typedef struct _XcpcMonitorSetup XcpcMonitorSetup;
typedef struct _XcpcMonitorState XcpcMonitorState;
typedef struct _XcpcMonitor      XcpcMonitor;
typedef struct _XcpcScanline     XcpcScanline;

struct _XcpcMonitorIface
{
    void* user_data;
};

struct _XcpcMonitorSetup
{
    XcpcMonitorType monitor_type;
    XcpcRefreshRate refresh_rate;
};

struct _XcpcMonitorState
{
    Display* display;
    Screen*  screen;
    Visual*  visual;
    XImage*  image;
    GC       gc;
    Window   window;
    Colormap colormap;
    int      depth;
    int      image_x;
    int      image_y;
    int      total_w;
    int      total_h;
    int      visible_x;
    int      visible_y;
    int      visible_w;
    int      visible_h;
    Bool     try_xshm;
    Bool     has_xshm;
    Bool     use_xshm;
    XColor   palette1[32];
    XColor   palette2[32];
};

struct _XcpcMonitor
{
    XcpcMonitorIface iface;
    XcpcMonitorSetup setup;
    XcpcMonitorState state;
};

struct _XcpcScanline
{
    uint8_t mode;
    struct {
        uint8_t  value;
        uint32_t pixel1;
        uint32_t pixel2;
    } color[17];
    uint8_t data[1024];
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MONITOR_IMPL_H__ */
