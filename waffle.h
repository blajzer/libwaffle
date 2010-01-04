//Waffle,
// a crappy modular Synth
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
