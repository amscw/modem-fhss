#include "access_log.h"
#include "daemon_process.h"
#include "error_log.h"
#include "master_process_creator.h"
#include "types.h"

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

class daemon_process::impl
{
public:
	impl() : process_(nullptr), process_creator_(nullptr) {};
	~impl() = default;

	/**
	 * @brief регистрация сигналов в обработчике сигналов
	 *
	 * @param sigset набор сигналов
	 * @param siginfo информация о сигналах
	 */
	void setup_signal(sigset_t& sigset, siginfo_t& siginfo) const noexcept;

public:
	// Объект process
	std::unique_ptr<process> process_;

	// Объект process_creator
	std::unique_ptr<process_creator> process_creator_;
};

daemon_process::daemon_process() : pimpl_(std::make_unique<impl>()) {};

daemon_process::~daemon_process() = default;

void daemon_process::start_process() noexcept
{
	pid_t sid = default_error_code;

	sid = setsid(); //создание нового сеанса (созданный процесс является ведущим и не имеет конролирующего терминала)
	if(sid == default_error_code)
	{
		error_log::get_instance()->save_log("set sid error");
		return;
	}

	if (chdir("/") == default_error_code) //установка текущего каталога
	{
		error_log::get_instance()->save_log("chdir error");
		return;
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	pimpl_->process_creator_ = std::make_unique<master_process_creator>();
	pimpl_->process_ = pimpl_->process_creator_->get_process();

	pid_t pid = pimpl_->process_creator_->create_process();

	switch(pid)
	{
		case static_cast<pid_t>(process_creator::state_process::CHILD_PROCESS):
		{
			access_log::get_instance()->save_log("Run master process");
			pimpl_->process_->start_process();
			break;
		}

		case static_cast<pid_t>(process_creator::state_process::ERROR_PROCESS):
		{
			error_log::get_instance()->save_log("Run master process");
			break;
		}

		default:
		{
			sigset_t sigset;
			siginfo_t siginfo;

			pimpl_->setup_signal(sigset, siginfo);

			sigwaitinfo(&sigset, &siginfo); // ожидание поступления сигнала

			if(siginfo.si_signo == SIGCHLD) // сигнал пришел от потомка
			{
				waitpid(pid, 0, WNOHANG);
				access_log::get_instance()->save_log
				  ("Daemon manager process handler received signal SIGCHLD: stopped server.");
				return;
			}
			else if(siginfo.si_signo == SIGUSR1) // сигнал о завершении программы
			{
				kill(pid, SIGUSR1);
				access_log::get_instance()->save_log("Daemon manager process handler received signal SIGUSR1.");
				return;
			}
			break;
		}
	}
}

void daemon_process::impl::setup_signal(sigset_t& sigset, siginfo_t& siginfo) const noexcept
{
	sigemptyset(&sigset);                   // настраиваем сигналы которые будем обрабатывать

	sigaddset(&sigset, SIGUSR1);            // пользовательский сигнал для завершения процесса

	sigaddset(&sigset, SIGCHLD);            // сигнал, посылаемый при изменении статуса дочернего процесса

	sigprocmask(SIG_BLOCK, &sigset, NULL);
}
