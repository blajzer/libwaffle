// Waffle - filters.h
// Various filters to play with
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

#ifndef _WAFFLE_FILTERS_H_
#define _WAFFLE_FILTERS_H_

#include "Module.h"

#include <list>
#include <vector>

namespace waffle {

class Filter : public Module {
public:
	Filter(){}
	virtual double run() = 0;
	virtual bool isValid();
	virtual Module *getChild(int n);
	virtual void setChild(int n, Module *m);
	
protected:
	std::vector<Module *> m_children;
};

class LowPass : public Filter {
public:
	LowPass():m_freq(NULL){}
	LowPass(Module *f, Module *m);
	virtual double run();
	virtual bool isValid();
	void setFreq(Module *f);
	
private:
	Module *m_freq;
	double m_prev;
};

class HighPass : public Filter {
public:
	HighPass():m_freq(NULL){}
	HighPass(Module *f, Module *m);
	virtual double run();
	virtual bool isValid();
	void setFreq(Module *f);
	
private:
	Module *m_freq;
	double m_prev;
};

class Delay : public Filter {
public:
	Delay():m_trig(NULL){}
	Delay(double len, double thresh, Module *m, Module *t);
	
	virtual double run();
	virtual bool isValid();
	void setLength(double len);
	void setThreshold(double t){m_thresh = t;}
	void setTrigger(Module *t){m_trig = t;}

private:
	double m_thresh;
	int m_length;
	Module *m_trig;
	bool m_first;
	std::list<double> m_queue;
};

class Mult : public Filter {
public:
	Mult(){}
	Mult(Module *m1, Module *m2);
	virtual double run();
};

class Add : public Filter {
public:
	Add(){}
	Add(Module *m1, Module *m2);
	virtual double run();
};

class Sub : public Filter {
public:
	Sub(){}
	Sub(Module *m1, Module *m2);
	virtual double run();
};

class Abs : public Filter {
public:
	Abs(){}
	Abs(Module *m);
	virtual double run();
};

class Envelope : public Filter {
public:
	Envelope():m_trig(NULL){}
	Envelope(double thresh, double a, double d, double s, double r, Module *t, Module *i);
	void setThresh(double t);
	void setAttack(double a);
	void setDecay(double d);
	void setSustain(double s);
	void setRelease(double r);
	void retrigger();
	virtual double run();
	virtual bool isValid(){if(Filter::isValid() && m_trig != NULL) return m_trig->isValid(); else return false;}

private:

	enum EnvelopeState
	{
		OFF,
		ATTACK,
		DECAY,
		SUSTAIN,
		RELEASE
	};

	Module *m_trig;
	EnvelopeState m_state;
	double m_thresh;
	double m_attack;
	double m_decay;
	double m_sustain;
	double m_release;
	double m_volume;
	int m_a_t; int m_a_c;
	int m_d_t; int m_d_c;
	int m_r_t; int m_r_c;
};

}

#endif
