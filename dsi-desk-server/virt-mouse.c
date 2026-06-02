#include "virt-mouse.h"
#include "virt-input.h"

int getMouseFd(const char* deviceName)
{
    struct uinput_setup usetup;

    // Open the uinput interface
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Failed to open /dev/uinput - running as root?");
        exit(EXIT_FAILURE);
    }

    // Enable mouse click buttons and relative movement capabilities
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT); // Enable right click capability
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

    // Set up device properties
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; /* Mock vendor */
    usetup.id.product = 0x5678; /* Mock product */
    strcpy(usetup.name, deviceName);

    // Create the virtual mouse device
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
    sleep(1); // Wait for OS to register device

    return fd;
}

void setMouseLeft(int fd, int val)
{
    emit(fd, EV_KEY, BTN_LEFT, val);
}

void moveMouse(int fd, int x, int y)
{
    emit(fd, EV_REL, REL_X, x);
    emit(fd, EV_REL, REL_Y, y);
}

void setMouseRight(int fd, int val)
{
    emit(fd, EV_KEY, BTN_RIGHT, val);
}

void updateMouse(int fd)
{
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void closeMouse(int fd)
{
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}
