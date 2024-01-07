/*
 * StringDefs.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __XemStringDefs_h__
#define __XemStringDefs_h__

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

#ifndef XtNhotkeyCallback
#define XtNhotkeyCallback "hotkeyCallback"
#endif

#ifndef XtNbackend
#define XtNbackend "backend"
#endif

#ifndef XtNjoystick0
#define XtNjoystick0 "joystick0"
#endif

#ifndef XtNjoystick1
#define XtNjoystick1 "joystick1"
#endif

#ifndef XtCPointer
#define XtCPointer "Pointer"
#endif

#ifndef XtCFunction
#define XtCFunction "Function"
#endif

#ifndef XtCBackend
#define XtCBackend "Backend"
#endif

#ifndef XtCJoystick
#define XtCJoystick "Joystick"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XemStringDefs_h__ */
