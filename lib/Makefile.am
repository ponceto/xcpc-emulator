#
# Makefile.am - Copyright (c) 2001-2023 - Olivier Poncet
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
	libdsk \
	lib765 \
	xcpc \
	$(NULL)

if LIBXEM
SUBDIRS += Xem
endif

if GTK3
SUBDIRS += gtk3
endif

if GTK4
SUBDIRS += gtk4
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
