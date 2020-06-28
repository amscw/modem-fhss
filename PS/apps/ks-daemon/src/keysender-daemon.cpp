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
			/*
			 * можно не беспокоиться о том, что дочерний процесс перетрет файл с ключами быстрее, чем его прочитает родительский
			 * так как дочерний перед перезаписью ключей отправляет файл по ssh, а это достаточно длительная процедура,
			 * при прочих равных родительский всегда будет первым получать доступ к файлу с ключами
			 * одновременно с этим стоит учитывать, что процесс установки ключей при включении должен быть именно здесь,
			 * чтобы все ключи гарантированно были установлены перед сбросом модема (перед исполнением shell-скрипта)
			 */
			daemonTool->InstallKeys();
			oss << "daemon running. Keys accepted. Exit";
			TRACE(oss);
		}		
	} catch (exc_c &exc) {
		// fail to run daemon
		exc.ToStderr();
	} 		

	return 0;
}
