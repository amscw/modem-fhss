#ifndef TRACERS_H
#define TRACERS_H

#include <iostream>
#include <sstream>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define TRACE(stream) {\
	std::cerr << "TRACE:" << __FILENAME__ << "(" << __FUNCTION__ << "): " << stream.str() << std::endl;\
	stream.str("");\
	stream.clear();\
}

#endif // TRACERS_H
