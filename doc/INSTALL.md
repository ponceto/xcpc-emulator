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
```

If you want to build the Gtk+-3.x user interface, you must install this package:

```
libgtk-3-dev
```

If you want to build the Motif-2.x user interface, you must install this package:

```
libmotif-dev
```

Optional dependencies :

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

Xcpc supports 4 differents user interface types:

  - Gtk+-3.x
  - Motif-2.x
  - Athena
  - Intrinsic

The user interface toolkit is detected automagically when running the `configure` script.

You can disable the support of a specific toolkit:

```
--disable-gtk3
--disable-motif2
--disable-athena
--disable-intrinsic
```

You can also force the user interface toolkit with the `--with-x11-toolkit` option:

```
--with-x11-toolkit=gtk3
--with-x11-toolkit=motif2
--with-x11-toolkit=athena
--with-x11-toolkit=intrinsic
```

### Build the emulator

Build the emulator

```
make -j5
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
make -j5
make install
ln -sf {HOME}/Apps/xcpc/share/applications/xcpc.desktop ${HOME}/.local/share/applications/xcpc.desktop
```

You can now run the emulator from your desktop menu `Games > Xcpc`
