#
# ax_check_libdsk.m4 - Copyright (c) 2001-2025 - Olivier Poncet
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
# AX_CHECK_LIBDSK
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_LIBDSK], [
AC_ARG_ENABLE([libdsk], [AS_HELP_STRING([--enable-libdsk], [add the support of libdsk (builtin) [default=yes]])], [], [enable_libdsk='yes'])
if test "x${enable_libdsk}" = 'xyes'; then
    have_libdsk='yes'
else
    have_libdsk='no'
fi
if test "x${have_libdsk}" = 'xyes'; then
    AC_DEFINE([HAVE_LIBDSK], [1], [Define to 1 if you have the libdsk library.])
    AC_DEFINE([USE_LIBDSK], [1], [Define to 1 if you want the libdsk library.])
    AM_CONDITIONAL([HAVE_LIBDSK], true)
    AM_CONDITIONAL([USE_LIBDSK], true)
else
    AM_CONDITIONAL([HAVE_LIBDSK], false)
    AM_CONDITIONAL([USE_LIBDSK], false)
fi
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
