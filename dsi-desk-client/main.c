#include <nds.h>
#include <dswifi9.h>
#include <fat.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "tcp-client.h"

#define SCREEN_SIZE (LCD_WIDTH * LCD_HEIGHT)
#define TILE_SIZE 16

#define BGR555(b, g, r) (r | (g << 5) | (b << 10) | (1 << 15))

typedef struct __attribute__((packed))
{
	u8 id;
	u16 data[256];
} Tile;

void displayTile(const Tile *tile, uint16_t *display)
{
	int tileX = tile->id & 15; //tileX % 16
	int tileY = tile->id >> 4; //tileY / 16

	int screenXBase = tileX << 4; //tileX * 16
	int screenYBase = tileY << 4; //tileY * 16

	for (int y = 0; y < TILE_SIZE; y++)
	{
		int yBase = y << 4; //y * 16
		int scrYBased = (screenYBase + y) << 8; //I'm sorry, I have no idea how I'm supposed to name this. This is the same as (screenYBase + y) * LCD_WIDTH

		for (int x = 0; x < TILE_SIZE; x++)
			display[scrYBased + (screenXBase + x)] = tile->data[yBase + x];
	}
}

void waitUntilKeyA()
{
	while (!(keysHeld() & KEY_A))
	{
		scanKeys();
		usleep(33333);
	}
}

void initWifi()
{
	while (1)
	{
		if (!Wifi_InitDefault(WFC_CONNECT))
		{
			iprintf("Failed to connect! Press A to try again!\n");
			waitUntilKeyA();
		}
		else
			break;
	}

	consoleClear();
	iprintf("Connected\n");
}

typedef struct __attribute__((packed))
{
	u16 tx;
	u16 ty;
	u32 keysHeld;
	int keyboardKeyPressed;
	int keyboardKeyReleased;
} Input;

volatile int keyboardKeyPressed = -1;
void onKeyboardPressed(int key)
{
	keyboardKeyPressed = key;
}

volatile int keyboardKeyReleased = -1;
void onKeyboardReleased(int key)
{
	keyboardKeyReleased = key;
}

void sendInput(int tcpSocket, bool keyboardOn)
{
	touchPosition touch;
	touchRead(&touch);

	Input input;
	input.keysHeld = keysHeld();
	
	if(!keyboardOn)
	{
		input.tx = touch.px;
		input.ty = touch.py;

		input.keyboardKeyPressed = -1;
		input.keyboardKeyReleased = -1;
	}
	else
	{
		input.tx = 0;
		input.ty = 0;

		input.keyboardKeyPressed = keyboardKeyPressed;
		input.keyboardKeyReleased = keyboardKeyReleased;
	}

	send(tcpSocket, &input, sizeof(input), 0);
}

void waitUntilExit()
{
	while (1)
	{
		scanKeys();
		if (keysDown() & KEY_START)
			exit(1);

		swiWaitForVBlank();
	}
}

void getAddress(char *address)
{
	if (!fatInitDefault())
	{
		iprintf("Failed to init FAT\nIs your SD card inserted?\n");
		iprintf("Press START to exit\n");
		waitUntilExit();
	}

	FILE *f = fopen("sd:/server-address.txt", "r");
	if (f == NULL)
	{
		iprintf("file: server-address.txt not found\n");
		waitUntilExit();
	}
	else
		iprintf("File: server-address.txt found\n");
	
	fseek(f, 0, SEEK_END);
	int fileSize = ftell(f);
	rewind(f);

	fclose(f);

	char buffer[16];	
	memset(buffer, 0 , 16);

	f = fopen("sd:/server-address.txt", "rb");
	fread(buffer, sizeof(char), fileSize, f);

	fclose(f);

	int bytesAdded = 0;
	for(int i = 0; i < 16; i++)
	{
		if((buffer[i] == '.') || (buffer[i] >= '0' && buffer[i] <= '9'))
		{
			address[bytesAdded] = buffer[i];
			bytesAdded++;
		}
	}

	address[bytesAdded + 1] = '\0';
}

void swapScreens(bool* mainOnTop)
{
	if(*mainOnTop)
	{
		lcdMainOnBottom();
		*mainOnTop = false;
	}
	else
	{
		lcdMainOnTop();
		*mainOnTop = true;
	}
}

void changeKeyboardState(bool* keyboardUp)
{
	if(*keyboardUp)
		keyboardHide();
	else
		keyboardShow();

	*keyboardUp = !*keyboardUp;
}

int main(void)
{
	consoleDemoInit();
	Keyboard* keyboard = keyboardDemoInit();
	keyboard->OnKeyPressed = onKeyboardPressed;
	keyboard->OnKeyReleased = onKeyboardReleased;

	initWifi();

	videoSetMode(MODE_5_2D);
	vramSetBankA(VRAM_A_MAIN_BG);

	char address[16];
	memset(address, 0, 16);
	getAddress(address);

	iprintf("Initializing tcp...\n");
	int tcpSocket = getTcpClient(address, 8081);
	iprintf("Tcp initialized\n");

	int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	u16 *fb = bgGetGfxPtr(bg);

	Tile tile;
	bool mainOnTop = true;
	bool keyboardOn = false;
	while (1)
	{
		// swiWaitForVBlank();
		scanKeys();
		keyboardUpdate();

		receiveAll(tcpSocket, &tile, sizeof(tile));
		if (tile.id != 255)
			displayTile(&tile, fb);
		else
			sendInput(tcpSocket, keyboardOn);

		if (keysDown() & KEY_START)
			break;
		if(keysDown() & KEY_X)
			swapScreens(&mainOnTop);
		if(keysDown() & KEY_SELECT)
			changeKeyboardState(&keyboardOn);
	}

	closesocket(tcpSocket);
	return 0;
}