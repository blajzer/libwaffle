// Waffle - Module.h
// The base module class

#ifndef _WAFFLE_MODULE_H_
#define _WAFFLE_MODULE_H_

namespace waffle {

//base module class
class Module {
public:
	Module(){};
	~Module(){};

	virtual double run()=0;
	virtual bool isValid()=0;
protected:

};

}

#endif
