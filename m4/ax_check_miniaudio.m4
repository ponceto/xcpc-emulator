#
# ax_check_miniaudio.m4 - Copyright (c) 2001-2026 - Olivier Poncet
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
# AX_CHECK_MINIAUDIO
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_MINIAUDIO], [
AC_ARG_ENABLE([miniaudio], [AS_HELP_STRING([--enable-miniaudio], [add the support of miniaudio (builtin) [default=yes]])], [], [enable_miniaudio='yes'])
if test "x${enable_miniaudio}" = 'xyes'; then
    have_miniaudio='yes'
else
    have_miniaudio='no'
fi
if test "x${have_miniaudio}" = 'xyes'; then
    AC_DEFINE([HAVE_MINIAUDIO], [1], [Define to 1 if you have the miniaudio library.])
    AC_DEFINE([USE_MINIAUDIO], [1], [Define to 1 if you want the miniaudio library.])
    AM_CONDITIONAL([HAVE_MINIAUDIO], true)
    AM_CONDITIONAL([USE_MINIAUDIO], true)
else
    AM_CONDITIONAL([HAVE_MINIAUDIO], false)
    AM_CONDITIONAL([USE_MINIAUDIO], false)
fi
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
