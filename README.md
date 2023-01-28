# FreeJoy Configurator

FreeJoy Configurator is a visual utility allowing you to setup your [FreeJoy](https://github.com/vostrenkov/FreeJoy) device for your own requirements.

![](https://github.com/FreeJoy-Team/FreeJoyConfiguratorQt/blob/master/src/Images/main.png)

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
