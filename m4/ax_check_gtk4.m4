#
# ax_check_gtk4.m4 - Copyright (c) 2001-2024 - Olivier Poncet
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
# AX_CHECK_GTK4
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_GTK4], [
AC_ARG_ENABLE([gtk4], [AS_HELP_STRING([--enable-gtk4], [add the support of gtk4 (if available) [default=yes]])], [], [enable_gtk4='yes'])
if test "x${enable_gtk4}" = 'xyes'; then
    PKG_CHECK_MODULES([gtk4], [gtk4], [have_gtk4='unsupported'], [have_gtk4='no'])
else
    have_gtk4='no'
fi
if test "x${have_gtk4}" = 'xyes'; then
    AC_DEFINE([HAVE_GTK4], [1], [Define to 1 if you have the gtk4 library.])
    AM_CONDITIONAL([HAVE_GTK4], true)
else
    AM_CONDITIONAL([HAVE_GTK4], false)
fi
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
