/*
 * DlgShellP.h - Copyright (c) 2001, 2006, 2007, 2008 Olivier Poncet
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
#ifndef _XemDlgShellP_h
#define _XemDlgShellP_h

#include <X11/ShellP.h>
#include <Xem/DlgShell.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XemDlgShellClassPart {
  XtPointer extension;
} XemDlgShellClassPart;

typedef struct _XemDlgShellClassRec {
  CoreClassPart           core_class;
  CompositeClassPart      composite_class;
  ShellClassPart          shell_class;
  WMShellClassPart        wm_shell_class;
  VendorShellClassPart    vendor_shell_class;
  TransientShellClassPart transient_shell_class;
  XemDlgShellClassPart    dlg_shell_class;
} XemDlgShellClassRec;

externalref XemDlgShellClassRec xemDlgShellClassRec;

typedef struct _XemDlgShellPart {
  Atom WM_PROTOCOLS;
  Atom WM_DELETE_WINDOW;
  XtCallbackList wm_close_callback;
  XtCallbackList drop_uri_callback;
} XemDlgShellPart;

typedef struct _XemDlgShellRec {
  CorePart           core;
  CompositePart      composite;
  ShellPart          shell;
  WMShellPart        wm_shell;
  VendorShellPart    vendor_shell;
  TransientShellPart transient_shell;	
  XemDlgShellPart    dlg_shell;
} XemDlgShellRec;

#ifdef __cplusplus
}
#endif

#endif
