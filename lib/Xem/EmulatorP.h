/*
 * EmulatorP.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __XemEmulatorP_h__
#define __XemEmulatorP_h__

#include <X11/CoreP.h>
#include <Xem/EmulatorI.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XemEmulatorClassPart
{
    XtPointer extension;
} XemEmulatorClassPart;

typedef struct _XemEmulatorClassRec
{
    CoreClassPart        core_class;
    XemEmulatorClassPart emulator_class;
} XemEmulatorClassRec;

externalref XemEmulatorClassRec xemEmulatorClassRec;

typedef struct _XemEmulatorPart
{
    XemVideo       video;
    XemAudio       audio;
    XemEvents      events;
    XemKeyboard    keyboard;
    XemJoystick    joystick0;
    XemJoystick    joystick1;
    XemBackend     backend;
    XemBackend*    backend_ptr;
    XtCallbackList hotkey_callback;
    XtIntervalId   timer;
} XemEmulatorPart;

typedef struct _XemEmulatorRec
{
    CorePart        core;
    XemEmulatorPart emulator;
} XemEmulatorRec;

#ifdef __cplusplus
}
#endif

#endif /* __XemEmulatorP_h__ */
