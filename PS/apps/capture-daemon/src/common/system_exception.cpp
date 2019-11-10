#include "system_exception.h"

system_exception::system_exception(msg_type msg) noexcept :
  msg_(user_msg_parser::get_instance()->find_user_msg(msg)) { }

const char* system_exception::what() const noexcept
{
	return msg_.c_str();
}
