/*
 * monitor-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_MONITOR_PRIV_H__
#define __XCPC_MONITOR_PRIV_H__

#include <xcpc/monitor/monitor.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcMonitorArea XcpcMonitorArea;

struct _XcpcMonitorArea
{
    int x1;
    int y1;
    int x2;
    int y2;
};

#define XCPC_MONITOR_50HZ_TOTAL_WIDTH    1024
#define XCPC_MONITOR_50HZ_TOTAL_HEIGHT    625
#define XCPC_MONITOR_50HZ_VISIBLE_X         0
#define XCPC_MONITOR_50HZ_VISIBLE_Y        24
#define XCPC_MONITOR_50HZ_VISIBLE_WIDTH   768
#define XCPC_MONITOR_50HZ_VISIBLE_HEIGHT  576

#define XCPC_MONITOR_60HZ_TOTAL_WIDTH    1024
#define XCPC_MONITOR_60HZ_TOTAL_HEIGHT    525
#define XCPC_MONITOR_60HZ_VISIBLE_X         0
#define XCPC_MONITOR_60HZ_VISIBLE_Y         8
#define XCPC_MONITOR_60HZ_VISIBLE_WIDTH   768
#define XCPC_MONITOR_60HZ_VISIBLE_HEIGHT  480

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MONITOR_PRIV_H__ */
