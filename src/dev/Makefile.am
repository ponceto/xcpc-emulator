# Makefile.am - Copyright (c) 2001, 2006 Olivier Poncet
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(X_CFLAGS) $(GLIB_CFLAGS)

noinst_LIBRARIES = libdev.a

libdev_a_SOURCES = \
	device.c device.h \
	z80cpu.c z80cpu.h \
	z80cpu_tables.h \
	z80cpu_opcode.h \
	z80cpu_opcode_CB.h \
	z80cpu_opcode_DD.h \
	z80cpu_opcode_DDCB.h \
	z80cpu_opcode_ED.h \
	z80cpu_opcode_FD.h \
	z80cpu_opcode_FDCB.h \
	z80cpu_opcode_XX.h \
	z80cpu_opcode_XXCB.h \
	garray.c garray.h \
	mc6845.c mc6845.h \
	ay8910.c ay8910.h \
	lib765.c lib765.h \
	upd765.c upd765.h \
	fdc765.c fdc765.h \
	fdd765.c fdd765.h \
	 i8255.c  i8255.h
