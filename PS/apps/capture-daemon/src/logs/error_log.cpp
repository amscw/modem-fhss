#include "error_log.h"
#include "server_log_exception.h"

error_log* error_log::log_ = nullptr;
destroyer_singleton<error_log> error_log::destroyer_;

error_log* error_log::get_instance() noexcept
{
	if (!log_)
	{
		log_ = new error_log;
		destroyer_.initialize(log_);
	}

	return log_;
}

void error_log::init_log_file(const std::string& file_name)
{
	assert( !file_name.empty() );
	log_filename_ = file_name;

	if (!save_log("Logfile error initialization complete"))
	{
		throw server_log_exception(msg_type::msg_init_error_log_err);
	}
}
