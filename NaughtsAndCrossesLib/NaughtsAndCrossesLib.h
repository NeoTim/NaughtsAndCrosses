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
#include <set>
#include <tuple>
#include <fstream>

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
	bool operator==(const Board& a);
};

class NAUGHTSANDCROSSESLIB_API GameTreeNode;
typedef NAUGHTSANDCROSSESLIB_API std::map<ShortBoard, GameTreeNode*> NodeMap;

class NAUGHTSANDCROSSESLIB_API GameTreeNode {
public:
	GameTreeNode();
	GameTreeNode(ShortBoard board);
	ShortBoard current;
	GameTreeNode** successors;
	unsigned char successorCount;
	int score;
	int choice;
	void serialize(std::ostream& os, std::set<ShortBoard>& visited);
	unsigned int deserialize(std::ifstream& is, NodeMap& nodeMap);
};

class NAUGHTSANDCROSSESLIB_API GameTree {
	NodeMap nodemap;
	unsigned char createSuccessors(GameTreeNode* node, XO player);
	void createTree(GameTreeNode* t, XO player);
public:
	GameTreeNode* start;
	GameTree();
	GameTree(const std::string& fileName);
	~GameTree();
	GameTreeNode* getNode(ShortBoard);
	size_t size();
	void serialize(const std::string& fileName);
};

class FileException : public std::runtime_error {
public:
	FileException(const std::string& fileName);
};

NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(const Board& board);
NAUGHTSANDCROSSESLIB_API ShortBoard shortBoard(Board* board);
NAUGHTSANDCROSSESLIB_API bool isWin(const Board& board, XO player);
NAUGHTSANDCROSSESLIB_API bool isFull(const Board& board);
NAUGHTSANDCROSSESLIB_API int calculateScore(GameTreeNode* node, XO player);
typedef NAUGHTSANDCROSSESLIB_API std::tuple<unsigned char, unsigned char> Bytes;
NAUGHTSANDCROSSESLIB_API Bytes boardToBytes(ShortBoard);
NAUGHTSANDCROSSESLIB_API ShortBoard bytesToBoard(Bytes chars);
NAUGHTSANDCROSSESLIB_API void serializeShortBoard(std::ostream& os, ShortBoard);
NAUGHTSANDCROSSESLIB_API ShortBoard deserializeShortBoard(std::ifstream& is);
NAUGHTSANDCROSSESLIB_API GameTree* lazyGameTree();
