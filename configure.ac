#
# configure.ac - Copyright (c) 2001-2024 - Olivier Poncet
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
m4_define([pkg_minor_version], [0])
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
# Initialize autoconf/automake/libtool
# ----------------------------------------------------------------------------

AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIRS([m4])
AC_INIT([pkg_name], [pkg_version], [pkg_bugreport])
AM_INIT_AUTOMAKE
LT_INIT

# ----------------------------------------------------------------------------
# Adjust CPPFLAGS if needed
# ----------------------------------------------------------------------------

case "${host_os}" in
    linux*)
        AX_APPEND_FLAG([-D_DEFAULT_SOURCE], [CPPFLAGS])
        ;;
esac

# ----------------------------------------------------------------------------
# Programs
# ----------------------------------------------------------------------------

AC_PROG_CC
AC_PROG_CXX

# ----------------------------------------------------------------------------
# Checks
# ----------------------------------------------------------------------------

AX_CHECK_C99
AX_CHECK_CXX14
AX_CHECK_BYTE_ORDER

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

AX_CHECK_HEADERS
AX_CHECK_FUNCTIONS
AX_CHECK_ZLIB
AX_CHECK_BZLIB
AX_CHECK_GUI_TOOLKIT
AX_CHECK_MINIAUDIO
AX_CHECK_LIBDSK
AX_CHECK_LIB765
AX_CHECK_LINUX_JOYSTICK_API

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
# Config header
# ----------------------------------------------------------------------------

AC_CONFIG_HEADERS([config.h])

# ----------------------------------------------------------------------------
# Config files
# ----------------------------------------------------------------------------

AC_CONFIG_FILES([
	Makefile
	bin/Makefile
	etc/Makefile
	doc/Makefile
	lib/Makefile
	lib/miniaudio/Makefile
	lib/libdsk/Makefile
	lib/lib765/Makefile
	lib/xcpc/Makefile
	src/Makefile
	src/utils/Makefile
	src/gtk3ui/Makefile
	src/gtk4ui/Makefile
	share/Makefile
	share/applications/Makefile
	share/applications/xcpc.desktop
	share/pixmaps/Makefile
	share/xcpc/Makefile
	share/xcpc/roms/Makefile
	share/xcpc/disks/Makefile
	share/xcpc/snapshots/Makefile
])

# ----------------------------------------------------------------------------
# Output
# ----------------------------------------------------------------------------

AC_OUTPUT

# ----------------------------------------------------------------------------
# Notice
# ----------------------------------------------------------------------------

AC_MSG_NOTICE([configured to build Xcpc:

  X11 .................... ${have_x}
  X11 XShm ............... ${have_xshm}
  gtk3 ................... ${have_gtk3}
  gtk4 ................... ${have_gtk4}
  miniaudio .............. ${have_miniaudio}
  libdsk ................. ${have_libdsk}
  lib765 ................. ${have_lib765}
  linux joystick api ..... ${have_linux_joystick_api}
  user interface ......... ${with_gui_toolkit}

Have fun with Xcpc ;-)
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
