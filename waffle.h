//Waffle,
// a crappy modular Synth

#ifndef _WAFFLE_H_
#define _WAFFLE_H_

#include <list>
#include <vector>
#include <jack/jack.h>
#include <jack/types.h>

namespace waffle {

//base module class
class Module {
public:
	Module(){};
	~Module(){};
	
	virtual double run()=0;
	virtual bool isValid()=0;
protected:

};

//the actual synth
class Waffle {
public:
	enum NormalizationMethod {
		NORM_CLIP=0,
		NORM_ABSOLUTE,
		NORM_RELATIVE,
		NUM_NORMALIZATION_METHODS
	};
	
	static Waffle *get();
	static double midiToFreq(int note);
	
	//patch management
	int addPatch(Module *m);
	void setPatch(int n, Module *m);
	bool removePatch(Module *m);
	bool removePatch(int n);
	
	void setNormMethod(NormalizationMethod n);
	
	void start();
	void stop();
	
	void run(jack_nframes_t nframes);
	
	//jack callbacks
	static int samplerate_callback(jack_nframes_t nframes, void *arg);
	static int buffersize_callback(jack_nframes_t nframes, void *arg);
	static int process_callback(jack_nframes_t nframes, void *arg);
	
	~Waffle();
	
	static float sampleRate;
	static int bufferSize;
	
private:
	Waffle();
	
	std::list<Module *> m_patches;
	static Waffle *m_singleton;
	int m_norm;
	
	jack_client_t *m_jackClient;
	jack_port_t *m_jackPort;
	bool m_silent;
};


//lots of fun modules to play with
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
	void setThresh(double t){m_thresh = t;};
	void setAttack(double a){m_a_t = (int)(a * Waffle::sampleRate);};
	void setDecay(double d){m_d_t = (int)(d * Waffle::sampleRate);};
	void setSustain(double s){m_sustain = s;};
	void setRelease(double r){m_r_t = (int)(r * Waffle::sampleRate);};
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
