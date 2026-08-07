#
# ax_check_byte_order.m4 - Copyright (c) 2001-2026 - Olivier Poncet
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
# AX_CHECK_BYTE_ORDER
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_BYTE_ORDER], [
AC_C_BIGENDIAN([
AC_DEFINE([MSB_FIRST], [1], [Define to 1 if the host byte-ordering is MSB first.])
], [
AC_DEFINE([LSB_FIRST], [1], [Define to 1 if the host byte-ordering is LSB first.])
], [
AC_MSG_ERROR([unable to determine the target byte-order])
])
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
