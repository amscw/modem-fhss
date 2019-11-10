#include "connection_except.h"

connection_except::connection_except(error_type err) : err_(err), msg_("")
{

}

const char *connection_except::what() const noexcept
{
	switch (err_)
	{
		case connection_except::error_type::SOCKET_ERROR:
			msg_ = "Can't create socket";
			break;
		case connection_except::error_type::BIND_ERROR:
			msg_ = "Bind error";
			break;
		case connection_except::error_type::LISTEN_ERROR:
			msg_ = "Listen error";
			break;
		case connection_except::error_type::ACCEPT_ERROR:
			msg_ = "Accept error";
			break;
		case connection_except::error_type::SET_REUSADDR_ERROR:
			msg_ = "Reusaddr error";
			break;
		case connection_except::error_type::SET_NONBLOCK_ERROR:
			msg_ = "Nonblock error";
			break;
		case connection_except::error_type::SET_TIMEOUT_ERROR:
			msg_ = "Timeout set error";
			break;
		default:
			msg_ = "Unknown error";
			break;
	}
	return msg_.c_str();
}

connection_except::error_type connection_except::get_error_type() const noexcept
{
	return err_;
}
