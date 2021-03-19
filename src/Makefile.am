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
# bin_PROGRAMS
# ----------------------------------------------------------------------------

bin_PROGRAMS = \
	xcpc \
	$(NULL)

# ----------------------------------------------------------------------------
# xcpc
# ----------------------------------------------------------------------------

xcpc_SOURCES = \
	xcpc.c \
	xcpc.h \
	amstrad-cpc.c \
	amstrad-cpc.h \
	$(NULL)

xcpc_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	$(GLIB_CFLAGS) \
	$(X_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS = \
	-L$(top_builddir)/lib \
	-L$(top_builddir)/src \
	$(GLIB_LDFLAGS) \
	$(X_LDFLAGS) \
	$(NULL)

xcpc_LDADD = \
	$(top_builddir)/lib/xcpc/libxcpc.la \
	$(NULL)

if MOTIF2
xcpc_SOURCES += \
	xcpc-motif2.c \
	xcpc-motif2.h \
	xcpc-motif2-priv.h \
	$(NULL)

xcpc_LDADD += \
	$(top_builddir)/lib/Xem/libXem.la \
	$(X_LIBS) -lXm -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) $(GLIB_LIBS) -lm \
	$(NULL)
endif

if ATHENA
xcpc_SOURCES += \
	xcpc-athena.c \
	xcpc-athena.h \
	xcpc-athena-priv.h \
	$(NULL)

xcpc_LDADD += \
	$(top_builddir)/lib/Xem/libXem.la \
	$(X_LIBS) -lXaw -lXmu -lXt \
	$(X_PRE_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) $(GLIB_LIBS) -lm \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
