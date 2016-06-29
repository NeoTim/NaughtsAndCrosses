#include "stdafx.h"
#include "CppUnitTest.h"
#include "../NaughtsAndCrossesLib/NaughtsAndCrossesLib.h"
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{	
	TEST_CLASS(Serialization)
	{
		TEST_METHOD(BoardConversions)
		{
			XO b[3][3] = {
				{ XO::E, XO::O, XO::X },
				{ XO::E, XO::O, XO::X },
				{ XO::X, XO::X, XO::O }
			};
			Board board = b;
			ShortBoard s = shortBoard(&board);
			Board board2 = s;
			Assert::IsTrue(board == board2);
		}

		TEST_METHOD(boardToBytesConversion) {
			ShortBoard board = 12345, original = board;
			Bytes bytes = boardToBytes(board);
			ShortBoard rehydrated = bytesToBoard(bytes);
			Assert::IsTrue(rehydrated == original);
		}

		TEST_METHOD(serializeBoard) {
			ShortBoard board = 12345, original = board;
			Bytes bytes = boardToBytes(board);
			{
				std::ofstream fout("file.bin", std::ios::binary | std::ios::out);
				fout << std::get<0>(bytes) << std::get<1>(bytes);
			}
			{
				std::ifstream fin("file.bin", std::ios::binary);
				unsigned char low, high;
				fin >> low >> high;
				ShortBoard rehydrated = bytesToBoard({ low, high });
				Assert::IsTrue(rehydrated == original);
			}

		}
		TEST_METHOD(serializeGameTree) {
			bool fileNotFound = false;
			try {
				GameTree gt("somefilethatdoesntexist");
			}
			catch (FileException x) {
				fileNotFound = true;
			}
			Assert::IsTrue(fileNotFound);

			GameTree gt;
			Assert::AreEqual(gt.size(), (size_t)5478);
			gt.serialize("TestGameTree.bin");

			GameTree rehydrated("TestGameTree.bin");
			size_t size = rehydrated.size();
			Assert::AreEqual(size, (size_t)5478);
		}
	};
	TEST_CLASS(GamePlay)
	{
	public:

		TEST_METHOD(NodeCount) {
			GameTree* gt = lazyGameTree();
			Assert::AreEqual(gt->start->successorCount, (unsigned char)9);
			// with duplicates
			//Assert::AreEqual(GameTreeNode::nodeCount, 549945);
			Assert::AreEqual(gt->size(), (size_t)5478);
		}

		TEST_METHOD(score) {
			GameTree* gt = lazyGameTree();
			XO b[][3] = {
				{ XO::X, XO::E, XO::E },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::E, XO::E }
			};
			Board board = b;
			ShortBoard s = shortBoard(board);
			GameTreeNode *n = gt->getNode(s);
			Board ne = n->successors[n->choice]->current;
			XO e[][3] = {
				{ XO::X, XO::E, XO::X },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::E, XO::E }
			};
			Assert::IsTrue(Board(e)==ne);
		}
		TEST_METHOD(score2) {
			GameTree* gt = lazyGameTree();
			XO b[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::O, XO::O, XO::E },
				{ XO::O, XO::X, XO::E }
			};
			Board board(b);
			ShortBoard s = shortBoard(&board);
			GameTreeNode *n = gt->getNode(s);
			Board ne = n->successors[n->choice]->current;
			XO e[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::O, XO::O, XO::X },
				{ XO::O, XO::X, XO::E }
			};
			Assert::IsTrue(Board(e) == ne);
		}
		TEST_METHOD(score3) {
			GameTree* gt = lazyGameTree();
			XO b[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::E, XO::E }
			};
			Board board = b;
			ShortBoard s = shortBoard(&board);
			GameTreeNode *n = gt->getNode(s);
			XO e[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::X, XO::E }
			};
			Assert::IsTrue(Board(e) == Board(n->successors[n->choice]->current));
		}
		void resetVisited(GameTreeNode* n) {
			n->score = 9999;
			for (int i = 0; i < n->successorCount; i++) {
				resetVisited(n->successors[i]);
			}
		}
		TEST_METHOD(score4) {
			GameTree* gt = lazyGameTree();
			XO b[][3] = {
				{ XO::O, XO::X, XO::O },
				{ XO::O, XO::X, XO::E },
				{ XO::E, XO::E, XO::E }
			};
			Board board = b;
			ShortBoard s = shortBoard(&board);
			GameTreeNode *n = gt->getNode(s);
			resetVisited(n);
			calculateScore(n, XO::X);
			XO e[][3] = {
				{ XO::O, XO::X, XO::O },
				{ XO::O, XO::X, XO::E },
				{ XO::E, XO::X, XO::E }
			};
			Assert::IsTrue(Board(e) == Board(n->successors[n->choice]->current));
		}
	};
}