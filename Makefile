all: main run

main: main.cpp
	g++ -std=c++17 -I include -L lib *.cpp -o main.exe -lmingw32 -lSDL2main -lSDL2

run: main
	./main.exe

.PHONY: all run
