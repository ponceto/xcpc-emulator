#
# Makefile.am - Copyright (c) 2001-2026 - Olivier Poncet
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
	libxcpcgtk4ui.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libxcpcgtk4ui
# ----------------------------------------------------------------------------

libxcpcgtk4ui_la_SOURCES = \
	xcpc-main.cc \
	xcpc-main.h \
	$(NULL)

libxcpcgtk4ui_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	$(NULL)

libxcpcgtk4ui_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	-L$(top_builddir)/src \
	$(NULL)

libxcpcgtk4ui_la_LIBADD = \
	$(NULL)

# ----------------------------------------------------------------------------
# xcpc flags/libraries
# ----------------------------------------------------------------------------

if TRUE
libxcpcgtk4ui_la_LIBADD += $(top_builddir)/lib/xcpc/libxcpc.la
endif

# ----------------------------------------------------------------------------
# gtk4 flags/libraries
# ----------------------------------------------------------------------------

if TRUE
libxcpcgtk4ui_la_CPPFLAGS += $(gtk4_CFLAGS)
libxcpcgtk4ui_la_LDFLAGS += $(gtk4_LDFLAGS)
libxcpcgtk4ui_la_LIBADD += $(gtk4_LIBS)
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
