## HOW TO EXECUTE

### COMMAND-LINE

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
    --turbo                     enable the turbo mode
    --no-turbo                  disable the turbo mode
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

You can adjust some environment variables if needed.

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

