## HOW TO INSTALL

The Xcpc emulator is fully autotoolized, so you can build it with the well-known workflow `configure && make && make install`.

### INSTALL THE BUILD DEPENDENCIES

Under Debian or derivatives (Ubuntu, Mint, ...), please install first these dependencies. On other systems, please install the equivalent packages.

Mandatory dependencies :

```
build-essential
xorg-dev
```

Optional dependencies :

```
zlib1g-dev
libbz2-dev
```

If you want to build Xcpc with the Gtk3 user interface, you must install this package:

```
libgtk-3-dev
```

If you want to build Xcpc with the Gtk4 user interface, you must install this package:

```
libgtk-4-dev
```

### GENERATE THE CONFIGURE SCRIPT

If the `configure` script does not exists, you must generate it. This step is mandatory in case you just have cloned the git repository.

First, please ensure you have the following packages installed on your system:

```
autoconf
automake
libtool
autoconf-archive
```

Then you just have to run this command:

```
autoreconf -v -i -f
```

### CONFIGURE THE SOURCES

Run the `configure` script

```
./configure --prefix={destination-path}
```

Xcpc supports 2 differents user interface types:

  - Gtk3
  - Gtk4

The graphical user interface toolkit is detected automagically when running the `configure` script.

You can disable the support of a specific toolkit:

```
--disable-gtk3
--disable-gtk4
```

You can also force the user gui toolkit with the `--with-gui-toolkit` option:

```
--with-gui-toolkit=gtk3
--with-gui-toolkit=gtk4
```

### BUILD THE EMULATOR

Build the emulator:

```
make -j${NB_JOBS}
```

The `NB_JOBS` value specifies the number of jobs (commands) to run simultaneously. We recommand a value of `NUMBER_OF_CPU_CORES + 1`.

### INSTALL THE EMULATOR

Install the emulator:

```
make install
```

### DESKTOP INTEGRATION

A XDG compliant `.desktop` file is provided, so you can copy or symlink this file in a relevant directory

For example, in the system directory:

```
ln -sf {destination-path}/share/applications/xcpc.desktop /usr/share/applications/xcpc.desktop
```

For example, or the user directory:

```
ln -sf {destination-path}/share/applications/xcpc.desktop ${HOME}/.local/share/applications/xcpc.desktop
```

You can now find the emulator in your desktop menu: `Games > Xcpc`.

## HOW TO INSTALL INTO YOUR HOME DIRECTORY

Quick installation instructions:

```
autoreconf -v -i -f
./configure --prefix=${HOME}/Apps/xcpc
make -j5
make install
ln -sf {HOME}/Apps/xcpc/share/applications/xcpc.desktop ${HOME}/.local/share/applications/xcpc.desktop
```
