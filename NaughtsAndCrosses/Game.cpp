#include "Game.h"

std::string toString(XO v) {
	switch (v) {
	case XO::O:
		return "O";
	case XO::X:
		return "X";
	}
	return " ";
}
void clearScreen() {
	std::cout << std::string(100, '\n');
}
void showBoard(ShortBoard b) {
	Board a = b;
	clearScreen();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			std::cout << toString(a.grid[i][j]);
		}
		std::cout << std::endl;
	}
	std::cout << "---" << std::endl;
}
Game::Game()
{
	GameTreeNode *s = gt.start;
	int i, j, k = 0;
	XO player = XO::E;
	char keys[] = "123qweasd";
	std::map<char, std::tuple<int, int>> keyIndex;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			keyIndex[keys[k++]] = std::make_tuple(i, j);
		}
	}
	clearScreen();
	std::cout << "---" << std::endl;
	do {
		for (k = 0, i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				std::cout << keys[k++];
			}
			std::cout << std::endl;
		}
		player = player == XO::O ? XO::X : XO::O;
		Board b = s->current;
		if (player == XO::O) {
			char c;
			do {
				std::cin >> c;
				std::tie(i, j) = keyIndex[c];
			} while (b.grid[i][j] != XO::E);
			b.grid[i][j] = player;
			s = gt.getNode(shortBoard(&b));
		}
		else {
			s = s->successors[s->choice];
		}
		showBoard(s->current);

		if (isWin(Board(s->current), player)) {
			std::cout << toString(player) << " Wins!" << std::endl;
			break;
		}
	} while (!isFull(Board(s->current)));
}


Game::~Game()
{
}
