#include "virt-input.h"
#include "virt-keyboard.h"

// Struct to store a keycode and whether Shift needs to be held down
typedef struct
{
    int keycode;
    int shift;
} KeyMap;

// Global lookup table for ASCII characters 32 (Space) through 126 (~)
KeyMap ascii_map[128];

// Populate the translation matrix based on a standard US QWERTY layout
void initialize_ascii_map()
{
    // Zero out the map initially
    memset(ascii_map, 0, sizeof(ascii_map));

    // Space and basic punctuation
    ascii_map[' '] = (KeyMap){KEY_SPACE, 0};
    ascii_map['-'] = (KeyMap){KEY_MINUS, 0};
    ascii_map['_'] = (KeyMap){KEY_MINUS, 1};
    ascii_map['='] = (KeyMap){KEY_EQUAL, 0};
    ascii_map['+'] = (KeyMap){KEY_EQUAL, 1};
    ascii_map['['] = (KeyMap){KEY_LEFTBRACE, 0};
    ascii_map['{'] = (KeyMap){KEY_LEFTBRACE, 1};
    ascii_map[']'] = (KeyMap){KEY_RIGHTBRACE, 0};
    ascii_map['}'] = (KeyMap){KEY_RIGHTBRACE, 1};
    ascii_map[';'] = (KeyMap){KEY_SEMICOLON, 0};
    ascii_map[':'] = (KeyMap){KEY_SEMICOLON, 1};
    ascii_map['\''] = (KeyMap){KEY_APOSTROPHE, 0};
    ascii_map['"'] = (KeyMap){KEY_APOSTROPHE, 1};
    ascii_map['`'] = (KeyMap){KEY_GRAVE, 0};
    ascii_map['~'] = (KeyMap){KEY_GRAVE, 1};
    ascii_map['\\'] = (KeyMap){KEY_BACKSLASH, 0};
    ascii_map['|'] = (KeyMap){KEY_BACKSLASH, 1};
    ascii_map[','] = (KeyMap){KEY_COMMA, 0};
    ascii_map['<'] = (KeyMap){KEY_COMMA, 1};
    ascii_map['.'] = (KeyMap){KEY_DOT, 0};
    ascii_map['>'] = (KeyMap){KEY_DOT, 1};
    ascii_map['/'] = (KeyMap){KEY_SLASH, 0};
    ascii_map['?'] = (KeyMap){KEY_SLASH, 1};

    // Numbers and their Shift alternatives
    int num_codes[] = {KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9};
    char num_shifts[] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
    for (int i = 0; i < 10; i++)
    {
        ascii_map['0' + i] = (KeyMap){num_codes[i], 0};
        ascii_map[(int)num_shifts[i]] = (KeyMap){num_codes[i], 1};
    }

    // Letters (Lowercase and Uppercase)
    int alpha_codes[] = {
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
        KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
        KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z};
    for (int i = 0; i < 26; i++)
    {
        ascii_map['a' + i] = (KeyMap){alpha_codes[i], 0};
        ascii_map['A' + i] = (KeyMap){alpha_codes[i], 1};
    }

    // Control characters
    ascii_map['\n'] = (KeyMap){KEY_ENTER, 0};
    ascii_map['\t'] = (KeyMap){KEY_TAB, 0};
    ascii_map['\b'] = (KeyMap){KEY_BACKSPACE, 0};
}

KeyMap getSpecialKeyMap(char c)
{
    KeyMap map;
    switch (c)
    {
        case -17:
            map.keycode = KEY_UP;
            map.shift = 0;
            break;
        case -19:
            map.keycode = KEY_DOWN;
            map.shift = 0;
            break;
        case -20:
            map.keycode = KEY_LEFT;
            map.shift = 0;
            break;
        case -18:
            map.keycode = KEY_RIGHT;
            map.shift = 0;
            break;
        case -5:
            map.keycode = KEY_LEFTMETA;
            map.shift = 0;
            break;
        case -23:
            map.keycode = KEY_ESC;
            map.shift = 0;
            break;
        default:
            break;
    }

    return map;
}

void pressCharacter(int fd, char c)
{
    KeyMap map = ascii_map[(unsigned char)c];
    if (c < -1)
        map = getSpecialKeyMap(c);
    else if (map.keycode == 0)
        return; // Ignore unsupported characters

    // 1. Press Shift if needed
    if (map.shift)
    {
        emit(fd, EV_KEY, KEY_LEFTSHIFT, 1);
        emit(fd, EV_SYN, SYN_REPORT, 0);
        usleep(5000); // 5ms stability delay
    }

    // 2. Press base key down
    emit(fd, EV_KEY, map.keycode, 1);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void releaseCharacter(int fd, char c)
{
    KeyMap map = ascii_map[(unsigned char)c];

    if (c < -1)
        map = getSpecialKeyMap(c);
    else if (map.keycode == 0)
        return; // Ignore unsupported characters

    // 3. Release base key up
    emit(fd, EV_KEY, map.keycode, 0);
    emit(fd, EV_SYN, SYN_REPORT, 0);
    usleep(5000);

    // 4. Release Shift if it was used
    if (map.shift)
    {
        emit(fd, EV_KEY, KEY_LEFTSHIFT, 0);
        emit(fd, EV_SYN, SYN_REPORT, 0);
        usleep(5000);
    }
}

// Decodes a character and outputs the required hardware sequence
void typeCharacter(int fd, char c)
{
    pressCharacter(fd, c);
    releaseCharacter(fd, c);
}

// Loops through an entire string array
void typeString(int fd, const char *str)
{
    while (*str)
    {
        typeCharacter(fd, *str);
        str++;
    }
}

int initVirtualKeyboard(const char *deviceName)
{
    initialize_ascii_map();

    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        perror("Failed to open /dev/uinput. Use sudo");
        return -1;
    }

    // Configure uinput capabilities
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    for (int key = KEY_ESC; key <= KEY_MICMUTE; key++)
    {
        ioctl(fd, UI_SET_KEYBIT, key);
    }

    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1122;
    usetup.id.product = 0x3344;
    strcpy(usetup.name, deviceName);

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    return fd;
}

void closeVirtualKeyboard(int fd)
{
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}