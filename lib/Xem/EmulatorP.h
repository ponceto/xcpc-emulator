/*
 * EmulatorP.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#include <Xem/Emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long (*XemEmulatorCreateProc)  (Widget widget, XtPointer data, XEvent* event);
typedef unsigned long (*XemEmulatorDestroyProc) (Widget widget, XtPointer data, XEvent* event);
typedef unsigned long (*XemEmulatorRealizeProc) (Widget widget, XtPointer data, XEvent* event);
typedef unsigned long (*XemEmulatorResizeProc)  (Widget widget, XtPointer data, XEvent* event);
typedef unsigned long (*XemEmulatorRedrawProc)  (Widget widget, XtPointer data, XEvent* event);
typedef unsigned long (*XemEmulatorInputProc)   (Widget widget, XtPointer data, XEvent* event);
typedef unsigned long (*XemEmulatorTimerProc)   (Widget widget, XtPointer data, XEvent* event);

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
    XtPointer              context;
    XemEmulatorCreateProc  create_proc;
    XemEmulatorDestroyProc destroy_proc;
    XemEmulatorRealizeProc realize_proc;
    XemEmulatorResizeProc  resize_proc;
    XemEmulatorRedrawProc  redraw_proc;
    XemEmulatorInputProc   input_proc;
    XemEmulatorTimerProc   timer_proc;
    XtIntervalId           timer;
    unsigned long          delay;
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
