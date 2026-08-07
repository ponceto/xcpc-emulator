#!/bin/sh
#
# ci-build-binary.sh - Copyright (c) 2001-2026 - Olivier Poncet
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
arg_jobs="$(nproc)"
arg_builddir="${arg_topdir}/_build"
arg_distdir="${arg_topdir}/_dist"
arg_tarball="$(ls xcpc-*.tar.gz 2>/dev/null | grep '^xcpc-[0-9]\+.[0-9]\+.[0-9]\+.tar.gz')"
arg_pkgname="$(echo "${arg_tarball:-not-set}" | sed -e 's/\.tar\.gz//g')"
arg_machine="$(uname -m 2>/dev/null)"

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
# cleanup
# ----------------------------------------------------------------------------

rm -rf "${arg_builddir}"                                             || exit 1
mkdir "${arg_builddir}"                                              || exit 1
rm -rf "${arg_distdir}"                                              || exit 1
mkdir "${arg_distdir}"                                               || exit 1

# ----------------------------------------------------------------------------
# build the binary package
# ----------------------------------------------------------------------------

cd "${arg_builddir}"                                                 || exit 1
tar xf "${arg_topdir}/${arg_tarball}"                                || exit 1
cd "${arg_pkgname}"                                                  || exit 1
./configure --prefix="${arg_prefix}"                                 || exit 1
make -j "${arg_jobs}"                                                || exit 1
make DESTDIR="${arg_distdir}" install                                || exit 1
cd "${arg_topdir}"                                                   || exit 1

# ----------------------------------------------------------------------------
# tarball
# ----------------------------------------------------------------------------

cd "${arg_distdir}"                                                  || exit 1
tar cvzf "${arg_topdir}/${arg_pkgname}_${arg_machine}.tar.gz" "."    || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
