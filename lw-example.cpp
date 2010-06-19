/*
Copyright (c) 2009-2010 Brett Lajzer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "waffle.h"
#include <iostream>
#include <unistd.h>

using namespace waffle;

int main(int argc, char *argv[]){
	Waffle *w = new Waffle();
	
	Module *g = new Add(new GenSine(new Value(440.0), new Value(0.0)),
						new GenSquare(new Add(new Mult(new GenSine(new Value(0.5), new Value(0.0)),
												new Value(20.0)),new Value(440.0)),
										new Value(0.0),
										new Value(0.5)));
	Value *v = new Value(0.0);
	Module *m = new Envelope(0.5, 0.5, 0.5, 0.5, 0.5, v, g);

	if(!m->isValid()) {
		std::cout << "looks bad ;(" << std::endl;
		return 1;
	}else{
		std::cout << "looks good!" << std::endl;
	}

	w->addPatch("testPatch", m);
	w->start("testPatch");
	sleep(1);
	v->setValue(1);
	sleep(20);
	v->setValue(0);
	sleep(2);
	delete w;
	return 0;
}

