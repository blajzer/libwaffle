all: waffle example

OBJS=waffle.o generators.o filters.o

waffle: ${OBJS}
	g++ -O3 -shared -o libwaffle.so ${OBJS} -lpthread -ljack -march=native

example: waffle
	g++ lw-example.cpp -o lw-example -L. -lwaffle -pthread -ljack -lm
	
%.o : %.cpp
	g++ -fPIC -c $< -o $@ -O3 -march=native
	
clean:
	rm -rf *.o *.so lw-example