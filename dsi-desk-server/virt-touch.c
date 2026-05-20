#include "virt-input.h"
#include "virt-touch.h"

// Helper macro to easily enable device properties
#define ioctl_check(fd, type, code) \
    if (ioctl(fd, type, code) < 0) { perror("ioctl error"); close(fd); return 1; }

int getTouchFd(int scrMaxX, int scrMaxY, const char* deviceName)
{
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Opening /dev/uinput failed");
        return 1;
    }

    // 1. Enable Touch and Absolute Axis events
    ioctl_check(fd, UI_SET_EVBIT, EV_KEY);
    ioctl_check(fd, UI_SET_KEYBIT, BTN_TOUCH);
    ioctl_check(fd, UI_SET_EVBIT, EV_ABS);

    // 2. Configure Absolute Axes (X and Y coordinates)
    ioctl_check(fd, UI_SET_ABSBIT, ABS_X);
    ioctl_check(fd, UI_SET_ABSBIT, ABS_Y);

    // 3. Define properties for the setup
    struct uinput_abs_setup abs_x = { .code = ABS_X, .absinfo = { .maximum = scrMaxX, .minimum = 0 } };
    struct uinput_abs_setup abs_y = { .code = ABS_Y, .absinfo = { .maximum = scrMaxY, .minimum = 0 } };
    struct uinput_setup usetup;

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234; 
    usetup.id.product = 0x5678;
    strcpy(usetup.name, deviceName);

    // 4. Apply configuration
    ioctl_check(fd, UI_ABS_SETUP, &abs_x);
    ioctl_check(fd, UI_ABS_SETUP, &abs_y);
    ioctl_check(fd, UI_DEV_SETUP, &usetup);
    ioctl_check(fd, UI_DEV_CREATE, 0);

    // Wait for the OS to register the new device
    sleep(1);

    return fd;
}

void simTouch(int fd, int x, int y)
{
    emit(fd, EV_ABS, ABS_X, x);
    emit(fd, EV_ABS, ABS_Y, y);
    emit(fd, EV_KEY, BTN_TOUCH, 1); // 1 = Press
    emit(fd, EV_SYN, SYN_REPORT, 0); // Sync forces the OS to process the frame
}

void simTouchRelease(int fd)
{
    emit(fd, EV_KEY, BTN_TOUCH, 0); // 0 = Release
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void closeTouch(int fd)
{
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}
