#pragma once
#include <iostream>
#include <string>
#include "../NaughtsAndCrossesLib/NaughtsAndCrossesLib.h"

std::string toString(XO v);
void showBoard(ShortBoard b);

class Game
{
	GameTree gt;
public:
	Game();
	~Game();
};

