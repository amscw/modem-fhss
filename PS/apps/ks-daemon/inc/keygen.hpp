/*
 * keygen.hpp
 *
 *  Created on: 22 июн. 2020 г.
 *      Author: moskvin
 */

#ifndef KEYGEN_HPP_
#define KEYGEN_HPP_

#include <cstdint>
#include <type_traits>
#include <string>
#include <fstream>
#include <regex>
#include "exc.h"

struct keyGenExc_c : public exc_c
{
	enum class errCode_t : std::uint32_t {
		ERROR_OPEN_FILE,
		ERROR_PARSE_KEY,
		ERROR_NOTFOUND_KEY,
	} m_errCode;

	keyGenExc_c(enum errCode_t code, const std::string &strFile, const std::string &strFunction, const std::string &strErrorDescription = "") noexcept :
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

	private:
		static std::string strErrorMessages[];
		std::ostringstream oss;
};

/**
 * Базовый класс для полиморфного использования ключей
 */
class Keygen_Basic
{
public:
	virtual void Generate() noexcept = 0;
	virtual void WriteTo(const std::string &filename) = 0;
	virtual void ReadFrom(const std::string &filename) = 0;
	virtual void Print() noexcept = 0;
	// virtual void WriteToHW();
	virtual ~Keygen_Basic(){}
};

/**
 * Для целых чисел только
 */
template<typename T> class Keygen
{
public:
	typedef typename std::enable_if<std::is_integral<T>::value, T>::type key_t;
	inline const key_t& GetKey() { return key; }

	Keygen(const std::string &s) : regname(s) {}

	void WriteTo(const std::string &filename)
	{
		std::ofstream ofs;

		ofs.exceptions(std::ios_base::failbit);
		try
		{
			ofs.open(filename, std::ios_base::out | std::ios_base::app);
		} catch (const std::ofstream::failure& e) {
			THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE, e.what());
		}
		if (ofs.is_open()) {
			ofs << regname << ":" << std::hex << key << std::endl;
			ofs.close();
		} else {
			THROW_EXC(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE);
		}
	}

	void ReadFrom(const std::string &filename)
	{
		std::ifstream ifs;
		std::string line;
		std::regex rx("^" + regname + ".*");
		bool isFound = false;

		// ifs.exceptions(std::ios_base::failbit);
		try
		{
			ifs.open(filename);
		} catch (const std::ifstream::failure &e) {
			THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE, e.what());
		}
		if (ifs.is_open()) {
			while(!ifs.eof() && !isFound)
			{
				ifs >> line;
				if (std::regex_match(line.begin(), line.end(), rx))
				{
					std::smatch m;
					if (std::regex_search(line, m, std::regex(":[a-f0-9]+")))
					{
						std::string s = m.str();
						std::stringstream ss;;

						// удалить ':'
						s.erase(std::remove(s.begin(), s.end(), ':'), s.end());
						ss << std::hex << s;
						ss >> key;
						isFound = true;
						// std::cout << "found the key: 0x" << std::hex << key << std::endl;
					} else {
						ifs.close();
						THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_PARSE_KEY, line);
					}
				}
			}
			ifs.close();
			if (isFound == false)
				THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_NOTFOUND_KEY, regname);
		} else { // ifs.is_open()
			THROW_EXC(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE);
		}
	}

	void Print() noexcept
	{
		std::cout << regname << ":" << std::hex << key << std::endl;
		std::cout << std::dec;
	}

protected:
	const std::string regname;
	key_t key { 0 };
};

/**
 * Специализация для массива беззнаковых целых
 */
template<> class Keygen<std::uint32_t[2]>
{
public:
	Keygen(const std::string &s) : regname(s) {}

	typedef union
	{
		struct {
			std::uint32_t L;
			std::uint32_t H;
		} words;
		std::uint64_t value;
	} key_t;
	const key_t& GetKey() { return key; }

	void WriteTo(const std::string &filename)
	{
		std::ofstream ofs;
		ofs.exceptions(std::ios_base::failbit);
		try
		{
			ofs.open(filename, std::ios_base::out | std::ios_base::app);
		} catch (const std::ofstream::failure& e) {
			THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE, e.what());
		}
		if (ofs.is_open()) {
			ofs << regname << "lsb:" << std::hex << key.words.L << std::endl;
			ofs << regname << "msb:" << std::hex << key.words.H << std::endl;
			ofs.close();
		} else {
			THROW_EXC(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE);
		}
	}

	void ReadFrom(const std::string &filename)
	{
		std::ifstream ifs;
		std::string line;
		std::regex rx_lsb("^" + regname + "lsb.*");
		std::regex rx_msb("^" + regname + "msb.*");
		bool isLSBFound = false, isMSBFound = false;

		// ifs.exceptions(std::ios_base::failbit);
		try
		{
			ifs.open(filename);
		} catch (const std::ifstream::failure &e) {
			THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE, e.what());
		}
		if (ifs.is_open()) {
			while(!ifs.eof() && !(isLSBFound & isMSBFound))
			{
				ifs >> line;
				if (std::regex_match(line.begin(), line.end(), rx_lsb))
				{
					// lsb-part found
					std::smatch m;
					if (std::regex_search(line, m, std::regex(":[a-f0-9]+")))
					{
						std::string s = m.str();
						std::stringstream ss;;

						// удалить ':'
						s.erase(std::remove(s.begin(), s.end(), ':'), s.end());
						ss << std::hex << s;
						ss >> key.words.L;
						isLSBFound = true;
						// std::cout << "found the LSB of key: 0x" << std::hex << key.words.L << std::endl;
					} else {
						ifs.close();
						THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_PARSE_KEY, line);
					}
				} else if (std::regex_match(line.begin(), line.end(), rx_msb)) {
					// msb part found
					std::smatch m;
					if (std::regex_search(line, m, std::regex(":[a-f0-9]+")))
					{
						std::string s = m.str();
						std::stringstream ss;;

						// удалить ':'
						s.erase(std::remove(s.begin(), s.end(), ':'), s.end());
						ss << std::hex << s;
						ss >> key.words.H;
						isMSBFound = true;
						// std::cout << "found the MSB of key: 0x" << std::hex << key.words.H << std::endl;
					} else {
						ifs.close();
						THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_PARSE_KEY, line);
					}

				}
			}
			ifs.close();
			if (!isLSBFound || !isMSBFound)
				THROW_EXC_MSG(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_NOTFOUND_KEY, regname);
		} else { // ifs.is_open()
			THROW_EXC(keyGenExc_c, keyGenExc_c::errCode_t::ERROR_OPEN_FILE);
		}
	}

	void Print() noexcept
	{
		std::cout << regname << "lsb:" << std::hex << key.words.L << std::endl;
		std::cout << regname << "msb:" << std::hex << key.words.H << std::endl;
		std::cout << std::dec;
	}

protected:
	const std::string regname;
	key_t key;
};

/**
 * Классы ключей-наследников, реализующих фабричный метод
 */
class CIKey : Keygen<std::uint32_t[2]>, public Keygen_Basic
{
public:
	CIKey();
	void Generate() noexcept override;
	void WriteTo(const std::string &filename) override;
	void ReadFrom(const std::string &filename) override;
	void Print() noexcept override;
};

class SAPKey : Keygen<std::uint16_t>, public Keygen_Basic
{
public:
	SAPKey();
	void Generate() noexcept override;
	void WriteTo(const std::string &filename) override;
	void ReadFrom(const std::string &filename) override;
	void Print() noexcept override;
};

class SAPIntrKey : Keygen<std::uint32_t>, public Keygen_Basic
{
public:
	SAPIntrKey();
	void Generate() noexcept override;
	void WriteTo(const std::string &filename) override;
	void ReadFrom(const std::string &filename) override;
	void Print() noexcept override;
};

class DLinkCoderKey : Keygen<std::uint16_t>, public Keygen_Basic
{
public:
	DLinkCoderKey();
	void Generate() noexcept override;
	void WriteTo(const std::string &filename) override;
	void ReadFrom(const std::string &filename) override;
	void Print() noexcept override;
};

class HopSeedKey : Keygen<std::uint32_t>, public Keygen_Basic
{
public:
	HopSeedKey();
	void Generate() noexcept override;
	void WriteTo(const std::string &filename) override;
	void ReadFrom(const std::string &filename) override;
	void Print() noexcept override;
};

class DLinkDataPreampbleKey : Keygen<std::uint32_t[2]>, public Keygen_Basic
{
	const std::int32_t table [36][2] = {
			{-2089607810, -787797425},
			{-1649176374, -1436468809},
			{-55281145, 1882134175},
			{2024526128, 420981308},
			{1758821869, -1107994448},
			{-80556058, 763493855},
			{280896504, 1729883312},
			{456168298, -746860366},
			{-1754528183, -334319047},
			{-86045016, -1739853055},
			{-1613690280, -109759607},
			{-211495878, 1587350465},
			{-1964094566, -1078038929},
			{981195141, -1568492574},
			{1708104527, 826477164},
			{-1787401617, 878084339},
			{-1663453691, -155730669},
			{271951951, 1822872260},
			{-1632157535, 1191314211},
			{-234709309, -521520277},
			{1087760201, 2135611976},
			{-1954021021, 29974213},
			{-319781938, -1207571157},
			{1716849113, -1999152892},
			{-1629961094, -1413192849},
			{1085957010, -1819493884},
			{-1947761224, -317768055},
			{-212278891, 1416658075},
			{1099929631, -875520584},
			{-1965916619, -1252845259},
			{-307561320, 211790555},
			{977792042, -1469070152},
			{1710468832, 1001291062},
			{-367653158, 802566237},
			{447527263, -93708560},
			{-1726497363, -200428697},
	};
public:
	DLinkDataPreampbleKey();
	void Generate() noexcept override;
	void WriteTo(const std::string &filename) override;
	void ReadFrom(const std::string &filename) override;
	void Print() noexcept override;
};
#endif /* KEYGEN_HPP_ */
