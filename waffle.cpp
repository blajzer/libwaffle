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

static const double PI = 3.141592653589732384626;
static const double TWO_PI = 2.0 * PI;

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


//Sine Wave Generator
GenSine::GenSine(Module *f):m_pos(0.0){
	m_freq = f;
}

void GenSine::setFreq(Module *f){
	m_freq = f;
	m_pos = 0.0;
}

double GenSine::run(){
	double data = sin(m_pos);
	m_pos += TWO_PI * ((m_freq->run())/Waffle::sampleRate);

	return data;
}

//Triangle Wave Generator
GenTriangle::GenTriangle(Module *f):m_pos(0.0){
	m_freq = f;
}

void GenTriangle::setFreq(Module *f){
	m_freq = f;
	m_pos = 0.0;
}

double GenTriangle::run(){
	double cpos = fmod(m_pos, TWO_PI)/(TWO_PI);
	double data = (cpos < 0.5) ? cpos : (1 - cpos);
	m_pos += TWO_PI * (m_freq->run())/Waffle::sampleRate;
	m_pos = fmod(m_pos, TWO_PI);
	return (4*data)-1;
}

//Sawtooth Wave Generator
GenSawtooth::GenSawtooth(Module *f):m_pos(0.0){
	m_freq = f;
}

void GenSawtooth::setFreq(Module *f){
	m_freq = f;
	m_pos = 0.0;
}

double GenSawtooth::run(){
	double data = (2*fmod(m_pos, TWO_PI)/(TWO_PI))-1;
	m_pos += TWO_PI * (m_freq->run())/Waffle::sampleRate;
	return data;
}

//Sawtooth Wave Generator
GenRevSawtooth::GenRevSawtooth(Module *f):m_pos(0.0){
	m_freq = f;
}

void GenRevSawtooth::setFreq(Module *f){
	m_freq = f;
	m_pos = 0.0;
}

double GenRevSawtooth::run(){
	double data = (2*(1 - fmod(m_pos, TWO_PI)/(TWO_PI))-1);
	m_pos += TWO_PI * (m_freq->run())/Waffle::sampleRate;
	return data;
}

//Square Wave Generator
GenSquare::GenSquare(Module *f, Module *t):m_pos(0.0){
	m_thresh = t;
	m_freq = f;
}

void GenSquare::setFreq(Module *f){
	m_freq = f;
	m_pos = 0.0;
}

void GenSquare::setThreshold(Module *t){
	m_thresh = t;
}

double GenSquare::run(){
	double cpos = fmod(m_pos, TWO_PI)/(TWO_PI);
	double data = (cpos < m_thresh->run()) ? -1 : 1;
	m_pos += TWO_PI * (m_freq->run())/Waffle::sampleRate;
	return data;
}

//Noise Generator
double GenNoise::run(){
	return (((double)rand() - ((double)RAND_MAX/2.0)) / ((double)RAND_MAX/2.0)); 
}

//value Generator
double Value::run(){
	return m_value;
}

void Value::setValue(double v){
	m_value = v;
}

//filter get child
Module *Filter::getChild(int n){
	if(n < m_children.size() && n > -1){
		return m_children[n];
	}else{
		return NULL;
	}
}

//filter set child
void Filter::setChild(int n, Module *m){
	if(n < m_children.size() && n > -1){
		m_children[n] = m;
	}
}

//obligatory ADSR envelope
Envelope::Envelope(double thresh, double a, double d, double s, double r, Module *t, Module *i):
m_thresh(thresh), m_attack(a), m_decay(d), m_sustain(s), m_release(r), m_a_c(0), m_d_c(0), m_r_c(0), m_volume(0.0)
{
	m_children.push_back(i);
	m_trig = t;
	m_a_t = (int)(a * Waffle::sampleRate);
	m_d_t = (int)(d * Waffle::sampleRate);
	m_r_t = (int)(r * Waffle::sampleRate);
	m_state = Envelope::OFF;
}


double Envelope::run(){
	double data = m_children[0]->run();
	double trigger = m_trig->run();

	switch(m_state){
		case Envelope::OFF:
			if(trigger < m_thresh){
				return 0.0;
			}else{
				m_state = Envelope::ATTACK;
				m_a_c = 0;
				return 0.0;
			}	
			break;
		case Envelope::ATTACK:		
			m_a_c++;
			if(m_a_c > m_a_t){
				m_state = Envelope::DECAY;
				m_d_c = 0;
				return data;
			}else{
				if(trigger < m_thresh){
					m_state = Envelope::RELEASE;
					m_r_c = 0;
				}
				m_volume = ((double)m_a_c/(double)m_a_t);				
				return data * m_volume;
			}
			break;
		case Envelope::DECAY:
			m_d_c++;
			if(m_d_c > m_d_t){
				m_state = Envelope::SUSTAIN;
				m_volume = m_sustain;
				return data * m_sustain;
			}else{
				if(trigger < m_thresh){
					m_state = Envelope::RELEASE;
					m_r_c = 0;
				}
				m_volume = ((1.0 - m_sustain) - ((1.0 - m_sustain) * ((double)m_d_c/(double)m_d_t)) + m_sustain);
				return data * m_volume;
			}
			break;
		case Envelope::SUSTAIN:
			if(trigger >= m_thresh){
				return data * m_sustain;
			}else{
				m_state = Envelope::RELEASE;
				m_r_c = 0;
				return data * m_sustain;
			}
			break;
		case Envelope::RELEASE:
			m_r_c++;
			if(m_r_c > m_r_t){
				m_volume = 0.0;
				m_state = Envelope::OFF;
				return 0.0;
			}else{
				if(trigger >= m_thresh){
					m_state = Envelope::ATTACK;
					m_a_c = 0;
					return 0.0;
				}
				return data * ((1 - (double)m_r_c/(double)m_r_t) * m_volume);
			}
			break;
	};
}

//Envelope retrigger
void Envelope::retrigger(){
	m_state = Envelope::ATTACK;
	m_a_c = 0;
}

//lowpass filter
LowPass::LowPass(Module *f, Module *m){
	m_freq = f;
	m_children.push_back(m);
	m_prev = 0.0;
}

double LowPass::run(){
	double rc = 1.0 / (m_freq->run() * TWO_PI);
	double dt = 1.0 / Waffle::sampleRate;
	double alpha = dt / (rc + dt);
	double v = m_children[0]->run();
	double out =  (alpha * v) + ((1-alpha) * m_prev);
	m_prev = out;
	return out;
}

bool LowPass::isValid(){
	if(m_freq != NULL && m_children[0] != NULL)
		return m_children[0]->isValid() && m_freq->isValid();
	else
		return false;
}

void LowPass::setFreq(Module *f){
	m_freq = f;
}

//highpass filter
HighPass::HighPass(Module *f, Module *m){
	m_freq = f;
	m_children.push_back(m);
	m_prev = 0.0;
}

double HighPass::run(){
	double rc = 1.0 / (m_freq->run() * TWO_PI);
	double dt = 1.0 / Waffle::sampleRate;
	double alpha = dt / (rc + dt);
	double v = m_children[0]->run();
	double out =  (alpha * m_prev) + ((1-alpha) * v);
	m_prev = out;
	return out;
}

bool HighPass::isValid(){
	if(m_freq != NULL && m_children[0] != NULL)
		return m_children[0]->isValid() && m_freq->isValid();
	else
		return false;
}

void HighPass::setFreq(Module *f){
	m_freq = f;
}

//multiplication filter
Mult::Mult(Module *m1, Module *m2){
	m_children.push_back(m1);
	m_children.push_back(m2);
}

double Mult::run(){
	return m_children[0]->run() * m_children[1]->run();
}

bool Mult::isValid(){
	if(m_children[0] != NULL && m_children[1] != NULL)
		return m_children[0]->isValid() && m_children[1]->isValid();
	else
		return false;
}

//addition filter
Add::Add(Module *m1, Module *m2){
	m_children.push_back(m1);
	m_children.push_back(m2);
}

double Add::run(){
	return m_children[0]->run() + m_children[1]->run();
}

bool Add::isValid(){
	if(m_children[0] != NULL && m_children[1] != NULL)
		return m_children[0]->isValid() && m_children[1]->isValid();
	else
		return false;
}

//subtraction filter
Sub::Sub(Module *m1, Module *m2){
	m_children.push_back(m1);
	m_children.push_back(m2);
}

double Sub::run(){
	return m_children[0]->run() - m_children[1]->run();
}

bool Sub::isValid(){
	if(m_children[0] != NULL && m_children[1] != NULL)
		return m_children[0]->isValid() && m_children[1]->isValid();
	else
		return false;
}

//absolute value filter
Abs::Abs(Module *m){
	m_children.push_back(m);
}

double Abs::run(){
	return fabs(m_children[0]->run());
}

//signal delay filter
Delay::Delay(double len, double thresh, Module *m, Module *t){
	m_length = (int)(len*Waffle::sampleRate);
	m_queue = std::list<double>(m_length, 0.0);
	m_children.push_back(m);
	m_trig = t;
	m_thresh = thresh;
}

void Delay::setLength(double len){
	m_length = (int)(len*Waffle::sampleRate);
	m_queue = std::list<double>(m_length, 0.0);
}

double Delay::run(){
	if(m_trig->run() > m_thresh){
		if(m_first == true){
			m_queue = std::list<double>(m_length, 0.0);
			m_first = false;
		}
		double data = m_queue.front();
		m_queue.pop_front();
		m_queue.push_back(m_children[0]->run());
		return data;
	}else{
		m_first = true;
		return m_children[0]->run();
	}
}

bool Delay::isValid(){
	if(m_children[0] != NULL && m_trig != NULL)
		return m_children[0]->isValid() && m_trig->isValid();
	else
		return false;
}

