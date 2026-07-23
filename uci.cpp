#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include "defs.h"

#define INPUTBUFFER 400 * 6

void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos)
{
    int depth = -1, movestogo = 30, movetime = -1, time = -1, inc = 0;
    char *ptr = NULL;
    info->timeSet = false;

    if ((ptr = strstr(line, "infinite")))
    {
    }
    if ((ptr = strstr(line, "binc")) && pos->side == BLACK)
    {
        inc = atoi(ptr + 5);
    }
    if ((ptr = strstr(line, "winc")) && pos->side == WHITE)
    {
        inc = atoi(ptr + 5);
    }
    if ((ptr = strstr(line, "wtime")) && pos->side == WHITE)
    {
        time = atoi(ptr + 6);
    }
    if ((ptr = strstr(line, "btime")) && pos->side == BLACK)
    {
        time = atoi(ptr + 6);
    }
    if ((ptr = strstr(line, "movestogo")))
    {
        movestogo = atoi(ptr + 10);
    }
    if ((ptr = strstr(line, "movetime")))
    {
        movetime = atoi(ptr + 9);
    }
    if ((ptr = strstr(line, "depth")))
    {
        depth = atoi(ptr + 6);
    }
    if (movetime != -1)
    {
        time = movetime;
        movestogo = 1;
    }

    info->startTime = GetTimeMs();
    info->depth = depth;

    if (time != -1)
    {
        info->timeSet = true;
        time /= movestogo;
        time -= 50;
        info->stopTime = info->startTime + time + inc;
    }

    if (depth == -1)
    {
        info->depth = MAX_DEPTH;
    }

    std::cout << "time: " << time << " start: " << info->startTime << " stop: " << info->stopTime << " depth: " << info->depth << " timeSet: " << info->timeSet << std::endl;
    SearchPosition(pos, info);
}

void ParsePosition(const char *lineIn, S_BOARD *pos)
{
    lineIn += 9; // skip the word "position"
    const char *ptrChar = lineIn;

    if (strncmp(lineIn, "startpos", 8) == 0)
    {
        ParseFen(START_FEN, pos);
    }
    else
    {
        ptrChar = strstr(lineIn, "fen");
        if (ptrChar == NULL)
        {
            ParseFen(START_FEN, pos);
        }
        else
        {
            ptrChar += 4;
            ParseFen(ptrChar, pos);
        }
    }

    ptrChar = strstr(lineIn, "moves");
    int move;

    if (pos->PvTable->pTable != NULL)
    {
        ClearPvTable(pos->PvTable);
    }

    if (ptrChar != NULL)
    {
        ptrChar += 6;
        while (*ptrChar)
        {
            move = ParseMove((char *)ptrChar, pos);
            std::cout << PrMove(move) << std::endl;
            if (move == NOMOVE)
                break;
            MakeMove(pos, move);
            pos->ply = 0;
            while (*ptrChar && *ptrChar != ' ')
                ptrChar++;
            ptrChar++;
        }
    }
    PrintBoard(pos);
}

void UciLoop()
{
    char line[INPUTBUFFER];
    std::cout << "id name" << NAME << std::endl;
    std::cout << "id author Jeenit\n";
    std::cout << "uciok\n";

    S_BOARD pos[1] = {};
    S_SEARCHINFO info[1] = {};
    InitPvTable(pos->PvTable);

    while (true)
    {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if (!fgets(line, INPUTBUFFER, stdin))
            continue;

        if (line[0] == '\n')
            continue;

        if (!strncmp(line, "isready", 7))
        {
            std::cout << "readyok\n";
            continue;
        }
        else if (!strncmp(line, "position", 8))
        {
            ParsePosition(line, pos);
        }
        else if (!strncmp(line, "ucinewgame", 10))
        {
            ParsePosition("position startpos\n", pos);
        }
        else if (!strncmp(line, "go", 2))
        {
            ParseGo(line, info, pos);
        }
        else if (!strncmp(line, "quit", 4))
        {
            info->quit = true;
            break;
        }
        else if (!strncmp(line, "uci", 3))
        {
            std::cout << "id name " << NAME << std::endl;
            std::cout << "id author Jeenit\n";
            std::cout << "uciok\n";
        }

        if (info->quit)
            break;
    }

    if (pos->PvTable->pTable != NULL)
    {
        free(pos->PvTable->pTable);
        pos->PvTable->pTable = NULL;
        pos->PvTable->numEntries = 0;
    }
}