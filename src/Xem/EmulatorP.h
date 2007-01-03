/*
 * EmulatorP.h - Copyright (c) 2001, 2006, 2007 Olivier Poncet
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
#ifndef _XemEmulatorP_h
#define _XemEmulatorP_h

#include <X11/CoreP.h>
#include <Xem/Emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XemEmulatorClassPart {
  XtPointer extension;
} XemEmulatorClassPart;

typedef struct _XemEmulatorClassRec {
  CoreClassPart        core_class;
  XemEmulatorClassPart emulator_class;
} XemEmulatorClassRec;

externalref XemEmulatorClassRec xemEmulatorClassRec;

typedef struct _XemEmulatorPart {
  void (*start_handler)(Widget widget, XtPointer data);
  void (*clock_handler)(Widget widget, XtPointer data);
  void (*close_handler)(Widget widget, XtPointer data);
  void (*input_handler)(Widget widget, XEvent *xevent);
  void (*paint_handler)(Widget widget, XEvent *xevent);
  XtIntervalId timer;
  unsigned long delay;
} XemEmulatorPart;

typedef struct _XemEmulatorRec {
  CorePart        core;
  XemEmulatorPart emulator;
} XemEmulatorRec;

#ifdef __cplusplus
}
#endif

#endif
