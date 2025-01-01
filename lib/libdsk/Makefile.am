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
# noinst_LTLIBRARIES
# ----------------------------------------------------------------------------

noinst_LTLIBRARIES = \
	libdsk.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libdsk
# ----------------------------------------------------------------------------

libdsk_la_SOURCES = \
	libdsk.h \
	drv.h \
	drvi.h \
	comp.h \
	compi.h \
	drivers.h \
	drivers.inc \
	compbz2.c \
	compbz2.h \
	compdskf.c \
	compdskf.h \
	compgz.c \
	compgz.h \
	compress.c \
	compress.h \
	compress.inc \
	compsq.c \
	compsq.h \
	comptlzh.c \
	comptlzh.h \
	crc16.c \
	crc16.h \
	crctable.c \
	crctable.h \
	drvadisk.c \
	drvadisk.h \
	drvcfi.c \
	drvcfi.h \
	drvcpcem.c \
	drvcpcem.h \
	drvdos16.c \
	drvdos16.h \
	drvdos32.c \
	drvdos32.h \
	drvdskf.c \
	drvdskf.h \
	drvimd.c \
	drvimd.h \
	drvint25.c \
	drvint25.h \
	drvjv3.c \
	drvjv3.h \
	drvlinux.c \
	drvlinux.h \
	drvlogi.c \
	drvlogi.h \
	drvmyz80.c \
	drvmyz80.h \
	drvntwdm.c \
	drvntwdm.h \
	drvnwasp.c \
	drvnwasp.h \
	drvposix.c \
	drvposix.h \
	drvqm.c \
	drvqm.h \
	drvrcpm.c \
	drvrcpm.h \
	drvsimh.c \
	drvsimh.h \
	drvtele.c \
	drvtele.h \
	drvwin16.c \
	drvwin16.h \
	drvwin32.c \
	drvwin32.h \
	drvydsk.c \
	drvydsk.h \
	dskcheck.c \
	dskcmt.c \
	dskdirty.c \
	dskerror.c \
	dskfmt.c \
	dskgeom.c \
	dskjni.c \
	dsklphys.c \
	dskopen.c \
	dskpars.c \
	dskread.c \
	dskreprt.c \
	dskretry.c \
	dskrtrd.c \
	dsksecid.c \
	dskseek.c \
	dsksgeom.c \
	dskstat.c \
	dsktread.c \
	dsktrkid.c \
	dskwrite.c \
	remall.h \
	remote.c \
	remote.h \
	remote.inc \
	rpccli.c \
	rpcfork.c \
	rpcfork.h \
	rpcfossl.c \
	rpcfossl.h \
	rpcfuncs.h \
	rpcmap.c \
	rpcpack.c \
	rpcserv.c \
	rpctios.c \
	rpctios.h \
	rpcwin32.c \
	rpcwin32.h \
	w16defs.h \
	w95defs.h \
	$(NULL)

libdsk_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(NULL)

libdsk_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(NULL)

libdsk_la_LIBADD = \
	$(NULL)

libdsk_la_CPPFLAGS += -DDISABLE_FLOPPY

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
