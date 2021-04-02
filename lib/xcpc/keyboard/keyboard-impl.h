/*
 * keyboard-impl.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_KEYBOARD_IMPL_H__
#define __XCPC_KEYBOARD_IMPL_H__

#include <xcpc/libxcpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XCPC_KEYBOARD_IFACE(instance) (&(instance)->iface)
#define XCPC_KEYBOARD_SETUP(instance) (&(instance)->setup)
#define XCPC_KEYBOARD_STATE(instance) (&(instance)->state)

typedef struct _XcpcKeyboardIface XcpcKeyboardIface;
typedef struct _XcpcKeyboardSetup XcpcKeyboardSetup;
typedef struct _XcpcKeyboardState XcpcKeyboardState;
typedef struct _XcpcKeyboard      XcpcKeyboard;

struct _XcpcKeyboardIface
{
    void* user_data;
};

struct _XcpcKeyboardSetup
{
    int reserved;
};

struct _XcpcKeyboardState
{
    uint8_t mode;
    uint8_t line;
    uint8_t keys[16];
};

struct _XcpcKeyboard
{
    XcpcKeyboardIface iface;
    XcpcKeyboardSetup setup;
    XcpcKeyboardState state;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_KEYBOARD_IMPL_H__ */
