#
# ax_check_gui_toolkit.m4 - Copyright (c) 2001-2025 - Olivier Poncet
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
# AX_CHECK_TOOLKITS
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_TOOLKITS], [
AX_CHECK_X11
AX_CHECK_XSHM
AX_CHECK_GTK3
AX_CHECK_GTK4
])

# ----------------------------------------------------------------------------
# AX_CHECK_GUI_TOOLKIT
# ----------------------------------------------------------------------------

AC_DEFUN([AX_CHECK_GUI_TOOLKIT], [
AX_CHECK_TOOLKITS
AC_ARG_WITH([gui-toolkit], [AS_HELP_STRING([--with-gui-toolkit], [select the graphical user interface toolkit (gtk4, gtk3)])])
if test "x${with_gui_toolkit}${have_gtk4}" = 'xyes'; then
    with_gui_toolkit='gtk4'
fi
if test "x${with_gui_toolkit}${have_gtk3}" = 'xyes'; then
    with_gui_toolkit='gtk3'
fi
if test "x${with_gui_toolkit}" = 'xgtk4'; then
    if test "x${have_gtk4}" != 'xyes'; then
        AC_MSG_ERROR([gtk4 toolkit was not found])
    fi
fi
if test "x${with_gui_toolkit}" = 'xgtk3'; then
    if test "x${have_gtk3}" != 'xyes'; then
        AC_MSG_ERROR([gtk3 toolkit was not found])
    fi
fi
case "${with_gui_toolkit}" in
    gtk4)
        AC_DEFINE([USE_GTK4], [1], [Define to 1 if you want the gtk4 interface.])
        AM_CONDITIONAL([USE_GTK4], true )
        AM_CONDITIONAL([USE_GTK3], false)
        ;;
    gtk3)
        AC_DEFINE([USE_GTK3], [1], [Define to 1 if you want the gtk3 interface.])
        AM_CONDITIONAL([USE_GTK4], false)
        AM_CONDITIONAL([USE_GTK3], true )
        ;;
    *)
        AC_MSG_ERROR([no usable GUI toolkit was found])
        ;;
esac
])

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
