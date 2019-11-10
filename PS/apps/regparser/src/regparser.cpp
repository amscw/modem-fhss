//============================================================================
// Name        : regparser.cpp
// Author      : moskvin
// Version     :
// Copyright   : elins
// Description : register map parser in C++, Ansi-style
//============================================================================

#include <memory>
#include "tracers.h"
#include "regmap.h"

int main(int argc, char **argv) {
	std::ostringstream oss;

	// std::cout << "hello, fucking world!" << std::endl; // prints hello, fucking world!
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
		return 0;
	}

	std::unique_ptr<regmap_c> regmap = nullptr;
	std::unique_ptr<regcreator_c> regcreator = nullptr;

	try
	{
		// классический парсинг axi_vhd.pkg
		regmap = std::make_unique<regmap_c>(argv[1]);
		oss << "total: " << regmap->Parse() << " registers found";
		TRACE(oss);
		// regmap->ToStdout();

		oss << "\"" << argv[1] << "\"" << " successfully parsed";
		TRACE(oss);

		// группировка регистров по узлам
		regcreator = std::make_unique<regcreator_c>();
		if (regcreator->DoEntries(regmap->GetRegs()) > 0)
		{
			regcreator->MakeDeviceRegs("/dev/mfhss0");
		}

	} catch (regmapExc_c &exc) {
		exc.ToStderr();
		return -1;
	} catch (regcreatorExc_c &exc) {
		exc.ToStderr();
		return -2;
	}

	oss << "device registers created successfully";
	TRACE(oss);
	return 0;
}
