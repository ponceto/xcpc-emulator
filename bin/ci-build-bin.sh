#!/bin/sh
#
# ci-build-bin.sh - Copyright (c) 2001-2023 - Olivier Poncet
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
# settings
# ----------------------------------------------------------------------------

arg_topdir="$(pwd)"
arg_prefix="/usr/local"
arg_jobs="$(cat /proc/cpuinfo | grep '^processor' | wc -l)"
arg_builddir="_build"
arg_distdir="_dist"
arg_tarball="$(ls xcpc-*.tar.gz 2>/dev/null | grep '^xcpc-[0-9]\+.[0-9]\+.[0-9]\+.tar.gz')"
arg_pkgname="$(echo "${arg_tarball:-not-set}" | sed -e 's/\.tar\.gz//g')"
arg_system="unknown"

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
# build the binary package
# ----------------------------------------------------------------------------

rm -rf "${arg_builddir}"                                             || exit 1
mkdir "${arg_builddir}"                                              || exit 1
cd "${arg_builddir}"                                                 || exit 1
tar xf "../${arg_tarball}"                                           || exit 1
cd "${arg_pkgname}"                                                  || exit 1
arg_system="$(./config.guess 2>/dev/null)"                           || exit 1
./configure --prefix="${arg_prefix}"                                 || exit 1
make -j "${arg_jobs}"                                                || exit 1
make DESTDIR="$(pwd)/${arg_distdir}" install                         || exit 1
cd "${arg_distdir}"                                                  || exit 1
tar cvzf "../${arg_pkgname}_${arg_system}.tar.gz" "."                || exit 1
cd "../"                                                             || exit 1
mv "${arg_pkgname}_${arg_system}.tar.gz" "${arg_topdir}"             || exit 1
cd "${arg_topdir}"                                                   || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
