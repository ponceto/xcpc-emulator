/*
 * machine.h - Copyright (c) 2001-2021 - Olivier Poncet
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
#ifndef __XCPC_MACHINE_H__
#define __XCPC_MACHINE_H__

#include <xcpc/machine/machine-impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern XcpcMachine* xcpc_machine_alloc     (void);
extern XcpcMachine* xcpc_machine_free      (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_construct (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_destruct  (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_new       (void);
extern XcpcMachine* xcpc_machine_delete    (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_set_iface (XcpcMachine* machine, const XcpcMachineIface* machine_iface);
extern XcpcMachine* xcpc_machine_reset     (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_clock     (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_start     (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_close     (XcpcMachine* machine);
extern XcpcMachine* xcpc_machine_parse     (XcpcMachine* machine, int* argc, char*** argv);

extern XcpcMachine* xcpc_machine_insert_drive0 (XcpcMachine* machine, const char* filename);
extern XcpcMachine* xcpc_machine_remove_drive0 (XcpcMachine* machine);

extern XcpcMachine* xcpc_machine_insert_drive1 (XcpcMachine* machine, const char* filename);
extern XcpcMachine* xcpc_machine_remove_drive1 (XcpcMachine* machine);

extern XcpcMachine* xcpc_machine_load_snapshot (XcpcMachine* machine, const char* filename);
extern XcpcMachine* xcpc_machine_save_snapshot (XcpcMachine* machine, const char* filename);

extern unsigned long xcpc_machine_create_proc  (XcpcMachine* machine, XEvent* event);
extern unsigned long xcpc_machine_destroy_proc (XcpcMachine* machine, XEvent* event);
extern unsigned long xcpc_machine_realize_proc (XcpcMachine* machine, XEvent* event);
extern unsigned long xcpc_machine_resize_proc  (XcpcMachine* machine, XEvent* event);
extern unsigned long xcpc_machine_expose_proc  (XcpcMachine* machine, XEvent* event);
extern unsigned long xcpc_machine_timer_proc   (XcpcMachine* machine, XEvent* event);
extern unsigned long xcpc_machine_input_proc   (XcpcMachine* machine, XEvent* event);

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_MACHINE_H__ */
