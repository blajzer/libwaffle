// Waffle - filters.h
// Various filters to play with

#ifndef _WAFFLE_FILTERS_H_
#define _WAFFLE_FILTERS_H_

#include "Module.h"

#include <list>
#include <vector>

namespace waffle {

class Filter : public Module {
public:
	Filter(){};
	virtual double run() = 0;
	virtual bool isValid() = 0;
	virtual Module *getChild(int n);
	virtual void setChild(int n, Module *m);
	
protected:
	std::vector<Module *> m_children;
};

class LowPass : public Filter {
public:
	LowPass(){};
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
	HighPass(){};
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
	Delay(){};
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
	Mult(){};
	Mult(Module *m1, Module *m2);
	virtual double run();
	virtual bool isValid();
};

class Add : public Filter {
public:
	Add(){};
	Add(Module *m1, Module *m2);
	virtual double run();
	virtual bool isValid();
};

class Sub : public Filter {
public:
	Sub(){};
	Sub(Module *m1, Module *m2);
	virtual double run();
	virtual bool isValid();
};

class Abs : public Filter {
public:
	Abs(){};
	Abs(Module *m);
	virtual double run();
	virtual bool isValid(){if(m_children[0] != NULL) return m_children[0]->isValid(); else return false;}
};

class Envelope : public Filter {
public:
	Envelope(){};
	Envelope(double thresh, double a, double d, double s, double r, Module *t, Module *i);
	void setThresh(double t);
	void setAttack(double a);
	void setDecay(double d);
	void setSustain(double s);
	void setRelease(double r);
	void retrigger();
	virtual double run();
	virtual bool isValid(){if(m_trig != NULL) return m_trig->isValid(); else return false;}

private:
	Module *m_trig;
	int m_state;
	double m_thresh;
	double m_attack;
	double m_decay;
	double m_sustain;
	double m_release;
	double m_volume;
	int m_a_t; int m_a_c;
	int m_d_t; int m_d_c;
	int m_r_t; int m_r_c;
	static const int OFF = 0;
	static const int ATTACK = 1;
	static const int DECAY = 2;
	static const int SUSTAIN = 3;
	static const int RELEASE = 4;
};

}

#endif
