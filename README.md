## README

Xcpc is a portable Amstrad CPC 464/664/6128 emulator written in C. It is designed to run on any POSIX compliant system having an X11 server, including Linux, BSD and Unix.

Note there is absolutely no support for Microsoft Windows operating systems and there never will be. Please do not ask me to add such functionality.

Xcpc is designed to emulate classic range of the Amstrad CPC systems :

  - Amstrad CPC 464 (v1).
  - Amstrad CPC 664 (v2).
  - Amstrad CPC 6128 (v3).

The Amstrad CPC+ range and the GX4000 console are not currently emulated and likely will not be.

### How to install Xcpc

Please read the file [doc/INSTALL.md](doc/INSTALL.md).

### How to run Xcpc

```
Usage: xcpc [toolkit-options] [program-options]

Help options:
    --help                      display this help and exit
    --version                   display the version and exit

Emulation options:
    --company={value}           Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad
    --machine={value}           cpc464, cpc664, cpc6128
    --monitor={value}           color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm12
    --refresh={value}           50Hz, 60Hz
    --keyboard={value}          qwerty, azerty
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
    --no-fps                    don't print framerate

Debug options:
    --quiet                     set the loglevel to quiet mode
    --trace                     set the loglevel to trace mode
    --debug                     set the loglevel to debug mode

```

### Release notes

Here is a non-exhaustive list of supported features :

  - A simple Athena user interface.
  - A complete Motif2 user interface.
  - Full X11 with XShm extension.
  - Adaptative frame-rate.
  - Full Floppy disk images support (.dsk).
  - Full snapshot support (.sna).
  - Drag and Drop support (.dsk, .sna).
  - Keyboard emulation (qwerty, azerty).
  - Joystick emulation with the numeric keypad when numlock is disabled.
  - Joystick emulation by pressing the « End » key (the use arrows + ctrl + alt).
  - CPU: Z80 with full documented instructions and most undocumented instructions.
  - VGA: Video Gate Array 40007/40008/40010, complete support.
  - VDC: CRTC 6845, almost complete full support.
  - PPI: PPI 8255, almost complete full support.
  - PSG: AY-3-8912, partial support (actually no sound is produced).
  - FDC: FDC 765A, complete support.

The emulator lacks some features. Here is a list of the most wanted features :

  - Sound support.
  - A Gtk+ user interface.
  - A Qt user interface.
  - Host Joystick support.
  - A built-in assembler.
  - A built-in debugger.

### License and legal informations

Please read the file [doc/LICENSE.md](doc/LICENSE.md).
