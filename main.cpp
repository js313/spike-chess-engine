#include <iostream>
#include "defs.h"

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

    ParseFen((char *)FEN_1, board);
    PrintBoard(board);

    ParseFen((char *)FEN_2, board);
    PrintBoard(board);

    ParseFen((char *)FEN_3, board);
    PrintBoard(board);

    ParseFen((char *)FEN_4, board);
    PrintBoard(board);

    return 0;
}