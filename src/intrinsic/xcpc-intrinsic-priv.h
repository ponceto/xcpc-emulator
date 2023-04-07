/*
 * xcpc-intrinsic-priv.h - Copyright (c) 2001-2023 - Olivier Poncet
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
#ifndef __XCPC_INTRINSIC_PRIV_H__
#define __XCPC_INTRINSIC_PRIV_H__

#include <X11/Intrinsic.h>
#include <Xem/StringDefs.h>
#include <Xem/AppShell.h>
#include <Xem/DlgShell.h>
#include <Xem/Emulator.h>
#include <xcpc/options/options.h>
#include <xcpc/machine/machine.h>
#include "xcpc-intrinsic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcResourcesRec   XcpcResourcesRec;
typedef struct _XcpcLayoutRec      XcpcLayoutRec;
typedef struct _XcpcApplicationRec XcpcApplicationRec;

struct _XcpcResourcesRec
{
    String  app_name;
    String  app_class;
    Boolean quiet_flag;
    Boolean trace_flag;
    Boolean debug_flag;
};

struct _XcpcLayoutRec
{
    Widget toplevel;
    Widget emulator;
};

struct _XcpcApplicationRec
{
    int*             argc;
    char***          argv;
    XcpcOptions*     options;
    XcpcMachine*     machine;
    XtAppContext     appcontext;
    XtIntervalId     intervalId;
    XcpcResourcesRec resources;
    XcpcLayoutRec    layout;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_INTRINSIC_PRIV_H__ */
