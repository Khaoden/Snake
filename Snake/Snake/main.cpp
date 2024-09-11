#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

#define H 28
#define W 60

enum BlockType {
	EMPTY = 0,
	FOOD = 1,
};

struct Map {
	BlockType data[H][W];
	bool hasFood;
};

struct Pos {
	int x;
	int y;
};

struct Snake {
	Pos snake[H * W];
	int snakeDir;
	int snakeLength;
	int lastMoveTime;
	int moveFrequency;
};

const int dir[4][2] = {
	{-1, 0},  // 上
	{0, 1},   // 右
	{1, 0},   // 下
	{0, -1}    // 左
};

//struct BlockType {
//	int EMPTY = 0;
//	int FOOD = 1;
//};

void initSnake(Snake* snk) {
	snk->snakeLength = 1;
	snk->snakeDir = 1;
	snk->snake[0] = { W / 2, H / 2 };
	snk->lastMoveTime = 0;
	snk->moveFrequency = 200;
}

void hideCursor() {
	HANDLE hOutput =  GetStdHandle(STD_OUTPUT_HANDLE);  // 拿到输出窗口的聚点，即拿到具体的窗口
	CONSOLE_CURSOR_INFO curInfo = { 1, FALSE };         // 拿到光标这个结构体，即窗口里的光标
	SetConsoleCursorInfo(hOutput, &curInfo);            // 隐藏光标，传参是这样的
}

void initMap(Map* map) {
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			BlockType a;
			//map->data[y][x] = a.EMPTY;                // test
			map->data[y][x] = BlockType::EMPTY;         // 等价于赋值为0，枚举的赋值方法
		}
	}
	map->hasFood = false;
}

void drawUnit(Pos p, const char unit[]) {
	COORD coord;
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);   // 拿到输出窗口的聚点，即拿到具体的窗口
	coord.X = p.x + 1;
	coord.Y = p.y + 1;
	SetConsoleCursorPosition(hOutput, coord);           // 光标移动到对应位置
	cout << unit;
}

void drawMap(Map* map) {
	system("cls");    // 将控制台清空
	cout << "┏";
	for (int i = 0; i < W; ++i) {
		cout << "━";
	}
	cout << "┓" << endl;
	for (int y = 0; y < H; ++y) {
		cout << "┃";
		for (int x = 0; x < W; ++x) {
			if (map->data[y][x] == BlockType::EMPTY) {  // 判断是否有食物
				cout << " ";
			}
		}
		cout << "┃" << endl;
	}
	cout << "┗";
	for (int x = 0; x < W; ++x) {
		cout << "━";
	}
	cout << "┛";

}

void drawSnake(Snake* snake) {
	for (int i = 0; i < snake->snakeLength; ++i) {
		drawUnit(snake->snake[i], "■");
	}
}

void checkEatFood(Snake* snk, Pos tail, Map* map) {
	Pos head = snk->snake[0];
	if (map->data[head.y][head.x] == BlockType::FOOD) {
	snk->snake[snk->snakeLength++] = tail;
		map->data[head.y][head.x] == BlockType::EMPTY;
		map->hasFood = false;
	}
}

void moveSnake(Snake* snk) {
	for (int i = snk->snakeLength - 1; i >= 1; --i) {
		snk->snake[i] = snk->snake[i - 1];
	}
	snk->snake[0].y += dir[snk->snakeDir][0];
	snk->snake[0].x += dir[snk->snakeDir][1];	
	//cout << snk->snake[0].x;
	//cout << snk->snake[0].y;                                    // 测试

}

bool checkOutOfBound(Pos p) {
	return (p.x == 0 || p.x == W+1 || p.y == 0 || p.y == H+1);
}

bool doMove(Snake* snk, Map* map) {                              // 蛇移动
	Pos tail = snk->snake[snk->snakeLength - 1];
	drawUnit(tail, " ");
	if (checkOutOfBound(snk->snake[0])) {
		return false;
	}
	moveSnake(snk);
	checkEatFood(snk, tail, map);
	drawUnit(snk->snake[0], "■");

	return true;	
}

bool checkSnakeMove(Snake* snk, Map* map) {
	int curTime = GetTickCount();
	if (curTime - snk->lastMoveTime > snk->moveFrequency) {
		if (false == doMove(snk, map))
			return false;
		snk->lastMoveTime = curTime;
	}
	return true;
}

void checkChangeDir(Snake* snk) {
	if (_kbhit()) {
		switch (_getch()) {
		case 'w':
			if (snk->snakeDir != 2)
				snk->snakeDir = 0;
			break;
		case 'd':
			if (snk->snakeDir != 3)
				snk->snakeDir = 1;
			break;
		case 's':
			if (snk->snakeDir != 0)
				snk->snakeDir = 2;
			break;
		case 'a':
			if (snk->snakeDir != 1)
				snk->snakeDir = 3;
			break;
		default:
			break;
		}
	}
}

void checkFoodGenerate(Snake* snk, Map* map) {
	if (false == map->hasFood) {
		while (1) {
			int x = rand() % W;                    // 获取0到W-1之间的随机数字
			int y = rand() % H;                    // 获取0到H-1之间的随机数字
			int i = 0;
			while (i < snk->snakeLength) {
				if (x == snk->snake[i].x && y == snk->snake[i].y) {
					break;
				}
				++i;
			}
			if (i == snk->snakeLength) {
				map->data[y][x] = BlockType::FOOD;
				map->hasFood = true;
				drawUnit({ x,y }, "●");
				return;
			}
		}
	}
}

void initGame(Map* map, Snake* snk) {
	hideCursor();
	initMap(map);
	initSnake(snk);
	drawMap(map);
	drawSnake(snk);
}

int main() {
	Map map;
	Snake snk;
	initGame(&map, &snk);
	// snk.snake[1] = { W / 2 + 1, H / 2 };        // 移动test
	while (1) {
		checkChangeDir(&snk);
		if (false == checkSnakeMove(&snk, &map)) {
			break;
		}
		checkFoodGenerate(&snk, &map);
	}
	drawUnit({ W / 2 - 4, H / 2 }, "Game Over");
	while (1) {};
	return 0;
}