#!/bin/sh
#
# ci-build-all.sh - Copyright (c) 2001-2023 - Olivier Poncet
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# ----------------------------------------------------------------------------
# variables
# ----------------------------------------------------------------------------

arg_prefix="/opt/xcpc"
arg_jobs="$(cat /proc/cpuinfo | grep '^processor' | wc -l)"
arg_builddir="_build"
arg_tarball="$(ls xcpc-*.tar.gz 2>/dev/null)"
arg_sources="$(echo "${arg_tarball:-not-set}" | sed -e 's/\.tar\.gz//g')"

# ----------------------------------------------------------------------------
# sanity checks
# ----------------------------------------------------------------------------

if [ ! -f "${arg_tarball}" ]
then
    echo "*** tarball not found ***"
    exit 1
fi

# ----------------------------------------------------------------------------
# debug
# ----------------------------------------------------------------------------

set -x

# ----------------------------------------------------------------------------
# build the project
# ----------------------------------------------------------------------------

rm -rf "${arg_builddir}"                                             || exit 1
mkdir "${arg_builddir}"                                              || exit 1
cd "${arg_builddir}"                                                 || exit 1
tar xf "../${arg_tarball}"                                           || exit 1
cd "${arg_sources}"                                                  || exit 1
./configure --prefix="${arg_prefix}"                                 || exit 1
make -j "${arg_jobs}"                                                || exit 1
cd "../"                                                             || exit 1
cd "../"                                                             || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
