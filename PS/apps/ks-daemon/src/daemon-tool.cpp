#include "daemon-tool.h"
#include <vector>
#include <experimental/filesystem>


std::string daemonToolExc_c::strErrorMessages[] = {
		"can't open file",
		"can't fork to new process",
		"can't exec new process",
		"exec process fail",
		"can't save file",
};

daemonTool_c::daemonTool_c(const std::string &filename) : cfgFilename(filename), pid(-1)
{
	std::ostringstream oss;

	try
	{
		logger = std::make_unique<logger_c>("/var/log/ks-daemon.log");
	} catch (exc_c &exc) {
		throw ;
	}

	pid = fork();
	switch (pid)
	{
	case -1:
		throw daemonToolExc_c(daemonToolExc_c::errCode_t::ERROR_FORK, __FILENAME__, __FUNCTION__);

	case 0:
		// child
		umask(0);
		setsid();

		// std streams no more need
		close(STDIN_FILENO);
		// close(STDOUT_FILENO);
		close(STDERR_FILENO);
		break;

	default:
		// parent
		oss << "created child: " << pid;
		// logger->Write(oss.str());
		TRACE(oss);
	}
}

int daemonTool_c::exec(std::unique_ptr<daemon_c> daemon)
{
	int wstatus;
	std::ostringstream oss;
	int err;

	if (daemon->IsChild())
	{
		// child process
		try
		{
			err = daemon->Exec();
		} catch (exc_c &exc) {
			throw;
		}
		// normally unreachable here. If not - throw exc
		oss << "error: " << err;
		throw daemonToolExc_c(daemonToolExc_c::errCode_t::ERROR_EXEC_RUN, __FILENAME__, __FUNCTION__, oss.str());
	} else {
		// parent process
		wait(&wstatus);
#if defined(DBG_PROCESS_TRACE)
		std::string str(daemon->Stdout());
		logger->Write(str);
		oss << "process " << daemon->Pid() << " terminated with status: " << "0x" << std::hex << wstatus;
		logger->Write(oss);
#endif // DBG_PROCESS_TRACE
	}

	if (!WIFEXITED(wstatus))
		throw daemonToolExc_c(daemonToolExc_c::errCode_t::ERROR_EXEC_FAIL, __FILENAME__, __FUNCTION__);

	return static_cast<int>(std::int8_t(WEXITSTATUS(wstatus)));
}

void daemonTool_c::savePIDToFile(const std::string &filename)
{
	std::ofstream ofs;

	ofs.open(filename, std::ios::out);
	if (!ofs.is_open())
		throw daemonToolExc_c(daemonToolExc_c::errCode_t::ERROR_OPEN, __FILENAME__, __FUNCTION__, filename);

	ofs << getpid();
	ofs.close();
}

void daemonTool_c::LoadConfigsFromFile(const std::string &filename)
{
	try
	{
		cfgparser = YAML::LoadFile(filename);

		// common settings
		cfg.cmn.ifname = cfgparser["common"]["ifname"].as<std::string>();
		cfg.cmn.ipaddr = cfgparser["common"]["ipaddr"].as<std::string>();
		cfg.cmn.login = cfgparser["common"]["login"].as<std::string>();
		cfg.cmn.password = cfgparser["common"]["password"].as<std::string>();
		cfg.cmn.srcfile = cfgparser["common"]["srcfile"].as<std::string>();
		cfg.cmn.dstdir = cfgparser["common"]["dstdir"].as<std::string>();
		cfg.cmn.isMaster = cfgparser["common"]["master"].as<bool>();

		// ping settings
		cfg.ping.ifname = cfgparser["ping"]["ifname"].as<std::string>();
		cfg.ping.count = cfgparser["ping"]["count"].as<int>();
		cfg.ping.timeout = cfgparser["ping"]["timeout"].as<int>();

		// ssh settings
		cfg.sshp.timeout = cfgparser["sshpass"]["timeout"].as<int>();
	} catch (YAML::ParserException &exc) {
		throw;
	}
}


/**
 * Создаёт демона, и процесс ответвляется в дочерний в конструкторе демона.
 *
 * В вызове exec():
 *
 * Дочерний процесс исполняет загруженную в демона командную строку.
 * Если исполнение невозможно, происходит бросок исключения ERROR_EXEC_RUN, которое обрабатывается в Run() дочернего процесса,
 * и дочерний процесс завершает работу с кодом -33.
 *
 * Родительский процесс блокируетя в exec(), ожидая завершения дочернего.
 * Если дочерний процесс не вернул 0, происходит бросок исключения ERROR_EXEC_FAIL, которое перехватывается в Run() родительского процесса, 
 * и родительский процесс завершает работу с кодом -1.
 *
 * Если дочерний процесс вернул 0, происходит возврат из exec() и создается следующий демон и т.д.
 */
int daemonTool_c::Run()
{
	std::ostringstream oss;
	int err;
	struct timespec ts = {0, 0};	// s, ns

	// prepare signal set
	sigemptyset(&sigset);

	// add signals to signal set
	sigaddset(&sigset, SIGQUIT); 	// сигнал остановки процесса пользователем
	sigaddset(&sigset, SIGINT);		// сигнал для остановки процесса пользователем с терминала
	sigaddset(&sigset, SIGTERM);	// сигнал запроса завершения процесса
	sigaddset(&sigset, SIGCHLD); 	// сигнал посылаемый при изменении статуса дочернего процесса
	sigaddset(&sigset, SIGUSR1);	// пользовательский сигнал который мы будем использовать для обновления конфига
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	// save PID
	try {
		savePIDToFile(pidFilename);
	} catch (exc_c &exc) {
		logger->Write(exc.ToString());
		exit(-1);
	}
	oss << "pid saved to file \"" << pidFilename << "\"";
	logger->Write(oss);

	// load configuration
	try {
		LoadConfigsFromFile(cfgFilename);
	} catch (YAML::ParserException &exc) {
		logger->Write(exc.msg);
		exit(-1);
	}
	oss << "configuration successfully loaded from \"" << cfgFilename << "\"";
	logger->Write(oss);

	// create hw level
	try {
		hw = std::make_unique<hw_c>(this->cfg.cmn.ifname);
		hw->SetMaster(cfg.cmn.isMaster);
		if (!hw->IsMaster())
		{
			oss << "hardware is slave. Exit";
			logger->Write(oss);
			exit(0);
		}
	} catch (exc_c &exc) {
		logger->Write(exc.ToString());
		exit(-1);
	}
	oss << "hardware abstraction level initialized";
	logger->Write(oss);

	// start engine
	while (1)
	{
		// TODO: process the signals
		err = sigtimedwait(&sigset, &siginfo, &ts);
		if (err > 0)
		{
			oss << "received the signal: " << err << "/";
			switch (siginfo.si_signo)
			{
				case SIGQUIT:
					oss << "SIGQUIT";
					exit(0);
					break;

				case SIGINT:
					oss << "SIGINT";
					break;

				case SIGTERM:
					oss << "SIGTERM";
					exit(0);
					break;

				case SIGCHLD:
					oss << "SIGCHLD";
					break;

				case SIGUSR1:
					oss << "SIGUSR1";
					exit(0);
					break;

				default:
					oss << "unknown";
			}
#if defined(DBG_SIGNALS)
			logger->Write(oss);
#else
			oss.str("");
			oss.clear();
#endif
		}

		try
		{
			// wait for linkup
			if (!hw->IsOnline())
				continue;
			oss << "link ok! try ping...";
			logger->Write(oss);

			// waiting connection to slave
			ping = std::make_unique<ping_c>(
					cfg.ping.ifname,
					cfg.cmn.ipaddr,
					cfg.ping.count,
					cfg.ping.timeout);
			err = exec(std::move(ping));
#if defined(DBG_PROCESS_TRACE)
			oss << "ping exit code: " << err << ", destroyed: " << std::boolalpha << !static_cast<bool>(ping);
			logger->Write(oss);
#endif
			if (err == 0)
			{
				oss << "ping ok! creating keys...";
				logger->Write(oss);
			} else continue;

			// create the new keys
			typedef std::unique_ptr<Keygen_Basic> item_type;
			std::vector<item_type> v;
			v.emplace_back(std::make_unique<CIKey>());
			v.emplace_back(std::make_unique<SAPKey>());
			v.emplace_back(std::make_unique<SAPIntrKey>());
			v.emplace_back(std::make_unique<DLinkCoderKey>());
			v.emplace_back(std::make_unique<HopSeedKey>());
			v.emplace_back(std::make_unique<DLinkDataPreampbleKey>());

			// save keys to file
			for (std::vector<item_type>::iterator it = v.begin(); it != v.end(); it++)
			{
				(*it)->Generate();
				(*it)->WriteTo("keys");
			}

			oss << "keys created! Try ssh...";
			logger->Write(oss);

			// attempt to pass the keys to slave
			sshpass = std::make_unique<sshpass_c>(
					cfg.cmn.ipaddr,
					sshpass_c::login_t(cfg.cmn.login, cfg.cmn.password),
					/*cfg.cmn.srcfile*/"keys",
					cfg.cmn.dstdir,
					cfg.sshp.timeout);
			err = exec(std::move(sshpass));
#if defined(DBG_PROCESS_TRACE)
			oss << "sshpass exit code: " << err << ", destroyed: " << std::boolalpha << !static_cast<bool>(sshpass);
			logger->Write(oss);
#endif
			if (err == 0)
			{
				oss << "keys fired! Saving...";
				logger->Write(oss);

				std::unique_ptr<char[]> buf = std::make_unique<char[]>(256);
				std::ifstream ifs("keys", std::ios_base::in | std::ios_base::binary);
				std::ofstream ofs(cfg.cmn.dstdir + "keys", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
				
				if (ifs.is_open() && ofs.is_open())
				{
					do {
						ifs.read(buf.get(), 256);
						ofs.write(buf.get(), ifs.gcount());
					} while (ifs.gcount() > 0);
					ifs.close();
					ofs.close();
				} else {
					oss << "can't save keys to file";
					logger->Write(oss);
				}
				remove("keys");
			} else {
				remove("keys");
				continue;
			}
			oss << "keys updated! Exit";
			logger->Write(oss);
			break;
		} catch (exc_c &exc) {
			logger->Write(exc.ToString());

			// process the exception
			daemonToolExc_c &dtexc = dynamic_cast<daemonToolExc_c&>(exc);
			switch (dtexc.Errcode())
			{
			case daemonToolExc_c::errCode_t::ERROR_EXEC_RUN:
				// no such program
				exit(-33);
				break;

			default:
				// other errors
				exit(-1);
			}
		}
	}
	return 0;
}
