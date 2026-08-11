#include <iostream>
#include "defs.h"

void ShowSqAtBySide(const int side, const S_BOARD *pos)
{
    int rank = 0;
    int file = 0;
    int sq = 0;

    std::cout << "\nSquares attacked by: " << SideChar[side] << std::endl;

    for (rank = RANK_8; rank >= -RANK_1; rank--)
    {
        for (file = FILE_A; file <= FILE_H; file++)
        {
            sq = FR2SQ(file, rank);
            if (SqAttacked(sq, side, pos) == true)
            {
                std::cout << "X";
            }
            else
            {
                std::cout << "-";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int main()
{
    AllInit();

    S_BOARD pos[1] = {};
    S_SEARCHINFO info[1] = {};
    InitPvTable(pos->PvTable);
    ConsoleLoop(pos, info);

    // An array to prevent the calls looking like:-
    // S_BOARD board;
    // ParseFen(START_FEN, &board);
    // PrintBoard(&board);
    // S_BOARD board[1];
    // S_MOVELIST list[1];
    // S_SEARCHINFO info[1];

    // // ParseFen(START_FEN, board);
    // ParseFen("r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1", board);
    // // PerftTest(5, board);

    // char input[6];
    // int move = NOMOVE;
    // while (true)
    // {
    //     PrintBoard(board);
    //     std::cout << "Enter a move > ";
    //     std::cin >> input;
    //     if (input[0] == 'q')
    //         break;
    //     else if (input[0] == 't')
    //         TakeMove(board);
    //     else if (input[0] == 'p')
    //         PerftTest(4, board);
    //     else if (input[0] == 'r')
    //     {
    //         int max = GetPvLine(4, board);
    //         std::cout << "PvLine of " << max << " Moves: ";
    //         for (int pvNum = 0; pvNum < max; pvNum++)
    //         {
    //             move = board->PvArray[pvNum];
    //             std::cout << PrMove(move) << " ";
    //         }
    //         std::cout << "\n";
    //     }
    //     else if (input[0] == 's')
    //     {
    //         info->depth = 6;
    //         info->startTime = GetTimeMs();
    //         info->stopTime = GetTimeMs() + 200000;
    //         SearchPosition(board, info);
    //     }
    //     else
    //     {
    //         move = ParseMove(input, board);
    //         if (move != NOMOVE)
    //         {
    //             StorePvMove(board, move);
    //             MakeMove(board, move);
    //         }
    //         else
    //         {
    //             std::cout << "Move not parsed: " << input << std::endl;
    //         }
    //     }
    // }

    // ParseFen((char *)START_FEN, board);
    // ASSERT(CheckBoard(board));
    // PrintBoard(board);

    // ParseFen((char *)("rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"), board);
    // ParseFen((char *)("rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b - e3 0 1"), board);
    // ParseFen((char *)("5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"), board);
    // ParseFen((char *)("6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"), board);
    // ParseFen((char *)("6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1"), board);
    // ParseFen((char *)("6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 w - - 0 1"), board);
    // ParseFen((char *)("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"), board);
    // ParseFen((char *)("r3k2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"), board);
    // ParseFen((char *)("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), board);

    // PrintBoard(board);

    // GenerateAllMoves(board, list);

    // PrMoveList(list);

    // ShowSqAtBySide(WHITE, board);
    // ShowSqAtBySide(BLACK, board);

    // int move = 0;
    // int from = A2, to = H7;
    // int cap = wR, prom = bR;

    // move = ((from) | (to << 7) | (cap << 14) | (prom << 20));

    // std::cout << "from: " << FROMSQ(move) << "\nto: " << TOSQ(move) << "\ncap: " << CAPTURED(move) << "\nprom: " << PROMOTED(move) << std::endl;

    // std::cout << "Algebraic from: " << PrSq(from) << std::endl;
    // std::cout << "Algebraic to: " << PrSq(to) << std::endl;
    // std::cout << "Algebraic move: " << PrMove(move) << std::endl;

    // ParseFen((char *)FEN_1, board);
    // PrintBoard(board);

    // ParseFen((char *)FEN_2, board);
    // PrintBoard(board);

    // ParseFen((char *)FEN_3, board);
    // PrintBoard(board);

    // ParseFen((char *)FEN_4, board);
    // PrintBoard(board);
    if (pos->PvTable->pTable != NULL)
    {
        free(pos->PvTable->pTable);
        pos->PvTable->pTable = NULL;
        pos->PvTable->numEntries = 0;
    }
    return 0;
}