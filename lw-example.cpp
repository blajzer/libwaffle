#include "waffle.h"
#include <unistd.h>

using namespace waffle;

int main(int argc, char *argv[]){
	Waffle *w = Waffle::get();
	
	Module *g = new Add(new GenSine(new Value(440.0)),	new GenSquare(new Add(new Mult(new GenSine(new Value(0.5)),new Value(20.0)),new Value(440.0)),new Value(0.5)));
	Value *v = new Value(0.0);
	Module *m = new Envelope(0.5, 0.5, 0.5, 0.5, 0.5, v, g);
	w->addPatch(m);
	w->start();
	sleep(1);
	v->setValue(1);
	sleep(20);
	v->setValue(0);
	sleep(2);
	return 0;
}
