#include "stdafx.h"
#include "CppUnitTest.h"
#include "../NaughtsAndCrossesLib/NaughtsAndCrossesLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		GameTree gt;
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

		TEST_METHOD(NodeCount) {
			Assert::AreEqual(gt.start->successorCount, 9);
			// with duplicates
			//Assert::AreEqual(GameTreeNode::nodeCount, 549945);
			Assert::AreEqual(gt.size(), (size_t)5478);
		}

		TEST_METHOD(score) {
			XO b[][3] = {
				{ XO::X, XO::E, XO::E },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::E, XO::E }
			};
			Board board = b;
			ShortBoard s = shortBoard(board);
			GameTreeNode *n = gt.getNode(s);
			Board ne = n->successors[n->choice]->current;
			XO e[][3] = {
				{ XO::X, XO::E, XO::X },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::E, XO::E }
			};
			Assert::IsTrue(Board(e)==ne);
		}
		TEST_METHOD(score2) {
			XO b[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::O, XO::O, XO::E },
				{ XO::O, XO::X, XO::E }
			};
			Board board(b);
			ShortBoard s = shortBoard(&board);
			GameTreeNode *n = gt.getNode(s);
			Board ne = n->successors[n->choice]->current;
			XO e[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::O, XO::O, XO::X },
				{ XO::O, XO::X, XO::E }
			};
			Assert::IsTrue(Board(e) == ne);
		}
		TEST_METHOD(score3) {
			XO b[][3] = {
				{ XO::X, XO::O, XO::X },
				{ XO::E, XO::O, XO::E },
				{ XO::O, XO::E, XO::E }
			};
			Board board = b;
			ShortBoard s = shortBoard(&board);
			GameTreeNode *n = gt.getNode(s);
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
			XO b[][3] = {
				{ XO::O, XO::X, XO::O },
				{ XO::O, XO::X, XO::E },
				{ XO::E, XO::E, XO::E }
			};
			Board board = b;
			ShortBoard s = shortBoard(&board);
			GameTreeNode *n = gt.getNode(s);
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