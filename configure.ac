# configure.ac - Copyright (c) 2001, 2006, 2007, 2008, 2009 Olivier Poncet
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

AC_INIT([xcpc], [0.0.0], [ponceto@free.fr])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PATH_X
AC_PATH_XTRA
AM_PATH_GLIB_2_0([2.6.0], [have_glib20="yes"], [have_glib20="no"], [gobject])
if test "x${have_glib20}" = "xno"; then
  AC_MSG_ERROR([Glib - version >= 2.6.0 is required])
fi
AC_ARG_ENABLE([debug], [AC_HELP_STRING([--enable-debug], [turn on debugging [default=no]])], [], [enable_debug="no"])
if test "x${enable_debug}" != "xyes"; then
  AC_DEFINE([G_DISABLE_ASSERT],      [1], [Define to 1 to disable GLib/GObject assert.])
  AC_DEFINE([G_DISABLE_CHECKS],      [1], [Define to 1 to disable GLib/GObject checks.])
  AC_DEFINE([G_DISABLE_CAST_CHECKS], [1], [Define to 1 to disable GObject cast checks.])
fi
AC_ARG_WITH([motif1], [AC_HELP_STRING([--with-motif1], [build the Motif1 version])])
AC_CHECK_HEADER([Xm/Xm.h],
[have_motif1="yes"], [have_motif1="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
AC_ARG_WITH([athena], [AC_HELP_STRING([--with-athena], [build the Athena version])])
AC_CHECK_HEADER([X11/Xaw/XawInit.h],
[have_athena="yes"], [have_athena="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
if   test "x${with_motif1}" = "xyes"; then
  if test "x${have_motif1}" = "xyes"; then
    AM_CONDITIONAL([MOTIF1], true )
    AM_CONDITIONAL([ATHENA], false)
    gui_toolkit="Motif1"
  else
    AC_MSG_ERROR([Motif1 toolkit not found])
  fi
elif test "x${with_athena}" = "xyes"; then
  if test "x${have_athena}" = "xyes"; then
    AM_CONDITIONAL([MOTIF1], false)
    AM_CONDITIONAL([ATHENA], true )
    gui_toolkit="Athena"
  else
    AC_MSG_ERROR([Athena toolkit not found])
  fi
else
  if   test "x${have_motif1}" = "xyes"; then
    AM_CONDITIONAL([MOTIF1], true )
    AM_CONDITIONAL([ATHENA], false)
    gui_toolkit="Motif1"
  elif test "x${have_athena}" = "xyes"; then
    AM_CONDITIONAL([MOTIF1], false)
    AM_CONDITIONAL([ATHENA], true )
    gui_toolkit="Athena"
  else
    AC_MSG_ERROR([no gui toolkit found ...])
  fi
fi
AC_ARG_ENABLE([xshm], [AC_HELP_STRING([--enable-xshm], [add the support of XShm if available [default=yes]])], [], [enable_xshm="yes"])
AC_CHECK_HEADER([sys/ipc.h], [], [enable_xshm="no"])
if test "x${ac_cv_header_sys_ipc_h}" = "xyes"; then
  AC_DEFINE([HAVE_SYS_IPC_H], [1], [Define to 1 if you have the <sys/ipc.h> header file.])
fi
AC_CHECK_HEADER([sys/shm.h], [], [enable_xshm="no"])
if test "x${ac_cv_header_sys_shm_h}" = "xyes"; then
  AC_DEFINE([HAVE_SYS_SHM_H], [1], [Define to 1 if you have the <sys/shm.h> header file.])
fi
AC_CHECK_HEADER([X11/extensions/XShm.h], [], [enable_xshm="no"], [
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
]
)
if test "x${enable_xshm}" = "xyes"; then
  AC_DEFINE([HAVE_XSHM], [1], [Define to 1 if you have the X11-SHM extension.])
fi

AC_C_BIGENDIAN([AC_DEFINE([MSB_FIRST], [1], [MSB First: 68k, sparc, ...])],
               [AC_DEFINE([LSB_FIRST], [1], [LSB First: x86, alpha, ...])],
               [AC_MSG_ERROR([unsupported byte-order ...])])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile src/dev/Makefile src/drv/Makefile src/Xem/Makefile])
AC_OUTPUT
AC_MSG_NOTICE([Configured to build Xcpc:

  GLib20 sdk .... ${have_glib20}
  Motif1 sdk .... ${have_motif1}
  Athena sdk .... ${have_athena}
  gui toolkit ... ${gui_toolkit}
  X11-SHM ....... ${enable_xshm}

Have fun with Xcpc ;-)
])
