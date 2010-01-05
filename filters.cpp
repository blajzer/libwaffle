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

#include "filters.h"
#include "waffle.h"

#include <cmath>

using namespace waffle;

static const double PI = 3.141592653589732384626;
static const double TWO_PI = 2.0 * PI;

//filter isValid
bool Filter::isValid() {
	for(int i; i < m_children.size(); ++i) {
		if(!m_children[i]->isValid())
			return false;
	}

	return true;
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

void Envelope::setThresh(double t){
	m_thresh = t;
}

void Envelope::setAttack(double a){
	m_a_t = (int)(a * Waffle::sampleRate);
}

void Envelope::setDecay(double d){
	m_d_t = (int)(d * Waffle::sampleRate);
}

void Envelope::setSustain(double s){
	m_sustain = s;
}

void Envelope::setRelease(double r){
	m_r_t = (int)(r * Waffle::sampleRate);
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
	if(Filter::isValid() && m_freq != NULL)
		return m_freq->isValid();
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
	if(Filter::isValid() && m_freq != NULL)
		return m_freq->isValid();
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

//addition filter
Add::Add(Module *m1, Module *m2){
	m_children.push_back(m1);
	m_children.push_back(m2);
}

double Add::run(){
	return m_children[0]->run() + m_children[1]->run();
}

//subtraction filter
Sub::Sub(Module *m1, Module *m2){
	m_children.push_back(m1);
	m_children.push_back(m2);
}

double Sub::run(){
	return m_children[0]->run() - m_children[1]->run();
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
	if(Filter::isValid() && m_trig != NULL)
		return m_trig->isValid();
	else
		return false;
}
