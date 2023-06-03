#include <iostream>
#include <ctime>
#include <Windows.h>
using namespace std;

#define WIDTH 10
#define HEIGHT 20
#define START_X 4
#define START_Y 0
#define GAME_TICK 0.5 * CLOCKS_PER_SEC

enum Type { I, J, L, O, S, Z, T };

typedef struct Point {
	int x, y;
	bool collision;
};

typedef struct Piece {
	Point* positions;
};

Point world[WIDTH][HEIGHT];
Piece currentMoving;
bool mustCreateNewPiece;
bool mustRotate;

void initialize() {
	mustRotate = false;
	mustCreateNewPiece = true;
	currentMoving.positions = new Point[4];
	for (int j = 0; j < HEIGHT; ++j) {
		for (int i = 0; i < WIDTH; ++i) {
			world[i][j].x = i;
			world[i][j].y = j;
			world[i][j].collision = false;
		}
	}
}

void display() {
	for (int j = 0; j < HEIGHT; ++j) {
		for (int i = 0; i < WIDTH; ++i) {
			cout << (world[i][j].collision ? "1" : "0") << " ";
		}
		cout << "\n";
	}
}

bool isValidPosition(int x, int y) {
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
		return false;
	}
	if (world[x][y].collision) {
		return false;
	}
	return true;
}

void createPiece(int type, int x, int y) {
	switch (type) {
	case 0: {
		if (isValidPosition(x, y) && isValidPosition(x + 1, y) && isValidPosition(x + 2, y) && isValidPosition(x -1, y)) {
			world[x][y].collision = true;
			world[x + 1][y].collision = true;
			world[x + 2][y].collision = true;
			world[x - 1][y].collision = true;
		}
		else {
			//cod pt game over
			cout << "Game Over!\n";
			break;
		}
		currentMoving.positions[0].x = x;
		currentMoving.positions[0].y = y;
		currentMoving.positions[1].x = x + 1;
		currentMoving.positions[1].y = y;
		currentMoving.positions[2].x = x + 2;
		currentMoving.positions[2].y = y;
		currentMoving.positions[3].x = x - 1;
		currentMoving.positions[3].y = y;
		mustCreateNewPiece = false;
		break;
	}
	case 1: {
		if (isValidPosition(x, y) && isValidPosition(x - 1, y) && isValidPosition(x + 1, y) && isValidPosition(x + 1, y + 1)) {
			world[x][y].collision = true;
			world[x - 1][y].collision = true;
			world[x + 1][y].collision = true;
			world[x + 1][y + 1].collision = true;
		}
		break;
	}
	case 2: {
		if (isValidPosition(x, y) && isValidPosition(x - 1, y) && isValidPosition(x + 1, y) && isValidPosition(x - 1, y + 1)) {
			world[x][y].collision = true;
			world[x - 1][y].collision = true;
			world[x + 1][y].collision = true;
			world[x - 1][y + 1].collision = true;
		}
		break;
	}
	case 3: {
		if (isValidPosition(x, y) && isValidPosition(x + 1, y) && isValidPosition(x + 1, y) && isValidPosition(x, y + 1)) {
			world[x][y].collision = true;
			world[x + 1][y].collision = true;
			world[x][y + 1].collision = true;
			world[x + 1][y + 1].collision = true;
		}
		break;
	}
	case 4: {
		if (isValidPosition(x, y) && isValidPosition(x + 1, y) && isValidPosition(x, y + 1) && isValidPosition(x - 1, y + 1)) {
			world[x][y].collision = true;
			world[x + 1][y].collision = true;
			world[x][y + 1].collision = true;
			world[x - 1][y + 1].collision = true;
		}
		break;
	}
	case 5: {
		if (isValidPosition(x, y) && isValidPosition(x - 1, y) && isValidPosition(x, y + 1) && isValidPosition(x + 1, y + 1)) {
			world[x][y].collision = true;
			world[x - 1][y].collision = true;
			world[x][y + 1].collision = true;
			world[x + 1][y + 1].collision = true;
		}
		break;
	}
	case 6: {
		if (isValidPosition(x, y) && isValidPosition(x - 1, y) && isValidPosition(x + 1, y) && isValidPosition(x, y + 1)) {
			world[x][y].collision = true;
			world[x - 1][y].collision = true;
			world[x + 1][y].collision = true;
			world[x][y + 1].collision = true;
		}
		break;
	}
	default: {
		cout << "Invalid piece type\n";
		exit(-1);
	}
	}
}

void moveDown() {
	if(isValidPosition(currentMoving.positions[0].x, currentMoving.positions[0].y + 1) && isValidPosition(currentMoving.positions[1].x, currentMoving.positions[1].y + 1) &&
		isValidPosition(currentMoving.positions[2].x, currentMoving.positions[2].y + 1) && isValidPosition(currentMoving.positions[3].x, currentMoving.positions[3].y + 1)) {

		world[currentMoving.positions[0].x][currentMoving.positions[0].y].collision = false;
		world[currentMoving.positions[1].x][currentMoving.positions[1].y].collision = false;
		world[currentMoving.positions[2].x][currentMoving.positions[2].y].collision = false;
		world[currentMoving.positions[3].x][currentMoving.positions[3].y].collision = false;

		world[currentMoving.positions[0].x][currentMoving.positions[0].y + 1].collision = true;
		world[currentMoving.positions[1].x][currentMoving.positions[1].y + 1].collision = true;
		world[currentMoving.positions[2].x][currentMoving.positions[2].y + 1].collision = true;
		world[currentMoving.positions[3].x][currentMoving.positions[3].y + 1].collision = true;

		++currentMoving.positions[0].y;
		++currentMoving.positions[1].y;
		++currentMoving.positions[2].y;
		++currentMoving.positions[3].y;
	} 
	else {
		mustCreateNewPiece = true;
	}
}

void rotate() {
	int x1a = currentMoving.positions[3].x;
	int y1a = currentMoving.positions[3].y;
	int px = currentMoving.positions[0].x;
	int py = currentMoving.positions[0].y;
	currentMoving.positions[3].x = y1a + px - py;
	currentMoving.positions[3].y = px + py - x1a - 1;

	int x1b = currentMoving.positions[1].x;
	int y1b = currentMoving.positions[1].y;
	currentMoving.positions[1].x = y1b + px - py;
	currentMoving.positions[1].y = px + py - x1b - 1;

	int x1c = currentMoving.positions[2].x;
	int y1c = currentMoving.positions[2].y;
	currentMoving.positions[2].x = y1c + px - py;
	currentMoving.positions[2].y = px + py - x1c - 1;
}

int main() {
	initialize();

	while (true) {
		clock_t now = clock();
		if (mustCreateNewPiece) {
			createPiece(I, START_X, START_Y);
		}

		system("CLS");
		display();
		if (GetKeyState(VK_SPACE) & 0x8000) {
			mustRotate = true;
		}
		while (clock() - now < GAME_TICK);
		if (mustRotate) {
			rotate();
			mustRotate = false;
		}
		moveDown();
	}

	return 0;
}