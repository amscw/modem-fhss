#include "hw.h"
#include "mfhssioctl.h"
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fstream>

std::string hwExc_c::strErrorMessages[] = {
	"can't open socket",
	"can't perform ioctl request",
	"can't open reg"
};

hw_c::hw_c(const std::string &ifn) : fd (socket(AF_INET, SOCK_DGRAM, 0)), ifname(ifn)
{
	std::ostringstream oss;

	if (fd < 0)
	{
		oss << "err = " << fd;
		throw hwExc_c(hwExc_c::errCode_t::ERROR_OPEN, __FILE__, __FUNCTION__, oss.str());
	}

	memset(&ifr, 0, sizeof ifr);
	memcpy(reinterpret_cast<char*>(ifr.ifr_name), ifname.c_str(), ifname.length());
}

hw_c::~hw_c()
{
	close(fd);
}

bool hw_c::IsOnline()
{
	std::ostringstream oss;
	int err;
	MFHSS_flags_TypeDef flags;

	flags.word = 0;
	ifr.ifr_data = reinterpret_cast<char*>(&flags.word);
	err = ioctl(fd, MFHSS_IOCHECK, &ifr);
	if (err != 0)
	{
		oss << "err = " << err;
		throw hwExc_c(hwExc_c::errCode_t::ERROR_IOCTL, __FILE__, __FUNCTION__, oss.str());
	}
	return (flags.bits.link_on == 1);
}

bool hw_c::IsMaster()
{
	std::ifstream ifs;
	bool isMaster = false;

	// ifs.exceptions(std::ios_base::failbit);
	
	try
	{
		ifs.open("/sys/mfhss-dynamic/m/master");
	} catch (const std::ifstream::failure &e) {
		THROW_EXC_MSG(hwExc_c, hwExc_c::errCode_t::ERROR_OPEN_REG, e.what());
	}
	if (ifs.is_open()) {
		ifs >> isMaster;
		ifs.close();
	} else { // ifs.is_open()
		THROW_EXC(hwExc_c, hwExc_c::errCode_t::ERROR_OPEN_REG);
	}

	return isMaster;	
}

void hw_c::SetMaster(bool isMaster)
{
	std::ofstream ofs;

	ofs.exceptions(std::ios_base::failbit);

	try {
		ofs.open("/sys/mfhss-dynamic/m/master");
	} catch (const std::ofstream::failure& e) {
		THROW_EXC_MSG(hwExc_c, hwExc_c::errCode_t::ERROR_OPEN_REG, e.what());
	}
	if (ofs.is_open()) {
		ofs << static_cast<int>(isMaster);
		ofs.close();
	} else {
		THROW_EXC(hwExc_c, hwExc_c::errCode_t::ERROR_OPEN_REG);
	}
}

void hw_c::Reset()
{
	std::ofstream ofs;

	ofs.exceptions(std::ios_base::failbit);

	try {
		ofs.open("/sys/mfhss-dynamic/m/rst");
	} catch (const std::ofstream::failure& e) {
		THROW_EXC_MSG(hwExc_c, hwExc_c::errCode_t::ERROR_OPEN_REG, e.what());
	}
	if (ofs.is_open()) {
		ofs << 1;
		ofs.flush();
		ofs << 0;
		ofs.close();
	} else {
		THROW_EXC(hwExc_c, hwExc_c::errCode_t::ERROR_OPEN_REG);
	}
}
