//Waffle,
// a crappy 8-Bit modular Synth

#ifndef _WAFFLE_H_
#define _WAFFLE_H_

#include <list>
#include <vector>
#include <jack/jack.h>
#include <jack/types.h>

#define Uint8 char

namespace waffle {

//base module class
class Module {
public:
	Module(){};
	~Module(){};
	
	virtual float run()=0;
	virtual bool isValid()=0;
protected:

};

//the actual synth
class Waffle {
public:
	static Waffle *get();
	static float midiToFreq(int note);
	int addPatch(Module *m);
	void setPatch(int n, Module *m);
	void setNormMethod(int n);
	void start();
	void stop();
	
	void run(jack_nframes_t nframes);
	
	//jack callbacks
	static int samplerate_callback(jack_nframes_t nframes, void *arg);
	static int buffersize_callback(jack_nframes_t nframes, void *arg);
	static int process_callback(jack_nframes_t nframes, void *arg);
	
	~Waffle();
	
	static const int NORM_CLIP=0;
	static const int NORM_ABSOLUTE=1;
	static const int NORM_RELATIVE=2;
	
	static float sampleRate;
	static int bufferSize;
	
private:
	Waffle();
	
	std::vector<Module *> m_channels;
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
	virtual float run() = 0;
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
	virtual float run();
	virtual bool isValid();
	void setFreq(Module *f);
	
private:
	Module *m_freq;
	float m_prev;
};

class HighPass : public Filter {
public:
	HighPass(){};
	HighPass(Module *f, Module *m);
	virtual float run();
	virtual bool isValid();
	void setFreq(Module *f);
	
private:
	Module *m_freq;
	float m_prev;
};

class Delay : public Filter {
public:
	Delay(){};
	Delay(float len, float thresh, Module *m, Module *t);
	
	virtual float run();
	virtual bool isValid();
	void setLength(float len);
	void setThreshold(float t){m_thresh = t;}
	void setTrigger(Module *t){m_trig = t;}

private:
	float m_thresh;
	int m_length;
	Module *m_trig;
	bool m_first;
	std::list<float> m_queue;
};

class Mult : public Filter {
public:
	Mult(){};
	Mult(Module *m1, Module *m2);
	virtual float run();
	virtual bool isValid();
};

class Add : public Filter {
public:
	Add(){};
	Add(Module *m1, Module *m2);
	virtual float run();
	virtual bool isValid();
};

class Sub : public Filter {
public:
	Sub(){};
	Sub(Module *m1, Module *m2);
	virtual float run();
	virtual bool isValid();
};

class Abs : public Filter {
public:
	Abs(){};
	Abs(Module *m);
	virtual float run();
	virtual bool isValid(){if(m_children[0] != NULL) return m_children[0]->isValid(); else return false;}
};

class Envelope : public Filter {
public:
	Envelope(){};
	Envelope(float thresh, float a, float d, float s, float r, Module *t, Module *i);
	void setThresh(float t){m_thresh = t;};
	void setAttack(float a){m_a_t = (int)(a * Waffle::sampleRate);};
	void setDecay(float d){m_d_t = (int)(d * Waffle::sampleRate);};
	void setSustain(float s){m_sustain = s;};
	void setRelease(float r){m_r_t = (int)(r * Waffle::sampleRate);};
	virtual float run();
	virtual bool isValid(){if(m_trig != NULL) return m_trig->isValid(); else return false;}

private:
	Module *m_trig;
	int m_state;
	float m_thresh;
	float m_attack;
	float m_decay;
	float m_sustain;
	float m_release;
	float m_volume;
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
	
	virtual float run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	float m_pos;
};

class GenTriangle : public Module {
public:
	GenTriangle(){};
	GenTriangle(Module *f);
	void setFreq(Module *f);
	
	virtual float run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	float m_pos;
};

class GenSawtooth : public Module {
public:
	GenSawtooth(){};
	GenSawtooth(Module *f);
	void setFreq(Module *f);
	
	virtual float run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	float m_pos;
};

class GenRevSawtooth : public Module {
public:
	GenRevSawtooth(){};
	GenRevSawtooth(Module *f);
	void setFreq(Module *f);
	
	virtual float run();
	virtual bool isValid(){if(m_freq != NULL) return m_freq->isValid(); else return false;}
private:
	Module *m_freq;
	float m_pos;
};

class GenSquare : public Module {
public:
	GenSquare(){};
	GenSquare(Module *f, Module *t);
	void setFreq(Module *f);
	void setThreshold(Module *t);
	
	virtual float run();
	virtual bool isValid(){if(m_freq != NULL && m_thresh != NULL) return m_freq->isValid() && m_thresh->isValid(); else return false;}
private:
	Module *m_freq;
	float m_pos;
	Module *m_thresh;
};

class GenNoise : public Module {
public:
	GenNoise(){};
	
	virtual float run();
	virtual bool isValid(){ return true; }
};

class Value : public Module {
public:
	Value(){};
	Value(float v):m_value(v){};
	virtual float run();
	virtual bool isValid(){ return true; }
	void setValue(float v);
	
private:
	float m_value;
};

}

#endif
