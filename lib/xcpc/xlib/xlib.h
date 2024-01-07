/*
 * xlib.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XCPC_XLIB_H__
#define __XCPC_XLIB_H__

#include <xcpc/xlib/xlib-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XImage* XcpcCreateImage       ( Display*     display
                                     , Visual*      visual
                                     , unsigned int depth
                                     , int          format
                                     , unsigned int width
                                     , unsigned int height );

extern XImage* XcpcCreateShmImage    ( Display*     display
                                     , Visual*      visual
                                     , unsigned int depth
                                     , int          format
                                     , unsigned int width
                                     , unsigned int height );

extern Bool    XcpcAttachShmImage    ( Display* display
                                     , XImage* image );

extern Bool    XcpcDetachShmImage    ( Display* display
                                     , XImage* image );

extern Bool    XcpcQueryShmExtension ( Display* display );

extern int     XcpcPutImage          ( Display*     display
                                     , Drawable     drawable
                                     , GC           gc
                                     , XImage*      image
                                     , int          src_x
                                     , int          src_y
                                     , int          dst_x
                                     , int          dst_y
                                     , unsigned int width
                                     , unsigned int height
                                     , Bool         xshm_image
                                     , Bool         send_event );

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_XLIB_H__ */
