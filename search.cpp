#include "defs.h"

static void CheckUp()
{
}

static int IsRepitition(const S_BOARD *pos)
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

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info)
{
    for (int index = 0; index < 13; index++)
    {
        for (int index2 = 0; index2 < BRD_SQ_NUM; index2++)
        {
            pos->searchHistory[index][index2] = 0;
        }
    }

    for (int index = 0; index < 2; index++)
    {
        for (int index2 = 0; index2 < MAX_DEPTH; index2++)
        {
            pos->searchKillers[index][index2] = 0;
        }
    }

    ClearPvTable(pos->PvTable);
    pos->ply = 0;

    info->startTime = GetTimeMs();
    info->stopped = 0;
    info->nodes = 0;

    info->fh = 0;
    info->fhf = 0;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull)
{
    ASSERT(CheckBoard(pos));

    if (depth == 0)
    {
        info->nodes++;
        return EvalPosition(pos);
    }

    info->nodes++;

    if (IsRepitition(pos) || pos->fiftyMove >= 100)
    {
        return 0;
    }

    if (pos->ply > MAX_DEPTH - 1)
    {
        return EvalPosition(pos);
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    int score = -INFINITE;

    for (MoveNum = 0; MoveNum < list->count; MoveNum++)
    {
        if (!MakeMove(pos, list->moves[MoveNum].move))
        {
            continue;
        }

        legal++;
        score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, true);
        TakeMove(pos);

        if (score > alpha)
        {
            if (score >= beta)
            {
                if (legal == 1)
                {
                    info->fhf++;
                }
                info->fh++;
                return beta;
            }
            alpha = score;
            bestMove = list->moves[MoveNum].move;
        }
    }

    if (legal == 0)
    {
        if (SqAttacked(pos->kingSq[pos->side], pos->side ^ 1, pos))
        {
            return -MATE + pos->ply;
        }
        else
        {
            return 0;
        }
    }

    if (alpha != oldAlpha)
    {
        StorePvMove(pos, bestMove);
    }

    return alpha;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info)
{
    return 0;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info)
{
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;
    ClearForSearch(pos, info);

    for (currentDepth = 1; currentDepth <= info->depth; currentDepth++)
    {
        bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, true);
        pvMoves = GetPvLine(currentDepth, pos);
        bestMove = pos->PvArray[0];

        std::cout << "Depth: " << currentDepth << " | Score: " << bestScore << " | Move: " << PrMove(bestMove) << " | Nodes: " << info->nodes;

        pvMoves = GetPvLine(currentDepth, pos);
        std::cout << " | PV:";
        for (pvNum = 0; pvNum < pvMoves; pvNum++)
        {
            std::cout << " " << PrMove(pos->PvArray[pvNum]);
        }
        std::cout << "\n";
        std::cout << "Ordering: " << ((float)info->fhf / info->fh) << std::endl;
    }
}