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
	libxcpcgtk3ui.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libxcpcgtk3ui
# ----------------------------------------------------------------------------

libxcpcgtk3ui_la_SOURCES = \
	gtkemulator.c \
	gtkemulator.h \
	gtkemulatortypes.h \
	gtkemulatorprivate.h \
	all.cc \
	all.h \
	gtk3-base.cc \
	gtk3-base.h \
	gtk3-application.cc \
	gtk3-application.h \
	gtk3-widget.cc \
	gtk3-widget.h \
	gtk3-container.cc \
	gtk3-container.h \
	gtk3-bin.cc \
	gtk3-bin.h \
	gtk3-box.cc \
	gtk3-box.h \
	gtk3-button.cc \
	gtk3-button.h \
	gtk3-frame.cc \
	gtk3-frame.h \
	gtk3-window.cc \
	gtk3-window.h \
	gtk3-application-window.cc \
	gtk3-application-window.h \
	gtk3-dialog.cc \
	gtk3-dialog.h \
	gtk3-about-dialog.cc \
	gtk3-about-dialog.h \
	gtk3-message-dialog.cc \
	gtk3-message-dialog.h \
	gtk3-file-chooser-dialog.cc \
	gtk3-file-chooser-dialog.h \
	gtk3-gl-area.cc \
	gtk3-gl-area.h \
	gtk3-label.cc \
	gtk3-label.h \
	gtk3-menu-shell.cc \
	gtk3-menu-shell.h \
	gtk3-menu-bar.cc \
	gtk3-menu-bar.h \
	gtk3-menu.cc \
	gtk3-menu.h \
	gtk3-menu-item.cc \
	gtk3-menu-item.h \
	gtk3-toolbar.cc \
	gtk3-toolbar.h \
	gtk3-tool-item.cc \
	gtk3-tool-item.h \
	gtk3-emulator.cc \
	gtk3-emulator.h \
	gdk3-pixbuf.cc \
	gdk3-pixbuf.h \
	xcpc-application.cc \
	xcpc-application.h \
	xcpc-snapshot-dialog.cc \
	xcpc-snapshot-dialog.h \
	xcpc-disk-dialog.cc \
	xcpc-disk-dialog.h \
	xcpc-help-dialog.cc \
	xcpc-help-dialog.h \
	xcpc-about-dialog.cc \
	xcpc-about-dialog.h \
	$(NULL)

libxcpcgtk3ui_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	$(NULL)

libxcpcgtk3ui_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	-L$(top_builddir)/src \
	$(NULL)

libxcpcgtk3ui_la_LIBADD = \
	$(NULL)

# ----------------------------------------------------------------------------
# xcpc flags/libraries
# ----------------------------------------------------------------------------

if TRUE
libxcpcgtk3ui_la_LIBADD += $(top_builddir)/lib/xcpc/libxcpc.la
endif

# ----------------------------------------------------------------------------
# gtk3 flags/libraries
# ----------------------------------------------------------------------------

if TRUE
libxcpcgtk3ui_la_CPPFLAGS += $(gtk3_CFLAGS)
libxcpcgtk3ui_la_LDFLAGS += $(gtk3_LDFLAGS)
libxcpcgtk3ui_la_LIBADD += $(gtk3_LIBS)
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
