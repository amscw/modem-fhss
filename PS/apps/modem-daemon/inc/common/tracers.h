#ifndef _TRACERS_H
#define _TRACERS_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include "access_log.h"

#define LOG_CSTR(msg) {\
	std::ostringstream osslog;\
	osslog << "TRACE: (" << __FUNCTION__ << ")-" << msg << std::endl;\
	access_log::get_instance()->save_log(osslog.str().c_str());\
	osslog.str("");\
	osslog.clear();\
}

#define LOG_STR(msg) {\
	std::ostringstream osslog;\
	osslog << "TRACE: (" << __FUNCTION__ << ")-" << msg << std::endl;\
	access_log::get_instance()->save_log(osslog.str().c_str());\
	msg.clear();\
}

#define LOG_STREAM(strm) {\
	std::string msg("TRACE: (");\
	msg.append(__FUNCTION__);\
	msg.append(")-");\
	msg.append(strm.str());\
	access_log::get_instance()->save_log(msg);\
	strm.str("");\
	strm.clear();\
}

#endif // _TRACERS_H
