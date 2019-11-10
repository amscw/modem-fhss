#include "types.h"
#include "access_log.h"
#include "connection_creator.h"
#include "connection_except.h"
#include "endpoint_ipv4.h"
#include "error_log.h"
#include "master_process.h"
#include "server_config.h"
#include "tcp_connection_creator.h"
#include "worker_process.h"
#include "worker_process_creator.h"

void master_process::start_process() noexcept
{
	const std::uint16_t port = server_config::get_instance()->get_config()->connection_.port_;
	const std::string ip_addr = server_config::get_instance()->get_config()->connection_.ip_address_;

	std::unique_ptr<tcp_connection_creator> tcp_create = nullptr;
	try
	{
		tcp_create = std::make_unique<tcp_connection_creator>(std::make_unique<endpoint_ipv4>(ip_addr, port));
	}
	catch (connection_except &ex)
	{
		error_log::get_instance()->save_log(ex.what());
		return;
	}

	auto process_creator = std::make_unique<worker_process_creator>();
	auto process = std::make_unique<worker_process>();

	process->get_connection(std::move(tcp_create));

	pid_t pid = process_creator->create_process();

	switch(pid)
	{
		case process_creator::CHILD_PROCESS:
		{
			access_log::get_instance()->save_log("Created worker process");
			process->start_process();
			break;
		}

		case process_creator::ERROR_PROCESS:
		{
			error_log::get_instance()->save_log("Create worker process");
			return;
			break;
		}
	}

	sigset_t sigset;
	siginfo_t siginfo;

	setup_signal(sigset, siginfo);

	sigwaitinfo(&sigset, &siginfo); // ожидание поступления сигнала

	if(siginfo.si_signo == SIGUSR1) // сигнал о завершении программы
	{
		kill(pid, SIGTERM);
		access_log::get_instance()->save_log("Master manager process handler received signal SIGUSR1: stopped server.");
		exit(EXIT_SUCCESS);
	}
}

void master_process::setup_signal(sigset_t& sigset, siginfo_t& siginfo) noexcept
{
	sigemptyset(&sigset);                   // настраиваем сигналы которые будем обрабатывать

	sigaddset(&sigset, SIGUSR1);            // пользовательский сигнал для завершения процесса

	sigprocmask(SIG_BLOCK, &sigset, NULL);
}
