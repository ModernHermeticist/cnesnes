#include "main.h"

bool debugging = true;


int main(int argc, char *argv[])
{
	std::string romName = argv[1];
	std::fstream romFile;
	if (debugging)
	{
		printf(romName.c_str());
		printf("\n");
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
		for (int i = 0; i < 100; i++)
		{
			cpu->printStatus();
			system("pause");
			cpu->determineOpCode();
			printf("\n");
		}
	}

	system("pause");

	return 0;
}
