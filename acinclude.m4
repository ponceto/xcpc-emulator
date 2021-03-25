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
# AX_CHECK_TYPES
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_TYPES], [
AC_TYPE_INT8_T
if test "x${ac_cv_c_int8_t}" != "xyes"
then
    AC_MSG_ERROR([int8_t was not found])
fi
AC_TYPE_UINT8_T
if test "x${ac_cv_c_uint8_t}" != "xyes"
then
    AC_MSG_ERROR([uint8_t was not found])
fi
AC_TYPE_INT16_T
if test "x${ac_cv_c_int16_t}" != "xyes"
then
    AC_MSG_ERROR([int16_t was not found])
fi
AC_TYPE_UINT16_T
if test "x${ac_cv_c_uint16_t}" != "xyes"
then
    AC_MSG_ERROR([uint16_t was not found])
fi
AC_TYPE_INT32_T
if test "x${ac_cv_c_int32_t}" != "xyes"
then
    AC_MSG_ERROR([int32_t was not found])
fi
AC_TYPE_UINT32_T
if test "x${ac_cv_c_uint32_t}" != "xyes"
then
    AC_MSG_ERROR([uint32_t was not found])
fi
AC_TYPE_INT64_T
if test "x${ac_cv_c_int64_t}" != "xyes"
then
    AC_MSG_ERROR([int64_t was not found])
fi
AC_TYPE_UINT64_T
if test "x${ac_cv_c_uint64_t}" != "xyes"
then
    AC_MSG_ERROR([uint64_t was not found])
fi
])dnl AX_CHECK_TYPES

# ----------------------------------------------------------------------------
# AX_CHECK_ENDIANNESS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_ENDIANNESS], [
AC_C_BIGENDIAN([AC_DEFINE([MSB_FIRST], [1], [MSB First: 68k, sparc, ...])],
               [AC_DEFINE([LSB_FIRST], [1], [LSB First: x86, alpha, ...])],
               [AC_MSG_ERROR([unsupported byte-order ...])])
])dnl AX_CHECK_ENDIANNESS

# ----------------------------------------------------------------------------
# AX_CHECK_X11
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_X11], [
AC_PATH_X
AC_PATH_XTRA
])dnl AX_CHECK_X11

# ----------------------------------------------------------------------------
# AX_CHECK_XMU
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_XMU], [
AC_CHECK_HEADER([X11/Xmu/Xmu.h], [have_x11xmu="yes"], [have_x11xmu="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
])dnl AX_CHECK_XMU

# ----------------------------------------------------------------------------
# AX_CHECK_XAW
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_XAW], [
AC_ARG_WITH([athena], [AC_HELP_STRING([--with-athena], [build the Athena version])])
AC_CHECK_HEADER([X11/Xaw/XawInit.h], [have_athena="yes"], [have_athena="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
])dnl AX_CHECK_XAW

# ----------------------------------------------------------------------------
# AX_CHECK_XM2
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_XM2], [
AC_ARG_WITH([motif2], [AC_HELP_STRING([--with-motif2], [build the Motif2 version])])
AC_CHECK_HEADER([Xm/Xm.h], [have_motif2="yes"], [have_motif2="no"], [
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
])
])dnl AX_CHECK_XM2

# ----------------------------------------------------------------------------
# AX_CHECK_XSHM
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_XSHM], [
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
])
])dnl AX_CHECK_XSHM

# ----------------------------------------------------------------------------
# AX_CHECK_GUI
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_GUI], [
if test "x${x11_toolkit}${with_motif2}" = "xyes"; then
    if test "x${have_motif2}" = "xyes"; then
        AM_CONDITIONAL([ATHENA], false)
        AM_CONDITIONAL([MOTIF2], true )
        x11_toolkit="Motif2"
    else
        AC_MSG_ERROR([Motif2 toolkit was not found])
    fi
fi
if test "x${x11_toolkit}${with_athena}" = "xyes"; then
    if test "x${have_athena}" = "xyes"; then
        AM_CONDITIONAL([ATHENA], true )
        AM_CONDITIONAL([MOTIF2], false)
        x11_toolkit="Athena"
    else
        AC_MSG_ERROR([Athena toolkit was not found])
    fi
fi
if test "x${x11_toolkit}${have_motif2}" = "xyes"; then
    AM_CONDITIONAL([ATHENA], false)
    AM_CONDITIONAL([MOTIF2], true )
    x11_toolkit="Motif2"
fi
if test "x${x11_toolkit}${have_athena}" = "xyes"; then
    AM_CONDITIONAL([ATHENA], true )
    AM_CONDITIONAL([MOTIF2], false)
    x11_toolkit="Athena"
fi
if test "no${x11_toolkit}ne" = "none"; then
    AC_MSG_ERROR([Graphical toolkit was not found])
fi
])dnl AX_CHECK_GUI

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
# End-Of-File
# ----------------------------------------------------------------------------
