// Waffle - Module.h
// The base module class
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

#ifndef _WAFFLE_MODULE_H_
#define _WAFFLE_MODULE_H_

#include <iostream>
#include <set>

namespace waffle {

//base module class
class Module {
public:
	Module() : m_cachedValue(0.0), m_dirtyCache(true){};
	virtual ~Module(){};

	virtual double run()=0;
	virtual bool isValid()=0;
	virtual void reset() { m_dirtyCache = true; }
	virtual double getValue() {
		if(m_dirtyCache) {
			m_dirtyCache = false;
			m_cachedValue = run();
		}
		return m_cachedValue;
	}
	
	//TODO: profile and optimize this
	virtual void gatherSubModules(std::set<Module *> &modules) = 0;

protected:
	double m_cachedValue;
	bool m_dirtyCache;
};

}

#endif
