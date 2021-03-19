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
	libdev.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libdev
# ----------------------------------------------------------------------------

libdev_la_SOURCES = \
	device.c \
	device.h \
	z80cpu.c \
	z80cpu.h \
	z80cpu_tables.h \
	z80cpu_opcode.h \
	z80cpu_opcode_DD.h \
	z80cpu_opcode_ED.h \
	z80cpu_opcode_FD.h \
	z80cpu_opcode_XX.h \
	$(NULL)

libdev_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(GLIB_CFLAGS) \
	$(NULL)

libdev_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(GLIB_LDFLAGS) \
	$(NULL)

libdev_la_LIBADD = \
	$(GLIB_LIBS) \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
