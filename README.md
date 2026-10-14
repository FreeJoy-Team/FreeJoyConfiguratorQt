# FreeJoy Configurator

FreeJoy Configurator is a visual utility allowing you to setup your [FreeJoy](https://github.com/vostrenkov/FreeJoy) device for your own requirements.

<img src="https://github.com/FreeJoy-Team/FreeJoyWiki/blob/master/images/main.jpg" width="800"/>

## Features

* Pin mapping configuration
* Digital inputs configuration (buttons, toggle switches, encoders, etc.)
* Analog input configuration (calibration, smoothing, curve shapes, etc.)
* Axes to buttons configuration
* Shift registers configuration
* Saving and loading configuration to file
* Flashing device firmware

Check [our wiki](https://github.com/FreeJoy-Team/FreeJoyWiki) for detailed instructions.

## Downloads
- [Windows](https://github.com/FreeJoy-Team/FreeJoyConfiguratorQt/releases)
- [AppImage](https://github.com/FreeJoy-Team/FreeJoyConfiguratorQt/releases)

## Installation

### Windows
1. Download, unpack, run

### Linux
1. Downloand and unpack .tar.xz
2. Edit 99-hid-FreeJoy.rules or leave it as it is
3. Copy "99-hid-FreeJoy.rules" to /etc/udev/rules.d
	* `sudo cp 99-hid-FreeJoy.rules /etc/udev/rules.d`
4. Run AppImage or build from source

## Building

### Windows Build Requirements
* Qt 5.15 SDK or later
* Windows 7 or later
* [Visual Studio 2019](https://visualstudio.microsoft.com/downloads/) (Community edition is fine) or MinGW
* Select **MSVC 2019** or **MinGW** option during Qt installation.

### Linux/Unix Build Requirements
* Qt 5.11 SDK or later
* GCC or Clang
* Install the required packages:
  * `qt5-default libxcb-xinerama0 libudev-dev libusb-1.0-0-dev libfox-1.6-dev`

### Build Setup Steps
1. Install the latest Qt SDK (and optionally, the Qt Creator IDE) from https://www.qt.io/download
    * You may also use your Linux distro's package manager for the Qt SDK as long as the packages are Qt 5.11 or later.
2. Open the project in Qt Creator or build from qmake on the command line.
    * Run `qmake FreeJoyQt.pro` then `make`

## Binary-coded switches (custom BCD firmware)

On the pin configuration tab, select a mode such as `BCD1 bit0 (1) to GND`
for each switch input. There are four independent groups, `BCD1` through
`BCD4`, with up to four bits each. Assign every bit from `bit0` to the
highest bit exactly once, without gaps. The pin's location does not affect
its bit weight: bit0 = 1, bit1 = 2, bit2 = 4, bit3 = 8.

For example, assign A0–A3 to BCD1 bit0–bit3 to GND for a four-bit switch
that connects its inputs to ground. GND uses active-low inputs with pull-up;
VCC uses active-high inputs with pull-down. Both polarities may be mixed
within a group.

Each group adds 2, 4, 8 or 16 physical buttons for 1, 2, 3 or 4 bits.
All binary codes are supported, including zero, rather than only decimal
digits 0–9. `Buttons from BCD` shows the count; the summary below the pin
layout shows each group's physical button range. These buttons follow
matrices, shift registers, axes-to-buttons and single buttons, in BCD group
order. Unused groups add no buttons.

On the Buttons tab, assign those physical buttons to logical buttons using
the existing physical button selector. For ordinary position selection use
`Button normal`, without inversion or timers. For a BCD-only four-bit
switch, physical buttons 1–16 correspond to codes 0–15; code 5 activates
physical button 6. Existing logical mappings are preserved and are never
automatically overwritten by pin changes.

Missing or duplicate bits and configurations exceeding the shared limit of
128 physical buttons cannot be saved or written to the device. Correct the
error shown below the pin layout. Incomplete assignments may be edited while
setting up a switch. File loading and device reads remain available for
inspection and correction. Pin codes 32–63 require the custom BCD firmware;
the USB packet layout, firmware version and existing pin codes are unchanged.

### BCD regression tests

With the Qt 5 development tools and Qt Test installed, build outside the source tree:

```sh
mkdir -p ../freejoy-tests
cd ../freejoy-tests
qmake ../FreeJoyConfiguratorQt-BCD/tests/bcd-tests.pro
make -j4
QT_QPA_PLATFORM=offscreen ./bcd-tests
```

The tests cover all groups, bit widths and polarities, pin selection and
restoration, missing/duplicate bits, combined resource limits, button
numbering, and configuration file/USB packet round trips. Hardware behavior
and firmware flashing require a connected controller.
