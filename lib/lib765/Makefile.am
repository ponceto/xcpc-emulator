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
	lib765.la \
	$(NULL)

# ----------------------------------------------------------------------------
# lib765
# ----------------------------------------------------------------------------

lib765_la_SOURCES = \
	765.h \
	765i.h \
	error.c \
	765fdc.c \
	765dsk.c \
	765drive.c \
	765ldsk.c \
	$(NULL)

lib765_la_CPPFLAGS = \
	-I$(top_srcdir)/lib \
	$(NULL)

lib765_la_LDFLAGS = \
	-L$(top_builddir)/lib \
	$(NULL)

lib765_la_LIBADD = \
	$(NULL)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
