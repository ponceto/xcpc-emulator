/*
 * gememulator-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __GEM_EMULATOR_PRIV_H__
#define __GEM_EMULATOR_PRIV_H__

#include <Gem/gememulator.h>

G_BEGIN_DECLS

extern void gem_emulator_machine_construct  (GemEmulator* emulator, GemMachine* machine);
extern void gem_emulator_machine_destruct   (GemEmulator* emulator, GemMachine* machine);

extern void gem_emulator_keyboard_construct (GemEmulator* emulator, GemKeyboard* keyboard);
extern void gem_emulator_keyboard_destruct  (GemEmulator* emulator, GemKeyboard* keyboard);

extern void gem_emulator_joystick_construct (GemEmulator* emulator, GemJoystick* joystick);
extern void gem_emulator_joystick_destruct  (GemEmulator* emulator, GemJoystick* joystick);

G_END_DECLS

#endif /* __GEM_EMULATOR_PRIV_H__ */
