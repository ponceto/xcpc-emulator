#
# acinclude.m4 - Copyright (c) 2001, 2021 - Olivier Poncet
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
# AX_CHECK_C99
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_C99], [
AC_PROG_CC_C99
])dnl AX_CHECK_C99

# ----------------------------------------------------------------------------
# AX_CHECK_CXX11
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX11], [
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
])dnl AX_CHECK_CXX11

# ----------------------------------------------------------------------------
# AX_CHECK_CXX14
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX14], [
AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])
])dnl AX_CHECK_CXX14

# ----------------------------------------------------------------------------
# AX_CHECK_CXX17
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX17], [
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])
])dnl AX_CHECK_CXX17

# ----------------------------------------------------------------------------
# AX_RECURSIVE_EVAL
# ----------------------------------------------------------------------------

AC_DEFUN([AX_RECURSIVE_EVAL], [
_lcl_receval="$1"
$2=`(test "x$prefix" = xNONE && prefix="$ac_default_prefix"
     test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
     _lcl_receval_old=''
     while test "[$]_lcl_receval_old" != "[$]_lcl_receval"; do
       _lcl_receval_old="[$]_lcl_receval"
       eval _lcl_receval="\"[$]_lcl_receval\""
     done
     echo "[$]_lcl_receval")`
])dnl AX_RECURSIVE_EVAL

# ----------------------------------------------------------------------------
# AX_DEFINES
# ----------------------------------------------------------------------------

AC_DEFUN([AX_DEFINES], [
AX_RECURSIVE_EVAL("${bindir}", XCPC_BINDIR)
AC_DEFINE_UNQUOTED(XCPC_BINDIR, ["${XCPC_BINDIR}"], [XCPC_BINDIR])
AC_SUBST(XCPC_BINDIR)
AX_RECURSIVE_EVAL("${libdir}", XCPC_LIBDIR)
AC_DEFINE_UNQUOTED(XCPC_LIBDIR, ["${XCPC_LIBDIR}"], [XCPC_LIBDIR])
AC_SUBST(XCPC_LIBDIR)
AX_RECURSIVE_EVAL("${datadir}", XCPC_DATDIR)
AC_DEFINE_UNQUOTED(XCPC_DATDIR, ["${XCPC_DATDIR}"], [XCPC_DATDIR])
AC_SUBST(XCPC_DATDIR)
AX_RECURSIVE_EVAL("${docdir}", XCPC_DOCDIR)
AC_DEFINE_UNQUOTED(XCPC_DOCDIR, ["${XCPC_DOCDIR}"], [XCPC_DOCDIR])
AC_SUBST(XCPC_DOCDIR)
AX_RECURSIVE_EVAL("${datadir}/${PACKAGE_TARNAME}", XCPC_RESDIR)
AC_DEFINE_UNQUOTED(XCPC_RESDIR, ["${XCPC_RESDIR}"], [XCPC_RESDIR])
AC_SUBST(XCPC_RESDIR)
])dnl AX_DEFINES

# ----------------------------------------------------------------------------
# AX_CHECK_BYTE_ORDER
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_BYTE_ORDER], [
AC_C_BIGENDIAN([
AC_DEFINE([MSB_FIRST], [1], [Define to 1 if the host byte-ordering is MSB first.])
], [
AC_DEFINE([LSB_FIRST], [1], [Define to 1 if the host byte-ordering is LSB first.])
], [
AC_MSG_ERROR([unable to determine byte-order])
])
])dnl AX_CHECK_BYTE_ORDER

# ----------------------------------------------------------------------------
# AX_CHECK_HEADERS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_HEADERS], [
AC_HEADER_STDC
AC_HEADER_ASSERT
AC_HEADER_DIRENT
AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([assert.h])
AC_CHECK_HEADERS([direct.h])
AC_CHECK_HEADERS([dir.h])
AC_CHECK_HEADERS([dos.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([linux/fd.h])
AC_CHECK_HEADERS([linux/fdreg.h])
AC_CHECK_HEADERS([pwd.h])
AC_CHECK_HEADERS([shlobj.h])
AC_CHECK_HEADERS([stat.h])
AC_CHECK_HEADERS([sys/farptr.h])
AC_CHECK_HEADERS([sys/ioctl.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/utime.h])
AC_CHECK_HEADERS([termios.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([utime.h])
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADERS([winioctl.h])
AC_CHECK_HEADERS([sys/ipc.h])
AC_CHECK_HEADERS([sys/shm.h])
])dnl AX_CHECK_HEADERS

# ----------------------------------------------------------------------------
# AX_CHECK_FUNCTIONS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_FUNCTIONS], [
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([mkstemp])
AC_CHECK_FUNCS([fork])
AC_CHECK_FUNCS([GetTempFileName])
])dnl AX_CHECK_FUNCTIONS

# ----------------------------------------------------------------------------
# AX_CHECK_ZLIB
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_ZLIB], [
AC_CHECK_HEADERS(zlib.h)
AC_CHECK_LIB(z, zlibVersion)
])dnl AX_CHECK_ZLIB

# ----------------------------------------------------------------------------
# AX_CHECK_BZIP2
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_BZLIB], [
AC_CHECK_HEADERS(bzlib.h)
AC_CHECK_LIB(bz2, BZ2_bzlibVersion)
])dnl AX_CHECK_BZLIB

# ----------------------------------------------------------------------------
# AX_CHECK_X11
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_X11], [
AC_PATH_X
AC_PATH_XTRA
])dnl AX_CHECK_X11

# ----------------------------------------------------------------------------
# AX_CHECK_XSHM
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_XSHM], [
AC_ARG_ENABLE([xshm], [AC_HELP_STRING([--enable-xshm], [add the support of XShm if available [default=yes]])], [], [enable_xshm="yes"])
if test "x${enable_xshm}" = "xyes"; then
AC_CHECK_HEADERS([X11/extensions/XShm.h], [have_xshm="yes"], [have_xshm="no"], [
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
]
)
fi
if test "x${have_xshm}" = "xyes"; then
    AC_DEFINE([HAVE_XSHM], [1], [Define to 1 if you have the X11-SHM extension.])
else
    have_xshm="no"
fi
])
])dnl AX_CHECK_XSHM

# ----------------------------------------------------------------------------
# AX_CHECK_INTRINSIC
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_INTRINSIC], [
AC_CHECK_HEADERS([X11/Intrinsic.h], [have_intrinsic="yes"], [have_intrinsic="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
])dnl AX_CHECK_INTRINSIC

# ----------------------------------------------------------------------------
# AX_CHECK_ATHENA
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_ATHENA], [
AC_ARG_ENABLE([athena], [AC_HELP_STRING([--enable-athena], [add the support of Athena if available [default=yes]])], [], [enable_athena="yes"])
if test "x${enable_athena}" = "xyes"; then
AC_CHECK_HEADERS([X11/Xaw/XawInit.h], [have_athena="yes"], [have_athena="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
else
    have_athena="no"
fi
])dnl AX_CHECK_ATHENA

# ----------------------------------------------------------------------------
# AX_CHECK_MOTIF2
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_MOTIF2], [
AC_ARG_ENABLE([motif2], [AC_HELP_STRING([--enable-motif2], [add the support of Motif2 if available [default=yes]])], [], [enable_motif2="yes"])
if test "x${enable_motif2}" = "xyes"; then
AC_CHECK_HEADERS([Xm/Xm.h], [have_motif2="yes"], [have_motif2="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
else
    have_motif2="no"
fi
])dnl AX_CHECK_MOTIF2

# ----------------------------------------------------------------------------
# AX_CHECK_GTK3
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_GTK3], [
AC_ARG_ENABLE([gtk3], [AC_HELP_STRING([--enable-gtk3], [add the support of Gtk3 if available [default=yes]])], [], [enable_gtk3="yes"])
if test "x${enable_gtk3}" = "xyes"; then
    PKG_CHECK_MODULES([gtk3], [gtk+-3.0], [have_gtk3="yes"], [have_gtk3="no"])
else
    have_gtk3="no"
fi
])dnl AX_CHECK_GTK3

# ----------------------------------------------------------------------------
# AX_CHECK_GTK4
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_GTK4], [
AC_ARG_ENABLE([gtk4], [AC_HELP_STRING([--enable-gtk4], [add the support of Gtk4 if available [default=yes]])], [], [enable_gtk4="no"])
if test "x${enable_gtk4}" = "xyes"; then
    PKG_CHECK_MODULES([gtk4], [gtk4], [have_gtk4="yes"], [have_gtk4="no"])
else
    have_gtk4="no"
fi
])dnl AX_CHECK_GTK4

# ----------------------------------------------------------------------------
# AX_CHECK_X11_TOOLKIT
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_X11_TOOLKIT], [
AC_ARG_WITH([x11-toolkit], [AC_HELP_STRING([--with-x11-toolkit], [select the graphical toolkit (gtk4, gtk3, motif2, athena, intrinsic)])])
if test "x${with_x11_toolkit}${have_gtk4}" = "xyes"; then
    with_x11_toolkit="gtk4"
fi
if test "x${with_x11_toolkit}${have_gtk3}" = "xyes"; then
    with_x11_toolkit="gtk3"
fi
if test "x${with_x11_toolkit}${have_motif2}" = "xyes"; then
    with_x11_toolkit="motif2"
fi
if test "x${with_x11_toolkit}${have_athena}" = "xyes"; then
    with_x11_toolkit="athena"
fi
if test "x${with_x11_toolkit}${have_intrinsic}" = "xyes"; then
    with_x11_toolkit="intrinsic"
fi
if test "x${with_x11_toolkit}" = "xgtk4"; then
    if test "x${have_gtk4}" != "xyes"; then
        AC_MSG_ERROR([gtk4 toolkit was not found])
    fi
fi
if test "x${with_x11_toolkit}" = "xgtk3"; then
    if test "x${have_gtk3}" != "xyes"; then
        AC_MSG_ERROR([gtk3 toolkit was not found])
    fi
fi
if test "x${with_x11_toolkit}" = "xmotif2"; then
    if test "x${have_motif2}" != "xyes"; then
        AC_MSG_ERROR([motif2 toolkit was not found])
    fi
fi
if test "x${with_x11_toolkit}" = "xathena"; then
    if test "x${have_athena}" != "xyes"; then
        AC_MSG_ERROR([athena toolkit was not found])
    fi
fi
if test "x${with_x11_toolkit}" = "xintrinsic"; then
    if test "x${have_intrinsic}" != "xyes"; then
        AC_MSG_ERROR([intrinsic toolkit was not found])
    fi
fi
case "${with_x11_toolkit}" in
    gtk4)
        AM_CONDITIONAL([GTK4],      true )
        AM_CONDITIONAL([GTK3],      false)
        AM_CONDITIONAL([MOTIF2],    false)
        AM_CONDITIONAL([ATHENA],    false)
        AM_CONDITIONAL([INTRINSIC], false)
        AM_CONDITIONAL([LIBXEM],    false)
        ;;
    gtk3)
        AM_CONDITIONAL([GTK4],      false)
        AM_CONDITIONAL([GTK3],      true )
        AM_CONDITIONAL([MOTIF2],    false)
        AM_CONDITIONAL([ATHENA],    false)
        AM_CONDITIONAL([INTRINSIC], false)
        AM_CONDITIONAL([LIBXEM],    false)
        ;;
    motif2)
        AM_CONDITIONAL([GTK4],      false)
        AM_CONDITIONAL([GTK3],      false)
        AM_CONDITIONAL([MOTIF2],    true )
        AM_CONDITIONAL([ATHENA],    false)
        AM_CONDITIONAL([INTRINSIC], false)
        AM_CONDITIONAL([LIBXEM],    true )
        ;;
    athena)
        AM_CONDITIONAL([GTK4],      false)
        AM_CONDITIONAL([GTK3],      false)
        AM_CONDITIONAL([MOTIF2],    false)
        AM_CONDITIONAL([ATHENA],    true )
        AM_CONDITIONAL([INTRINSIC], false)
        AM_CONDITIONAL([LIBXEM],    true )
        ;;
    intrinsic)
        AM_CONDITIONAL([GTK4],      false)
        AM_CONDITIONAL([GTK3],      false)
        AM_CONDITIONAL([MOTIF2],    false)
        AM_CONDITIONAL([ATHENA],    false)
        AM_CONDITIONAL([INTRINSIC], true )
        AM_CONDITIONAL([LIBXEM],    true )
        ;;
    *)
        AC_MSG_ERROR([no usable X11 toolkit was found])
        ;;
esac
])dnl AX_CHECK_X11_TOOLKIT

# ----------------------------------------------------------------------------
# AX_CHECK_PORTAUDIO
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_PORTAUDIO], [
AC_ARG_ENABLE([portaudio], [AC_HELP_STRING([--enable-portaudio], [add the support of PortAudio if available [default=yes]])], [], [enable_portaudio="yes"])
if test "x${enable_portaudio}" = "xyes"; then
    PKG_CHECK_MODULES([portaudio], portaudio-2.0 >= 19, [have_portaudio="yes"], [have_portaudio="no"])
else
    have_portaudio="no"
fi
if test "x${have_portaudio}" = "xyes"; then
    AM_CONDITIONAL([PORTAUDIO], true)
else
    AM_CONDITIONAL([PORTAUDIO], false)
fi
])dnl AX_CHECK_PORTAUDIO

# ----------------------------------------------------------------------------
# AX_CHECK_LINUX_JOYSTICK_API
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_LINUX_JOYSTICK_API], [
AC_ARG_ENABLE([linux-joystick-api], [AC_HELP_STRING([--enable-linux-joystick-api], [add the support of Linux Joystick API if available [default=yes]])], [], [enable_linux_joystick_api="yes"])
if test "x${enable_linux_joystick_api}" = "xyes"; then
    AC_CHECK_HEADERS([linux/joystick.h], [have_linux_joystick_api="yes"], [have_linux_joystick_api="no"])
else
    have_linux_joystick_api="no"
fi
if test "x${have_linux_joystick_api}" = "xyes"; then
    AM_CONDITIONAL([LINUX_JOYSTICK_API], true)
else
    AM_CONDITIONAL([LINUX_JOYSTICK_API], false)
fi
])dnl AX_CHECK_LINUX_JOYSTICK_API

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
