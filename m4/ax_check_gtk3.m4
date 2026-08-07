#
# ax_check_gtk3.m4 - Copyright (c) 2001-2026 - Olivier Poncet
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
# AX_CHECK_GTK3
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_GTK3], [
AC_ARG_ENABLE([gtk3], [AS_HELP_STRING([--enable-gtk3], [add the support of gtk3 (if available) [default=yes]])], [], [enable_gtk3='yes'])
if test "x${enable_gtk3}" = 'xyes'; then
    PKG_CHECK_MODULES([gtk3], [gtk+-3.0 epoxy], [have_gtk3='yes'], [have_gtk3='no'])
else
    have_gtk3='no'
fi
if test "x${have_gtk3}" = 'xyes'; then
    AC_DEFINE([HAVE_GTK3], [1], [Define to 1 if you have the gtk3 library.])
    AM_CONDITIONAL([HAVE_GTK3], true)
else
    AM_CONDITIONAL([HAVE_GTK3], false)
fi
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
