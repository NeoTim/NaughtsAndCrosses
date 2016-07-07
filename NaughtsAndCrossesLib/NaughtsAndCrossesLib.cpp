// NaughtsAndCrossesLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NaughtsAndCrossesLib.h"
#include <vector>
#include <iterator>
#include <limits>

// convert ternary value stored in byte array to short integer value
NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(const Board& board)
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

const char UNVISITED = SCHAR_MAX;
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
		delete[] v.second->successors;
		delete v.second;
	}
}

GameTreeNode* GameTree::getNode(ShortBoard b)
{
	return nodemap[b];
}

size_t GameTree::size()
{
	return nodemap.size();
}

void GameTree::serialize(const std::string& fileName)
{
	std::ofstream fout(fileName, std::ios::binary | std::ios::out);
	std::set<ShortBoard> visited;
	start->serialize(fout, visited);
}
GameTreeNode* getOrCreateGameTreeNode(ShortBoard b, NodeMap& nodemap) {
	GameTreeNode* gtn = nodemap[b];
	if (gtn == NULL) {
		nodemap[b] = gtn = new GameTreeNode(b);
	}
	return gtn;
}

GameTreeNode::GameTreeNode() : GameTreeNode(0)
{
}

unsigned int nodeCount = 0;

GameTreeNode::GameTreeNode(ShortBoard board) 
	: current(board)
	, score(UNVISITED)
	, successorCount(0)
{
	nodeCount++;
}

unsigned int nodesSerialised = 0;
bool GameTreeNode::operator==(const GameTreeNode & other) const
{
	if (current != other.current) {
		return false;
	}
	if (choice != other.choice) {
		return false;
	}
	if (successorCount != other.successorCount) {
		return false;
	}
	for (unsigned char i = 0; i < successorCount; i++) {
		if (*successors[i] != *other.successors[i]) return false;
	}
	return true;
}
bool GameTreeNode::operator!=(const GameTreeNode & other) const
{
	return !(*this == other);
}
void GameTreeNode::serialize(std::ostream& os, std::set<ShortBoard>& visited)
{
	nodesSerialised++;
	visited.insert(current);
	serializeShortBoard(os, current);
	os << successorCount;
	os << choice;
	for (unsigned char i = 0; i < successorCount; ++i) {
		serializeShortBoard(os, successors[i]->current);
	}
	for (unsigned char i = 0; i < successorCount; ++i) {
		GameTreeNode* v = successors[i];
		if (visited.find(v->current) == visited.end()) {
			v->serialize(os, visited);
		}
	}
}

unsigned int GameTreeNode::deserialize(std::ifstream& is, NodeMap& nodemap)
{
	unsigned int bytesRead = 4; // first board and length
	char x;
	is.read(&x, 1);
	successorCount = static_cast<unsigned char>(x);
	is.read(&choice, 1);
	successors = new GameTreeNode*[successorCount];
	for (unsigned char i = 0; i < successorCount; ++i) {
		ShortBoard b = deserializeShortBoard(is);
		GameTreeNode* gtn = getOrCreateGameTreeNode(b, nodemap);
		successors[i] = gtn;
		bytesRead += 2;
	}
	return bytesRead;
}

GameTree::GameTree(const std::string & fileName)
{
	std::ifstream fin(fileName, std::ios::binary);
	if (!fin) {
		throw FileException(fileName);
	}
	fin.seekg(0, fin.end);
	unsigned int length = fin.tellg();
	fin.seekg(0, fin.beg);
	unsigned int bytesRead = 0;
	while (bytesRead < length) {
		ShortBoard b = deserializeShortBoard(fin);
		GameTreeNode* gtn = getOrCreateGameTreeNode(b, nodemap);
		if (b == 0) {
			start = gtn;
		}
		bytesRead += gtn->deserialize(fin, nodemap);
	}
	//calculateScore(start, XO::O);
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

unsigned char GameTree::createSuccessors(GameTreeNode* node, XO player)
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
			GameTreeNode* gtn = getOrCreateGameTreeNode(succ, nodemap);
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

char calculateScore(GameTreeNode *node, XO player)
{
	if (visited(node)) {
		return node->score;
	}
	XO nextPlayer = player == XO::O ? XO::X : XO::O;
	if (isWin(Board(node->current), nextPlayer)) {
		char depth = 9 - node->successorCount;
		return node->score = nextPlayer == XO::O ? depth - 10 : 10 - depth;
	}
	if (endGame(node)) {
		return node->score = 0;
	}
	char i, score, best = player == XO::O ? 20 : -20;
	for (i = 0; i < node->successorCount; ++i) {
		score = calculateScore(node->successors[i], nextPlayer);
		if (player == XO::O && score < best || player == XO::X && score > best) {
			node->choice = i;
			best = node->successors[i]->score;
		}
	}
	return node->score = node->successors[node->choice]->score;
}

NAUGHTSANDCROSSESLIB_API Bytes boardToBytes(ShortBoard board)
{
	unsigned char low = board & 0x00FF, high = (board & 0xFF00) >> 8;
	return { low, high };
}

NAUGHTSANDCROSSESLIB_API ShortBoard bytesToBoard(Bytes bytes)
{
	ShortBoard low, high;
	std::tie(low, high) = bytes;
	return (high << 8) | low;
}

NAUGHTSANDCROSSESLIB_API void serializeShortBoard(std::ostream& os, ShortBoard b)
{
	Bytes bytes = boardToBytes(b);
	os << std::get<0>(bytes) << std::get<1>(bytes);
}

NAUGHTSANDCROSSESLIB_API ShortBoard deserializeShortBoard(std::ifstream & is)
{
	unsigned char low, high;
	//is >> low >> high;
	char x;
	is.read(&x, 1);
	low = static_cast<unsigned char>(x);
	is.read(&x, 1);
	high = static_cast<unsigned char>(x);
	return bytesToBoard({ low, high });
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

FileException::FileException(const std::string & fileName)
	: std::runtime_error("File not found " + fileName)
{
}

GameTree* lazyGameTree() {
	std::string fileName("GameTree.bin");
	GameTree* gt;
	try {
		gt = new GameTree(fileName);
	}
	catch (FileException fe) {
		gt = new GameTree();
		gt->serialize(fileName);
	}
	return gt;
}