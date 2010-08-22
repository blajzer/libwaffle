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

#include "waffle.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace waffle;

float Waffle::sampleRate;
int Waffle::bufferSize;

Waffle::Waffle(const std::string &name){
	pthread_mutex_init(&m_lock, NULL);
	
	//connect to jack
	jack_status_t jack_status;
	if(!(m_jackClient = jack_client_open(name.c_str(),JackNoStartServer,&jack_status))){
		std::cerr << "Jack Error: Failed to connect client" << std::endl;
		exit(1);
	}
	
	//register callbacks
	jack_set_sample_rate_callback(m_jackClient, Waffle::samplerate_callback, NULL);
	jack_set_buffer_size_callback(m_jackClient, Waffle::buffersize_callback, NULL);
	jack_set_process_callback(m_jackClient, Waffle::process_callback, this);
	
	srand(time(NULL));
	
	Waffle::sampleRate = (float)jack_get_sample_rate(m_jackClient);
	Waffle::bufferSize = jack_get_buffer_size(m_jackClient);
	
	jack_activate(m_jackClient);
}

Waffle::~Waffle(){
	pthread_mutex_lock(&m_lock);
	std::map<std::string, Patch *>::iterator it = m_patches.begin();
	std::map<std::string, Patch *>::iterator end_cached = m_patches.end();
	for(; it != end_cached; ++it) {
		jack_port_unregister(m_jackClient, it->second->jackPort);
		delete it->second;
	}
	m_patches.clear();
	pthread_mutex_unlock(&m_lock);
		
	pthread_mutex_destroy(&m_lock);

	jack_client_close(m_jackClient);
}

void Waffle::addPatch(const std::string &name, Module *m){
	std::map<std::string, Patch *>::iterator it = m_patches.find(name);
	if(it == m_patches.end()) {
		Patch *p = new Patch(m);

		//register an output port
		if(!(p->jackPort = jack_port_register(m_jackClient,name.c_str(),JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0))){
			std::cerr << "Jack Error: Failed to register port: " << name << std::endl;
			exit(1);
		}

		m_patches[name] = p;
	} else {
		std::cerr << "Patch already exists for name \"" << name << "\", replacing." << std::endl;
		it->second->module = m;
	}
}

void Waffle::setPatch(const std::string &name, Module *m){
	std::map<std::string, Patch *>::iterator it = m_patches.find(name);
	if(it != m_patches.end()) {
		m_patches[name]->module = m;
	} else {
		std::cerr << "Patch with name \"" << name << "\" does not exist." << std::endl;
	}
}

bool Waffle::removePatch(const std::string &name){
	std::map<std::string, Patch *>::iterator it = m_patches.find(name);
	if(it != m_patches.end()){
		jack_port_unregister(m_jackClient, it->second->jackPort);
		delete it->second;
		m_patches.erase(it);
		return true;
	}else{
		return false;
	}
}


std::map< std::string, bool > Waffle::validatePatches() {
	std::map< std::string, bool > results;
	
	std::map<std::string, Patch *>::iterator it = m_patches.begin();
	for( ; it != m_patches.end(); ++it)
		results[it->first] = it->second->module->isValid();

	return results;
}

double Waffle::midiToFreq(int note){
	return 8.1758 * pow(2.0, (double)note/12.0);
}

//callbacks
int Waffle::samplerate_callback(jack_nframes_t nframes, void *arg){
	Waffle::sampleRate = (double)nframes;
	return 0;
}

int Waffle::buffersize_callback(jack_nframes_t nframes, void *arg){
	Waffle::bufferSize = nframes;
	return 0;
}

int Waffle::process_callback(jack_nframes_t nframes, void *arg){
	static_cast<Waffle *>(arg)->run(nframes);
	return 0;
}

void Waffle::start(const std::string &name){
	std::map<std::string, Patch *>::iterator it = m_patches.find(name);
	if(it != m_patches.end()){
		it->second->silent = false;
	}
}

void Waffle::stop(const std::string &name){
	std::map<std::string, Patch *>::iterator it = m_patches.find(name);
	if(it != m_patches.end()){
		it->second->silent = true;
	}
}

void Waffle::run(jack_nframes_t nframes){
	pthread_mutex_lock(&m_lock);
	
	std::map<std::string, Patch *>::iterator it = m_patches.begin();
	std::map<std::string, Patch *>::iterator end_cached = m_patches.end();
	for(; it != end_cached; ++it) {
		//get jack output port buffer
		jack_default_audio_sample_t *out;
		out = (jack_default_audio_sample_t *)jack_port_get_buffer(it->second->jackPort, nframes);

		Module *m = it->second->module;
		bool silent = it->second->silent;
		for(int b=0; b < nframes; ++b){
			double result = 0.0; 
			if(!silent){
				result = m->getValue();
		
				//Clip the audio
				if(result < -1.0f) result = -1.0f;
				if(result > 1.0f) result = 1.0f;
			}
			//put into the stream
			out[b] = (jack_default_audio_sample_t)result;

			//reset patch
			m->reset();
		}
	}
	pthread_mutex_unlock(&m_lock);
}

Waffle::Patch::~Patch() {
	std::set<Module *> modules;
	modules.insert(module);
	module->gatherSubModules(modules);
	
	std::set<Module *>::iterator it = modules.begin();
	std::set<Module *>::iterator endCached = modules.end();
	for( ; it != endCached; ++it) {
		std::cout << *it << std::endl;
		delete (*it);
	}
}

