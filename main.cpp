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
		cpu->printStatus();
	}

	if (debugging)
	{
		cpu->printMemory(cpu->CARTRIDGE_MEMORY_START, cpu->CARTRIDGE_MEMORY_START+0xA);
	}

	system("pause");

	return 0;
}
