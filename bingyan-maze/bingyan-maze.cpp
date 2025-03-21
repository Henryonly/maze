
//I HAVE SOME GREAT IDEAS:
//MAKE MONSTERS PLAY EVERYWHERE,SOME OF WHICH CAN CROSS WALLS
//RECORD TIME LIMIT AND TIME LEFT
//EAT COINS WHILE CROSSING THE MAZE

// pixel of each box is (20,20)

#ifndef _maze_h_
#define _maze_h_

#define WIN32_LEAN_AND_MEAN
#include<conio.h>
#include<time.h>
#include<easyx.h>
#include<stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include<Windows.h>
#include<graphics.h>
#include<stdio.h>//used to debug

#pragma comment(lib, "ws2_32.lib")

enum key
{
	ESC = 1,
	UP = 2,
	DOWN = 4,
	LEFT = 8,
	RIGHT = 16
};

enum map
{
	WALL = 0,
	PLAYER = 20,
	ROUTE = 40,
	ENTRANCE = 60,
	EXIT = 80,
	OUTSIDE = 100
};

//GLOBAL VARIABLE
char** mapmap = NULL;//the map!!!record state of items
//map item,map view
IMAGE mapitem(180, 20);//element of map items
IMAGE mapview(360, 280);//sight of map
SIZE map_size;//size of xmax,ymax of map
//player side
POINT where_are_you;//where are you? PLAYER!
RECT view;//man,what can i see? mamba out
//create mini map to adapt the small of screen
SIZE GPS;//mini map size
IMAGE imageGPS;//every item in mini map
POINT disGPS;//where are you in mini map to display?
clock_t start_time;
clock_t now_time;
char name[50];

//FUNCTIONS
int input(int* keyboard);//regulate activities according to keyboard input
void changeto(int keyboard);//execute the commands
void start();//welcome page
void picture();//draw items needed
void game();//start game,for all
void dfs(int width, int height);//build the game
void travel(int x, int y);//excavator excavate anywhere
void draw();//draw the final maze
void left();//if go left
void right();//if go right
void up();//if go up
void down();//if go down
bool winlema();//whether win
bool Quit();//quit the game?
wchar_t* MultiByteToWideCharWrapper(const char* str);//change data type
int my_inet_pton(int af, const char* src, void* dst);//ipv4 translation
int text(char* buffer);//text
int server();//as the server
int record[10000][100];//database


//MACRO DEFINITION
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_ESCAPE 0x18

void main()
{
	initgraph(640, 480);
	srand((unsigned int)time(NULL));
	start();
	picture();
	game();
	int keyboard;
	while (!((input(&keyboard) == ESC && Quit())))
	{
		changeto(keyboard);
		draw();
		if (winlema())
		{
			break;
		}
		Sleep(100);
	}
	closegraph();
}

int input(int* keyboard)
{
	*keyboard = 0;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
	{
		*keyboard = LEFT;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
	{
		*keyboard = RIGHT;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
	{
		*keyboard = UP;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
	{
		*keyboard = DOWN;
	}
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		*keyboard = ESC;
	}
	return *keyboard;
}

void changeto(int keyboard)
{
	if (keyboard & LEFT)
		left();
	if (keyboard & RIGHT)
		right();
	if (keyboard & UP)
		up();
	if (keyboard & DOWN)
		down();
}

void start()
{
	int i;
	for (i = 0;i < 128;i++)
	{
		setcolor(RGB(0, (127 - i) << 1, 0));
		rectangle(149 - i, 109 - (i >> 1), 490 + i, 370 + (i >> 1));
	}
	setcolor(RED);
	setbkmode(TRANSPARENT);
	setfont(36, 0, L"宋体");//L is learned in csdn,turn to unicode
	outtextxy(350, 500, L"冰岩小迷宫");
}

void picture()
{
	SetWorkingImage(&mapitem);
	cleardevice();
	//draw PLAYER 
	setorigin(PLAYER, 0);
	setfillstyle(YELLOW);
	setcolor(YELLOW);
	rectangle(2, 2, 17, 17);
	setcolor(BLACK);
	fillellipse(7, 7, 8, 8);
	fillellipse(12, 7, 12, 8);
	arc(5, 6, 14, 14, 3.26, 6.02);
	//draw WALL
	setorigin(WALL, 0);
	setfillstyle(BROWN);
	setcolor(BROWN);
	bar(1, 1, 18, 18);
	rectangle(0, 0, 19, 19);
	//draw  ENTRANCE
	setorigin(ENTRANCE, 0);
	setcolor(GREEN);
	outtextxy(4, 4, L"->");
	//draw EXIT
	setorigin(EXIT, 0);
	setcolor(GREEN);
	outtextxy(4, 4, L"->");
	//draw OUTSIDE
	setorigin(OUTSIDE, 0);
	setfillstyle(GREEN);
	bar(0, 0, 19, 19);
	//recover axis
	setorigin(0, 0);
}

void game()
{
	map_size.cx = map_size.cy = 0;
	//learned in csdn
	wchar_t* shuru = new wchar_t[50];
	//learned in doubao

	while (map_size.cx < 50 || map_size.cx>300)
	{
		InputBox(shuru, 4, L"输入宽度吧！\n范围：50~300", L"输入", L"50");
		int len = WideCharToMultiByte(CP_UTF8, 0, shuru, -1, NULL, 0, NULL, NULL);
		char* str = (char*)malloc(len);
		WideCharToMultiByte(CP_UTF8, 0, shuru, -1, str, len, NULL, NULL);
		map_size.cx = atoi(str);
	}
	while (map_size.cy < 50 || map_size.cy>300)
	{
		InputBox(shuru, 4, L"输入高度吧！\n范围：50~300", L"输入", L"50");
		int len = WideCharToMultiByte(CP_UTF8, 0, shuru, -1, NULL, 0, NULL, NULL);
		char* str = (char*)malloc(len);
		WideCharToMultiByte(CP_UTF8, 0, shuru, -1, str, len, NULL, NULL);
		map_size.cy = atoi(str);
	}
	x0:memset(shuru, 0, 50);
	InputBox(shuru, 50, L"给自己起个好听的名字！不能带阿拉伯数字\n", L"输入", L"");
	int len = WideCharToMultiByte(CP_UTF8, 0, shuru, -1, NULL, 0, NULL, NULL);
	char* str = (char*)malloc(len);
	WideCharToMultiByte(CP_UTF8, 0, shuru, -1, str, len, NULL, NULL);
	for (int i = 0;i < 50;i++)
	{
		if (shuru[i] >= '0' || shuru[i] <= '9')
		{
			goto x0;
		}
	}
	strcpy_s(name, str);

	//make the width and height odd:if not,wall of the game security border may break!
	if (map_size.cx % 2 == 0)
	{
		map_size.cx++;
	}
	if (map_size.cy % 2 == 0)
	{
		map_size.cy++;
	}
	if (mapmap != NULL)
	{
		for (int i = 0;i < map_size.cx + 2;i++)
		{
			delete[] mapmap[i];
		}
		delete[] mapmap;
	}
	dfs(map_size.cx, map_size.cy);
	where_are_you.x = 2;
	where_are_you.y = 2;
	//We set the map size view as (17,13)
	view.left = 0;
	view.right = 17;
	view.top = 0;
	view.bottom = 13;
	setfillstyle(BLUE);
	bar(522, 368, 637, 471);
	if (map_size.cx > map_size.cy)
	{
		GPS.cx = 100;
		GPS.cy = (int)(100 * map_size.cy / map_size.cx + 0.5);
	}
	else
	{
		GPS.cy = 100;
		GPS.cx = (int)(100 * map_size.cx / map_size.cy + 0.5);
	}
	Resize(&imageGPS, GPS.cx, GPS.cy);
	disGPS.x = 530 + 50 - GPS.cx / 2;
	disGPS.y = 370 + 50 - GPS.cy / 2;
	setcolor(RED);
	rectangle(disGPS.x - 1, disGPS.y - 1, disGPS.x + GPS.cx, disGPS.y + GPS.cy);
	setcolor(YELLOW);
	moveto(disGPS.x - 8, disGPS.y + GPS.cy / map_size.cy);
	linerel(7, 0);
	linerel(-3, -3);
	moverel(3, 3);
	linerel(-3, 3);
	moveto(disGPS.x + GPS.cx, disGPS.y + GPS.cy - GPS.cy / map_size.cy);
	linerel(7, 0);
	linerel(-3, -3);
	moverel(3, 3);
	linerel(-3, 3);
	draw();
	start_time = clock();
}

void dfs(int width, int height)
{
	if (width % 2 == 0 || height % 2 == 0)
	{
		return;
		//must be odd! 
	}
	int i, j;
	mapmap = new char* [width + 2];
	for (i = 0;i <= width + 1;i++)
	{
		mapmap[i] = new char[height + 2];
		memset(mapmap[i], WALL, height + 2);
	}
	for (i = 0;i <= width + 1;i++)
	{
		mapmap[i][0] = mapmap[i][height + 1] = ROUTE;
	}
	for (j = 1;j <= height;j++)
	{
		mapmap[0][j] = mapmap[width + 1][j] = ROUTE;
	}
	mapmap[1][2] = ENTRANCE;
	mapmap[width][height - 1] = EXIT;
	travel(((rand() % (width - 1)) & 0xfffe) + 2, ((rand() % (height - 1)) & 0xfffe) + 2);
	for (i = 0;i <= width + 1;i++)
	{
		mapmap[i][0] = mapmap[i][height + 1] = OUTSIDE;
	}
	for (j = 1;j <= height;j++)
	{
		mapmap[0][j] = mapmap[width + 1][j] = OUTSIDE;
	}
}

void travel(int x, int y)
{
	int direction[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };
	int i, temp, trans;
	//randomly disorganize the array
	for (i = 0;i < 4;i++)
	{
		temp = rand() % 4;
		trans = direction[i][0];
		direction[i][0] = direction[temp][0];
		direction[temp][0] = trans;
		trans = direction[i][1];
		direction[i][1] = direction[temp][1];
		direction[temp][1] = trans;
	}
	//return to the initial ROUTE
	mapmap[x][y] = ROUTE;
	for (i = 0;i < 4;i++)
	{
		if (mapmap[x + 2 * direction[i][0]][y + 2 * direction[i][1]] == WALL)//no footprint
		{
			mapmap[x + direction[i][0]][y + direction[i][1]] = ROUTE;
			travel(x + direction[i][0] * 2, y + direction[i][1] * 2);
		}
	}
}

void draw()
{
	int x1, y1;
	SetWorkingImage(&mapview);
	for (int i = view.left;i <= view.right;i++)
	{
		for (int j = view.top;j <= view.bottom;j++)
		{
			x1 = (i - view.left) * 20;
			y1 = (j - view.top) * 20;
			putimage(x1, y1, 20, 20, &mapitem, (map)mapmap[i][j], 0);
		}
	}
	x1 = (where_are_you.x - view.left) * 20;
	y1 = (where_are_you.y - view.top) * 20;
	putimage(x1, y1, 20, 20, &mapitem, PLAYER, 0);
	SetWorkingImage(&imageGPS);
	cleardevice();
	int tx = (int)((where_are_you.x - 1) * GPS.cx / (double)(map_size.cx - 1) + 0.5);
	int ty = (int)((where_are_you.y - 1) * GPS.cy / (double)(GPS.cy - 1) + 0.5);
	setcolor(YELLOW);
	circle(tx, ty, 1);
	SetWorkingImage();
	putimage(150, 110, 340, 260, &mapview, 10, 10);
	putimage(disGPS.x, disGPS.y, &imageGPS);
}

void left()
{
	if (where_are_you.x > 1 && (map)mapmap[where_are_you.x - 1][where_are_you.y] != WALL
		&& (map)mapmap[where_are_you.x - 1][where_are_you.y] != ENTRANCE)
	{
		//move the player
		where_are_you.x--;
		if (where_are_you.x - view.left < 5 && view.left > 0)
			//don't move too oddly
		{
			//move the view
			view.left--;
			view.right--;
		}
	}
}

void right()
{
	if (where_are_you.x < map_size.cx && (map)mapmap[where_are_you.x + 1][where_are_you.y] != WALL)
	{
		where_are_you.x++;
		if (view.right - where_are_you.x < 5 && view.right <= map_size.cx)
		{
			view.left++;
			view.right++;
		}
	}
}

void up()
{
	if (where_are_you.y > 1 && (map)mapmap[where_are_you.x][where_are_you.y - 1] != WALL)
	{
		where_are_you.y--;
		if (where_are_you.y - view.top < 5 && view.top > 0)
		{
			view.top--;
			view.bottom--;
		}
	}
}

void down()
{
	if (where_are_you.y < map_size.cy && (map)mapmap[where_are_you.x][where_are_you.y + 1] != WALL)
	{
		where_are_you.y++;
		if (view.bottom - where_are_you.y < 4 && view.top <= map_size.cy)
		{
			view.top++;
			view.bottom++;
		}
	}
}

bool winlema()
{
	if (where_are_you.x == map_size.cx && where_are_you.y == map_size.cy - 1)
	{
		HWND hwnd = GetHWnd();
		now_time = clock();
		char arr1[1000], arr2[500], arr3[500], arr4[500], arr5[500];
		strcpy_s(arr1, "挑战成功！\n您的用时是：");
		int integer = now_time / 1000;
		int point = now_time % 1000;
		_itoa_s(integer, arr2, 10);
		strcpy_s(arr3, ".");
		_itoa_s(point, arr4, 10);
		strcpy_s(arr5, "秒，还要再来一次吗？");
		strcat_s(arr1, arr2);
		strcat_s(arr1, arr3);
		strcat_s(arr1, arr4);
		strcat_s(arr1, arr5);
		wchar_t* widestr = MultiByteToWideCharWrapper(arr1);
		if (MessageBoxW(hwnd, widestr, L"恭喜", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			if (MessageBoxW(hwnd, L"您要查看您的成绩吗？黑方框可以看", L"求求了", MB_YESNO | MB_ICONQUESTION) == IDYES)))
			{
				char str1[50];
				char* str2 = "华科是985";
				str1 = "127.0.0.1";
				strcat_s(str1, str2);
				text(str1);
				server();
			}
			strcat_s(arr1, name);
			text(arr1);
			game();
			return 0;
		}
		else
		{
			strcat_s(arr1, name);
			text(arr1);
			return 1;
		}
	}
	return 0;
}

bool Quit()
{
	HWND hwnd = GetHWnd();
	return (MessageBox(hwnd, L"别走啊！你真的要丢下我了吗？", L"询问", MB_OKCANCEL | MB_ICONQUESTION) == IDOK);
}

wchar_t* MultiByteToWideCharWrapper(const char* str) 
{
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t* wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
	if (wstr) 
	{
		MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
	}
	return wstr;
}

int text(char* buffer) 
{
	WSADATA wsa;
	SOCKET client_socket;
	struct sockaddr_in server_addr;
	// initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//if fail
		printf("WSAStartup失败: %d\n", WSAGetLastError());
		return 1;
	}

	// create Socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		//if fail
		printf("创建Socket失败: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// server's address + port
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5210);
	if (my_inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) != 1)
	{
		//if fail
		printf("IP地址转换失败\n");
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	// connect the server
	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		//if fail
		printf("连接服务器失败: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	// send off messages
	send(client_socket, buffer, strlen(buffer) + 1, 0);
	recv(client_socket, buffer, sizeof(buffer), 0);
	printf("%s\n", buffer);

	// close Socket and clean
	closesocket(client_socket);
	WSACleanup();
	return 0;
}

int my_inet_pton(int af, const char* src, void* dst) 
{
	if (af == AF_INET) 
	{
		unsigned char* p = (unsigned char*)dst;
		int a, b, c, d;
		if (sscanf_s(src, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
			a >= 0 && a <= 255 &&
			b >= 0 && b <= 255 &&
			c >= 0 && c <= 255 &&
			d >= 0 && d <= 255) 
		{
			p[0] = (unsigned char)a;
			p[1] = (unsigned char)b;
			p[2] = (unsigned char)c;
			p[3] = (unsigned char)d;
			return 1;
		}
	}
	return 0;
}

int server()
{
	WSADATA wsa;
	SOCKET server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	int addr_len = sizeof(client_addr);
	char buffer[1024] = { 0 };

	// 初始化 Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//失败了怎么办
		printf("WSAStartup失败: %d\n", WSAGetLastError());
		return 1;
	}

	// 创建 Socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		//失败了怎么办
		printf("创建Socket失败: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 配置服务器地址和端口
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(5210);

	// 绑定 Socket
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		//失败了怎么办
		printf("绑定失败: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	// 监听连接，时刻在听
	listen(server_socket, 3);

	// 接受客户端连接，听完拿走
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
	if (client_socket == INVALID_SOCKET)
	{
		//失败了怎么办
		printf("接受连接失败: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	// 接收客户端消息并回传
	recv(client_socket, buffer, sizeof(buffer), 0);
	printf("%s\n", buffer);
	send(client_socket, buffer, strlen(buffer) + 1, 0);

	// 关闭 Socket 并清理
	closesocket(client_socket);
	closesocket(server_socket);
	WSACleanup();
	return 0;
}

#endif