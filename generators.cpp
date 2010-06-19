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

#include "generators.h"
#include "waffle.h"

#include <cmath>

using namespace waffle;

static const double PI = 3.141592653589732384626;
static const double TWO_PI = 2.0 * PI;

//Base WaveformGenerator
void WaveformGenerator::setFreq(Module *f){
	m_freq = f;
	m_pos = 0.0;
}

bool WaveformGenerator::isValid() { 
	if(m_freq != NULL && m_phase != NULL)
		return m_freq->isValid() && m_phase->isValid();
	else
		return false;
}

void WaveformGenerator::reset() {
	if(!m_dirtyCache)
	{
		m_dirtyCache = true;
		m_freq->reset();
		m_phase->reset();
	}
}

//Sine Wave Generator
GenSine::GenSine(Module *f, Module *p) : WaveformGenerator(f, p) {
}

double GenSine::run(){
	double data = sin(m_pos + (m_phase->getValue() * PI));
	m_pos += TWO_PI * ((m_freq->getValue())/Waffle::sampleRate);
	m_pos = fmod(m_pos, TWO_PI);

	return data;
}

//Triangle Wave Generator
GenTriangle::GenTriangle(Module *f, Module *p) : WaveformGenerator(f, p) {
}

double GenTriangle::run(){
	double cpos = fmod(m_pos + (m_phase->getValue() * PI), TWO_PI)/(TWO_PI);
	double data = (cpos < 0.5) ? cpos : (1 - cpos);
	m_pos += TWO_PI * (m_freq->getValue())/Waffle::sampleRate;
	m_pos = fmod(m_pos, TWO_PI);
	return (4*data)-1;
}

//Sawtooth Wave Generator
GenSawtooth::GenSawtooth(Module *f, Module *p) : WaveformGenerator(f, p) {
}

double GenSawtooth::run(){
	double data = (2*fmod(m_pos + (m_phase->getValue() * PI), TWO_PI)/(TWO_PI))-1;
	m_pos += TWO_PI * (m_freq->getValue())/Waffle::sampleRate;
	m_pos = fmod(m_pos, TWO_PI);
	return data;
}

//Sawtooth Wave Generator
GenRevSawtooth::GenRevSawtooth(Module *f, Module *p) : WaveformGenerator(f, p) {
}

double GenRevSawtooth::run(){
	double data = (2*(1 - fmod(m_pos + (m_phase->getValue() * PI), TWO_PI)/(TWO_PI))-1);
	m_pos += TWO_PI * (m_freq->getValue())/Waffle::sampleRate;
	m_pos = fmod(m_pos, TWO_PI);
	return data;
}

//Square Wave Generator
GenSquare::GenSquare(Module *f, Module *p, Module *t) : WaveformGenerator(f, p) {
	m_thresh = t;
}

void GenSquare::setThreshold(Module *t){
	m_thresh = t;
}

double GenSquare::run(){
	double cpos = fmod(m_pos + (m_phase->getValue() * PI), TWO_PI)/(TWO_PI);
	double data = (cpos < m_thresh->getValue()) ? -1 : 1;
	m_pos += TWO_PI * (m_freq->getValue())/Waffle::sampleRate;
	m_pos = fmod(m_pos, TWO_PI);
	return data;
}

//Noise Generator
double GenNoise::run(){
	return ((double)rand() / (double)RAND_MAX) - 0.5; 
}

//value Generator
double Value::getValue(){
	return m_value;
}

void Value::setValue(double v){
	m_value = v;
}

