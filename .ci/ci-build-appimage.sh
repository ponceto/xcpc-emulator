#!/bin/sh
#
# ci-build-appimage.sh - Copyright (c) 2001-2026 - Olivier Poncet
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
arg_prefix="/usr"
arg_jobs="$(nproc)"
arg_builddir="${arg_topdir}/_build"
arg_distdir="${arg_topdir}/_dist"
arg_tarball="$(ls xcpc-*.tar.gz 2>/dev/null | grep '^xcpc-[0-9]\+.[0-9]\+.[0-9]\+.tar.gz')"
arg_pkgname="$(echo "${arg_tarball:-not-set}" | sed -e 's/\.tar\.gz//g')"
arg_machine="$(uname -m 2>/dev/null)"
arg_linuxdeploy_bin="linuxdeploy-${arg_machine}.AppImage"
arg_linuxdeploy_url="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous"
arg_appimagetool_bin="appimagetool-${arg_machine}.AppImage"
arg_appimagetool_url="https://github.com/AppImage/appimagetool/releases/download/continuous"

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
# download linuxdeploy
# ----------------------------------------------------------------------------

rm -f "${arg_linuxdeploy_bin}"                                       || exit 1
wget "${arg_linuxdeploy_url}/${arg_linuxdeploy_bin}"                 || exit 1
chmod 755 "${arg_linuxdeploy_bin}"                                   || exit 1

# ----------------------------------------------------------------------------
# download appimagetool
# ----------------------------------------------------------------------------

rm -f "${arg_appimagetool_bin}"                                      || exit 1
wget "${arg_appimagetool_url}/${arg_appimagetool_bin}"               || exit 1
chmod 755 "${arg_appimagetool_bin}"                                  || exit 1

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
# linuxdeploy
# ----------------------------------------------------------------------------

export APPIMAGE_EXTRACT_AND_RUN="1"
${arg_topdir}/${arg_linuxdeploy_bin} --appdir "${arg_distdir}" --create-desktop-file --executable="${arg_distdir}/${arg_prefix}/bin/xcpc" --desktop-file="${arg_distdir}/${arg_prefix}/share/applications/xcpc.desktop" --icon-file="${arg_distdir}/${arg_prefix}/share/pixmaps/xcpc.png" || exit 1

# ----------------------------------------------------------------------------
# appimagetool
# ----------------------------------------------------------------------------

export APPIMAGE_EXTRACT_AND_RUN="1"
${arg_topdir}/${arg_appimagetool_bin} "${arg_distdir}" "${arg_pkgname}_${arg_machine}.AppImage" || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
