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
# SUBDIRS
# ----------------------------------------------------------------------------

SUBDIRS = \
	$(NULL)

# ----------------------------------------------------------------------------
# miniaudio
# ----------------------------------------------------------------------------

if USE_MINIAUDIO
if HAVE_MINIAUDIO
SUBDIRS += miniaudio
endif
endif

# ----------------------------------------------------------------------------
# libdsk
# ----------------------------------------------------------------------------

if USE_LIBDSK
if HAVE_LIBDSK
SUBDIRS += libdsk
endif
endif

# ----------------------------------------------------------------------------
# lib765
# ----------------------------------------------------------------------------

if USE_LIB765
if HAVE_LIB765
SUBDIRS += lib765
endif
endif

# ----------------------------------------------------------------------------
# xcpc
# ----------------------------------------------------------------------------

if TRUE
SUBDIRS += xcpc
endif

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
