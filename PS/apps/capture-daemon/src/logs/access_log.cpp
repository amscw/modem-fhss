#include "access_log.h"
#include "server_log_exception.h"

access_log* access_log::log_ = nullptr;
destroyer_singleton<access_log> access_log::destroyer_;

access_log* access_log::get_instance() noexcept
{
	if (!log_)
	{
		log_ = new access_log;
		destroyer_.initialize(log_);
	}

	return log_;
}

void access_log::init_log_file(const std::string& file_name)
{
	assert(!file_name.empty());
	log_filename_ = file_name;

	if (!save_log("Logfile access initialization complete"))
	{
		throw server_log_exception(msg_type::msg_init_access_log_err);
	}
}
