#include "access_log.h"
#include "endpoint_ipv4.h"
#include "error_log.h"
#include "master_process.h"
#include "server_config.h"
#include "tcp_server_sync.h"
#include "tcp_server_async.h"
#include "types.h"

#include <signal.h>

class master_process::impl
{
public:
	impl() : tcp_server_(nullptr) {};
	~impl() = default;

	/**
	 * @brief регистрация сигналов в обработчике сигналов
	 *
	 * @param sigset набор сигналов
	 * @param siginfo информация о сигналах
	 */
	void setup_signal(sigset_t& sigset, siginfo_t& siginfo) const noexcept;

public:
	// Объект tcp_server
	std::unique_ptr<tcp_server> tcp_server_;
};

master_process::master_process() : pimpl_(std::make_unique<impl>()) {};

master_process::~master_process() = default;

void master_process::start_process() noexcept
{
	std::uint16_t port;
	std::string ip_addr;
	std::string server_type;

	// FIXME: на кой черт здесь блок try если get_config() помечен как noexcept ?!
	try
	{
		server_type = server_config::get_instance()->get_config()->type_;
		port = server_config::get_instance()->get_config()->connection_.port_;
		ip_addr = server_config::get_instance()->get_config()->connection_.ip_address_;
	}
	catch(YAML::ParserException &ex)
	{
		error_log::get_instance()->save_log(ex.what());
		return;
	}

	std::unique_ptr<endpoint_ipv4> ep = std::make_unique<endpoint_ipv4>(ip_addr, port);

	if (server_type == sync_server_name)
	{
		pimpl_->tcp_server_ = std::make_unique<tcp_server_sync>();
	}
	else if (server_type == async_server_name)
	{
		pimpl_->tcp_server_ = std::make_unique<tcp_server_async>();
	}
	else
	{
		error_log::get_instance()->save_log("Incorrect server type: " + server_type);
		return;
	}

	//Запуск сервера
	pimpl_->tcp_server_->run(ep);

	sigset_t sigset;
	siginfo_t siginfo;

	pimpl_->setup_signal(sigset, siginfo);	// регистрация обработчика сигналов

	sigwaitinfo(&sigset, &siginfo); // ожидание поступления сигнала

	if(siginfo.si_signo == SIGUSR1) // сигнал о завершении программы
	{
		access_log::get_instance()->save_log("Master manager process handler received signal SIGUSR1: stopped server.");
		pimpl_->tcp_server_->stop();
		return;
	}
}

void master_process::impl::setup_signal(sigset_t& sigset, siginfo_t& siginfo) const noexcept
{
	sigemptyset(&sigset);                   // настраиваем сигналы которые будем обрабатывать

	sigaddset(&sigset, SIGUSR1);            // пользовательский сигнал для завершения процесса

	sigprocmask(SIG_BLOCK, &sigset, NULL);
}
