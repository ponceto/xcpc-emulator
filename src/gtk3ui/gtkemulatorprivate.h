/*
 * gtkemulatorprivate.h - Copyright (c) 2001-2024 - Olivier Poncet
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
#ifndef __GTK_EMULATOR_PRIVATE_H__
#define __GTK_EMULATOR_PRIVATE_H__

#include <gtk3ui/gtkemulator.h>

G_BEGIN_DECLS

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

#ifndef CAST_WIDGET
#define CAST_WIDGET(widget) ((GtkWidget*)(widget))
#endif

#ifndef CAST_EMULATOR
#define CAST_EMULATOR(widget) ((GtkEmulator*)(widget))
#endif

#ifndef CAST_XEVENT
#define CAST_XEVENT(event) ((XEvent*)(event))
#endif

#ifndef EMULATOR_DEFAULT_TIMEOUT
#define EMULATOR_DEFAULT_TIMEOUT 100UL
#endif

#ifndef EMULATOR_DEFAULT_WIDTH
#define EMULATOR_DEFAULT_WIDTH  768
#endif

#ifndef EMULATOR_DEFAULT_HEIGHT
#define EMULATOR_DEFAULT_HEIGHT 576
#endif

#ifndef EMULATOR_MIN_WIDTH
#define EMULATOR_MIN_WIDTH  320
#endif

#ifndef EMULATOR_MIN_HEIGHT
#define EMULATOR_MIN_HEIGHT 200
#endif

#ifndef KEY_DELAY_THRESHOLD
#define KEY_DELAY_THRESHOLD 20
#endif

#ifndef KEY_REPEAT_THRESHOLD
#define KEY_REPEAT_THRESHOLD 100
#endif

#ifndef JS_EVENT_BUTTON
#define JS_EVENT_BUTTON 0x01
#endif

#ifndef JS_EVENT_AXIS
#define JS_EVENT_AXIS 0x02
#endif

#ifndef JS_EVENT_INIT
#define JS_EVENT_INIT 0x80
#endif

G_END_DECLS

#endif /* __GTK_EMULATOR_PRIVATE_H__ */
