CXXFLAGS=-O3 -march=native
LDFLAGS=-pthread -ljack -lm

all: waffle example

OBJS=waffle.o generators.o filters.o

waffle: ${OBJS}
	g++ -shared -o libwaffle.so ${OBJS} ${CXXFLAGS} ${LDFLAGS}

example: waffle
	g++ lw-example.cpp -o lw-example -L. -lwaffle ${CXXFLAGS} ${LDFLAGS}
	
%.o : %.cpp
	g++ -fPIC -c $< -o $@ ${CXXFLAGS}
	
clean:
	rm -rf *.o *.so lw-example
