#ifndef REGMAP_H
#define REGMAP_H

#include <fstream>
#include <vector>
#include <regex>
#include <list>
#include "tracers.h"
#include "exc.h"

struct regmapExc_c : public exc_c
{
	enum class errCode_t : std::uint32_t {
		ERROR_OPENFILE,
		ERROR_PARSE,
		ERROR_EMPTY_REGMAP
	} m_errCode;

	regmapExc_c(enum errCode_t code, const std::string &strFile, const std::string &strFunction, const std::string &strErrorDescription = "") noexcept :
			exc_c(strFile, strFunction, strErrorDescription), m_errCode(code)
	{}

	const std::string &Msg() const noexcept override { return strErrorMessages[(int)m_errCode]; }
	void ToStderr() const noexcept override
	{
		std::cerr << "WTF:" << m_strFile << "(" << m_strFunction << "):" << strErrorMessages[(int)m_errCode] << "-" << m_strErrorDescription << std::endl;
	}

private:
	static std::string strErrorMessages[];
};

/**
 * Варианты хранения регистровой карты
 * 1 последовательный контейнер - вектор пар строка-адрес
 * 2 ассоциативный контейнер - словарь, индексируемый именем регистра
 */
class regmap_c
{
public:
	using regentry_t = std::pair<std::string, std::uint32_t>;
	using regmap_t = std::vector<regentry_t>;
	using regit_t = regmap_t::iterator;

private:
	const char DELIMETER_TOKEN = ';';

	regmap_t regmap;
	std::ifstream m_in;

	std::regex m_rxComment, m_rxValidator, m_rxRegName, m_rxRegNamePrefix;

public:
	regmap_c (const std::string &filename) throw (regmapExc_c);
	~regmap_c() noexcept;

	size_t Parse() throw (regmapExc_c);
	void ToStdout() throw (regmapExc_c);

	inline const regmap_t &GetRegs() const noexcept { return regmap; }
};

/**
 * Дескриптор группы регистров
 * nodeName - имя каталога
 * regs - список файлов каталога
 */
struct regentry_c
{
	using reg_t = std::pair<std::string, std::uint32_t>;

	std::string nodeName;
	std::list<reg_t> regs;

	regentry_c(){};
	regentry_c(const std::string &name, reg_t firstReg) : nodeName(name)
	{
		regs.emplace_back(firstReg);
	}
};

/**
 * Функтор для определения факта наличия узла в указанном списке
 */
struct IsExist : public std::unary_function<regentry_c, bool>
{
	IsExist(const std::string &nextNodeName) : m_nextNodeName(nextNodeName) {};

	bool operator ()(const regentry_c &regentry) const
	{
		return m_nextNodeName == regentry.nodeName;
	}

private:
	const std::string &m_nextNodeName;
};


struct regcreatorExc_c : public exc_c
{
	enum class errCode_t : std::uint32_t {
		ERROR_OPENDEVICE,
		ERROR_MAKEGROUP,
		ERROR_MAKEREG
	} m_errCode;

	regcreatorExc_c(enum errCode_t code, const std::string &strFile, const std::string &strFunction, const std::string &strErrorDescription = "") noexcept :
			exc_c(strFile, strFunction, strErrorDescription), m_errCode(code)
	{}

	const std::string &Msg() const noexcept override { return strErrorMessages[(int)m_errCode]; }
	void ToStderr() const noexcept override
	{
		std::cerr << "WTF:" << m_strFile << "(" << m_strFunction << "):" << strErrorMessages[(int)m_errCode] << "-" << m_strErrorDescription << std::endl;
	}

private:
	static std::string strErrorMessages[];
};

/**
 * Группирует регистры по узлам (каталогам), указанным в префиксной части имени
 * Далее взаимодействует с драйвером для создания модели устройства
 */
class regcreator_c
{
	using It = std::list<regentry_c>::iterator;

	int fd;
	std::list<regentry_c> entries;

public:
	std::size_t DoEntries(const regmap_c::regmap_t &regmap) noexcept;
	void MakeDeviceRegs(const std::string &deviceName) throw (regcreatorExc_c);
};
#endif // REGMAP_H
