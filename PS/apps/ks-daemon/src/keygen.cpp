/*
 * keygen.cpp
 *
 *  Created on: 22 июн. 2020 г.
 *      Author: moskvin
 */

#include "keygen.hpp"
#include <random>
#include <string.h>
#include <iostream>

std::string keyGenExc_c::strErrorMessages[] = {
		"can't open file",
		"can't parse key",
		"can't found key",
		"can't write to HW"
};

//-----------------------------------------------------------------------------
// ADDR_REG_CI_KEY_
//-----------------------------------------------------------------------------
CIKey::CIKey() : Keygen("ci/key_") {}

void CIKey::Generate() noexcept
{
	std::uniform_int_distribution<std::uint32_t> dL(0, 0xFFFFFFFF);
	std::uniform_int_distribution<std::uint32_t> dH(0, 0x00FFFFFF);
	std::random_device rd;
	key.words.L = dL(rd);
	key.words.H = dH(rd);
}

void CIKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<std::uint32_t[2]>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void CIKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<std::uint32_t[2]>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void CIKey::Print() noexcept
{
	Keygen<std::uint32_t[2]>::Print();
}

void CIKey::WriteToHW()
{
	Keygen<std::uint32_t[2]>::WriteToHW();	
}
//-----------------------------------------------------------------------------
// ADDR_REG_SAP_KEY_SAP
//-----------------------------------------------------------------------------
SAPKey::SAPKey() : Keygen("sap/key_sap") {}

void SAPKey::Generate() noexcept
{
	std::uniform_int_distribution<key_t> d(0, 0xFFFF);
	std::random_device rd;
	key = d(rd);
}

void SAPKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<key_t>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void SAPKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<key_t>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void SAPKey::Print() noexcept
{
	Keygen<key_t>::Print();
}

void SAPKey::WriteToHW()
{
	Keygen<key_t>::WriteToHW();	
}

//-----------------------------------------------------------------------------
// ADDR_REG_SAP_KEY_INTR
//-----------------------------------------------------------------------------
SAPIntrKey::SAPIntrKey() : Keygen("sap/key_intr") {}

/**
 * ключ для перемежителя уровня защиты информации
 * по 4 бита на индекс, всего 8 индексов без повторения
 * (пример - 0x12345678)
 */
// void SAPIntrKey::Generate() noexcept
// {
// 	std::uniform_int_distribution<std::uint32_t> d(0, 0xF);
// 	std::random_device rd;
// 	std::uint8_t indexes[8], next_index;

// 	memset(indexes, 0, sizeof indexes);

// 	for (int i = 0; i < 8; )
// 	{
// 		bool isExist = false;

// 		next_index = d(rd);
// 		for (int j = 0; j < i; j++)
// 			if (next_index == indexes[j])
// 			{
// 				isExist = true;
// 				break;
// 			}
// 		if (isExist)
// 			continue;

// 		key |= (next_index << (i*4));
// 		indexes[i] = next_index;
// 		i++;
// 	}
// }

/**
 * Ключ для перемежителя уровня защиты информации по 4 бита на индекс
 * (индекс от 0 до 7), всего 8 индексов без повторения
 * (пример - 0x01234567)
 */
void SAPIntrKey::Generate() noexcept
{
	using distr_t = std::uniform_int_distribution<std::uint32_t>;
	std::random_device rd;
	std::uint8_t items[] = {0, 1, 2, 3, 4, 5, 6, 7};

	// перетасовка Фишера-Йетса
	for (int i = sizeof items - 1; i > 0 ; i--)
	{
		distr_t d(0, i);
		std::uint32_t j = d(rd);
		std::swap(items[j], items[i]);
	}

	// записать перетасованные индексы в ключ
	for (int i = 0; i < sizeof items; i++)
		key |= (items[i] << (i*4));
	
}

void SAPIntrKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<key_t>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void SAPIntrKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<key_t>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void SAPIntrKey::Print() noexcept
{
	Keygen<key_t>::Print();
}

void SAPIntrKey::WriteToHW()
{
	Keygen<key_t>::WriteToHW();	
}

//-----------------------------------------------------------------------------
// ADDR_REG_DLINK_CODER_KEY
//-----------------------------------------------------------------------------
DLinkCoderKey::DLinkCoderKey() : Keygen ("dlink/coder_key") {}

void DLinkCoderKey::Generate() noexcept
{
	std::uniform_int_distribution<key_t> d(0, 0xFFF);
	std::random_device rd;
	key = d(rd);
}

void DLinkCoderKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<key_t>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void DLinkCoderKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<key_t>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void DLinkCoderKey::Print() noexcept
{
	Keygen<key_t>::Print();
}

void DLinkCoderKey::WriteToHW()
{
	Keygen<key_t>::WriteToHW();	
}

//-----------------------------------------------------------------------------
// ADDR_REG_HOP_SEED
//-----------------------------------------------------------------------------
HopSeedKey::HopSeedKey() : Keygen("hop/seed") {}

void HopSeedKey::Generate() noexcept
{
	std::uniform_int_distribution<key_t> d(0, 0xFFFFFFFF);
	std::random_device rd;
	key = d(rd);
}

void HopSeedKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<key_t>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void HopSeedKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<key_t>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void HopSeedKey::Print() noexcept
{
	Keygen<key_t>::Print();
}

void HopSeedKey::WriteToHW()
{
	Keygen<key_t>::WriteToHW();	
}

//-----------------------------------------------------------------------------
// ADDR_REG_DLINK_DATA_PREAMPLE_
//-----------------------------------------------------------------------------
DLinkDataPreampbleKey::DLinkDataPreampbleKey() : Keygen("dlink/data_preample_") {}

void DLinkDataPreampbleKey::Generate() noexcept
{
	std::uniform_int_distribution<int> d(0, (sizeof table / sizeof *table) - 1);
	std::random_device rd;
	int index = d(rd);

	// снять константность и сохранить без изменения представления
	key.words.L = *(reinterpret_cast<std::uint32_t*>(const_cast<std::int32_t*>(&table[index][1])));
	key.words.H = *(reinterpret_cast<std::uint32_t*>(const_cast<std::int32_t*>(&table[index][0])));
}

void DLinkDataPreampbleKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<std::uint32_t[2]>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void DLinkDataPreampbleKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<std::uint32_t[2]>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void DLinkDataPreampbleKey::Print() noexcept
{
	Keygen<std::uint32_t[2]>::Print();
}

void DLinkDataPreampbleKey::WriteToHW()
{
	Keygen<std::uint32_t[2]>::WriteToHW();	
}

//-----------------------------------------------------------------------------
// ADDR_REG_DLINK_KEY_COMMON
//-----------------------------------------------------------------------------
DLinkCommonKey::DLinkCommonKey() : Keygen ("dlink/key_common") {}

void DLinkCommonKey::Generate() noexcept
{
	std::uniform_int_distribution<key_t> d(0, 0xFFF);
	std::random_device rd;
	key = d(rd);
}

void DLinkCommonKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<key_t>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void DLinkCommonKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<key_t>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void DLinkCommonKey::Print() noexcept
{
	Keygen<key_t>::Print();
}

void DLinkCommonKey::WriteToHW()
{
	Keygen<key_t>::WriteToHW();	
}

//-----------------------------------------------------------------------------
// ADDR_REG_PHY_KEY_COMMON
//-----------------------------------------------------------------------------
PhyCommonKey::PhyCommonKey() : Keygen ("phy/key_common") {}

void PhyCommonKey::Generate() noexcept
{
	std::uniform_int_distribution<key_t> d(0, 0x7FF);
	std::random_device rd;
	key = d(rd);
}

void PhyCommonKey::WriteTo(const std::string &filename)
{
	try {
		Keygen<key_t>::WriteTo(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void PhyCommonKey::ReadFrom(const std::string &filename)
{
	try {
		Keygen<key_t>::ReadFrom(filename);
	} catch (exc_c &exc) {
		throw ;
	}
}

void PhyCommonKey::Print() noexcept
{
	Keygen<key_t>::Print();
}

void PhyCommonKey::WriteToHW()
{
	Keygen<key_t>::WriteToHW();	
}
