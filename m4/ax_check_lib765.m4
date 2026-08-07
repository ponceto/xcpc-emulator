#
# ax_check_lib765.m4 - Copyright (c) 2001-2026 - Olivier Poncet
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
# AX_CHECK_LIB765
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_LIB765], [
AC_ARG_ENABLE([lib765], [AS_HELP_STRING([--enable-lib765], [add the support of lib765 (builtin) [default=yes]])], [], [enable_lib765='yes'])
if test "x${enable_lib765}" = 'xyes'; then
    have_lib765='yes'
else
    have_lib765='no'
fi
if test "x${have_lib765}" = 'xyes'; then
    AC_DEFINE([HAVE_LIB765], [1], [Define to 1 if you have the lib765 library.])
    AC_DEFINE([USE_LIB765], [1], [Define to 1 if you want the lib765 library.])
    AM_CONDITIONAL([HAVE_LIB765], true)
    AM_CONDITIONAL([USE_LIB765], true)
else
    AM_CONDITIONAL([HAVE_LIB765], false)
    AM_CONDITIONAL([USE_LIB765], false)
fi
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
