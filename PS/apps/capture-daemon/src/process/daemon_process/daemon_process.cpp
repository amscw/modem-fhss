#include "types.h"
#include "process.h"
#include "error_log.h"
#include "access_log.h"
#include "daemon_process.h"
#include "process_creator.h"
#include "master_process_creator.h"

#include <sys/wait.h>
#include <unistd.h>

void daemon_process::start_process() noexcept
{
	pid_t sid = default_error_code;

	sigset_t sigset;
	siginfo_t siginfo;

	sid = setsid(); //создание нового сеанса (созданный процесс является ведущим и не имеет конролирующего терминала)
	if(sid == default_error_code)
	{
		error_log::get_instance()->save_log("set sid error");
		return;
	}

	chdir("/");

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	setup_signal(sigset, siginfo);

	auto process_creator = std::make_unique<master_process_creator>();
	auto process(process_creator->get_process());

	pid_t pid = process_creator->create_process();

	switch(pid)
	{
		case process_creator::CHILD_PROCESS:
		{
			access_log::get_instance()->save_log("Created master process");
			process->start_process();
			break;
		}

		case process_creator::ERROR_PROCESS:
		{
			error_log::get_instance()->save_log("Create master process");
			break;
		}

		default:
		{
			sigwaitinfo(&sigset, &siginfo); // ожидание поступления сигнала

			if(siginfo.si_signo == SIGCHLD) // сигнал пришел от потомка
			{
				waitpid(pid, 0, WNOHANG);
				access_log::get_instance()->save_log("Daemon manager process handler received signal SIGCHLD: stopped server.");
				exit(EXIT_SUCCESS);
			}
			else if(siginfo.si_signo == SIGUSR1) // сигнал о завершении программы
			{
				kill(pid, SIGUSR1);
				access_log::get_instance()->save_log("Daemon manager process handler received signal SIGUSR1.");
				exit(EXIT_SUCCESS);
			}
			break;
		}
	}
}

void daemon_process::setup_signal(sigset_t& sigset, siginfo_t& siginfo) noexcept
{
	sigemptyset(&sigset);                   // настраиваем сигналы которые будем обрабатывать

	sigaddset(&sigset, SIGUSR1);            // пользовательский сигнал для завершения процесса

	sigaddset(&sigset, SIGCHLD);            // сигнал, посылаемый при изменении статуса дочернего процесса

	sigprocmask(SIG_BLOCK, &sigset, NULL);
}
