# Makefile.am - Copyright (c) 2006 Olivier Poncet
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

AM_CPPFLAGS = -I$(top_srcdir) $(X_CFLAGS) -DROMSDIR=\"$(datadir)/@PACKAGE@/roms\"

bin_PROGRAMS = xcpc

xcpc_SOURCES = \
	amstrad_cpc.c amstrad_cpc.h \
	crtc_6845.c crtc_6845.h \
	ay_3_8910.c ay_3_8910.h \
	ppi_8255.c ppi_8255.h \
	fdc_765.c fdc_765.h \
	cpu_z80.c cpu_z80.h \
	common.c common.h

if MOTIF1
xcpc_SOURCES += \
	XArea.c XArea.h XAreaP.h \
	xcpc_motif1.c xcpc.h

xcpc_LDADD = \
	$(X_LIBS) -lXm -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm
endif

if ATHENA
xcpc_SOURCES += \
	XArea.c XArea.h XAreaP.h \
	xcpc_athena.c xcpc.h

xcpc_LDADD = \
	$(X_LIBS) -lXaw -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm
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
