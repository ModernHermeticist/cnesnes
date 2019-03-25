#include "main.h"

bool usingTestFile = true;
bool debugging = true;
bool debuggingTestFile = true;
bool debuggingSDL = true;


int main(int argc, char *argv[])
{

	Window *mainWindow = new Window;
	std::string image = "test.bmp";
	std::string font = "yay.png";
	//int lineCount = 0;
	
	// Event handler
	SDL_Event e;

	if (mainWindow->initSDL() == false) return 1;

	if (mainWindow->loadMedia(image) == false) return 1;

	if (mainWindow->loadTexture(font) == false) return 1;

	mainWindow->drawSplashScreen();

	std::string romName = argv[1];
	std::fstream romFile;
	std::fstream logFile;
	if (debugging)
	{
		printf(romName.c_str());
		printf("\n");
	}

	if (usingTestFile)
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

	if (usingTestFile)
	{
		cpu->setPC(0xC000);
		cpu->setMemoryOffset(0xBFF0);
	}

	cpu->loadCartridgeToMemory(cart);

	if (usingTestFile)
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
		int counter = 1;
		SDL_Delay(2000);
		while(true)
		{
			//SDL_Delay(1000);
			if (usingTestFile)
			{
				std::getline(logFile, logLine);
				if (!logFile)
				{
					printf("End of Cartridge Reached!\n");
					break;
				}
				logP = logLine.substr(65, 2);
				logA = logLine.substr(50, 2);
				logX = logLine.substr(55, 2);
				logY = logLine.substr(60, 2);
				logSP = logLine.substr(71, 2);
				logLine = logLine.substr(0, 4);
			}
			cpu->mergePRegister();
			oldPC = cpu->getPC();
			p = cpu->getP();
			a = cpu->getA();
			y = cpu->getY();
			x = cpu->getX();
			sp = cpu->getSP();
			if (debugging)
			{
				printf("\n");
				printf("Counter: %d\n", counter);
				if (usingTestFile)
				{
					std::cout << "logLine: " << logLine << std::endl;
				}
				std::cout << "PC:      " << std::hex << cpu->getPC() << std::endl;

				if (usingTestFile)
				{
					std::cout << "logP:    0x" << logP << std::endl;
				}
				printf("P:       %#x\n", p);
				printf("_____________\n");
				if (usingTestFile)
				{
					std::cout << "logA:    0x" << logA << std::endl;
				}
				if (a == 0) printf("A:       0x00\n");
				else printf("A:       %#x\n", a);
				printf("_____________\n");
				if (usingTestFile)
				{
					std::cout << "logX:    0x" << logX << std::endl;
				}
				if (x == 0) printf("X:       0x00\n");
				else printf("X:       %#x\n", x);
				printf("_____________\n");
				if (usingTestFile)
				{
					std::cout << "logY:    0x" << logY << std::endl;
				}
				if (y == 0) printf("Y:       0x00\n");
				else printf("Y:       %#x\n", y);
				printf("_____________\n");
				if (usingTestFile)
				{
					std::cout << "logSP:   0x" << logSP << std::endl;
				}
				printf("SP:      %#x\n", sp);
				printf("___________________________________________________________________\n");
				cpu->printStatus();
				if (usingTestFile)
				{
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
				}
			}
			cpu->determineOpCode();
			counter += 1;
		}
	}
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT) quit = true;

			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_UP:
					{
						if (debuggingSDL) printf("\nUp key pressed!\n");
						break;
					}
					case SDLK_DOWN:
					{
						if (debuggingSDL) printf("\nDown key pressed!\n");
						break;
					}
					case SDLK_LEFT:
					{
						if (debuggingSDL) printf("\nLeft key pressed!\n");
						break;
					}
					case SDLK_RIGHT:
					{
						if (debuggingSDL) printf("\nRight key pressed!\n");
						break;
					}
					case SDLK_ESCAPE:
					{
						quit = true;
						break;
					}
					default: break;
				}
			}
		}
		mainWindow->updateTextureDisplay();
	}

	romFile.close();
	logFile.close();

	
	mainWindow->closeSDL();
	delete mainWindow;
	

	return 0;
}
