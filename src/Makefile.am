# Makefile.am - Copyright (c) 2001, 2006, 2007, 2008, 2009 Olivier Poncet
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

SUBDIRS = \
	dev drv Xem

AM_CPPFLAGS = -I$(top_srcdir) $(X_CFLAGS) $(GLIB_CFLAGS) -DROMSDIR=\"$(datadir)/@PACKAGE@/roms\"

bin_PROGRAMS = xcpc

xcpc_SOURCES = \
	amstrad_cpc.c amstrad_cpc.h \
	xcpc.h

if MOTIF1
xcpc_SOURCES += xcpc_motif1.c

xcpc_LDADD = \
	./Xem/libXem.a \
	./drv/libdrv.a \
	./dev/libdev.a \
	$(X_LIBS) -lXm -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) $(GLIB_LIBS) -lm
endif

if ATHENA
xcpc_SOURCES += xcpc_athena.c

xcpc_LDADD = \
	./Xem/libXem.a \
	./drv/libdrv.a \
	./dev/libdev.a \
	$(X_LIBS) -lXaw -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) $(GLIB_LIBS) -lm
endif

pixmapsdir = "$(datadir)/pixmaps"

pixmaps_DATA = \
        xcpc.xpm

desktopdir = "$(datadir)/applications"

desktop_DATA = \
        xcpc.desktop

romsdir = "$(datadir)/@PACKAGE@/roms"

roms_DATA = \
	roms/cpc6128.rom \
	roms/cpc664.rom \
	roms/cpc464.rom \
	roms/amsdos.rom

EXTRA_DIST = \
	xcpc.xpm \
	xcpc.desktop \
	roms/cpc6128.rom \
	roms/cpc664.rom \
	roms/cpc464.rom \
	roms/amsdos.rom
