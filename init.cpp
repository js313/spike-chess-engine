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
}