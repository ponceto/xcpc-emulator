#!/bin/sh
#
# ci-build-debian.sh - Copyright (c) 2001-2026 - Olivier Poncet
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
# check logname
# ----------------------------------------------------------------------------

if [ "${LOGNAME:-not-set}" = 'not-set' ]
then
    export LOGNAME="$(whoami)"
fi

# ----------------------------------------------------------------------------
# debug
# ----------------------------------------------------------------------------

set -x

# ----------------------------------------------------------------------------
# build the debian package
# ----------------------------------------------------------------------------

rm -rf "${arg_builddir}"                                             || exit 1
mkdir "${arg_builddir}"                                              || exit 1
cd "${arg_builddir}"                                                 || exit 1
tar xf "../${arg_tarball}"                                           || exit 1
cd "${arg_pkgname}"                                                  || exit 1
dh_make --yes --single --file "../../${arg_tarball}"                 || exit 1
rm -rf "debian"                                                      || exit 1
cp -rf "../../debian" "./debian"                                     || exit 1
dpkg-buildpackage --build=full --no-sign                             || exit 1
cd "${arg_topdir}"                                                   || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
