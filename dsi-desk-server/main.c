#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <stdbool.h>

#include "screen.h"
#include "tcp-server.h"
#include "virt-mouse.h"
#include "timer.h"
#include "nds-buttons.h"
#include "virt-keyboard.h"
#include "virt-touch.h"

#define SCREEN_WIDTH 768
#define SCREEN_HEIGHT 576

#define DS_SCREEN_WIDTH 256
#define DS_SCREEN_HEIGHT 192
#define PIXEL_COUNT (DS_SCREEN_WIDTH * DS_SCREEN_HEIGHT)

#define TILE_SIZE 16
#define TILE_COUNT 192

#define BGR555(r, g, b) ((r) | ((g) << 5) | ((b) << 10) | (1 << 15))

typedef struct __attribute__((packed))
{
    uint8_t id;
    uint16_t data[256];
} Tile;

// resizes the screen and turns BGRA to BGR
void resizeScreen(char *screenData, char *resizedScreen)
{
    int scaleX = SCREEN_WIDTH / DS_SCREEN_WIDTH;
    int scaleY = SCREEN_HEIGHT / DS_SCREEN_HEIGHT;

    for (int y = 0; y < DS_SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < DS_SCREEN_WIDTH; x++)
        {
            int startX = x * scaleX;
            int startY = y * scaleY;

            int r = 0, g = 0, b = 0;

            for (int dy = 0; dy < scaleY; dy++)
            {
                for (int dx = 0; dx < scaleX; dx++)
                {
                    int srcX = startX + dx;
                    int srcY = startY + dy;

                    int srcIndex = (srcY * SCREEN_WIDTH + srcX) * 4;

                    r += (uint8_t)screenData[srcIndex + 0];
                    g += (uint8_t)screenData[srcIndex + 1];
                    b += (uint8_t)screenData[srcIndex + 2];
                }
            }

            int count = scaleX * scaleY;
            int dstIndex = (y * DS_SCREEN_WIDTH + x) * 3;

            resizedScreen[dstIndex + 0] = r / count;
            resizedScreen[dstIndex + 1] = g / count;
            resizedScreen[dstIndex + 2] = b / count;
        }
    }
}

void bgrToBGR555(uint16_t *buffer555, char *buffer)
{
    for (int i = 0; i < PIXEL_COUNT; i++)
    {
        uint8_t b = buffer[i * 3];
        uint8_t g = buffer[i * 3 + 1];
        uint8_t r = buffer[i * 3 + 2];

        buffer555[i] = BGR555(r >> 3, g >> 3, b >> 3);
    }
}

void setTilesId(Tile *tiles)
{
    for (uint8_t i = 0; i < TILE_COUNT; i++)
        tiles[i].id = i;
}

void setTilesData(Tile *tiles, uint16_t *data)
{
    for (int y = 0; y < DS_SCREEN_HEIGHT; y++)
        for (int x = 0; x < DS_SCREEN_WIDTH; x++)
        {
            int dataIndex = ((y % TILE_SIZE) * TILE_SIZE) + (x % TILE_SIZE);
            int tilesIndex = ((y / TILE_SIZE) * 16) + (x / TILE_SIZE);
            tiles[tilesIndex].data[((y % TILE_SIZE) * TILE_SIZE) + (x % TILE_SIZE)] = data[(y * DS_SCREEN_WIDTH) + x];
        }
}

typedef struct __attribute__((packed))
{
    uint16_t tx; // touch x
    uint16_t ty; // touch y
    uint32_t keysHeld;
    char keyboardKeyPressed;
    bool mainOnTop;
} Input;

void handleInput(int clientSocket, int mouseFd, int keyboardFd, char *prevChar, int touchFd)
{
    Input input;
    recv(clientSocket, &input, sizeof(input), 0);

    if (input.tx == 0 && input.ty == 0)
    {
        input.tx = 128;
        input.ty = 96;
    }

    if (input.keysHeld & KEY_UP)
        moveMouse(mouseFd, 0, -24);
    if (input.keysHeld & KEY_DOWN)
        moveMouse(mouseFd, 0, 24);
    if (input.keysHeld & KEY_LEFT)
        moveMouse(mouseFd, -24, 0);
    if (input.keysHeld & KEY_RIGHT)
        moveMouse(mouseFd, 24, 0);

    if(input.mainOnTop)
        moveMouse(mouseFd, (input.tx - 128) * 0.4, (input.ty - 96) * 0.4);
    else if(!(input.tx == 128 && input.ty == 96))
        simTouch(touchFd, input.tx * 3, input.ty * 3);
    else
        simTouchRelease(touchFd);

    if (input.keysHeld & KEY_A)
        setMouseLeft(mouseFd, 1);
    else
        setMouseLeft(mouseFd, 0);

    if (input.keysHeld & KEY_B)
        setMouseRight(mouseFd, 1);
    else
        setMouseRight(mouseFd, 0);

    if (input.keyboardKeyPressed != -1)
        pressCharacter(keyboardFd, input.keyboardKeyPressed);
    else if (*prevChar != -1)
        releaseCharacter(keyboardFd, *prevChar);

    updateMouse(mouseFd);
    *prevChar = input.keyboardKeyPressed;
}

void sendEndTile(int clientSocket)
{
    Tile tile;
    ((uint8_t *)(&tile))[0] = 255;
    send(clientSocket, &tile, sizeof(tile), 0);
}

void handleConnection(Tile *tiles, Tile *prevTiles, TcpServer *tcpServer, int mouseFd, int keyboardFd, char *prevChar, int touchFd, struct timespec* connectionTimer)
{
    for (int i = 0; i < TILE_COUNT; i++)
    {
        if (memcmp(tiles[i].data, prevTiles[i].data, sizeof(uint16_t) * 256) != 0)
            sendAll(tcpServer, &tiles[i], sizeof(Tile));

        if (getTimePassed(connectionTimer) > (NS_PER_SEC / 30))
        {
            sendEndTile(tcpServer->clientSocket);
            handleInput(tcpServer->clientSocket, mouseFd, keyboardFd, prevChar, touchFd);
            timerStart(connectionTimer);
        }
    }
}

int main()
{
    printf("Initializing virtual keyboard...\n");
    int keyboardFd = initVirtualKeyboard("Nintendo DSi Virtual Keyboard");
    if (keyboardFd != -1)
        printf("Keyboard initialized\n");
    else
        printf("Can't initialize virtual keyboard\n");

    printf("Initializing virtual touchscreen...\n");
    int touchFd = getTouchFd(SCREEN_WIDTH, SCREEN_HEIGHT, "Nintendo DSi Virtual touchscreen");
    if(touchFd != -1)
        printf("Virtual touchscreen initialized\n");
    else
        printf("Can't initialize virtual touchscreen\n");

    printf("Initializing screen...\n");
    ScreenStruct *screenStruct = initScreen(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    printf("Screen initialized\n");

    char resizedScreen[PIXEL_COUNT * 3];
    uint16_t dsBuffer[PIXEL_COUNT];

    Tile tiles[192];
    Tile prevTiles[192];

    setTilesId(tiles);

    const uint16_t port = 8081;
    printf("Starting TCP server on port %i...\n", port);
    TcpServer *tcpServer = getTcpServer(port);
    printf("TCP server started\n");

    int mouseFd = getMouseFd("Nintendo DSi Virtual Mouse");
    uint16_t prevTouch[2] = {0, 0};
    struct timespec startTime;
    char prevChar = -1;
    struct timespec connectionTimer;
    while (1)
    {
        timerStart(&startTime);
        char *screenData = getScreenData(screenStruct);

        resizeScreen(screenData, resizedScreen);
        bgrToBGR555(dsBuffer, resizedScreen);
        setTilesData(tiles, dsBuffer);

        handleConnection(tiles, prevTiles, tcpServer, mouseFd, keyboardFd, &prevChar, touchFd, &connectionTimer);

        memcpy(prevTiles, tiles, sizeof(tiles));

        free(screenData);
        waitForFrame(&startTime);
    }

    closeScreen(screenStruct);
    closeTcpServer(tcpServer);
    closeMouse(mouseFd);
    closeVirtualKeyboard(keyboardFd);
}