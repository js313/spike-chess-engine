#include <stdio.h>
#include <iostream>
#include "defs.h"

int ParseFen(char *fen, S_BOARD *pos)
{
    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    ResetBoard(pos);

    int rank = RANK_8;
    int file = FILE_A;
    int count = 0;
    int piece = 0;

    while (rank >= RANK_1 && *fen)
    {
        count = 1;
        switch (*fen)
        {
        case 'p':
            piece = bP;
            break;
        case 'r':
            piece = bR;
            break;
        case 'n':
            piece = bN;
            break;
        case 'b':
            piece = bB;
            break;
        case 'k':
            piece = bK;
            break;
        case 'q':
            piece = bQ;
            break;
        case 'P':
            piece = wP;
            break;
        case 'R':
            piece = wR;
            break;
        case 'N':
            piece = wN;
            break;
        case 'B':
            piece = wB;
            break;
        case 'K':
            piece = wK;
            break;
        case 'Q':
            piece = wQ;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            piece = EMPTY;
            count = *fen - '0';
            break;
        case '/':
        case ' ':
            rank--;
            file = FILE_A;
            fen++;
            continue;
        default:
            std::cout << "FEN Error\n";
            return -1;
        }

        for (int i = 0; i < count; i++)
        {
            int sq64 = rank * 8 + file;
            int sq120 = SQ120(sq64);
            if (piece != EMPTY)
            {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    for (int i = 0; i < 4; i++)
    {
        if (*fen == ' ')
            break;
        switch (*fen)
        {
        case 'K':
            pos->castlePerm |= WKCA;
            break;
        case 'Q':
            pos->castlePerm |= WQCA;
            break;
        case 'k':
            pos->castlePerm |= BKCA;
            break;
        case 'q':
            pos->castlePerm |= BQCA;
            break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    if (*fen != '-')
    {
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file > FILE_A && file < FILE_H);
        ASSERT(rank > RANK_1 && rank < RANK_8);

        pos->enPas = FR2SQ(file, rank);
    }

    pos->posKey = GeneratePosKey(pos);

    return 0;
}

void ResetBoard(S_BOARD *pos)
{
    for (int i = 0; i < BRD_SQ_NUM; i++)
    {
        pos->pieces[i] = OFFBOARD;
    }

    for (int i = 0; i < 64; i++)
    {
        pos->pieces[SQ120(i)] = EMPTY;
    }

    for (int i = 0; i < 3; i++)
    {
        pos->bigPce[i] = 0;
        pos->majPce[i] = 0;
        pos->minPce[i] = 0;
        pos->pawns[i] = 0ULL;
    }

    for (int i = 0; i < 13; i++)
    {
        pos->pceNum[i] = 0;
    }

    pos->kingSq[WHITE] = pos->kingSq[BLACK] = NO_SQ;

    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0;
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
}

void PrintBoard(const S_BOARD *pos)
{
    int sq, file, rank, piece;
    std::cout << "\n\nGame Board: \n\n";

    for (rank = RANK_8; rank >= RANK_1; rank--)
    {
        std::cout << rank + 1 << "  ";
        for (file = FILE_A; file <= FILE_H; file++)
        {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
            printf("%3c", PceChar[piece]);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl
              << "   ";
    for (file = FILE_A; file <= FILE_H; file++)
        printf("%3c", 'a' + file);
    std::cout << std::endl;
    std::cout << "Side: " << SideChar[pos->side] << std::endl;
    std::cout << "EnPas: " << pos->enPas << std::endl;
    std::cout << "Castle: "
              << ((pos->castlePerm & WKCA) ? 'K' : '-')
              << ((pos->castlePerm & WQCA) ? 'Q' : '-')
              << ((pos->castlePerm & BKCA) ? 'k' : '-')
              << ((pos->castlePerm & BQCA) ? 'q' : '-')
              << std::endl;
    std::cout << "PosKey: " << pos->posKey;
}