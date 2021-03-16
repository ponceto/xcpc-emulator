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
	builtin-roms.c \
	builtin-roms.h \
	builtin-roms-impl.h \
	builtin-roms-priv.h \
	blitter.c \
	blitter.h \
	blitter-impl.h \
	blitter-priv.h \
	monitor.c \
	monitor.h \
	monitor-impl.h \
	monitor-priv.h \
	keyboard.c \
	keyboard.h \
	keyboard-impl.h \
	keyboard-priv.h \
	joystick.c \
	joystick.h \
	joystick-impl.h \
	joystick-priv.h \
	vga-core.c \
	vga-core.h \
	vga-core-impl.h \
	vga-core-priv.h \
	vdc-6845.c \
	vdc-6845.h \
	vdc-6845-impl.h \
	vdc-6845-priv.h \
	ppi-8255.c \
	ppi-8255.h \
	ppi-8255-impl.h \
	ppi-8255-priv.h \
	psg-8910.c \
	psg-8910.h \
	psg-8910-impl.h \
	psg-8910-priv.h \
	ram-bank.c \
	ram-bank.h \
	ram-bank-impl.h \
	ram-bank-priv.h \
	rom-bank.c \
	rom-bank.h \
	rom-bank-impl.h \
	rom-bank-priv.h \
	snapshot.c \
	snapshot.h \
	snapshot-impl.h \
	snapshot-priv.h \
	snapshot-reader.c \
	snapshot-reader.h \
	snapshot-reader-impl.h \
	snapshot-reader-priv.h \
	snapshot-writer.c \
	snapshot-writer.h \
	snapshot-writer-impl.h \
	snapshot-writer-priv.h \
	xlib.c \
	xlib.h \
	xlib-impl.h \
	xlib-priv.h \
	$(NULL)

libxcpc_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(GLIB_CFLAGS) \
	$(NULL)

libxcpc_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(GLIB_LDFLAGS) \
	$(NULL)

libxcpc_la_LIBADD = \
	$(GLIB_LIBS) \
	$(X_LIBS) \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
