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
	libgem3.la \
	$(NULL)

# ----------------------------------------------------------------------------
# libgem3
# ----------------------------------------------------------------------------

libgem3_la_SOURCES = \
	gememulator.c \
	gememulator.h \
	gememulator-impl.h \
	gememulator-priv.h \
	$(NULL)

libgem3_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(gtk3_CFLAGS) \
	$(NULL)

libgem3_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(gtk3_LDFLAGS) \
	$(NULL)

libgem3_la_LIBADD = \
	$(gtk3_LIBS) \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
