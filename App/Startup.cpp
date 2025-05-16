#include "Startup.h"

#include "stm32f1xx_api.h"
#include "AppVersion.h"
#include "Modules/Process/MainThread.hpp"

int doMain()
{
	initializeAPI();
	logString("Start GameConsole. Version: %s", Core::Export::VERSION);

	MainThread();
	logString("Finish GameConsole");
	return 0;
}
