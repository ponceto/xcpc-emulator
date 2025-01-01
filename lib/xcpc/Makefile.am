#
# Makefile.am - Copyright (c) 2001-2025 - Olivier Poncet
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
	libxcpc.cc \
	libxcpc.h \
	libxcpc-cxx.h \
	libxcpc-priv.h \
	libxcpc-events.h \
	libxcpc-keysyms.h \
	xlib/xlib.cc \
	xlib/xlib.h \
	amstrad/cpc/cpc-machine.cc \
	amstrad/cpc/cpc-machine.h \
	amstrad/cpc/cpc-mainboard.cc \
	amstrad/cpc/cpc-mainboard.h \
	amstrad/cpc/cpc-settings.cc \
	amstrad/cpc/cpc-settings.h \
	devices/dpy/dpy-device.cc \
	devices/dpy/dpy-device.h \
	devices/kbd/kbd-device.cc \
	devices/kbd/kbd-device.h \
	devices/cpu/cpu-device.cc \
	devices/cpu/cpu-device.h \
	devices/cpu/cpu-microcode.inc \
	devices/cpu/cpu-opcodes.inc \
	devices/cpu/cpu-opcodes-cb.inc \
	devices/cpu/cpu-opcodes-ed.inc \
	devices/cpu/cpu-opcodes-dd.inc \
	devices/cpu/cpu-opcodes-fd.inc \
	devices/cpu/cpu-opcodes-ddcb.inc \
	devices/cpu/cpu-opcodes-fdcb.inc \
	devices/vga/vga-device.cc \
	devices/vga/vga-device.h \
	devices/vdc/vdc-device.cc \
	devices/vdc/vdc-device.h \
	devices/ppi/ppi-device.cc \
	devices/ppi/ppi-device.h \
	devices/psg/psg-device.cc \
	devices/psg/psg-device.h \
	devices/fdc/fdc-device.cc \
	devices/fdc/fdc-device.h \
	devices/mem/mem-device.cc \
	devices/mem/mem-device.h \
	formats/cdt/cdt-format.cc \
	formats/cdt/cdt-format.h \
	formats/dsk/dsk-format.cc \
	formats/dsk/dsk-format.h \
	formats/sna/sna-format.cc \
	formats/sna/sna-format.h \
	$(NULL)

libxcpc_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(NULL)

libxcpc_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(NULL)

libxcpc_la_LIBADD = \
	$(NULL)

if USE_LIB765
if HAVE_LIB765
libxcpc_la_LIBADD += $(top_builddir)/lib/lib765/lib765.la
endif
endif

if USE_LIBDSK
if HAVE_LIBDSK
libxcpc_la_LIBADD += $(top_builddir)/lib/libdsk/libdsk.la
endif
endif

if USE_MINIAUDIO
if HAVE_MINIAUDIO
libxcpc_la_LIBADD += $(top_builddir)/lib/miniaudio/libminiaudio.la
endif
endif

if USE_X11
if HAVE_X11
libxcpc_la_LIBADD += $(X_LIBS) $(X_PRE_LIBS) -lXext -lX11 $(X_EXTRA_LIBS)
endif
endif

if TRUE
libxcpc_la_LIBADD += -lm
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
