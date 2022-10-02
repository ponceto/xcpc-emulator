#
# Makefile.am - Copyright (c) 2001-2024 - Olivier Poncet
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

noinst_PROGRAMS = \
	xcpc-dsk \
	$(NULL)

# ----------------------------------------------------------------------------
# xcpc-dsk
# ----------------------------------------------------------------------------

xcpc_dsk_SOURCES = \
	arglist.cc \
	arglist.h \
	console.cc \
	console.h \
	program.cc \
	program.h \
	xcpc-dsk.cc \
	xcpc-dsk.h \
	$(NULL)

xcpc_dsk_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(NULL)

xcpc_dsk_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(NULL)

xcpc_dsk_LDADD = \
	$(top_builddir)/lib/xcpc/libxcpc.la \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
