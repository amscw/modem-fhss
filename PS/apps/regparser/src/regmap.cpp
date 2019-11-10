#include "regmap.h"
#include <iomanip>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "mfhssdrv_ioctl.h"

std::string regmapExc_c::strErrorMessages[] = {
		"can't open file",
		"parse error",
		"regmap is empty"
};

std::string regcreatorExc_c::strErrorMessages[] = {
		"can't open device",
		"make group error",
		"make register error"
};

/**
 * экранирование выполнять двойным слешем "\\"
 */
regmap_c::regmap_c(const std::string &filename) throw (regmapExc_c) : m_in(filename, std::ifstream::binary),
	m_rxComment("[ \t]*--[^\n]*(\n|$)"),
	m_rxValidator("^[ \t]*constant[ \t]+ADDR_REG_[A-Z0-9_]+[ \t]*:[ \t]*integer[ \t]*:=[ \t]*[0-9]+[\r\n\t\v]*$"),
	m_rxRegName("ADDR_REG_[A-Z0-9_]+"),
	m_rxRegNamePrefix("ADDR_REG_")
{
	if (!m_in.good())
		throw regmapExc_c(regmapExc_c::errCode_t::ERROR_OPENFILE, __FILE__, __FUNCTION__, filename);
}

regmap_c::~regmap_c() noexcept
{
	m_in.close();
}

size_t regmap_c::Parse() throw (regmapExc_c)
{
	std::ostringstream ossBlock, oss;
	std::string strBlock, str, strRegName/*, strRegAddr*/;
	std::smatch match;
	std::uint32_t nRegAddr;

	m_in.seekg(0);
	while (m_in.good())
	{
		// считываем очередной VHDL-оператор
		m_in.get(*ossBlock.rdbuf(), DELIMETER_TOKEN);
		strBlock = std::move(ossBlock.str());

		// удалить комментарии, если имеются
		str.clear();
		std::regex_replace(std::back_inserter(str), strBlock.begin(), strBlock.end(), m_rxComment, "");

		// удалить символы возврата каретки и переноса строки
		strBlock.clear();
		std::regex_replace(std::back_inserter(strBlock), str.begin(), str.end(), std::regex("(\r|\n)"), "");

		if (strBlock.length() > 0)
		{
			// std::cout << strBlock << std::endl;

			// валидация строки описания регистра
			if (std::regex_match(strBlock.begin(), strBlock.end(), m_rxValidator))
			{
				// ищем имя регистра
				if (std::regex_search(strBlock, match, m_rxRegName))
				{
					// удалить префикс "ADDR_REG_" и перевести в нижний регистр
					str = match.str();
					strRegName.clear();
					std::regex_replace(std::back_inserter(strRegName), str.begin(), str.end(), m_rxRegNamePrefix, "");
					std::transform(strRegName.begin(), strRegName.end(), strRegName.begin(), ::tolower);
				} else {
					throw (regmapExc_c(regmapExc_c::errCode_t::ERROR_PARSE, __FILE__, __FUNCTION__, strBlock));
				}

				// ищем адрес регистра
				if (std::regex_search(strBlock, match, std::regex("[0-9]+")))
				{
					str = match.str();
					nRegAddr = std::stoul(str, nullptr, 10) * 4;
				} else {
					throw (regmapExc_c(regmapExc_c::errCode_t::ERROR_PARSE, __FILE__, __FUNCTION__, strBlock));
				}

				// записываем в контейнер
				regmap.emplace_back(regentry_t(strRegName, nRegAddr));

				// oss << "register: \"" << strRegName << "@0x" << std::hex << nRegAddr;
				// TRACE(oss);
			} else {
				oss << "register not valid:(\"" <<  strBlock << "\")";
				TRACE(oss);
			}
		} // if (str.length() > 0)
		// очистить буфер
		ossBlock.str("");
		ossBlock.clear();

		// сдвиг позицици на +1
		m_in.seekg(1, std::ios_base::cur);
	}
	return regmap.size();
}

void regmap_c::ToStdout() throw (regmapExc_c)
{
	if (regmap.size() > 0)
	{
		std::cout << std::hex;
		for (regentry_t reg : regmap)
			std::cout << reg.first << " @0x" << reg.second << ";";
		std::cout << std::endl;
	} else {
		throw regmapExc_c(regmapExc_c::errCode_t::ERROR_EMPTY_REGMAP, __FILE__, __FUNCTION__);
	}
}


std::size_t regcreator_c::DoEntries(const regmap_c::regmap_t &regmap) noexcept
{
	std::ostringstream oss;
	std::string regName, nodeName;
	std::string::size_type pos;
	It it;

	for (auto reg : regmap)
	{
		if ((pos = reg.first.find('_')) != std::string::npos && pos != 0)
		{
			nodeName = reg.first.substr(0, pos);
			regName = reg.first.substr(pos + 1);
		} else {
			nodeName = "common";
			regName = reg.first;
		}
//		std::cout << nodeName << ":" << regName << std::endl;


		if ( (it = std::find_if<It, IsExist>(entries.begin(), entries.end(), IsExist(nodeName))) != entries.end() )
		{
			// узел уже существует, добавляем регистр
			(*it).regs.emplace_back(regName, reg.second);
		} else if (entries.size() == 0) {
			// ещё нет ни одного узла, создаем
			oss << "find new node: " << nodeName;
			TRACE(oss);
			entries.emplace_back(nodeName, regentry_c::reg_t(regName, reg.second));
		} else {
			// найден новый узел, добавляем в список
			oss << "find new node: " << nodeName;
			TRACE(oss);
			entries.emplace_back(nodeName, regentry_c::reg_t(regName, reg.second));
		}
		oss << "register \"" << std::setw(16) << regName << "\" added to node \"" << std::setw(16) << nodeName << "\"";
		TRACE(oss);
	}

	oss << "total nodes: " << entries.size();
	TRACE(oss);
	return entries.size();
}

void regcreator_c::MakeDeviceRegs(const std::string &deviceName) throw (regcreatorExc_c)
{
	int res;

	fd = open(deviceName.c_str(), O_RDWR);
	if (fd == -1)
		throw regcreatorExc_c(regcreatorExc_c::errCode_t::ERROR_OPENDEVICE, __FILE__, __FUNCTION__, deviceName);

	for (auto e : entries)
	{
		MFHSS_GROUP_TypeDef groupDescr = {0};

		e.nodeName.copy(groupDescr.nodeName, e.nodeName.size());
		res = ioctl(fd, MFHSSDRV_IOMAKEGROUP, &groupDescr);
		if (res != 0)
		{
			close(fd);
			throw regcreatorExc_c(regcreatorExc_c::errCode_t::ERROR_MAKEGROUP, __FILE__, __FUNCTION__, e.nodeName);
		}
		for (auto r : e.regs)
		{
			MFHSS_REG_TypeDef regDescr = {0};
			e.nodeName.copy(regDescr.targetNode, e.nodeName.size());
			r.first.copy(regDescr.regName, r.first.size());
			regDescr.address = r.second;
			res = ioctl(fd, MFHSSDRV_IOMAKEREG, &regDescr);
			if (res != 0)
			{
				close(fd);
				throw regcreatorExc_c(regcreatorExc_c::errCode_t::ERROR_MAKEREG, __FILE__, __FUNCTION__, r.first);
			}
		}
	}

	close(fd);
}
