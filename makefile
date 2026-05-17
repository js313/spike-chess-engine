all:
	clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -g ./main.cpp ./init.cpp ./bitboards.cpp ./hashkeys.cpp ./board.cpp ./data.cpp ./attack.cpp -o spike && ./spike