# DSiDesk

An app that allows you to view your Linux desktop on a Nintendo DSi.

## Overview

DSiDesk is a two-part application consisting of a server component that captures your Linux desktop and a client component that runs on your Nintendo DSi. It enables remote desktop viewing on your Nintendo DSi console, letting you access and interact with your Linux desktop from your portable handheld device.

## Controls

- **Touch Screen / D-Pad:** Moves the PC mouse cursor.
- **A Button:** Executes a **Left-Click**.
- **B Button:** Executes a **Right-Click**.
- **X Button:** Swaps the active display view between the top and bottom DSi screens.
- **SELECT Button:** Enable or disable keyboard

Warning: Controls won't work without sudo

## Requirements

- Nintendo DSi console
- Linux system with X11 support
- Build tools (Make, GCC)
- X11 development libraries:
  - `libx11-dev`
  - `libxfixes-dev`
  - `libxtst-dev`
- **devkitpro** (for building the DSi client)

### Screen Resolution

DSiDesk captures your desktop at a fixed resolution of **768x576**. To capture your entire screen, you'll need to set your Linux desktop to this resolution.

You can do this using `xrandr`

## Installation

### 1. Building the Server

The server component requires X11 libraries to be installed on your Linux system. Install the dependencies:

```bash
# Debian/Ubuntu
sudo apt-get install libx11-dev libxfixes-dev libxtst-dev

# Fedora/RHEL
sudo dnf install libX11-devel libXfixes-devel libXtst-devel

# Arch
sudo pacman -S libx11 libxfixes libxtst
```

Then compile the server:

```bash
gcc -fdiagnostics-color=always -g *.c -o dsi-desk-server -lX11 -lXfixes -lXtst
```

### 2. Building the Client

The client is built for the Nintendo DSi using devkitpro. First, install devkitpro following the [official installation guide](https://devkitpro.org/wiki/Getting_Started).

Once devkitpro is installed, navigate to the `dsi-desk-client` folder and build:

```bash
cd dsi-desk-client
make
```

This will compile the DSi client application.

### 3. Setup

1. Clone the repository:
```bash
git clone https://github.com/MindlessLeader/DSiDesk.git
cd DSiDesk
```

2. Set your display to 768x576 resolution (see Screen Resolution section above)

3. Build both components using the instructions above

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
