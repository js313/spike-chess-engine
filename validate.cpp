#include "defs.h"

int SqOnBoard(const int sq)
{
    return FilesBrd[sq] != OFFBOARD;
}

int SideValid(const int side)
{
    return (side == WHITE || side == BLACK);
}

int FileRankValid(const int fr)
{
    return (fr >= 0 && fr <= 7);
}

int PieceValidEmpty(const int pce)
{
    return (pce >= EMPTY && pce <= bK);
}

int PieceValid(const int pce)
{
    return (pce >= wP && pce <= bK);
}