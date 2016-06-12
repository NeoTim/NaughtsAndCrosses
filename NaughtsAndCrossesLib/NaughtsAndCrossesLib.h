// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NAUGHTSANDCROSSESLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NAUGHTSANDCROSSESLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NAUGHTSANDCROSSESLIB_EXPORTS
#define NAUGHTSANDCROSSESLIB_API __declspec(dllexport)
#else
#define NAUGHTSANDCROSSESLIB_API __declspec(dllimport)
#endif
#include <map>

enum XO {
	E,
	O,
	X
};

typedef unsigned short ShortBoard;
class NAUGHTSANDCROSSESLIB_API Board {
public:
	Board(ShortBoard b);
	Board(const XO b[3][3]);
	~Board();
	XO grid[3][3];
	bool operator==(Board const& a);
};

class NAUGHTSANDCROSSESLIB_API GameTreeNode;
typedef NAUGHTSANDCROSSESLIB_API std::map<ShortBoard, GameTreeNode*> NodeMap;
class NAUGHTSANDCROSSESLIB_API GameTreeNode {
public:
	GameTreeNode();
	GameTreeNode(ShortBoard board);
	ShortBoard current;
	GameTreeNode** successors;
	int successorCount;
	int score;
	int choice;
};
class NAUGHTSANDCROSSESLIB_API GameTree {
	NodeMap nodemap;
	int createSuccessors(GameTreeNode* node, XO player);
	void createTree(GameTreeNode* t, XO player);
public:
	GameTreeNode* start;
	GameTree();
	~GameTree();
	GameTreeNode* getNode(ShortBoard);
	size_t size();

};

NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(const Board const& board);
NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(Board* board);
NAUGHTSANDCROSSESLIB_API bool isWin(const Board& board, XO player);
NAUGHTSANDCROSSESLIB_API bool isFull(const Board& board);
NAUGHTSANDCROSSESLIB_API int calculateScore(GameTreeNode* node, XO player);
