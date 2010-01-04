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

class GenSine : public Module {
public:
	GenSine(){};
	GenSine(Module *f);
	void setFreq(Module *f);
	
	virtual double run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	double m_pos;
};

class GenTriangle : public Module {
public:
	GenTriangle(){};
	GenTriangle(Module *f);
	void setFreq(Module *f);
	
	virtual double run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	double m_pos;
};

class GenSawtooth : public Module {
public:
	GenSawtooth(){};
	GenSawtooth(Module *f);
	void setFreq(Module *f);
	
	virtual double run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	double m_pos;
};

class GenRevSawtooth : public Module {
public:
	GenRevSawtooth(){};
	GenRevSawtooth(Module *f);
	void setFreq(Module *f);
	
	virtual double run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	double m_pos;
};

class GenSquare : public Module {
public:
	GenSquare(){};
	GenSquare(Module *f, Module *t);
	void setFreq(Module *f);
	void setThreshold(Module *t);
	
	virtual double run();
	virtual bool isValid(){if(m_freq != NULL && m_thresh != NULL) return m_freq->isValid() && m_thresh->isValid(); else return false;}
private:
	Module *m_freq;
	double m_pos;
	Module *m_thresh;
};

class GenNoise : public Module {
public:
	GenNoise(){};
	
	virtual double run();
	virtual bool isValid(){ return true; }
};

class Value : public Module {
public:
	Value(){};
	Value(double v):m_value(v){};
	virtual double run();
	virtual bool isValid(){ return true; }
	void setValue(double v);
	
private:
	double m_value;
};

}
#endif
