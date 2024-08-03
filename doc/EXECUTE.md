## HOW TO EXECUTE

### COMMAND-LINE OPTIONS

```
Usage: xcpc [toolkit-options] [program-options]

Help options:
    --help                      display this help and exit
    --version                   display the version and exit

Emulation options:
    --company={value}           Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad
    --machine={value}           cpc464, cpc664, cpc6128
    --monitor={value}           color, green, gray, ctm640, ctm644, gt64, gt65, cm14, mm12
    --refresh={value}           50Hz, 60Hz
    --keyboard={value}          english, french, german, spanish, danish
    --memory={value}            64kb, 128kb, 192kb, 256kb, 320kb, 384kb, 448kb, 512kb
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
    --speedup={factor}          speeds up emulation by an integer factor
    --xshm                      use the XShm extension
    --no-xshm                   don't use the XShm extension
    --scanlines                 simulate crt scanlines
    --no-scanlines              don't simulate crt scanlines

Debug options:
    --quiet                     set the loglevel to quiet mode
    --trace                     set the loglevel to trace mode
    --debug                     set the loglevel to debug mode

```

### ENVIRONMENT VARIABLES

#### LOGLEVEL

The default loglevel is overridden with the `--quiet`, `--trace`, `--debug` options.

You can also set the default loglevel with the `XCPC_LOGLEVEL` variable:

```
export XCPC_LOGLEVEL={xcpc-loglevel}
```

The available loglevel values are:

```
0 = quiet
1 = error
2 = alert
3 = print
4 = trace
5 = debug
```

#### Joysticks

You can select the joysticks if not correctly handled by default.

  - `XCPC_JOYSTICK0`: the path to the joystick0 device (defaults to `/dev/input/js0` on Linux)
  - `XCPC_JOYSTICK1`: the path to the joystick1 device (defaults to `/dev/input/js1` on Linux)

Example:

```
export XCPC_JOYSTICK0="{path-to-joystick0}"
export XCPC_JOYSTICK1="{path-to-joystick1}"
```

#### Audio

You can adjust audio parameters if they are not good by default.

  - `XCPC_AUDIO_CHANNELS`: the channel count, `1` for mono, `2` for stereo
  - `XCPC_AUDIO_SAMPLERATE`: the sample rate, for example `11025`, `22050`, `44100`, `48000`

Example for a low-end hardware:

```
export XCPC_AUDIO_CHANNELS="1"
export XCPC_AUDIO_SAMPLERATE="11025"
```

Example for a high-end hardware:

```
export XCPC_AUDIO_CHANNELS="2"
export XCPC_AUDIO_SAMPLERATE="48000"
```

### HOTKEYS

Some Hotkeys/shortcuts are available:

  - `F1` for help.
  - `F2` for loading snapshots.
  - `F3` for saving snapshots.
  - `F5` for resetting the emulator.
  - `F6` for inserting disk into drive A.
  - `F7` for removing disk from drive A.
  - `F8` for inserting disk into drive B.
  - `F9` for removing disk from drive B.

### KEYBOARD

The left control keys (`Left Control`, `Left Alt`, ...) are sent to the emulated machine, so if you want to send some keycodes (eg. when you are using an `AZERTY` keyboard), you must use the right control keys of your keyboard (`Right Control`, `Right Shift`, `AltGr`, ...).

If you don't have a joystick or if the joystick support is not available at compilation time, a joystick emulator is provided:

  - `Home` or `End` for toggling the joystick emulation.
  - `Cursor Up` for moving the joystick up.
  - `Cursor Down` for moving the joystick down.
  - `Cursor Left` for moving the joystick left.
  - `Cursor Right` for moving the joystick right.
  - `Left Control` for Fire 1.
  - `Left Alt` for Fire 2.

### JOYSTICK

A joystick support is available under Linux:

  - Up to two physical joysticks can be used.
  - Joysticks must be plugged before launching Xcpc.
  - Joysticks are tied by default to `/dev/input/js0` and `/dev/input/js1`.
  - You can set the `XCPC_JOYSTICK0` environment variable to specify 1st joystick device.
  - You can set the `XCPC_JOYSTICK1` environment variable to specify 2nd joystick device.

### DRAG'N DROP

Xcpc supports drag'n drop, so you can use your file manager to load snapshots and/or disk images.

The supported extensions are:

  - `.sna` for loading snapshots
  - `.dsk` for loading raw disk images
  - `.dsk.gz` for loading compressed disk images with the zlib algorithm.
  - `.dsk.bz2` for loading compressed disk images with the bz2 algorithm.

Note: `.zip` disk images are currently not supported, so you have to extract the disk images from the zip archives.

