#include <iostream>
#include <string>
#include "defs.h"

const char *PrSq(const int sq)
{
    // SqStr must be static because we return a pointer to it.
    // A non-static local array has automatic storage duration and is destroyed when the function returns, leaving a dangling pointer.
    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];

    // could use sprintf as well
    SqStr[0] = ('a' + file);
    SqStr[1] = ('1' + rank);
    SqStr[2] = '\0';

    return SqStr; // Array decayed to a pointer
}

char *PrMove(const int move)
{
    static char MvStr[6];

    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int tf = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];

    int promoted = PROMOTED(move);

    if (promoted)
    {
        char pchar = 'q';
        if (IsKn(promoted))
        {
            pchar = 'n';
        }
        else if (IsRQ(promoted) && !IsBQ(promoted))
        {
            pchar = 'r';
        }
        else if (!IsRQ(promoted) && IsBQ(promoted))
        {
            pchar = 'b';
        }
        snprintf(MvStr, 6, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + tf), ('1' + rt), pchar);
    }
    else
    {
        snprintf(MvStr, 5, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + tf), ('1' + rt));
    }

    return MvStr;
}

void PrMoveList(const S_MOVELIST *list)
{
    int index = 0;
    int score = 0;
    int move = 0;
    std::cout << "MoveList:\n";

    for (index = 0; index < list->count; index++)
    {
        move = list->moves[index].move;
        score = list->moves[index].score;

        std::cout << "Move: " << index + 1 << " > " << PrMove(move) << " (score: " << score << ")\n";
    }
    std::cout << "MoveList Total " << list->count << " Moves\n\n";
}