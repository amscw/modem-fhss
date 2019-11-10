#include "types.h"
#include "daemon.h"
#include "access_log.h"
#include "error_log.h"
#include "daemon_tool_exception.h"
#include "server_config.h"
#include "server_log_exception.h"
#include "daemon_process_creator.h"

#include <cstring>
#include <cassert>

void daemon_tool::init_config()
{
	try
	{
		server_config::get_instance()->load_config_file("/etc/capture-daemon/config.yaml");
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

void daemon_tool::start_daemon() const
{
	auto process_creator = std::make_unique<daemon_process_creator>();
	auto process(process_creator->get_process());

	pid_t pid = process_creator->create_process();

	switch (pid)
	{
		case process_creator::CHILD_PROCESS:
		{
			access_log::get_instance()->save_log("Created daemon process");
			process->start_process();
			break;
		}

		case process_creator::ERROR_PROCESS:
		{
			error_log::get_instance()->save_log("Create daemon process");
			throw daemon_tool_exception(msg_type::msg_start_daemon_error);
			break;
		}

		default:
		{
			const std::string pid_filename = server_config::get_instance()->get_config()->directories_.pid_.pid_path_;
			if(write_pid(pid, pid_filename))
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

bool daemon_tool::write_pid(const int& pid, const std::string& pid_filename) const noexcept
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
