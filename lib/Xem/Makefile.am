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
	libXem.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libXem
# ----------------------------------------------------------------------------

libXem_la_SOURCES = \
	AppShell.c \
	AppShell.h \
	AppShellP.h \
	DlgShell.c \
	DlgShell.h \
	DlgShellP.h \
	Emulator.c \
	Emulator.h \
	EmulatorP.h \
	StringDefs.h \
	$(NULL)

libXem_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(X_CFLAGS) \
	$(NULL)

libXem_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(X_LDFLAGS) \
	$(NULL)

libXem_la_LIBADD = \
	$(X_LIBS) -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------