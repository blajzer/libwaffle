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

Waffle *Waffle::m_singleton = NULL;

Waffle::Waffle(){
	//connect to jack
	jack_status_t jack_status;
	if(!(m_jackClient = jack_client_open("waffle",JackNoStartServer,&jack_status))){
		std::cerr << "Jack Error: Failed to connect client" << std::endl;
		exit(1);
	}
	
	//register an output port
	if(!(m_jackPort = jack_port_register(m_jackClient,"out",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0))){
		std::cerr << "Jack Error: Failed to register port" << std::endl;
		exit(1);
	}
	
	//register callbacks
	jack_set_sample_rate_callback(m_jackClient, Waffle::samplerate_callback, NULL);
	jack_set_buffer_size_callback(m_jackClient, Waffle::buffersize_callback, NULL);
	jack_set_process_callback(m_jackClient, Waffle::process_callback, NULL);
	
	srand(time(NULL));
	
	Waffle::sampleRate = (float)jack_get_sample_rate(m_jackClient);
	Waffle::bufferSize = jack_get_buffer_size(m_jackClient);
	
	m_norm = Waffle::NORM_CLIP;
	m_silent = true;
	
	jack_activate(m_jackClient);
}

Waffle::~Waffle(){
	jack_port_unregister(m_jackClient, m_jackPort);
	jack_client_close(m_jackClient);
}

Waffle *Waffle::get(){
	if(m_singleton != NULL)
		return m_singleton;
	else
		return m_singleton = new Waffle();
}

int Waffle::addPatch(Module *m){
	m_patches.push_back(m);
	return m_patches.size()-1;
}

void Waffle::setPatch(int n, Module *m){
	if(n < m_patches.size() && n > -1){
		std::list<Module *>::iterator it = m_patches.begin();
		while(n > 0){
			++it;
			--n;
		}
			
		(*it) = m;
	}
}

bool Waffle::removePatch(Module *m){
	std::list<Module *>::iterator it = find(m_patches.begin(), m_patches.end(), m);
	if(it != m_patches.end()){
		m_patches.erase(it);
		return true;
	}else{
		return false;
	}
}

bool Waffle::removePatch(int n){
	if(n < m_patches.size() && n > -1){
		std::list<Module *>::iterator it = m_patches.begin();
		while(n > 0){
			++it;
			--n;
		}
		m_patches.erase(it);
		
		return true;		
	}else{
		return false;
	}
}


std::list< std::pair<Module *, bool> > Waffle::validatePatches() {
	std::list< std::pair<Module *, bool> > results;
	
	std::list<Module *>::iterator it = m_patches.begin();
	for( ; it != m_patches.end(); ++it) {
		if( (*it)->isValid() )
			results.push_back( std::make_pair(*it, true) );
		else
			results.push_back( std::make_pair(*it, true) );
	}

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
  Waffle::get()->run(nframes);
  return 0;
}

void Waffle::setNormMethod(NormalizationMethod n){
	if(n < NORM_CLIP || n > NUM_NORMALIZATION_METHODS){
		std::cerr << "ERROR: Invalid normalization method" << std::endl;
		exit(1);
	}
	m_norm = n;
}

void Waffle::start(){
	m_silent = false;
}

void Waffle::stop(){
	m_silent = true;
}

void Waffle::run(jack_nframes_t nframes){
	//get jack output port buffer
	jack_default_audio_sample_t *out;
    out = (jack_default_audio_sample_t *)jack_port_get_buffer(m_jackPort, nframes);
    
	int nchan = m_patches.size();
	for(int b=0; b < nframes; ++b){
		double mixdown = 0.0;
		if(!m_silent){
			for(std::list<Module *>::iterator i=m_patches.begin(); i != m_patches.end(); ++i){
				mixdown += (*i)->run();
			}
			
			//normalization method
			switch(m_norm){
				case Waffle::NORM_CLIP: //clipping, doesn't normalize
					break;
				case Waffle::NORM_RELATIVE:
					if(mixdown != 0)
						mixdown /= ceil(fabs(mixdown)); // relative normalization
					break;
				case Waffle::NORM_ABSOLUTE:
					if(nchan != 0)
						mixdown /= nchan; //absolute normalization
					break;
				default:
					break;
			}
			if(mixdown < -1.0f) mixdown = -1.0f;
			if(mixdown > 1.0f) mixdown = 1.0f;
		}
		//put into the stream
		out[b] = (jack_default_audio_sample_t)mixdown;
	}
}
