/*
 * keyboard.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_KEYBOARD_H__
#define __XCPC_KEYBOARD_H__

#include <xcpc/keyboard/keyboard-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcKeyboard* xcpc_keyboard_alloc     (void);
extern XcpcKeyboard* xcpc_keyboard_free      (XcpcKeyboard* keyboard);
extern XcpcKeyboard* xcpc_keyboard_construct (XcpcKeyboard* keyboard, const XcpcKeyboardIface* keyboard_iface);
extern XcpcKeyboard* xcpc_keyboard_destruct  (XcpcKeyboard* keyboard);
extern XcpcKeyboard* xcpc_keyboard_new       (const XcpcKeyboardIface* keyboard_iface);
extern XcpcKeyboard* xcpc_keyboard_delete    (XcpcKeyboard* keyboard);
extern XcpcKeyboard* xcpc_keyboard_reset     (XcpcKeyboard* keyboard);
extern XcpcKeyboard* xcpc_keyboard_clock     (XcpcKeyboard* keyboard);
extern XcpcKeyboard* xcpc_keyboard_qwerty    (XcpcKeyboard* keyboard, XKeyEvent* event);
extern XcpcKeyboard* xcpc_keyboard_azerty    (XcpcKeyboard* keyboard, XKeyEvent* event);
extern XcpcKeyboard* xcpc_keyboard_joystick  (XcpcKeyboard* keyboard, XEvent* event);
extern uint8_t       xcpc_keyboard_set_line  (XcpcKeyboard* keyboard, uint8_t line);
extern uint8_t       xcpc_keyboard_get_data  (XcpcKeyboard* keyboard, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_KEYBOARD_H__ */
