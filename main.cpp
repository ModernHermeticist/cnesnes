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
		uint8_t p = 0x0;
		uint8_t a = 0x0;
		uint8_t x = 0x0;
		uint8_t y = 0x0;
		uint8_t sp = 0x0;
		std::string logLine;
		std::string logP;
		std::string logA;
		std::string logX;
		std::string logY;
		std::string logSP;
		int counter = 0;
		while(true)
		{
			std::getline(logFile, logLine);
			logP = logLine.substr(65, 2);
			logA = logLine.substr(50, 2);
			logX = logLine.substr(55, 2);
			logY = logLine.substr(60, 2);
			logSP = logLine.substr(71, 2);
			logLine = logLine.substr(0, 4);
			cpu->mergePRegister();
			oldPC = cpu->getPC();
			p = cpu->getP();
			a = cpu->getA();
			y = cpu->getY();
			x = cpu->getX();
			sp = cpu->getSP();
			printf("Counter: %d\n", counter);
			std::cout << "logLine: " << logLine << std::endl;
			std::cout << "PC:      " << std::hex << cpu->getPC() << std::endl;
			std::cout << "logP: 0x" << logP << std::endl;
			printf("P:    %#x\n", p);
			std::cout << "logA: 0x" << logA << std::endl;
			printf("A:    %#x\n", a);
			std::cout << "logX: 0x" << logX << std::endl;
			printf("X:    %#x\n", x);
			std::cout << "logY: 0x" << logY << std::endl;
			printf("Y:    %#x\n", y);
			std::cout << "logSP: 0x" << logSP << std::endl;
			printf("SP:    %#x\n", sp);
			cpu->printStatus();
			if (std::stoi(logLine, nullptr, 16) != oldPC)
			{
				printf("PC out of sync!\n");
				system("pause");
			}
			if (std::stoi(logP, nullptr, 16) != p) 
			{
				printf("P out of sync!\n");
				system("pause");
			}
			if (std::stoi(logA, nullptr, 16) != a)
			{
				{
					printf("A out of sync!\n");
					system("pause");
				}
			}
			if (std::stoi(logX, nullptr, 16) != x)
			{
				{
					printf("X out of sync!\n");
					system("pause");
				}
			}
			if (std::stoi(logY, nullptr, 16) != y)
			{
				{
					printf("Y out of sync!\n");
					system("pause");
				}
			}
			if (std::stoi(logSP, nullptr, 16) != sp)
			{
				{
					printf("SP out of sync!\n");
					system("pause");
				}
			}
			//system("pause");
			cpu->determineOpCode();
			printf("\n");
			//if (cpu->getPC() == oldPC) break;
			//sleep_for(500ms);
			counter += 1;
		}
	}

	system("pause");

	romFile.close();
	logFile.close();

	return 0;
}
