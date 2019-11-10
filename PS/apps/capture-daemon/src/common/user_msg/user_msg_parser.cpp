#include <cassert>
#include "user_msg_parser.h"

user_msg_parser* user_msg_parser::instance_ = nullptr;
destroyer_singleton<user_msg_parser> user_msg_parser::destroyer_;

user_msg_parser* user_msg_parser::get_instance() noexcept
{
	if( instance_ == nullptr )
	{
		instance_ = new user_msg_parser();
		destroyer_.initialize( instance_ );
	}

	return instance_;
}

const std::string& user_msg_parser::find_user_msg(msg_type msg) const noexcept
{
	assert( msg < msg_type::msg_end );

	return user_msg_[msg];
}

const std::string user_msg_parser::user_msg_[] =
{
    "System_error",                                             // msg_system_error
    "System error. Can not to start server",                    // msg_start_daemon_error
    "Can not found access log path in server config file",      // msg_init_access_log_err
    "Can not found error log path in server config file",       // msg_init_error_log_err
};
