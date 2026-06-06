#include <iostream>
#include <string>
#include <vector>
#include "defs.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: perft_cli \"<fen>\" <depth>\n";
        return 1;
    }

    const std::string fen = argv[1];
    const int depth = std::stoi(argv[2]);

    if (depth < 0)
    {
        std::cerr << "Depth must be >= 0\n";
        return 1;
    }

    AllInit();

    S_BOARD board[1];
    std::vector<char> fenBuffer(fen.begin(), fen.end());
    fenBuffer.push_back('\0');

    if (ParseFen(fenBuffer.data(), board) != 0)
    {
        std::cerr << "Invalid FEN\n";
        return 2;
    }

    const long nodes = PerftCount(depth, board);
    std::cout << nodes << "\n";
    return 0;
}
