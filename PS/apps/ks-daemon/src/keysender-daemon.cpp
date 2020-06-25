//============================================================================
// Name        : keysender-daemon.cpp
// Author      : amscw
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "tracers.h"
#include "daemon-tool.h"

static std::unique_ptr<daemonTool_c>(daemonTool);

int main(int argc, char *argv[]) {
	std::ostringstream oss;

	if (argc != 2)
	{
		oss << "Usage: " << argv[0] << " <filename>.yaml";
		TRACE(oss);
		return -1;
	}

	try
	{
		/**
		 * Дочерний процесс занимается генерацией и обновлением новых ключей
		 * Родительский процесс устанавливает старые ключи
		 */
		std::string cfgFilename = std::string(const_cast<const char*>(argv[1]));
		daemonTool = std::make_unique<daemonTool_c>(cfgFilename);
		if (daemonTool->IsChild())
		{
			daemonTool->Run();
		} else {
			oss << "daemon running. Load config...";
			TRACE(oss);
			
			// load configuration (in parent)
			daemonTool->LoadConfigsFromFile(std::string(const_cast<const char*>(argv[1])));
			oss << "configuration successfully loaded from \"" << cfgFilename << "\". Setup keys...";
			TRACE(oss);

			// installing the keys
			typedef std::unique_ptr<Keygen_Basic> item_type;
			std::vector<item_type> v;
			if (daemonTool->ModemType().compare("rtk_u") == 0)
			{
				// РТК-У
				v.emplace_back(std::make_unique<CIKey>());
				v.emplace_back(std::make_unique<SAPKey>());
				v.emplace_back(std::make_unique<SAPIntrKey>());
				v.emplace_back(std::make_unique<DLinkCoderKey>());
				v.emplace_back(std::make_unique<HopSeedKey>());
				v.emplace_back(std::make_unique<DLinkDataPreampbleKey>());
			} else if (daemonTool->ModemType().compare("rtk_v") == 0) {
				// РТК-В
				v.emplace_back(std::make_unique<CIKey>());
				v.emplace_back(std::make_unique<SAPKey>());
				v.emplace_back(std::make_unique<SAPIntrKey>());
				v.emplace_back(std::make_unique<DLinkCommonKey>());
				v.emplace_back(std::make_unique<PhyCommonKey>());
			} else {
				oss << "bad modem type (" << daemonTool->ModemType() << "). Exit";
				TRACE(oss);
				exit(-1);
			}
			for (std::vector<item_type>::iterator it = v.begin(); it != v.end(); it++)
			{
				(*it)->ReadFrom(daemonTool->Keydir() + "keys");
				(*it)->Print();
				(*it)->WriteToHW();
			}
			v.clear();
			oss << "Done! Exit";
			TRACE(oss);
		}
	} catch (exc_c &exc) {
		// fail to run daemon
		exc.ToStderr();
	} catch (YAML::ParserException &exc) {
		oss << "YAML::ParserException: " << exc.msg;
		TRACE(oss);
		exit(-1);
	}		

	return 0;
}
