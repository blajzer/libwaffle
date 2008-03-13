all: waffle

waffle: waffle.h waffle.cpp
	g++ -fPIC -c waffle.cpp -O3
	g++ -O3 -shared -o libwaffle.so waffle.o -lpthread -ljack

example: waffle
	g++ lw-example.cpp -o lw-example -L. -lwaffle -pthread -ljack