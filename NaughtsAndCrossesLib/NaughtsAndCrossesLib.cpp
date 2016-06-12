// NaughtsAndCrossesLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NaughtsAndCrossesLib.h"

// convert ternary value stored in byte array to short integer value
NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(const Board const& board)
{
	ShortBoard v, b = 0, pow = 1;
	for (int j, i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			v = board.grid[i][j];
			b += v*pow;
			pow *= 3;
		}
	}
	return b;
}
NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(Board* board)
{
	return shortBoard(*board);
}

const int UNVISITED = 9999;
void GameTree::createTree(GameTreeNode* t, XO player) {
	int n = createSuccessors(t, player);
	player = (player == XO::O) ? XO::X : XO::O;
	for (int i = 0; i < n; ++i) {
		createTree(t->successors[i], player);
	}
}

GameTree::GameTree()
	:start(new GameTreeNode())
{
	nodemap[start->current] = start;
	createTree(start, XO::O);
	calculateScore(start, XO::O);
}

GameTree::~GameTree() {
	for (auto v : nodemap) {
		delete v.second;
	}
}

GameTreeNode * GameTree::getNode(ShortBoard b)
{
	return nodemap[b];
}

size_t GameTree::size()
{
	return nodemap.size();
}

GameTreeNode::GameTreeNode() : GameTreeNode(0)
{
}

GameTreeNode::GameTreeNode(ShortBoard board) 
	: current(board)
	, score(UNVISITED)
{
}

inline void countPlayer(int v, XO player, int& count) {
	if (v == player) ++count;
}
bool isWin(const Board& b, XO player) {
	int d1Sum = 0, d2Sum = 0, rowSum, colSum, i, j;
	for (i = 0; i < 3; ++i) {
		rowSum = 0, colSum = 0;
		for (j = 0; j < 3; ++j) {
			countPlayer(b.grid[i][j], player, rowSum);
			countPlayer(b.grid[j][i], player, colSum);
		}
		if (rowSum == 3 || colSum == 3) return true;
		countPlayer(b.grid[i][i], player, d1Sum);
		countPlayer(b.grid[2-i][i], player, d2Sum);
	}
	if (d1Sum == 3 || d2Sum == 3) return true;
	return false;
}
inline bool isFull(const Board& b) {
	for (int j, i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			if (b.grid[i][j] == XO::E) return false;
		}
	}
	return true;
}
int GameTree::createSuccessors(GameTreeNode* node, XO player)
{
	Board board = node->current;
	node->successorCount = 0;
	if (!isWin(board, player == XO::O ? XO::X : XO::O)) {
		Board** successorBoards = new Board*[9];
		for (int j, i = 0; i < 3; ++i) {
			for (j = 0; j < 3; ++j) {
				if (board.grid[i][j] == XO::E) {
					Board* successor = new Board(node->current);
					successor->grid[i][j] = player;
					successorBoards[node->successorCount++] = successor;
				}
			}
		}
		node->successors = new GameTreeNode*[node->successorCount];
		for (int i = 0; i < node->successorCount; ++i) {
			ShortBoard succ = shortBoard(successorBoards[i]);
			GameTreeNode* gtn = nodemap[succ];
			if (gtn == NULL) {
				nodemap[succ] = gtn = new GameTreeNode(succ);
			}
			node->successors[i] = gtn;
			delete successorBoards[i];
		}
		delete successorBoards;
	}
	return node->successorCount;
}

inline bool endGame(GameTreeNode* node) {
	return node->successorCount == 0;
}
inline bool visited(GameTreeNode* node) {
	return node->score != UNVISITED;
}
int calculateScore(GameTreeNode *node, XO player)
{
	if (visited(node)) {
		return node->score;
	}
	XO nextPlayer = player == XO::O ? XO::X : XO::O;
	if (isWin(Board(node->current), nextPlayer)) {
		int depth = 9 - node->successorCount;
		return node->score = nextPlayer == XO::O ? depth - 10 : 10 - depth;
	}
	if (endGame(node)) {
		return node->score = 0;
	}
	int i, score, best = player == XO::O ? 20 : -20;
	for (i = 0; i < node->successorCount; ++i) {
		score = calculateScore(node->successors[i], nextPlayer);
		if (player == XO::O && score < best || player == XO::X && score > best) {
			node->choice = i;
			best = node->successors[i]->score;
		}
	}
	return node->score = node->successors[node->choice]->score;
}

Board::Board(ShortBoard b)
{
	ShortBoard pow = 3;
	unsigned char i, j;
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			int val = b%pow;
			grid[i][j] = (XO)(val / (pow / 3));
			b -= val;
			pow *= 3;
		}
	}
}

Board::Board(const XO b[3][3])
{
	unsigned char i, j;
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			grid[i][j] = b[i][j];
		}
	}
}

bool Board::operator==(Board const & a)
{
	return shortBoard(a) == shortBoard(this);
}

Board::~Board()
{
}
