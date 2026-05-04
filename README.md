# DSiDesk

An app that allows you to view your Linux desktop on a Nintendo DSi.

## Overview

DSiDesk is a two-part application consisting of a server component that captures your Linux desktop and a client component that runs on your Nintendo DSi. It enables remote desktop viewing on your Nintendo DSi console, letting you access and interact with your Linux desktop from your portable handheld device.

## Features

- View your Linux desktop on Nintendo DSi at 768x576 resolution
- Remote desktop streaming
- Cross-platform Linux support

## Requirements

- Nintendo DSi console
- Linux system with X11 support
- Build tools (Make, GCC)
- X11 development libraries:
  - `libx11-dev`
  - `libxfixes-dev`
  - `libxtst-dev`

### Screen Resolution

DSiDesk captures your desktop at a fixed resolution of **768x576**. To capture your entire screen, you'll need to set your Linux desktop to this resolution.

You can do this using `xrandr`:

```bash
# View available resolutions
xrandr

# Set resolution to 768x576 (replace HDMI-1 with your display output)
xrandr --output HDMI-1 --mode 768x576

# Revert to previous resolution
xrandr --auto
```

Replace `HDMI-1` with your actual display output (common outputs include `HDMI-1`, `DP-1`, `eDP-1`, etc.).

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

The client is built for the Nintendo DSi using Make. Navigate to the `dsi-desk-client` folder:

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

1. Start the server on your Linux system:
```bash
./dsi-desk-server
```

2. Transfer the compiled client to your Nintendo DSi

3. Run the client on your DSi to view your desktop

## Project Structure

- **C source files** (73.8%) - Core application logic for server and client
- **Makefile** (26.2%) - Build configuration for the DSi client
- **dsi-desk-client/** - Nintendo DSi client application

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

[Add your license here]

## Support

For issues, questions, or feature requests, please open an issue on the GitHub repository.

---

Made with ❤️ for Nintendo DSi enthusiasts
