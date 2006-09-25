# configure.ac - Copyright (c) 2001, 2006 Olivier Poncet
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

AC_INIT([xcpc], [20060925], [ponceto@noos.fr])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PATH_X
AC_PATH_XTRA
AC_ARG_WITH([motif1], AC_HELP_STRING([--with-motif1], [build the Motif1 version]))
AC_ARG_WITH([athena], AC_HELP_STRING([--with-athena], [build the Athena version]))
if   test "x${with_motif1}" = "xyes"; then
  AM_CONDITIONAL([MOTIF1], true )
  AM_CONDITIONAL([ATHENA], false)
elif test "x${with_athena}" = "xyes"; then
  AM_CONDITIONAL([MOTIF1], false)
  AM_CONDITIONAL([ATHENA], true )
else
  AM_CONDITIONAL([MOTIF1], true )
  AM_CONDITIONAL([ATHENA], false)
fi
AC_C_BIGENDIAN([AC_DEFINE([MSB_FIRST], [1], [MSB First: 68k, sparc, ...])],
               [AC_DEFINE([LSB_FIRST], [1], [LSB First: x86, alpha, ...])],
               [AC_ERROR([Unsupported byte-order ...])])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
