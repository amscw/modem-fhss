#include "server_config.h"

config_struct* server_config::cfg_ = nullptr;

server_config* server_config::instance_ = nullptr;

destroyer_singleton<server_config> server_config::destroyer_;

server_config::server_config() noexcept
{
	if(cfg_ == nullptr)
	{
		cfg_ = new config_struct();
	}
}

server_config::~server_config() noexcept
{
	if(cfg_)
	{
		delete cfg_;
	}
}

server_config* server_config::get_instance() noexcept
{
	if(instance_ == nullptr)
	{
		instance_ = new server_config();
		destroyer_.initialize(instance_);
	}
	return instance_;
}

void server_config::delete_instance() noexcept
{
	delete instance_;
}

void server_config::load_config_file(const std::string& file_name)
{
	assert(!file_name.empty());

	try
	{
		config_ = YAML::LoadFile(file_name);

		get_server_name();
		get_server_type();
		get_ip_version();
		get_ip_address();
		get_port();
		get_access_log();
		get_error_log();
		get_pidfile();
	}
	catch(YAML::ParserException &ex)
	{
		throw ex;
	}
}

const config_struct* server_config::get_config() noexcept
{
	return cfg_;
}

void server_config::get_server_name()
{
	try
	{
		cfg_->name_ = config_["Server"]["Name"].as<std::string>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_server_type()
{
	try
	{
		cfg_->type_ = config_["Server"]["Type"].as<std::string>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_ip_version()
{
	try
	{
		cfg_->connection_.ip_version_ = config_["Server"]["Connection"]["ip_version"].as<std::size_t>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_ip_address()
{
	try
	{
		cfg_->connection_.ip_address_ = config_["Server"]["Connection"]["ip_address"].as<std::string>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_port()
{
	try
	{
		cfg_->connection_.port_ = config_["Server"]["Connection"]["port"].as<std::uint16_t>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_access_log()
{
	try
	{
		cfg_->directories_.logs_.access_log_ = config_["Server"]["Directories"]["Logs"]["access_log"].as<std::string>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_error_log()
{
	try
	{
		cfg_->directories_.logs_.error_log_ = config_["Server"]["Directories"]["Logs"]["error_log"].as<std::string>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}

void server_config::get_pidfile()
{
	try
	{
		cfg_->directories_.pid_.pid_path_ = config_["Server"]["Directories"]["Pid"]["pid_path"].as<std::string>();
	}
	catch(YAML::ParserException& ex)
	{
		throw ex;
	}
}
