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

#ifndef _WAFFLE_GENERATORS_H_
#define _WAFFLE_GENERATORS_H_

#include "Module.h"

#include <cstdlib>

namespace waffle {

class WaveformGenerator : public Module {
public:
	void setFreq(Module *f);
	void setPhase(Module *p);
	virtual bool isValid(){if(m_freq != NULL && m_phase != NULL) return m_freq->isValid() && m_phase->isValid(); else return false;}

protected:
	WaveformGenerator() : m_freq(NULL), m_phase(NULL), m_pos(0.0){} //should never be explicitly instantiated
	WaveformGenerator(Module *f, Module *p) : m_freq(f), m_phase(p), m_pos(0.0){} //should never be explicitly instantiated

	Module *m_freq;
	Module *m_phase;
	double m_pos;
};

class GenSine : public WaveformGenerator {
public:
	GenSine(Module *f, Module *p);
	
	virtual double run();
};

class GenTriangle : public WaveformGenerator {
public:
	GenTriangle(Module *f, Module *p);
	
	virtual double run();
};

class GenSawtooth : public WaveformGenerator {
public:
	GenSawtooth(Module *f, Module *p);
	
	virtual double run();
};

class GenRevSawtooth : public WaveformGenerator {
public:
	GenRevSawtooth(Module *f, Module *p);
	
	virtual double run();
};

class GenSquare : public WaveformGenerator {
public:
	GenSquare() : WaveformGenerator(), m_thresh(NULL) {}
	GenSquare(Module *f, Module *p, Module *t);
	void setThreshold(Module *t);
	
	virtual double run();
	virtual bool isValid() {
		if(WaveformGenerator::isValid() && m_thresh != NULL)
			return m_thresh->isValid();
		else
			return false;
	}

protected:
	Module *m_thresh;
};

class GenNoise : public Module {
public:	
	virtual double run();
	virtual bool isValid(){ return true; }
};

class Value : public Module {
public:
	Value():m_value(0.0){}
	Value(double v):m_value(v){}
	virtual double run();
	virtual bool isValid(){ return true; }
	void setValue(double v);
	
protected:
	double m_value;
};

}
#endif
