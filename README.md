# README

## How to install Xcpc

```
See the 'INSTALL' file provided with this package.
```

## How to run Xcpc

```
Usage:
  xcpc [OPTION?]

Help Options:
  -h, --help                        Show help options

Application Options:
  --no-fps                          Don't show fps statistics
  --no-xshm                         Don't use the XShm extension
  --model={computer-model}          cpc464, cpc664, cpc6128
  --monitor={monitor-model}         color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm14
  --keyboard={keyboard-layout}      qwerty, azerty
  --refresh={refresh-rate}          50Hz, 60Hz
  --manufacturer={manufacturer}     Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad
  --snapshot=filename               Snapshot to load at start
  --sysrom=filename                 32Kb system rom
  --rom000=filename                 16Kb expansion rom #00
  --rom001=filename                 16Kb expansion rom #01
  --rom002=filename                 16Kb expansion rom #02
  --rom003=filename                 16Kb expansion rom #03
  --rom004=filename                 16Kb expansion rom #04
  --rom005=filename                 16Kb expansion rom #05
  --rom006=filename                 16Kb expansion rom #06
  --rom007=filename                 16Kb expansion rom #07
  --rom008=filename                 16Kb expansion rom #08
  --rom009=filename                 16Kb expansion rom #09
  --rom010=filename                 16Kb expansion rom #10
  --rom011=filename                 16Kb expansion rom #11
  --rom012=filename                 16Kb expansion rom #12
  --rom013=filename                 16Kb expansion rom #13
  --rom014=filename                 16Kb expansion rom #14
  --rom015=filename                 16Kb expansion rom #15

```

## Release notes

  * Xcpc doesn't have sound emulation yet.
  * You can emulate the joystick by disabling the 'Num Lock' key.

## License terms

```
Xcpc - Copyright (c) 2001-2020 - Olivier Poncet

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

