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

# ----------------------------------------------------------------------------
# AUTOMAKE_OPTIONS
# ----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = \
	subdir-objects \
	$(NULL)

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
	xcpc-main.c \
	xcpc-main.h \
	xcpc-main-priv.h \
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
	$(top_builddir)/lib/xcpc/libxcpc.la \
	$(NULL)

# ----------------------------------------------------------------------------
# xcpc - gtk4
# ----------------------------------------------------------------------------

if GTK4
xcpc_SOURCES += \
	gtk4/xcpc-gtk4.c \
	gtk4/xcpc-gtk4.h \
	gtk4/xcpc-gtk4-priv.h \
	$(NULL)

xcpc_CPPFLAGS += \
	$(gtk4_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS += \
	$(gtk4_LDFLAGS) \
	$(NULL)

xcpc_LDADD += \
	$(top_builddir)/lib/gtk4/libgem4.la \
	$(gtk4_LIBS) \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# xcpc - gtk3
# ----------------------------------------------------------------------------

if GTK3
xcpc_SOURCES += \
	gtk3/xcpc-gtk3.c \
	gtk3/xcpc-gtk3.h \
	gtk3/xcpc-gtk3-priv.h \
	$(NULL)

xcpc_CPPFLAGS += \
	$(gtk3_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS += \
	$(gtk3_LDFLAGS) \
	$(NULL)

xcpc_LDADD += \
	$(top_builddir)/lib/gtk3/libgem3.la \
	$(gtk3_LIBS) \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# xcpc - motif2
# ----------------------------------------------------------------------------

if MOTIF2
xcpc_SOURCES += \
	motif2/xcpc-motif2.c \
	motif2/xcpc-motif2.h \
	motif2/xcpc-motif2-priv.h \
	$(NULL)

xcpc_CPPFLAGS += \
	$(X_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS += \
	$(X_LDFLAGS) \
	$(NULL)

xcpc_LDADD += \
	-lXm -lXmu -lXt \
	$(top_builddir)/lib/Xem/libXem.la \
	$(X_PRE_LIBS) $(X_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# xcpc - athena
# ----------------------------------------------------------------------------

if ATHENA
xcpc_SOURCES += \
	athena/xcpc-athena.c \
	athena/xcpc-athena.h \
	athena/xcpc-athena-priv.h \
	$(NULL)

xcpc_CPPFLAGS += \
	$(X_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS += \
	$(X_LDFLAGS) \
	$(NULL)

xcpc_LDADD += \
	-lXaw -lXmu -lXt \
	$(top_builddir)/lib/Xem/libXem.la \
	$(X_PRE_LIBS) $(X_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# xcpc - intrinsic
# ----------------------------------------------------------------------------

if INTRINSIC
xcpc_SOURCES += \
	intrinsic/xcpc-intrinsic.c \
	intrinsic/xcpc-intrinsic.h \
	intrinsic/xcpc-intrinsic-priv.h \
	$(NULL)

xcpc_CPPFLAGS += \
	$(X_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS += \
	$(X_LDFLAGS) \
	$(NULL)

xcpc_LDADD += \
	-lXt \
	$(top_builddir)/lib/Xem/libXem.la \
	$(X_PRE_LIBS) $(X_LIBS) -lXext -lX11 \
	$(X_EXTRA_LIBS) -lm \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# xcpc - portaudio
# ----------------------------------------------------------------------------

if PORTAUDIO
xcpc_CPPFLAGS += \
	$(portaudio_CFLAGS) \
	$(NULL)

xcpc_LDFLAGS += \
	$(portaudio_LDFLAGS) \
	$(NULL)

xcpc_LDADD += \
	$(portaudio_LIBS) \
	$(NULL)
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
