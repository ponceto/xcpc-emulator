#
# Makefile.am - Copyright (c) 2001-2021 - Olivier Poncet
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#

# ----------------------------------------------------------------------------
# AUTOMAKE_OPTIONS
# ----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = \
	subdir-objects \
	$(NULL)

# ----------------------------------------------------------------------------
# noinst_LTLIBRARIES
# ----------------------------------------------------------------------------

noinst_LTLIBRARIES = \
	libxcpc.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libxcpc
# ----------------------------------------------------------------------------

libxcpc_la_SOURCES = \
	libxcpc.c \
	libxcpc.h \
	libxcpc-impl.h \
	libxcpc-priv.h \
	xlib/xlib.c \
	xlib/xlib.h \
	xlib/xlib-impl.h \
	xlib/xlib-priv.h \
	blitter/blitter.c \
	blitter/blitter.h \
	blitter/blitter-impl.h \
	blitter/blitter-priv.h \
	monitor/monitor.c \
	monitor/monitor.h \
	monitor/monitor-impl.h \
	monitor/monitor-priv.h \
	keyboard/keyboard.c \
	keyboard/keyboard.h \
	keyboard/keyboard-impl.h \
	keyboard/keyboard-priv.h \
	joystick/joystick.c \
	joystick/joystick.h \
	joystick/joystick-impl.h \
	joystick/joystick-priv.h \
	cpu-z80a/cpu-z80a.c \
	cpu-z80a/cpu-z80a.h \
	cpu-z80a/cpu-z80a-impl.h \
	cpu-z80a/cpu-z80a-priv.h \
	cpu-z80a/cpu-z80a-opcodes.inc \
	cpu-z80a/cpu-z80a-opcodes-dd.inc \
	cpu-z80a/cpu-z80a-opcodes-ed.inc \
	cpu-z80a/cpu-z80a-opcodes-fd.inc \
	cpu-z80a/cpu-z80a-opcodes-xx.inc \
	cpu-z80a/cpu-z80a-opcodes-cb.inc \
	cpu-z80a/cpu-z80a-opcodes-ddcb.inc \
	cpu-z80a/cpu-z80a-opcodes-fdcb.inc \
	vga-core/vga-core.c \
	vga-core/vga-core.h \
	vga-core/vga-core-impl.h \
	vga-core/vga-core-priv.h \
	vdc-6845/vdc-6845.c \
	vdc-6845/vdc-6845.h \
	vdc-6845/vdc-6845-impl.h \
	vdc-6845/vdc-6845-priv.h \
	ppi-8255/ppi-8255.c \
	ppi-8255/ppi-8255.h \
	ppi-8255/ppi-8255-impl.h \
	ppi-8255/ppi-8255-priv.h \
	psg-8910/psg-8910.c \
	psg-8910/psg-8910.h \
	psg-8910/psg-8910-impl.h \
	psg-8910/psg-8910-priv.h \
	fdc-765a/fdc-765a.c \
	fdc-765a/fdc-765a.h \
	fdc-765a/fdc-765a-impl.h \
	fdc-765a/fdc-765a-priv.h \
	ram-bank/ram-bank.c \
	ram-bank/ram-bank.h \
	ram-bank/ram-bank-impl.h \
	ram-bank/ram-bank-priv.h \
	rom-bank/rom-bank.c \
	rom-bank/rom-bank.h \
	rom-bank/rom-bank-impl.h \
	rom-bank/rom-bank-priv.h \
	settings/settings.c \
	settings/settings.h \
	settings/settings-impl.h \
	settings/settings-priv.h \
	snapshot/snapshot.c \
	snapshot/snapshot.h \
	snapshot/snapshot-impl.h \
	snapshot/snapshot-priv.h \
	snapshot/snapshot-reader.c \
	snapshot/snapshot-reader.h \
	snapshot/snapshot-reader-impl.h \
	snapshot/snapshot-reader-priv.h \
	snapshot/snapshot-writer.c \
	snapshot/snapshot-writer.h \
	snapshot/snapshot-writer-impl.h \
	snapshot/snapshot-writer-priv.h \
	builtin-roms/builtin-roms.c \
	builtin-roms/builtin-roms.h \
	builtin-roms/builtin-roms-impl.h \
	builtin-roms/builtin-roms-priv.h \
	$(NULL)

libxcpc_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(NULL)

libxcpc_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(NULL)

libxcpc_la_LIBADD = \
	$(top_builddir)/lib/lib765/lib765.la \
	$(top_builddir)/lib/libdsk/libdsk.la \
	$(X_LIBS) \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
