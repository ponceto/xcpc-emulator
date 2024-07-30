#
# ax_check_x11.m4 - Copyright (c) 2001-2024 - Olivier Poncet
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
# AX_CHECK_X11
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_X11], [
AC_PATH_X
AC_PATH_XTRA
AC_DEFINE([HAVE_X11], [1], [Define to 1 if you have the X11 library.])
AC_DEFINE([USE_X11], [1], [Define to 1 if you want the X11 library.])
AM_CONDITIONAL([HAVE_X11], true)
AM_CONDITIONAL([USE_X11], true)
])

# ----------------------------------------------------------------------------
# AX_CHECK_XSHM
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_XSHM], [
AC_ARG_ENABLE([xshm], [AS_HELP_STRING([--enable-xshm], [add the support of XShm (if available) [default=yes]])], [], [enable_xshm='yes'])
if test "x${enable_xshm}" = 'xyes'; then
AC_CHECK_HEADERS([X11/extensions/XShm.h], [have_xshm='yes'], [have_xshm='no'], [
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
])
fi
if test "x${have_xshm}" = 'xyes'; then
    AC_DEFINE([HAVE_XSHM], [1], [Define to 1 if you have the X11-SHM extension.])
else
    have_xshm='no'
fi
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
