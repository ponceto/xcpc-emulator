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
# SUBDIRS
# ----------------------------------------------------------------------------

SUBDIRS = \
	utils \
	$(NULL)

# ----------------------------------------------------------------------------
# Gtk+-3.0
# ----------------------------------------------------------------------------

if USE_GTK3
if HAVE_GTK3
SUBDIRS += gtk3ui
endif
endif

# ----------------------------------------------------------------------------
# Gtk+-4.0
# ----------------------------------------------------------------------------

if USE_GTK4
if HAVE_GTK4
SUBDIRS += gtk4ui
endif
endif

# ----------------------------------------------------------------------------
# bin_PROGRAMS
# ----------------------------------------------------------------------------

bin_PROGRAMS = \
	xcpc \
	$(NULL)

# ----------------------------------------------------------------------------
# xcpc
# ----------------------------------------------------------------------------

xcpc_SOURCES = \
	xcpc.cc \
	xcpc.h \
	$(NULL)

xcpc_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	$(NULL)

xcpc_LDFLAGS = \
	-L$(top_builddir)/lib \
	-L$(top_builddir)/src \
	$(NULL)

xcpc_LDADD = \
	$(NULL)

if USE_GTK3
if HAVE_GTK3
xcpc_LDADD += $(top_builddir)/src/gtk3ui/libxcpcgtk3ui.la
endif
endif

if USE_GTK4
if HAVE_GTK4
xcpc_LDADD += $(top_builddir)/src/gtk4ui/libxcpcgtk4ui.la
endif
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
