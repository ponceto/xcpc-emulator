/*
 * xcpc.h - Copyright (c) 2001, 2006, 2007, 2008, 2009 Olivier Poncet
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
#ifndef __XCPC_H__
#define __XCPC_H__

#include <glib/gi18n.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcResourcesRec {
  Boolean about_flag;
  Boolean usage_flag;
  Boolean edres_flag;
} XcpcResourcesRec, *XcpcResources;

extern int main(int argc, char *argv[]);

#ifndef _XMU_H_
extern void _XEditResCheckMessages(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch);
#endif

#ifdef __cplusplus
}
#endif

#endif
