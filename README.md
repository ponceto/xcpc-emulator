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
libglib2.0-dev
```

Optionnal dependencies :

```
zlib1g-dev
libbz2-dev
```

### Generate configure

Generate the `configure` script if it does not exists

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
Usage:
  xcpc [OPTION?]

Help Options:
  -h, --help                        Show help options

Application Options:
  --no-xshm                         Don't use the XShm extension
  --show-fps                        Show fps statistics
  --model={computer-model}          cpc464, cpc664, cpc6128
  --monitor={monitor-model}         color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm14
  --keyboard={keyboard-layout}      qwerty, azerty
  --refresh={refresh-rate}          50Hz, 60Hz
  --manufacturer={manufacturer}     Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad
  --snapshot={filename}             Snapshot to load at start
  --sysrom={filename}               32Kb system rom
  --rom000={filename}               16Kb expansion rom #00
  --rom001={filename}               16Kb expansion rom #01
  --rom002={filename}               16Kb expansion rom #02
  --rom003={filename}               16Kb expansion rom #03
  --rom004={filename}               16Kb expansion rom #04
  --rom005={filename}               16Kb expansion rom #05
  --rom006={filename}               16Kb expansion rom #06
  --rom007={filename}               16Kb expansion rom #07
  --rom008={filename}               16Kb expansion rom #08
  --rom009={filename}               16Kb expansion rom #09
  --rom010={filename}               16Kb expansion rom #10
  --rom011={filename}               16Kb expansion rom #11
  --rom012={filename}               16Kb expansion rom #12
  --rom013={filename}               16Kb expansion rom #13
  --rom014={filename}               16Kb expansion rom #14
  --rom015={filename}               16Kb expansion rom #15

```

## Release notes

  * Xcpc doesn't have sound emulation yet.
  * You can emulate the joystick with the keypad by disabling the 'Num Lock' key.

The joystick mode can be toggled (enabled/disabled) by pressing the `End` key.
The Up/Down/Left/Right + CTRL-L & ALT-L keys will then emulate the joystick 0.

## License terms

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

## Legal Informations

```
Amstrad has kindly given it's permission for it's copyrighted
material to be redistributed but Amstrad retains it's copyright.

Some of the Amstrad CPC ROM code is copyright Locomotive Software.

ROM and DISK images are protected under the copyrights of their authors,
and cannot be distributed in this package. You can download and/or use
ROM and DISK images at your own risk and responsibility.
```

## License notes

```
This software is partly based on the libdsk and lib765 libraries from John Elliott.

    http://www.seasip.info/Unix/LibDsk/index.html

Theses libraries are embedded into this projet and some parts have been rewritten.
```

```
This software is partly based on the Z80 emulator from Marat Fayzullin.

    http://www.komkon.org/fms/

A complete rewrite of the Z80 emulator is in progress. This new core will not
contain any source code from the original Marat's implementation and will be
available under the GNU General Public License (GPL).
```

