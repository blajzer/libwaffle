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

#include <map>
#include <string>
#include <jack/jack.h>
#include <jack/types.h>

namespace waffle {


//the actual synth
class Waffle {
public:
	Waffle(const std::string &name = "waffle");
	~Waffle();
	
	static double midiToFreq(int note);
	
	//patch management
	void addPatch(const std::string &name, Module *m);
	void setPatch(const std::string &name, Module *m);
	bool removePatch(const std::string &name);
	std::map< std::string, bool > validatePatches();
	
	void start(const std::string &name);
	void stop(const std::string &name);
	
	static float sampleRate;
	static int bufferSize;

private:
	//jack callbacks
	static int samplerate_callback(jack_nframes_t nframes, void *arg);
	static int buffersize_callback(jack_nframes_t nframes, void *arg);
	static int process_callback(jack_nframes_t nframes, void *arg);

	void run(jack_nframes_t nframes);

	class Patch
	{
	public:
		Patch(Module *m) : module(m), jackPort(NULL), silent(true){}
		
		Module *module;
		jack_port_t *jackPort;
		bool silent;
	};

	std::map<std::string, Patch *> m_patches;
	
	jack_client_t *m_jackClient;
};

}

#endif
