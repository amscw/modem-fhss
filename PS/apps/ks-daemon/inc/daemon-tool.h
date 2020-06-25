#ifndef _DAEMON_TOOL
#define _DAEMON_TOOL

#include <memory>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <yaml-cpp/yaml.h>

#include "common.h"
#include "logger.h"
#include "exc.h"
#include "daemon.h"
#include "hw.h"
#include "keygen.hpp"

struct daemonToolExc_c : public exc_c
{
	enum class errCode_t : std::uint32_t {
		ERROR_OPEN,
		ERROR_FORK,
		ERROR_EXEC_RUN,
		ERROR_EXEC_FAIL,
		ERROR_SAVE_FILE,
	} m_errCode;

	daemonToolExc_c(enum errCode_t code, const std::string &strFile, const std::string &strFunction, const std::string &strErrorDescription = "") noexcept :
			exc_c(strFile, strFunction, strErrorDescription), m_errCode(code)
	{}

	const std::string &Msg() const noexcept override { return strErrorMessages[(int)m_errCode]; }
	void ToStderr() const noexcept override
	{
		std::cerr << "WTF:" << m_strFile << "(" << m_strFunction << "):" << strErrorMessages[(int)m_errCode] << "-" << m_strErrorDescription << std::endl;
	}

	std::string ToString() noexcept override
	{
		oss.str("");
		oss.clear();
		oss << "WTF:" << m_strFile << "(" << m_strFunction << "):" << strErrorMessages[(int)m_errCode] << "-" << m_strErrorDescription;
		return oss.str();
	}

	const errCode_t &Errcode() const noexcept{ return m_errCode; }

	private:
		static std::string strErrorMessages[];
		std::ostringstream oss;
};


class daemonTool_c
{
	// Logger
	std::unique_ptr<logger_c>logger = nullptr;
	
	// Daemons
	std::unique_ptr<daemon_c> sshpass = nullptr;
	std::unique_ptr<ping_c> ping = nullptr;
	std::unique_ptr<fakeDaemon_c> fake = nullptr;
	
	// Hardware abstraction layer
	std::unique_ptr<hw_c> hw = nullptr;

	const std::string pidFilename{"pid"};
	/* const */std::string cfgFilename{"ks-configs.yaml"};

	int pid;
	sigset_t sigset;
	siginfo_t siginfo;

	YAML::Node cfgparser;
	struct {
		struct {
			std::string ifname;
			std::string ipaddr;
			std::string login;
			std::string password;
			std::string srcfile;
			std::string dstdir;
			bool isMaster;
			std::string modem_type;
		} cmn;
		struct {
			std::string ifname;
			int timeout;
			int count;
		} ping;
		struct {
			int timeout;
		} sshp;
	} cfg;


public:
	daemonTool_c(const std::string &filename);
	inline bool IsChild() const noexcept {return (pid == 0); }
	int Run();
	void LoadConfigsFromFile(const std::string &filename);

	const std::string &Keydir() const noexcept { return cfg.cmn.dstdir; }
	const std::string &ModemType() const noexcept { return cfg.cmn.modem_type; }

private:
	int exec(std::unique_ptr<daemon_c> daemon);
	void savePIDToFile(const std::string &filename);
};

#endif // _DAEMON_TOOL
