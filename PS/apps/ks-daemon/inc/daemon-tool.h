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
	typedef std::unique_ptr<Keygen_Basic> key_t;

	// Logger
	std::unique_ptr<logger_c>logger = nullptr;
	
	// Daemons
	std::unique_ptr<daemon_c> sshpass = nullptr;
	std::unique_ptr<ping_c> ping = nullptr;
	std::unique_ptr<fakeDaemon_c> fake = nullptr;
	
	// Hardware abstraction layer
	std::unique_ptr<hw_c> hw = nullptr;

	// The keys
	std::vector<key_t> v;

	// State machine
	enum class state_t : std::uint32_t {
		MODEM_WAIT_CONNECT, 
		MODEM_PING,
		MODEM_CREATE_KEYS,
		MODEM_SEND_KEYS,
		MODEM_SAVE_KEYS,
		MODEM_WAIT_DISCONNECT,
	} state;

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
			bool keygen_en;
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
	void InstallKeys();

private:
	int exec(std::unique_ptr<daemon_c> daemon);
	void savePIDToFile(const std::string &filename);
	void saveKeysToFile(const std::string &filename);
	void loadConfigsFromFile(const std::string &filename);

	void switchTo(state_t new_state) noexcept
	{
		static const char* ss[] = {
			"MODEM_WAIT_CONNECT",
			"MODEM_PING",
			"MODEM_CREATE_KEYS",
			"MODEM_SEND_KEYS",
			"MODEM_SAVE_KEYS",
			"MODEM_WAIT_DISCONNECT",
		};
		std::ostringstream oss;
		state = new_state;
		oss << "daemon switched to [" << ss[static_cast<int>(new_state)] << "]";
		logger->Write(oss);
	} 
};

#endif // _DAEMON_TOOL
