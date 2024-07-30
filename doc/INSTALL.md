## HOW TO INSTALL

The Xcpc emulator is fully autotoolized, so you can build it with the well-known workflow `configure && make && make install`.

### DEPENDENCIES

Under Debian or derivatives (Ubuntu, Mint, ...), please install first these dependencies. On other systems, please install the equivalent packages.

Mandatory dependencies :

```
build-essential
xorg-dev
libgtk-3-dev
```

Optional dependencies :

```
zlib1g-dev
libbz2-dev
```

### GENERATE THE CONFIGURE SCRIPT

Generate the `configure` script if it does not exists. This step is mandatory in case you just have cloned the git repository.

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

### BUILD THE EMULATOR

Build the emulator:

```
make -j$(nproc)
```

### INSTALL THE EMULATOR

Install the emulator:

```
make install
```

### RUN THE EMULATOR

Run the emulator

```
{destination-path}/bin/xcpc
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

## HOW TO INSTALL INTO YOUR HOME DIRECTORY

Quick installation instructions:

```
autoreconf -v -i -f
./configure --prefix=${HOME}/Apps/xcpc
make -j$(nproc)
make install
ln -sf {HOME}/Apps/xcpc/share/applications/xcpc.desktop ${HOME}/.local/share/applications/xcpc.desktop
```

You can now run the emulator from your desktop menu `Games > Xcpc`

