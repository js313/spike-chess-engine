#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "defs.h"

int ThreeFoldRep(const S_BOARD *pos)
{
    int i = 0, r = 0;
    for (i = 0; i < pos->hisPly; i++)
    {
        if (pos->history[i].posKey == pos->posKey)
            r++;
    }
    return r;
}

bool DrawMaterial(const S_BOARD *pos)
{
    if (pos->pceNum[wP] || pos->pceNum[bP])
        return false;
    if (pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR])
        return false;
    if (pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1)
        return false;
    if (pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1)
        return false;
    if (pos->pceNum[wN] && pos->pceNum[wB])
        return false;
    if (pos->pceNum[bN] && pos->pceNum[bB])
        return false;

    return true;
}

bool CheckResult(S_BOARD *pos)
{
    if (pos->fiftyMove > 100)
    {
        std::cout << "1/2-1/2 {fifty move rule (claimed by spike)}\n";
        return true;
    }
    if (ThreeFoldRep(pos) >= 2)
    {
        std::cout << "1/2-1/2 {3-fold repetition (claimed by spike)}\n";
        return true;
    }
    if (DrawMaterial(pos) == true)
    {
        std::cout << "1/2-1/2 {insufficient material (claimed by spike)}\n";
        return true;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int moveNum = 0;
    int found = 0;
    for (moveNum = 0; moveNum < list->count; moveNum++)
    {
        if (!MakeMove(pos, list->moves[moveNum].move))
            continue;
        found++;
        TakeMove(pos);
        break;
    }

    if (found)
        return false;

    int inCheck = SqAttacked(pos->kingSq[pos->side], pos->side ^ 1, pos);

    if (inCheck)
    {
        if (pos->side == WHITE)
        {
            std::cout << "0-1 {black mates (claimed by spike)}\n";
            return true;
        }
        else
        {
            std::cout << "1-0 {white mates (claimed by spike)}\n";
            return true;
        }
    }
    else
    {
        std::cout << "1/2-1/2 {stalemate (claimed by spike)}\n";
        return true;
    }

    return false;
}

void xBoardLoop(S_BOARD *pos, S_SEARCHINFO *info)
{
    int depth = -1, movestogo[2] = {30, 30}, movetime = -1;
    int time = -1, inc = 0, engineSide = BOTH;
    int mps = 0, move = NOMOVE;
    char inBuf[80], command[80];

    info->GAME_MODE = XBOARDMODE;
    info->POST_THINKING = false;

    while (true)
    {
        fflush(stdout);
        if (pos->side == engineSide && !CheckResult(pos))
        {
            info->startTime = GetTimeMs();
            info->depth = depth;

            if (time != -1)
            {
                info->timeSet = true;
                time /= movestogo[pos->side];
                time -= 50;
                info->stopTime = info->startTime + time + inc;
            }

            if (depth == -1 || depth > MAX_DEPTH)
            {
                info->depth = MAX_DEPTH;
            }

            std::cout << "time:" << time << " start:" << info->startTime << " stop:" << info->stopTime << " depth:" << info->depth << " timeset:" << info->timeSet << " movestogo:" << movestogo[pos->side] << " mps:" << mps << "\n";

            SearchPosition(pos, info);

            if (mps != 0)
            {
                movestogo[pos->side ^ 1]--;
                if (movestogo[pos->side ^ 1] < 1)
                {
                    movestogo[pos->side ^ 1] = mps;
                }
            }
        }
        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        if (!strcmp(command, "quit"))
        {
            info->quit = true;
            break;
        }

        if (!strcmp(command, "xboard"))
        {
            // Entering xboard mode; nothing special needed
            continue;
        }

        if (!strcmp(command, "force"))
        {
            engineSide = BOTH;
            continue;
        }

        if (!strcmp(command, "protover"))
        {
            std::cout << "feature ping=1 setboard=1 colors=0 usermove=1 myname=\"" << NAME << "\"\n";
            std::cout << "feature done=1\n";
            continue;
        }

        if (!strcmp(command, "accepted"))
        {
            continue;
        }

        if (!strcmp(command, "rejected"))
        {
            continue;
        }

        if (!strcmp(command, "sd"))
        {
            sscanf(inBuf, "sd %d", &depth);
            continue;
        }

        if (!strcmp(command, "st"))
        {
            sscanf(inBuf, "st %d", &movetime);
            time = movetime;
            continue;
        }

        // level <mps> <base> <inc>
        // base is in minutes (or mm:ss), inc is in seconds
        if (!strcmp(command, "level"))
        {
            int baseMin = 0, baseSec = 0, levelInc = 0;
            mps = 0;
            // Try mm:ss format for base time first
            if (sscanf(inBuf, "level %d %d:%d %d", &mps, &baseMin, &baseSec, &levelInc) != 4)
            {
                sscanf(inBuf, "level %d %d %d", &mps, &baseMin, &levelInc);
                baseSec = 0;
            }
            time = (baseMin * 60 + baseSec) * 1000;
            inc = levelInc * 1000;
            if (mps != 0)
            {
                movestogo[WHITE] = mps;
                movestogo[BLACK] = mps;
            }
            continue;
        }

        // Engine clock (centiseconds)
        if (!strcmp(command, "time"))
        {
            int engineTime = 0;
            sscanf(inBuf, "time %d", &engineTime);
            time = engineTime * 10; // convert centiseconds to milliseconds
            continue;
        }

        // Opponent clock (centiseconds) - we don't use it but must accept it
        if (!strcmp(command, "otim"))
        {
            continue;
        }

        if (!strcmp(command, "ping"))
        {
            std::cout << "pong" << inBuf + 4;
            fflush(stdout);
            continue;
        }

        if (!strcmp(command, "new"))
        {
            engineSide = BLACK;
            ParseFen(START_FEN, pos);
            depth = -1;
            time = -1;
            continue;
        }

        if (!strcmp(command, "setboard"))
        {
            engineSide = BOTH;
            ParseFen(inBuf + 9, pos);
            continue;
        }

        if (!strcmp(command, "go"))
        {
            engineSide = pos->side;
            continue;
        }

        if (!strcmp(command, "white"))
        {
            pos->side = WHITE;
            engineSide = BLACK;
            continue;
        }

        if (!strcmp(command, "black"))
        {
            pos->side = BLACK;
            engineSide = WHITE;
            continue;
        }

        if (!strcmp(command, "undo"))
        {
            if (pos->hisPly > 0)
                TakeMove(pos);
            continue;
        }

        if (!strcmp(command, "remove"))
        {
            if (pos->hisPly >= 2)
            {
                TakeMove(pos);
                TakeMove(pos);
            }
            continue;
        }

        if (!strcmp(command, "result"))
        {
            // Game over; stop engine side
            engineSide = BOTH;
            continue;
        }

        if (!strcmp(command, "post"))
        {
            info->POST_THINKING = true;
            continue;
        }

        if (!strcmp(command, "nopost"))
        {
            info->POST_THINKING = false;
            continue;
        }

        if (!strcmp(command, "hard"))
        {
            // Pondering on - not implemented, just acknowledge
            continue;
        }

        if (!strcmp(command, "easy"))
        {
            // Pondering off - not implemented, just acknowledge
            continue;
        }

        if (!strcmp(command, "random"))
        {
            // Random mode - ignore
            continue;
        }

        if (!strcmp(command, "computer"))
        {
            // Opponent is a computer - ignore
            continue;
        }

        if (!strcmp(command, "draw"))
        {
            // Decline draw offer
            std::cout << "tellusererror I decline the draw offer.\n";
            continue;
        }

        if (!strcmp(command, "usermove"))
        {
            movestogo[pos->side]--;
            move = ParseMove(inBuf + 9, pos);
            if (move == NOMOVE)
            {
                std::cout << "Illegal move: " << inBuf + 9;
                continue;
            }
            MakeMove(pos, move);
            pos->ply = 0;
        }
    }
}

void ConsoleLoop(S_BOARD *pos, S_SEARCHINFO *info)
{
    char inBuf[80], command[80];
    int move = NOMOVE;
    int engineSide = BLACK;

    info->GAME_MODE = CONSOLEMODE;
    info->POST_THINKING = true;

    PrintBoard(pos);
    std::cout << "\n Type 'help' for a list of commands.\n\n";

    while (true)
    {
        fflush(stdout);
        std::cout << "\n Spike > ";
        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        if (!strcmp(command, "help"))
        {
            std::cout << "Commands:\n";
            std::cout << "  quit            - Exit the program\n";
            std::cout << "  new             - Start a new game\n";
            std::cout << "  force           - Stop engine from playing\n";
            std::cout << "  go              - Engine plays the current side\n";
            std::cout << "  sd <depth>      - Set search depth\n";
            std::cout << "  st <time>       - Set move time in milliseconds\n";
            std::cout << "  setboard <fen>  - Set position from FEN\n";
            std::cout << "  undo            - Take back last move\n";
            std::cout << "  perft <depth>   - Run perft test\n";
            std::cout << "  eval            - Evaluate current position\n";
            std::cout << "  post            - Show search thinking\n";
            std::cout << "  nopost          - Hide search thinking\n";
            std::cout << "  board           - Print the current board\n";
            std::cout << "  <move>          - Enter a move (e.g. e2e4, e7e8q)\n";
            continue;
        }

        if (!strcmp(command, "mirror"))
        {
            PrintBoard(pos);
            std::cout << "Eval:" << EvalPosition(pos) << std::endl;
            MirrorBoard(pos);
            PrintBoard(pos);
            std::cout << "Eval:" << EvalPosition(pos) << std::endl;
            MirrorBoard(pos);
            continue;
        }

        if (!strcmp(command, "quit"))
        {
            info->quit = true;
            break;
        }

        if (!strcmp(command, "new"))
        {
            engineSide = BLACK;
            ParseFen(START_FEN, pos);
            PrintBoard(pos);
            continue;
        }

        if (!strcmp(command, "force"))
        {
            engineSide = BOTH;
            continue;
        }

        if (!strcmp(command, "go"))
        {
            engineSide = pos->side;
            info->depth = MAX_DEPTH;
            info->timeSet = false;
            SearchPosition(pos, info);
            PrintBoard(pos);
            continue;
        }

        if (!strcmp(command, "sd"))
        {
            int d = MAX_DEPTH;
            sscanf(inBuf, "sd %d", &d);
            info->depth = d;
            std::cout << "Search depth set to " << d << "\n";
            continue;
        }

        if (!strcmp(command, "st"))
        {
            int ms = 1000;
            sscanf(inBuf, "st %d", &ms);
            info->startTime = GetTimeMs();
            info->stopTime = info->startTime + ms;
            info->timeSet = true;
            std::cout << "Move time set to " << ms << "ms\n";
            continue;
        }

        if (!strcmp(command, "setboard"))
        {
            ParseFen(inBuf + 9, pos);
            PrintBoard(pos);
            continue;
        }

        if (!strcmp(command, "undo"))
        {
            if (pos->hisPly > 0)
            {
                TakeMove(pos);
                PrintBoard(pos);
            }
            else
            {
                std::cout << "No moves to undo.\n";
            }
            continue;
        }

        if (!strcmp(command, "perft"))
        {
            int d = 1;
            sscanf(inBuf, "perft %d", &d);
            PerftTest(d, pos);
            continue;
        }

        if (!strcmp(command, "eval"))
        {
            std::cout << "Evaluation: " << EvalPosition(pos) << "\n";
            continue;
        }

        if (!strcmp(command, "post"))
        {
            info->POST_THINKING = true;
            continue;
        }

        if (!strcmp(command, "nopost"))
        {
            info->POST_THINKING = false;
            continue;
        }

        if (!strcmp(command, "board"))
        {
            PrintBoard(pos);
            continue;
        }

        // Try to parse as a move
        move = ParseMove(command, pos);
        if (move == NOMOVE)
        {
            std::cout << "Unknown command or illegal move: " << command << "\n";
            continue;
        }
        MakeMove(pos, move);
        pos->ply = 0;
        PrintBoard(pos);

        // After user move, let engine respond if it should play
        if (engineSide == pos->side)
        {
            info->depth = MAX_DEPTH;
            info->timeSet = false;
            SearchPosition(pos, info);
            PrintBoard(pos);
        }
    }
}