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
            if (SqAttacked(sq, side, pos) == TRUE)
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

    // An array to prevent the calls looking like:-
    // S_BOARD board;
    // ParseFen(START_FEN, &board);
    // PrintBoard(&board);
    S_BOARD board[1];

    ParseFen((char *)START_FEN, board);
    ASSERT(CheckBoard(board));
    PrintBoard(board);

    ParseFen((char *)("8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2"), board);
    PrintBoard(board);

    ShowSqAtBySide(WHITE, board);
    ShowSqAtBySide(BLACK, board);

    // ParseFen((char *)FEN_1, board);
    // PrintBoard(board);

    // ParseFen((char *)FEN_2, board);
    // PrintBoard(board);

    // ParseFen((char *)FEN_3, board);
    // PrintBoard(board);

    // ParseFen((char *)FEN_4, board);
    // PrintBoard(board);

    return 0;
}