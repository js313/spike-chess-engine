all:
	clang++ -std=c++20 -Wall -Wextra -Wpedantic -g ./main.cpp ./init.cpp ./bitboards.cpp ./hashkeys.cpp ./board.cpp ./data.cpp ./attack.cpp ./io.cpp ./movegen.cpp ./validate.cpp -o spike && ./spike