/*
 * xcpc-gtk3.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_GTK3_H__
#define __XCPC_GTK3_H__

#include "xcpc-main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcApplicationRec XcpcApplication;

extern XcpcApplication* xcpc_application_new    (int* argc, char*** argv);
extern XcpcApplication* xcpc_application_delete (XcpcApplication* self);
extern XcpcApplication* xcpc_application_run    (XcpcApplication* self);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_GTK3_H__ */
