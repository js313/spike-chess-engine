#include "defs.h"
#include <stdlib.h>

#define RAND_64 ((U64)rand() |         \
                 ((U64)rand() << 15) | \
                 ((U64)rand() << 30) | \
                 ((U64)rand() << 45) | \
                 (((U64)rand() & 0xf) << 60))

int Sq120ToSq64[BRD_SQ_NUM];
int Sq64ToSq120[64];

U64 SetMask[64];
U64 ClearMask[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

U64 FileBBMask[8];
U64 RankBBMask[8];

U64 BlackPassedMask[64];
U64 WhitePassedMask[64];
U64 IsolatedMask[64];

void InitEvalMasks()
{
    int sq, tsq, r, f;

    for (sq = 0; sq < 8; sq++)
    {
        FileBBMask[sq] = 0ULL;
        RankBBMask[sq] = 0ULL;
    }

    for (r = RANK_8; r >= RANK_1; r--)
    {
        for (f = FILE_A; f <= FILE_H; f++)
        {
            sq = r * 8 + f;
            FileBBMask[f] |= (1ULL << sq);
            RankBBMask[r] |= (1ULL << sq);
        }
    }

    for (sq = 0; sq < 64; sq++)
    {
        BlackPassedMask[64] = 0ULL;
        WhitePassedMask[64] = 0ULL;
        IsolatedMask[64] = 0ULL;
    }

    for (sq = 0; sq < 64; sq++)
    {
        tsq = sq + 8;
        while (tsq < 64)
        {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while (tsq >= 0)
        {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if (FilesBrd[SQ120(sq)] > FILE_A)
        {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while (tsq < 64)
            {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while (tsq >= 0)
            {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        if (FilesBrd[SQ120(sq)] < FILE_H)
        {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while (tsq < 64)
            {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while (tsq >= 0)
            {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }
    }
}

void InitFilesRanksBrd()
{
    for (int i = 0; i < BRD_SQ_NUM; i++)
    {
        FilesBrd[i] = OFFBOARD;
        RanksBrd[i] = OFFBOARD;
    }
    for (int rank = RANK_1; rank <= RANK_8; rank++)
    {
        for (int file = FILE_A; file <= FILE_H; file++)
        {
            int sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

void InitHashKeys()
{
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 120; j++)
        {
            PieceKeys[i][j] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for (int i = 0; i < 16; i++)
    {
        CastleKeys[i] = RAND_64;
    }
}

void InitBitMasks()
{

    for (int index = 0; index < 64; index++)
    {
        SetMask[index] = (1ULL << index);
        ClearMask[index] = ~SetMask[index];
    }
}

void InitSq120To64()
{
    int sq = A1;
    int sq64 = 0;

    for (int index = 0; index < BRD_SQ_NUM; index++)
    {
        Sq120ToSq64[index] = 65;
    }

    for (int index = 0; index < 64; index++)
    {
        Sq64ToSq120[index] = 120;
    }

    for (int r = RANK_1; r <= RANK_8; r++)
    {
        for (int f = FILE_A; f <= FILE_H; f++)
        {
            sq = FR2SQ(f, r);
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}

void AllInit()
{
    InitSq120To64();
    InitBitMasks();
    InitHashKeys();
    InitFilesRanksBrd();
    InitEvalMasks();
    InitMvvLva();
}