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

#ifndef _WAFFLE_OSC_H_
#define _WAFFLE_OSC_H_

#include "Module.h"

#include <lo/lo.h>
#include <pthread.h>

namespace waffle {

//! Base for all OSC modules
class OSCModule : public Module {
public:
	OSCModule();
	virtual ~OSCModule();
	
	static void setPort(unsigned int portNum) { ms_portNum = portNum; }
	
protected:
	lo_server_thread getServerThread();
	pthread_mutex_t m_lock;

private:
	static void errorHandler(int num, const char *msg, const char *path);

	static lo_server_thread ms_pServerThread;
	static unsigned int ms_portNum;
};

//! Basic OSC trigger
class OSCTrigger : public OSCModule {
public:
	OSCTrigger(const std::string &path);
	
	double run();
	bool isValid() { return true; }
private:
	void trigger();
	
	static int oscCallback(const char *path, const char *types, lo_arg **argv, int argc, lo_message  msg, void *user_data);
	bool m_trigger;
};

//! OSC trigger that stays high for an amount of time
class OSCTimedTrigger : public OSCModule {
public:
	OSCTimedTrigger(const std::string &path);
	
	double run();
	bool isValid() { return true; }
private:
	void trigger(float time);
	
	static int oscCallback(const char *path, const char *types, lo_arg **argv, int argc, lo_message  msg, void *user_data);
	int m_timer;
};

//! OSC Value
class OSCValue : public OSCModule {
public:
	OSCValue(const std::string &path);
	
	double run(){};
	double getValue();
	bool isValid() { return true; }
private:
	void setValue(double v);
	
	static int oscCallback(const char *path, const char *types, lo_arg **argv, int argc, lo_message  msg, void *user_data);
	double m_value;
};

}
#endif

