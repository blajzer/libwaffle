#include "generators.h"
#include "waffle.h"

#include <cmath>

using namespace waffle;

static const double PI = 3.141592653589732384626;
static const double TWO_PI = 2.0 * PI;

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
