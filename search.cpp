#include "defs.h"

static void CheckUp()
{
}

static void PickNextMove(int moveNum, S_MOVELIST *list)
{
    S_MOVE temp;
    int bestScore = 0;
    int bestNum = moveNum;

    for (int index = moveNum; index < list->count; index++)
    {
        if (list->moves[index].score > bestScore)
        {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
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

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info)
{
    ASSERT(CheckBoard(pos));
    info->nodes++;

    if (IsRepitition(pos) || pos->fiftyMove >= 100)
    {
        return EvalPosition(pos);
    }

    int score = EvalPosition(pos);

    if (score >= beta)
    {
        return beta;
    }
    if (score > alpha)
    {
        alpha = score;
    }

    S_MOVELIST list[1];
    GenerateAllCaps(pos, list);

    int MoveNum = 0;
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    score = -INFINITE;
    int pvMove = ProbePvTable(pos);

    for (MoveNum = 0; MoveNum < list->count; MoveNum++)
    {
        PickNextMove(MoveNum, list);
        if (!MakeMove(pos, list->moves[MoveNum].move))
        {
            continue;
        }

        legal++;
        score = -Quiescence(-beta, -alpha, pos, info);
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

    if (alpha != oldAlpha)
    {
        StorePvMove(pos, bestMove);
    }

    return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull)
{
    ASSERT(CheckBoard(pos));

    if (depth == 0)
    {
        return Quiescence(alpha, beta, pos, info);
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
    int pvMove = ProbePvTable(pos);

    if (pvMove != NOMOVE)
    {
        for (MoveNum = 0; MoveNum < list->count; MoveNum++)
        {
            if (list->moves[MoveNum].move == pvMove)
            {
                list->moves[MoveNum].score = 2000000;
                break;
            }
        }
    }

    for (MoveNum = 0; MoveNum < list->count; MoveNum++)
    {
        PickNextMove(MoveNum, list);
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

                if (!list->moves[MoveNum].move & MOVEFLAGCAP)
                {
                    pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                    pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
                }

                return beta;
            }
            alpha = score;
            bestMove = list->moves[MoveNum].move;
            if (!list->moves[MoveNum].move & MOVEFLAGCAP)
            {
                pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] += depth;
            }
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