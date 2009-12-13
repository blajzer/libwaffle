//Waffle,
// a crappy modular Synth

#ifndef _WAFFLE_H_
#define _WAFFLE_H_

#include "Module.h"
#include "generators.h"
#include "filters.h"

#include <list>
#include <vector>
#include <jack/jack.h>
#include <jack/types.h>

namespace waffle {


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

}

#endif
