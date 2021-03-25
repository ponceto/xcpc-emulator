/*
 * blitter-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_BLITTER_IMPL_H__
#define __XCPC_BLITTER_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_BLITTER_WIDTH  768
#define XCPC_BLITTER_HEIGHT 576

typedef struct _XcpcBlitterIface XcpcBlitterIface;
typedef struct _XcpcBlitterState XcpcBlitterState;
typedef struct _XcpcBlitter      XcpcBlitter;

#ifndef XCPC_BLITTER_IFACE
#define XCPC_BLITTER_IFACE(instance, field) instance->iface.field
#endif

#ifndef XCPC_BLITTER_STATE
#define XCPC_BLITTER_STATE(instance, field) instance->state.field
#endif

struct _XcpcBlitterIface
{
    void* user_data;
};

struct _XcpcBlitterState
{
    Display* display;
    Screen*  screen;
    Visual*  visual;
    XImage*  image;
    GC       gc;
    Window   window;
    Colormap colormap;
    int      depth;
    int      px;
    int      py;
    Bool     try_xshm;
    Bool     has_xshm;
    Bool     use_xshm;
    XColor   palette[32];
};

struct _XcpcBlitter
{
    XcpcBlitterIface iface;
    XcpcBlitterState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_BLITTER_IMPL_H__ */
