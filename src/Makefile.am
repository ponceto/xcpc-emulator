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

SUBDIRS = \
	dev drv Xem

AM_CPPFLAGS = -I$(top_srcdir) $(X_CFLAGS) $(GLIB_CFLAGS) -DROMSDIR=\"$(datadir)/@PACKAGE@/roms\"

bin_PROGRAMS = xcpc

xcpc_SOURCES = \
	amstrad_cpc.c amstrad_cpc.h \
	crtc_6845.c crtc_6845.h \
	ay_3_8910.c ay_3_8910.h \
	ppi_8255.c ppi_8255.h \
	fdc_765.c fdc_765.h \
	cpu_z80.c cpu_z80.h \
	Z80Core.h \
	Z80Opcodes.h \
	Z80OpcodesCB.h \
	Z80OpcodesDD.h \
	Z80OpcodesDDCB.h \
	Z80OpcodesED.h \
	Z80OpcodesFD.h \
	Z80OpcodesFDCB.h \
	Z80OpcodesXX.h \
	Z80OpcodesXXCB.h \
	common.c common.h \
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

romsdir = "$(datadir)/@PACKAGE@/roms"

roms_DATA = \
	roms/cpc6128.rom \
	roms/cpc664.rom \
	roms/cpc464.rom \
	roms/amsdos.rom

EXTRA_DIST = \
	roms/cpc6128.rom \
	roms/cpc664.rom \
	roms/cpc464.rom \
	roms/amsdos.rom
