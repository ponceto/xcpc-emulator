#
# acinclude.m4 - Copyright (c) 2001-2025 - Olivier Poncet
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
AC_PROG_CC
])

# ----------------------------------------------------------------------------
# AX_CHECK_CXX11
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX11], [
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
])

# ----------------------------------------------------------------------------
# AX_CHECK_CXX14
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX14], [
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])
])

# ----------------------------------------------------------------------------
# AX_CHECK_CXX17
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_CXX17], [
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])
])

# ----------------------------------------------------------------------------
# AX_CHECK_HEADERS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_HEADERS], [
AC_HEADER_ASSERT
AC_HEADER_DIRENT
AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
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
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([sys/utime.h])
AC_CHECK_HEADERS([termios.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([utime.h])
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADERS([winioctl.h])
AC_CHECK_HEADERS([sys/ipc.h])
AC_CHECK_HEADERS([sys/shm.h])
])

# ----------------------------------------------------------------------------
# AX_CHECK_FUNCTIONS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_FUNCTIONS], [
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([mkstemp])
AC_CHECK_FUNCS([fork])
AC_CHECK_FUNCS([GetTempFileName])
])

# ----------------------------------------------------------------------------
# AX_CHECK_PTHREAD
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_PTHREAD], [
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_LIB(pthread, pthread_create)
])

# ----------------------------------------------------------------------------
# AX_CHECK_LIBDL
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_LIBDL], [
AC_CHECK_HEADERS(dlfcn.h)
AC_CHECK_LIB(dl, dlopen)
])

# ----------------------------------------------------------------------------
# AX_CHECK_ZLIB
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_ZLIB], [
AC_CHECK_HEADERS(zlib.h)
AC_CHECK_LIB(z, zlibVersion)
])

# ----------------------------------------------------------------------------
# AX_CHECK_BZIP2
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_BZLIB], [
AC_CHECK_HEADERS(bzlib.h)
AC_CHECK_LIB(bz2, BZ2_bzlibVersion)
])

# ----------------------------------------------------------------------------
# AX_CHECK_LINUX_JOYSTICK_API
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_LINUX_JOYSTICK_API], [
AC_ARG_ENABLE([linux-joystick-api], [AS_HELP_STRING([--enable-linux-joystick-api], [add the support of Linux Joystick API if available [default=yes]])], [], [enable_linux_joystick_api='yes'])
if test "x${enable_linux_joystick_api}" = 'xyes'; then
    AC_CHECK_HEADERS([linux/joystick.h], [have_linux_joystick_api='yes'], [have_linux_joystick_api='no'])
else
    have_linux_joystick_api='no'
fi
if test "x${have_linux_joystick_api}" = 'xyes'; then
    AC_DEFINE([HAVE_LINUX_JOYSTICK_API], [1], [Define to 1 if you have the Linux Joystick API.])
fi
if test "x${have_linux_joystick_api}" = 'xyes'; then
    AM_CONDITIONAL([HAVE_LINUX_JOYSTICK_API], true)
else
    AM_CONDITIONAL([HAVE_LINUX_JOYSTICK_API], false)
fi
])

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
AX_RECURSIVE_EVAL("${datadir}/${PACKAGE_TARNAME}/roms", XCPC_ROMDIR)
AC_DEFINE_UNQUOTED(XCPC_ROMDIR, ["${XCPC_ROMDIR}"], [XCPC_ROMDIR])
AC_SUBST(XCPC_ROMDIR)
AX_RECURSIVE_EVAL("${datadir}/${PACKAGE_TARNAME}/disks", XCPC_DSKDIR)
AC_DEFINE_UNQUOTED(XCPC_DSKDIR, ["${XCPC_DSKDIR}"], [XCPC_DSKDIR])
AC_SUBST(XCPC_DSKDIR)
AX_RECURSIVE_EVAL("${datadir}/${PACKAGE_TARNAME}/snapshots", XCPC_SNADIR)
AC_DEFINE_UNQUOTED(XCPC_SNADIR, ["${XCPC_SNADIR}"], [XCPC_SNADIR])
AC_SUBST(XCPC_SNADIR)
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
