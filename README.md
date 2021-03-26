# README

## How to install Xcpc

The xcpc emulator is fully autotoolized.

### Dependencies

Under Debian or derivatives (Ubuntu, Mint, ...), please install first these dependencies

Mandatory dependencies :

```
build-essential
autoconf
automake
libtool
git
xorg-dev
libmotif-dev
```

Optionnal dependencies :

```
zlib1g-dev
libbz2-dev
```

### Generate the configure script

Generate the `configure` script if it does not exists (i.e in case you just have cloned the repository)

```
autoreconf -v -i -f
```

### Configure the sources

Run the `configure` script

```
./configure --prefix={destination-path}
```

### Build the emulator

Build the emulator

```
make -j4
```

### Install the emulator

Install the emulator

```
make install
```

### Run the emulator

Run the emulator

```
{destination-path}/bin/xcpc
```

### Desktop integration

A XDG compliant `.desktop` file is provided, so you can copy or symlink this file in a relevant directory

For example, in the system directory

```
ln -sf {destination-path}/share/applications/xcpc.desktop /usr/share/applications/xcpc.desktop
```

For example, or the user directory

```
ln -sf {destination-path}/share/applications/xcpc.desktop ${HOME}/.local/share/applications/xcpc.desktop
```

## How to install into your home directory

Quick installation instructions

```
autoreconf -v -i -f
./configure --prefix=${HOME}/Apps/xcpc
make -j4
make install
ln -sf {HOME}/Apps/xcpc/share/applications/xcpc.desktop ${HOME}/.local/share/applications/xcpc.desktop
```

You can now run the emulator from your desktop menu `Games > Xcpc`

## How to run Xcpc

```
Usage: xcpc [toolkit-options] [program-options]

Help options:
    --help                      display this help and exit
    --version                   display the version and exit

Emulation options:
    --model={value}             cpc464, cpc664, cpc6128
    --monitor={value}           color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm12
    --keyboard={value}          qwerty, azerty
    --refresh={value}           50Hz, 60Hz
    --manufacturer={value}      Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad
    --sysrom={filename}         32Kb system rom
    --rom000={filename}         16Kb expansion rom #00
    --rom001={filename}         16Kb expansion rom #01
    --rom002={filename}         16Kb expansion rom #02
    --rom003={filename}         16Kb expansion rom #03
    --rom004={filename}         16Kb expansion rom #04
    --rom005={filename}         16Kb expansion rom #05
    --rom006={filename}         16Kb expansion rom #07
    --rom007={filename}         16Kb expansion rom #08
    --rom008={filename}         16Kb expansion rom #09
    --rom009={filename}         16Kb expansion rom #10
    --rom010={filename}         16Kb expansion rom #11
    --rom011={filename}         16Kb expansion rom #12
    --rom012={filename}         16Kb expansion rom #13
    --rom013={filename}         16Kb expansion rom #14
    --rom014={filename}         16Kb expansion rom #15
    --rom015={filename}         16Kb expansion rom #16
    --drive0={filename}         drive0 disk image
    --drive1={filename}         drive1 disk image
    --snapshot={filename}       initial snapshot

Misc. options:
    --turbo                     enable the turbo mode
    --no-turbo                  disable the turbo mode
    --xshm                      use the XShm extension
    --no-xshm                   don't use the XShm extension
    --fps                       print framerate
    --no-fps                    don'tprint framerate

Debug options:
    --quiet                     set the loglevel to quiet mode
    --trace                     set the loglevel to trace mode
    --debug                     set the loglevel to debug mode

```

## Release notes

  * Xcpc doesn't have sound emulation yet.
  * You can emulate the joystick with the keypad by disabling the 'Num Lock' key.

The joystick mode can be toggled (enabled/disabled) by pressing the `End` key.
The Up/Down/Left/Right + CTRL-L & ALT-L keys will then emulate the joystick 0.

## License and legal informations

Xcpc is a free software, so it's source code is free (free as freedom, not as free beer).

Xcpc is an emulator, so it has been designed to deal with old copyrighted softwares, so you should take your own responsibility about these softwares.

### License terms

Xcpc is released under the terms of the General Public License version 2.

```
Xcpc - Copyright (c) 2001-2021 - Olivier Poncet

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
```

### Legal Informations

Xcpc is distributed with some copyrighted materials from Amstrad with their permission.

```
Amstrad has kindly given it's permission for it's copyrighted
material to be redistributed but Amstrad retains it's copyright.

Some of the Amstrad CPC ROM code is copyright Locomotive Software.
```

You should not distribute Xcpc with other copyrighted materials.

```
ROM and DISK images are protected under the copyrights of their authors,
and cannot be distributed in this package. You can download and/or use
ROM and DISK images at your own risk and responsibility.
```

### libdsk

libdsk v1.4.2, a general floppy and diskimage access library.

This library is embedded into this projet with some little fixes.

  - http://www.seasip.info/Unix/LibDsk/index.html

```
Copyright (C) 2001-2015 John Elliott <seasip.webmaster@gmail.com>

Modifications to add dsk_dirty()
(c) 2005 Philip Kendall <pak21-spectrum@srcf.ucam.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA
```

### lib765

lib765 v0.4.2, a library to emulate the uPD765a floppy controller (aka Intel 8272).

This library is embedded into this projet with some little fixes.

  - http://www.seasip.info/Unix/LibDsk/index.html

```

Copyright (C) 2002,2003,2004  John Elliott <jce@seasip.demon.co.uk>

Modifications to add dirty flags
(c) 2005 Philip Kendall <pak21-spectrum@srcf.ucam.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
```

### Third party code

This software is very partly based on the Z80 emulator from Marat Fayzullin.

  - http://www.komkon.org/fms/

A complete rewrite of the Z80 emulator is in progress.

The new core will not contain any source code from the original Marat's implementation and will be available under the GNU General Public License (GPL).
