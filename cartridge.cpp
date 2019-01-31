#pragma once
#include "main.h"

Cartridge::Cartridge(std::string romName)
{
	name = romName;
}

Cartridge::~Cartridge()
{

}

bool Cartridge::loadRomFile()
{
	file.open(name, std::ios::binary);

	if (!file.is_open())
	{
		std::cout << "ROM File failed to open! Exiting.\n";
		return false;
	}
	std::cout << "ROM File opened!\n";
	return true;
}

void Cartridge::setNumberOfBytes()
{
	file.seekg(0, std::ios::end);
	size = (int)file.tellg();
	file.seekg(0);
}

int Cartridge::getNumberOfBytes()
{
	return size;
}
