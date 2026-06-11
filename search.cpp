#include "defs.h"

int IsRepitition(const S_BOARD *pos)
{
    int index = 0;

    // can start from index=0, but for optimisation we start from the last pos where fifty move counter was reset
    // as it only resets when there are capture or pawn moves, which are irreversible, so won't be any repetitions
    for (index = pos->hisPly - pos->fiftyMove; index < pos->hisPly - 1; index++)
    {
        if (pos->posKey == pos->history[index].posKey)
        {
            return true;
        }
    }

    return false;
}