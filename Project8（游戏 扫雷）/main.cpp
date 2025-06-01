#define _CRT_SECURE_NO_WARNINGS 1
#undef UNICODE
#undef _UNICODE

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<easyx.h>
#include<graphics.h>
#include<math.h>
#include<mmsystem.h>
#include<Windows.h>

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#pragma comment(lib, "winmm.lib")

#define COL 20
#define ROW 20
#define Mine 40
#define Size 40

IMAGE background[2];
IMAGE imgs[12];
bool isfirst = true;
bool t = false;

void init(int map[ROW][COL]);
void show(int map[ROW][COL]);
void draw(int map[ROW][COL]);
void drawB(int map[ROW][COL]);
void MouseMsg(ExMessage* msg, int map[ROW][COL]);
void Blank(int map[ROW][COL], int x, int y);
int Judge(int map[ROW][COL], int x, int y);

int main()
{	
	HWND hwnd = initgraph(20 * Size, 20 * Size/*, EW_SHOWCONSOLE*/);
	loadimage(&background[1], "./images/封面.jpg");
	putimage(0, -2, &background[1]);

	SetWindowText(hwnd, _T("扫雷(20*20 40雷)"));

	int map[ROW][COL] = { 0 };

	//加载音乐
	mciSendString("open ./images/start.mp3 alias b", NULL, 0, NULL);
	mciSendString("play b wait", NULL, 0, NULL);
	mciSendString("close b", NULL, 0, NULL);

	mciSendString("open ./images/在出发之前.mp3 alias a", NULL, 0, NULL);
	mciSendString("play a repeat", NULL, 0, NULL);

	//设置音量
	char a[100];
	mciSendString("status a volume", a, 100, NULL);
	int num = 500;
	sprintf(a, "setaudio a volume to %d", num);
	mciSendString(a, NULL, 0, NULL);

	//加载图片资源
	for (int i = 0; i < 12; i++)
	{
		char img[100] = { 0 };
		sprintf(img, "./images/%d.jpg", i);
		loadimage(&imgs[i], img);
	}

	init(map);
	//show(map);
	while (true)
	{
		ExMessage msg;
		while (peekmessage(&msg, EM_MOUSE))
		{
			switch (msg.message)
			{
			case WM_LBUTTONDOWN:    //鼠标左键点击
			case WM_RBUTTONDOWN:    //鼠标右键点击
				MouseMsg(&msg, map);
				//system("cls");
				//show(map);
				int ret = Judge(map, msg.y / Size, msg.x / Size);
				if (ret == 1 || ret == -1)
				{
					mciSendString("status a volume", a, 100, NULL);
					num = 100;
					sprintf(a, "setaudio a volume to %d", num);
					mciSendString(a, NULL, 0, NULL);
				}
				if (ret == -1)
				{
					if (!t)
					{
						drawB(map);
						t = PlaySound("./images/defeat.wav", NULL, SND_SYNC | SND_FILENAME);
					}
					if (t)
					{
						mciSendString("status a volume", a, 100, NULL);
						num = 500;
						sprintf(a, "setaudio a volume to %d", num);
						mciSendString(a, NULL, 0, NULL);
					}

					int select = MessageBox(GetHWnd(), "菜逼，是否再来一局", "呵呵", MB_OKCANCEL);
					if (select == IDOK)//再来一局
					{
						init(map);
					}
					else
					{
						exit(0);
					}
				}
				if (ret == 1)
				{
					if (!t)
					{
						drawB(map);
						t = PlaySound("./images/victory.wav", NULL, SND_SYNC | SND_FILENAME);
					}
					if (t)
					{
						mciSendString("status a volume", a, 100, NULL);
						num = 500;
						sprintf(a, "setaudio a volume to %d", num);
						mciSendString(a, NULL, 0, NULL);
					}

					int select = MessageBox(GetHWnd(), "恭喜你，是否再来一局", "嗯哼", MB_OKCANCEL);
					if (select == IDOK)//再来一局
					{
						init(map);
					}
					else
					{
						exit(0);
					}
				}
				t = false;
				break;
			}
		}
		draw(map);
	}

	getchar();
	return 0;
}

//初始化数据，-1表示雷
void init(int map[ROW][COL])
{
	//随机设置10个雷
	srand((unsigned int)time(NULL));
	memset(map, 0, sizeof(int) * ROW * COL);
	int g = 0;
	while (g < Mine)
	{
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] != -1)
		{
			map[r][c] = -1;
			g++;
		}
	}

	//每个雷周围+1
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			if (map[i][j] == -1)
			{
				for (int p = i - 1; p <= i + 1; p++)
				{
					for (int q = j - 1; q <= j + 1; q++)
					{
						if (p >= 0 && p < ROW && q >= 0 && q < COL && map[p][q] != -1)
						{
							map[p][q]++;
						}
					}
				}
			}
		}
	}

	//加密格子
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			map[i][j] += 20;
		}
	}
}

void show(int map[ROW][COL])
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			printf("%2d ", map[i][j]);
		}
		printf("\n");
	}
}

//绘制图片
void draw(int map[ROW][COL])
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				putimage(j * Size, i * Size, &imgs[map[i][j]]);
			}
			else if (map[i][j] == -1)
			{
				putimage(j * Size, i * Size, &imgs[9]);
			}
			else if (map[i][j] >= 19 && map[i][j] <= 28)
			{
				putimage(j * Size, i * Size, &imgs[11]);
			}
			else if (map[i][j] >= 39 && map[i][j] <= 48)
			{
				putimage(j * Size, i * Size, &imgs[10]);
			}
		}
	}
}

void drawB(int map[ROW][COL])
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				putimage(j * Size, i * Size, &imgs[map[i][j]]);
			}
			else if (map[i][j] == -1)
			{
				putimage(j * Size, i * Size, &imgs[9]);
			}
			else if (map[i][j] >= 19 && map[i][j] <= 28)
			{
				map[i][j] -= 20;
				if (map[i][j] == -1)
				{
					putimage(j * Size, i * Size, &imgs[9]);
				}
				putimage(j * Size, i * Size, &imgs[map[i][j]]);
			}
			else if (map[i][j] >= 39 && map[i][j] <= 48)
			{
				map[i][j] -= 40;
				if (map[i][j] == -1)
				{
					putimage(j * Size, i * Size, &imgs[9]);
				}
				putimage(j * Size, i * Size, &imgs[map[i][j]]);
			}
		}
	}
}

//鼠标信息
void MouseMsg(ExMessage* msg, int map[ROW][COL])
{
	int r = msg->y / Size;
	int c = msg->x / Size;

	//左键打开格子
	if (msg->message == WM_LBUTTONDOWN)
	{
		if ((map[r][c] >= 19 && map[r][c] <= 28))
		{
			PlaySound("./images/left.wav", NULL, SND_ASYNC | SND_FILENAME);
			map[r][c] -= 20;
			Blank(map, r, c);
			isfirst = true;
		}
	}
	//右键标记格子
	if (msg->message == WM_RBUTTONDOWN)
	{
		PlaySound("./images/right.wav", NULL, SND_ASYNC | SND_FILENAME);
		if (map[r][c] >= 19 && map[r][c] <= 28)
		{
			map[r][c] += 20;
		}
		else if (map[r][c] >= 39 && map[r][c] <= 48)
		{
			map[r][c] -= 20;
		}
	}
}

//判断点开空白格子周围是否空白
void Blank(int map[ROW][COL], int x, int y)
{
	if (map[x][y] == 0)
	{
		for (int r = x - 1; r <= x + 1; r++)
		{
			for (int c = y - 1; c <= y + 1; c++)
			{
				if (r >= 0 && r < ROW && c >= 0 && c < COL && map[r][c] >= 19 && map[r][c] <= 28)
				{
					if (isfirst)
					{
						PlaySound("./images/boom.wav", NULL, SND_ASYNC | SND_FILENAME);
						isfirst = false;
					}
					map[r][c] -= 20;
					Blank(map, r, c);
				}
			}
		}
	}
}

//判断输赢
int Judge(int map[ROW][COL], int x, int y)
{
	if (map[x][y] == -1)
	{
		return -1;
	}

	int cnt = 0;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				cnt++;
			}
		}
	}
	if (cnt == ROW * COL - Mine)
	{
		return 1;
	}

	return 0;
}
