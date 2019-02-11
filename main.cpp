#include "main.h"

bool debugging = true;


int main(int argc, char *argv[])
{
	std::string romName = argv[1];
	std::fstream romFile;
	std::fstream logFile;
	if (debugging)
	{
		printf(romName.c_str());
		printf("\n");
	}

	if (debugging)
	{
		logFile.open("nestest_log.txt");
		if (!logFile.is_open())
		{
			std::cout << "Log File failed to open! Exiting.\n";
			return 1;
		}
		std::cout << "Log File opened!\n";
	}

	Cartridge *cart = new Cartridge(romName);

	if (!cart->loadRomFile())
	{
		system("pause");
		return 1;
	}

	cart->setNumberOfBytes();

	if (debugging)
	{
		std::cout << "ROM Size: " << cart->getNumberOfBytes() << std::endl;
	}

	CPU *cpu = new CPU();

	cpu->loadCartridgeToMemory(cart);

	if (debugging)
	{
		uint16_t oldPC = 0x0;
		std::string logLine;
		while(true)
		{
			std::getline(logFile, logLine);
			logLine = logLine.substr(0, 4);
			std::cout << "logLine: " << logLine << std::endl;
			std::cout << "PC:      " << std::hex << cpu->getPC() << std::endl;
			oldPC = cpu->getPC();
			if (std::stoi(logLine, nullptr, 16) != oldPC) system("pause");
			cpu->printStatus();
			//system("pause");
			cpu->determineOpCode();
			printf("\n");
			if (cpu->getPC() == oldPC) break;
		}
	}

	system("pause");

	romFile.close();
	logFile.close();

	return 0;
}
