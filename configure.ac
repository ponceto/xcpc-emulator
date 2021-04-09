#
# configure.ac - Copyright (c) 2001-2021 - Olivier Poncet
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
# Package informations
# ----------------------------------------------------------------------------

m4_define([pkg_name], [xcpc])
m4_define([pkg_major_version], [0])
m4_define([pkg_minor_version], [33])
m4_define([pkg_micro_version], [0])
m4_define([pkg_interface_age], [0])
m4_define([pkg_binary_age], [m4_eval(100 * pkg_minor_version + pkg_micro_version)])
m4_define([pkg_version], [pkg_major_version.pkg_minor_version.pkg_micro_version])
m4_define([pkg_bugreport], [ponceto@free.fr])

# ----------------------------------------------------------------------------
# Libtool black magic
# ----------------------------------------------------------------------------

m4_define([pkg_lt_release], [pkg_major_version.pkg_minor_version])
m4_define([pkg_lt_current], [m4_eval(100 * pkg_minor_version + pkg_micro_version - pkg_interface_age)])
m4_define([pkg_lt_revision], [m4_eval(pkg_interface_age)])
m4_define([pkg_lt_age], [m4_eval(pkg_binary_age - pkg_interface_age)])

# ----------------------------------------------------------------------------
# Autoconf/Automake/Libtool
# ----------------------------------------------------------------------------

AC_INIT([pkg_name], [pkg_version], [pkg_bugreport])
AM_INIT_AUTOMAKE
LT_INIT

# ----------------------------------------------------------------------------
# Programs
# ----------------------------------------------------------------------------

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC

# ----------------------------------------------------------------------------
# Checks
# ----------------------------------------------------------------------------

AX_CHECK_C99
AX_CHECK_CXX14
AX_CHECK_BYTE_ORDER

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

AX_CHECK_X11
AX_CHECK_XMU
AX_CHECK_XAW
AX_CHECK_XM2
AX_CHECK_XSHM
AX_CHECK_GUI
AX_CHECK_HEADERS
AX_CHECK_FUNCTIONS
AX_CHECK_ZLIB
AX_CHECK_BZLIB

# ----------------------------------------------------------------------------
# Defines
# ----------------------------------------------------------------------------

AC_DEFINE(PACKAGE_MAJOR_VERSION, [pkg_major_version], [Define to the major version of this package.])
AC_DEFINE(PACKAGE_MINOR_VERSION, [pkg_minor_version], [Define to the minor version of this package.])
AC_DEFINE(PACKAGE_MICRO_VERSION, [pkg_micro_version], [Define to the micro version of this package.])
AC_DEFINE(PACKAGE_INTERFACE_AGE, [pkg_interface_age], [Define to the interface age of this package.])

AX_DEFINES

# ----------------------------------------------------------------------------
# Libtool versionning
# ----------------------------------------------------------------------------

AC_SUBST(LT_RELEASE, [pkg_lt_release])
AC_SUBST(LT_CURRENT, [pkg_lt_current])
AC_SUBST(LT_REVISION, [pkg_lt_revision])
AC_SUBST(LT_AGE, [pkg_lt_age])
AC_SUBST(LT_VERSION_INFO, [pkg_lt_current:pkg_lt_revision:pkg_lt_age])

# ----------------------------------------------------------------------------
# Output
# ----------------------------------------------------------------------------

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	lib/Makefile
	lib/libdsk/Makefile
	lib/lib765/Makefile
	lib/xcpc/Makefile
	lib/Xem/Makefile
	src/Makefile
	share/Makefile
	share/applications/Makefile
	share/applications/xcpc.desktop
	share/pixmaps/Makefile
	share/xcpc/Makefile
	share/xcpc/bitmaps/Makefile
	share/xcpc/pixmaps/Makefile
	share/xcpc/roms/Makefile
	share/xcpc/disks/Makefile
	share/xcpc/snapshots/Makefile
])
AC_OUTPUT

# ----------------------------------------------------------------------------
# Notice
# ----------------------------------------------------------------------------

AC_MSG_NOTICE([Configured to build Xcpc:

  Athena sdk .... ${have_athena}
  Motif2 sdk .... ${have_motif2}
  X11 toolkit ... ${x11_toolkit}
  X11-SHM ....... ${enable_xshm}

Have fun with Xcpc ;-)
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
