/*
 * StringDefs.h - Copyright (c) 2001, 2006, 2007, 2008, 2009, 2010, 2011 Olivier Poncet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef _XemStringDefs_h
#define _XemStringDefs_h

#include <X11/StringDefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XtNwmCloseCallback
#define XtNwmCloseCallback "wmCloseCallback"
#endif
#ifndef XtNdropURICallback
#define XtNdropURICallback "dropURICallback"
#endif
#ifndef XtNemuStartHandler
#define XtNemuStartHandler "emuStartHandler"
#endif
#ifndef XtNemuClockHandler
#define XtNemuClockHandler "emuClockHandler"
#endif
#ifndef XtNemuCloseHandler
#define XtNemuCloseHandler "emuCloseHandler"
#endif
#ifndef XtNemuInputHandler
#define XtNemuInputHandler "emuInputHandler"
#endif
#ifndef XtNemuPaintHandler
#define XtNemuPaintHandler "emuPaintHandler"
#endif

#ifdef __cplusplus
}
#endif

#endif
