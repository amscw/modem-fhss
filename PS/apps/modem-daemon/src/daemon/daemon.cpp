#include "access_log.h"
#include "daemon.h"
#include "daemon_process_creator.h"
#include "daemon_tool_exception.h"
#include "error_log.h"
#include "server_config.h"
#include "server_log_exception.h"
#include "types.h"

#include <cstring>
#include <cassert>
#include <signal.h>

class daemon_tool::impl
{
public:
	impl() : process_(nullptr), process_creator_(nullptr) {};
	~impl() = default;

	/**
	 *@brief запись  номера созданного процесса pid в файл
	 *
	 *@param pid_filename имя файла для записи номера pid
	 */
	bool write_pid(const int& pid, const std::string& pid_filename) const noexcept;

public:
	// Объект process
	std::unique_ptr<process> process_;

	// Объект process_creator
	std::unique_ptr<process_creator> process_creator_;
};

daemon_tool::daemon_tool() : pimpl_(std::make_unique<impl>()) {};

daemon_tool::~daemon_tool() = default;

void daemon_tool::init_config()
{
	try
	{
		server_config::get_instance()->load_config_file("/etc/modem-daemon/config.yaml");
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void daemon_tool::init_log()
{
	try
	{
		access_log::get_instance()->
		  init_log_file(server_config::get_instance()->get_config()->directories_.logs_.access_log_);
		error_log::get_instance()->
		  init_log_file(server_config::get_instance()->get_config()->directories_.logs_.error_log_);
	}
	catch(const server_log_exception& ex)
	{
		throw ex;
	}
}

void daemon_tool::start_daemon()
{
	pimpl_->process_creator_ = std::make_unique<daemon_process_creator>();
	pimpl_->process_ = pimpl_->process_creator_->get_process();

	pid_t pid = pimpl_->process_creator_->create_process();

	switch (pid)
	{
		case static_cast<pid_t>(process_creator::state_process::CHILD_PROCESS):
		{
			access_log::get_instance()->save_log("Run daemon process");
			pimpl_->process_->start_process();
			break;
		}

		case static_cast<pid_t>(process_creator::state_process::ERROR_PROCESS):
		{
			error_log::get_instance()->save_log("Run daemon process");
			throw daemon_tool_exception(msg_type::msg_start_daemon_error);
			break;
		}

		default:
		{
			const std::string pid_filename = server_config::get_instance()->get_config()->directories_.pid_.pid_path_;
			if(pimpl_->write_pid(pid, pid_filename))
			{
				access_log::get_instance()->save_log("Save pid: " + std::to_string(pid) + " to file");
			}
			else
			{
				error_log::get_instance()->save_log("Can not save pid: " + std::to_string(pid) + " to file");
				kill(pid, SIGUSR1);
				throw daemon_tool_exception(msg_type::msg_start_daemon_error);
			}
			break;
		}
	}
}

bool daemon_tool::impl::write_pid(const int& pid, const std::string& pid_filename) const noexcept
{
	assert(!pid_filename.empty());

	FILE* stream = fopen( pid_filename.c_str(), "w+");
	bool res = false;
	if(stream)
	{
		res = (fwrite((std::to_string(pid)).c_str(), 1, (std::to_string(pid)).size(), stream) == (std::to_string(pid)).size());
		fclose( stream );
	}
	return res;
}
