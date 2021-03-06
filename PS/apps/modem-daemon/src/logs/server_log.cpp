#include "types.h"
#include "server_log.h"

#include <cassert>
#include <fstream>

const int server_log::time_length_ = 80;

bool server_log::save_log(const std::string& msg) const noexcept
{
	assert(!msg.empty());

	std::string log_msg = create_log_struct(msg);

	std::ofstream stream;
	{
		std::lock_guard<std::mutex> guard(mut_);
		stream.open(log_filename_, std::ios::out | std::ios::app);
		if(!stream.is_open())
		{
			return false;
		}

		stream.write(log_msg.c_str(), log_msg.size());

		stream.close();
	}

	return true;
}

std::string server_log::create_log_struct(const std::string& msg) const noexcept
{
	assert(!msg.empty());

	return get_data_time() + msg + "\n";;
}

std::string server_log::get_data_time(void) const noexcept
{
	char data_time[time_length_];
	time_t raw_time;
	struct tm* time_info;

	time(&raw_time);
	time_info = localtime(&raw_time);

	strftime(data_time, time_length_, "[%d-%m-%Y %I:%M:%S] ", time_info);

	return data_time;
}
