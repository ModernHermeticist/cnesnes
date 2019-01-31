#pragma once
#ifndef CARTRIDGE_H
#define CARTRIDGE_H

class Cartridge
{
public:
	Cartridge(std::string romName);
	~Cartridge();

	bool loadRomFile();
	void setNumberOfBytes();
	int getNumberOfBytes();
	std::ifstream file;
private:
	std::string name;
	int size;
};


#endif