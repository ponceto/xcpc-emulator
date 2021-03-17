/*
 * joystick.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_JOYSTICK_H__
#define __XCPC_JOYSTICK_H__

#include <xcpc/joystick/joystick-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcJoystick* xcpc_joystick_alloc     (void);
extern XcpcJoystick* xcpc_joystick_free      (XcpcJoystick* joystick);
extern XcpcJoystick* xcpc_joystick_construct (XcpcJoystick* joystick);
extern XcpcJoystick* xcpc_joystick_destruct  (XcpcJoystick* joystick);
extern XcpcJoystick* xcpc_joystick_new       (void);
extern XcpcJoystick* xcpc_joystick_delete    (XcpcJoystick* joystick);
extern XcpcJoystick* xcpc_joystick_reset     (XcpcJoystick* joystick);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_JOYSTICK_H__ */
