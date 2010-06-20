/*
Copyright (c) 2010 Brett Lajzer

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

#include "osc.h"
#include "waffle.h"

#include <iostream>
#include <sstream>

using namespace waffle;

lo_server_thread OSCModule::ms_pServerThread = NULL;
unsigned int OSCModule::ms_portNum = 7770;

OSCModule::OSCModule() {
	pthread_mutex_init(&m_lock, NULL);
}

OSCModule::~OSCModule() {
	pthread_mutex_destroy(&m_lock);
}

lo_server_thread OSCModule::getServerThread() {
	if(!ms_pServerThread) {
		std::stringstream s;
		s << ms_portNum;
		ms_pServerThread = lo_server_thread_new_with_proto(s.str().c_str(), LO_UDP, OSCModule::errorHandler);
		lo_server_thread_start(ms_pServerThread);
	}
	return ms_pServerThread;
}

void OSCModule::errorHandler(int num, const char *msg, const char *path) {
	std::cerr << "OSC error " << num << " in path " << path << ": " << msg << std::endl;
}

OSCTrigger::OSCTrigger(const std::string &path) : OSCModule(), m_trigger(false) {
	lo_server_thread_add_method(getServerThread(), path.c_str(), "", OSCTrigger::oscCallback, this);
}
	
double OSCTrigger::run() {
	double result = 0.0;
	pthread_mutex_lock(&m_lock);
	if(m_trigger) {
		m_trigger = false;
		result = 1.0;
	}
	pthread_mutex_unlock(&m_lock);
	return result;
}

int OSCTrigger::oscCallback(const char *path, const char *types, lo_arg **argv, int argc, lo_message  msg, void *user_data) {
	static_cast<OSCTrigger *>(user_data)->trigger();
	return 0;
}

void OSCTrigger::trigger() {
	pthread_mutex_lock(&m_lock);
	m_trigger = true;
	pthread_mutex_unlock(&m_lock);
}


OSCTimedTrigger::OSCTimedTrigger(const std::string &path) : OSCModule(), m_timer(0) {
	lo_server_thread_add_method(getServerThread(), path.c_str(), "f", OSCTimedTrigger::oscCallback, this);
}
	
double OSCTimedTrigger::run() {
	double result = 0.0;
	pthread_mutex_lock(&m_lock);
	if(m_timer) {
		--m_timer;
		result = 1.0;
	}
	pthread_mutex_unlock(&m_lock);
	return result;
}

void OSCTimedTrigger::trigger(float time) {
	pthread_mutex_lock(&m_lock);
	m_timer = (int)(time * Waffle::sampleRate);
	pthread_mutex_unlock(&m_lock);
}
	
int OSCTimedTrigger::oscCallback(const char *path, const char *types, lo_arg **argv, int argc, lo_message  msg, void *user_data) {
	static_cast<OSCTimedTrigger *>(user_data)->trigger(argv[0]->f);
	return 0;
}

OSCValue::OSCValue(const std::string &path) : OSCModule(), m_value(0.0) {
	lo_server_thread_add_method(getServerThread(), path.c_str(), "f", OSCValue::oscCallback, this);
}
	
int OSCValue::oscCallback(const char *path, const char *types, lo_arg **argv, int argc, lo_message  msg, void *user_data) {
	static_cast<OSCValue *>(user_data)->setValue(argv[0]->f);
	return 0;
}

void OSCValue::setValue(double v) {
	pthread_mutex_lock(&m_lock);
	m_value = v;
	pthread_mutex_unlock(&m_lock);
}

double OSCValue::getValue() {
	pthread_mutex_lock(&m_lock);
	double val = m_value;
	pthread_mutex_unlock(&m_lock);
	return val;
}

