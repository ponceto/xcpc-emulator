/*
 * xlib-priv.h - Copyright (c) 2001-2020 - Olivier Poncet
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
#ifndef __XCPC_XLIB_PRIV_H__
#define __XCPC_XLIB_PRIV_H__

#include <xcpc/xlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_XSHM
#ifndef XSHM_SEGMENT_INFO
#define XSHM_SEGMENT_INFO(pointer) ((XShmSegmentInfo*)(pointer))
#endif
#endif

#ifndef Xmalloc
#define Xmalloc(size) xcpc_malloc("byte", (size))
#endif

#ifndef Xrealloc
#define Xrealloc(pointer, size) xcpc_realloc("byte", (pointer), (size))
#endif

#ifndef Xcalloc
#define Xcalloc(count, size) xcpc_calloc("byte", (count), (size))
#endif

#ifndef Xfree
#define Xfree(pointer) xcpc_free("byte", (pointer))
#endif

#ifndef Xnew
#define Xnew(type) ((type*)(xcpc_malloc(nameof(type), sizeof(type))))
#endif

#ifndef Xdelete
#define Xdelete(type, pointer) ((type*)(xcpc_free(nameof(type), (pointer))))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_XLIB_PRIV_H__ */
