
//I HAVE SOME GREAT IDEAS:
//MAKE MONSTERS PLAY EVERYWHERE,SOME OF WHICH CAN CROSS WALLS
//RECORD TIME LIMIT AND TIME LEFT
//EAT COINS WHILE CROSSING THE MAZE

// pixel of each box is (20,20)

#include<conio.h>
#include<time.h>
#include<easyx.h>
#include<stdlib.h>
#include<Windows.h>
#include<graphics.h>
#include<stdio.h>//used to debug

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
	wchar_t* shuru = new wchar_t[4];
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
		if (MessageBox(hwnd, L"挑战成功！\n还要再来一次吗？", L"恭喜", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			game();
			return 0;
		}
		else
		{
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