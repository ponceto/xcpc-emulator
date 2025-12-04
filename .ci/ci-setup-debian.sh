#!/bin/sh
#
# ci-setup-debian.sh - Copyright (c) 2001-2025 - Olivier Poncet
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

arg_packages="
build-essential
devscripts
debhelper
fakeroot
dh-make
dh-exec
equivs
autoconf
autoconf-archive
automake
libtool
cmake
bison
flex
git
xorg-dev
zlib1g-dev
libbz2-dev
"

# ----------------------------------------------------------------------------
# check command-line
# ----------------------------------------------------------------------------

case "${1:-default}" in
    gtk3)
        arg_packages="${arg_packages} libgtk-3-dev"
        ;;
    gtk4)
        arg_packages="${arg_packages} libgtk-4-dev"
        ;;
    *)
        arg_packages="${arg_packages} libgtk-3-dev"
        ;;
esac

# ----------------------------------------------------------------------------
# enable/disable debug mode
# ----------------------------------------------------------------------------

set -x

# ----------------------------------------------------------------------------
# debian specific variables
# ----------------------------------------------------------------------------

export DEBIAN_FRONTEND="noninteractive"
export DEBIAN_PRIORITY="critical"

# ----------------------------------------------------------------------------
# update the package manager
# ----------------------------------------------------------------------------

apt-get update                                                       || exit 1

# ----------------------------------------------------------------------------
# upgrade the whole system
# ----------------------------------------------------------------------------

apt-get dist-upgrade -y                                              || exit 1

# ----------------------------------------------------------------------------
# install the dependencies
# ----------------------------------------------------------------------------

apt-get install -y ${arg_packages}                                   || exit 1

# ----------------------------------------------------------------------------
# remove packages that are no longer needed
# ----------------------------------------------------------------------------

apt-get autoremove --purge -y                                        || exit 1

# ----------------------------------------------------------------------------
# clean the local repository
# ----------------------------------------------------------------------------

apt-get clean                                                        || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
