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
