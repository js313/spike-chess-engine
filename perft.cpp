#include "defs.h"

long leafNodes = 0;

void Perft(int depth, S_BOARD *pos)
{
    ASSERT(CheckBoard(pos));

    if (depth == 0)
    {
        leafNodes++;
        return;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    for (MoveNum = 0; MoveNum < list->count; MoveNum++)
    {
        if (!MakeMove(pos, list->moves[MoveNum].move))
        {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }
}

void PerftTest(int depth, S_BOARD *pos)
{
    ASSERT(CheckBoard(pos));

    PrintBoard(pos);
    std::cout << "\nStarting test to depth: " << depth << std::endl;
    leafNodes = 0;

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int move;
    int MoveNum = 0;
    for (MoveNum = 0; MoveNum < list->count; MoveNum++)
    {
        // if (MoveNum == 17)
        // {
        //     PrintBoard(pos);
        // }
        // else
        //     continue;
        move = list->moves[MoveNum].move;
        if (!MakeMove(pos, move))
        {
            continue;
        }
        long cumnodes = leafNodes;
        Perft(depth - 1, pos);
        TakeMove(pos);
        long oldnodes = leafNodes - cumnodes;
        std::cout << "move " << MoveNum + 1 << " : " << PrMove(move) << " : " << oldnodes << std::endl;
    }

    std::cout << "\nTest Complete : " << leafNodes << " nodes visited\n";
}