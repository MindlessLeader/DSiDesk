# DSiDesk

An app that allows you to view your Linux desktop on a Nintendo DSi.

## Overview

DSiDesk is a two-part application consisting of a server component that captures your Linux desktop and a client component that runs on your Nintendo DSi. It enables remote desktop viewing on your Nintendo DSi console, letting you access and interact with your Linux desktop from your portable handheld device.

## Controls

> [!WARNING]  
> **Root Privileges Required:** Remote input controls will not function unless the server is executed with `sudo`.

| Input Device | Action on PC |
| :--- | :--- |
| **Touch Screen / D-Pad** | Moves the PC mouse cursor |
| **A Button** | Left-Click |
| **B Button** | Right-Click |
| **X Button** | Swap active view between top and bottom DSi screens |
| **SELECT Button** | Toggle virtual keyboard On/Off |

## Requirements

### Hardware & Operating System
* **Nintendo DSi** console capable of running homebrew.
* **Linux OS** with an active **X11 display server** session (Wayland is not supported).

### Core Dependencies
* **Build tools**: `make` and `gcc`
* **X11 development libraries**: `libx11`, `libxfixes`, and `libxtst`
* **devkitPro**: Required exclusively for compiling the DSi client binary.

### Screen Resolution

DSiDesk captures your desktop at a fixed resolution of **768x576**. To capture your entire screen, you'll need to set your Linux desktop to this resolution.

You can do this using `xrandr`

## Installation

> [!TIP]
> **Skip the build process:** If you do not want to compile the project yourself, you can download the latest precompiled server and client binaries directly from the [Releases](https://github.com) section of this repository.

### 1. Clone the Repository (For Compiling From Source)
If you prefer to build the project from scratch, begin by downloading the source code to your Linux machine:

Begin by downloading the project source code to your Linux machine:
```bash
git clone https://github.com/MindlessLeader/DSiDesk.git
cd DSiDesk
```

### 2. Build the Linux Server
Install the required development headers based on your Linux distribution:

```bash
# Ubuntu / Debian
sudo apt-get install libx11-dev libxfixes-dev libxtst-dev

# Fedora / RHEL
sudo dnf install libX11-devel libXfixes-devel libXtst-devel

# Arch Linux
sudo pacman -S libx11 libxfixes libxtst
```

Compile the server executable:
```bash
gcc -fdiagnostics-color=always -g *.c -o dsi-desk-server -lX11 -lXfixes -lXtst
```

### 3. Build the DSi Client
Ensure you have devkitPro configured on your system by following the [official installation guide](https://devkitpro.org/wiki/Getting_Started).

Navigate to the client subdirectory and compile:
```bash
cd dsi-desk-client
make
```

## Usage

1. Transfer the compiled client to your Nintendo DSi

2. Make a file with the name "server-address.txt" with your pc's local ip and transfer it to the root of your DSi's SD card 

3. Start the server on your Linux system:
```bash
sudo ./dsi-desk-server
```

4. Run the client on your DSi to view your desktop

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

For issues, questions, or feature requests, please open an issue on the GitHub repository.
