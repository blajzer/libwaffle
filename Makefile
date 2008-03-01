all: waffle

waffle: waffle.h waffle.cpp
	g++ -fPIC -c waffle.cpp
	g++ -shared -o libwaffle.so waffle.o -lpthread -lSDL

example: waffle
	g++ lw-example.cpp -o lw-example -L. -lwaffle -pthread -lSDL