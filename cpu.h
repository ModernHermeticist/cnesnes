#pragma once
#ifndef CPU_H
#define CPU_H

class CPU
{
public:
	const int CARTRIDGE_MEMORY_START = 0x4020;
	const int MEMORY_END = 0xFFFF;
	const int STACK_POINTER_OFFSET = 0x100;

	CPU();
	~CPU();

	void printStatus();
	void loadCartridgeToMemory(Cartridge *cart);
	void printMemory();
	void printMemory(int start, int end);
	uint16_t getPC();
	uint8_t getP();
	uint8_t getA();
	uint8_t getY();
	uint8_t getX();
	uint8_t getSP();
	void mergePRegister();
	void determineOpCode();
	void leftNib0(uint8_t rightNib);
	void leftNib1(uint8_t rightNib);
	void leftNib2(uint8_t rightNib);
	void leftNib3(uint8_t rightNib);
	void leftNib4(uint8_t rightNib);
	void leftNib5(uint8_t rightNib);
	void leftNib6(uint8_t rightNib);
	void leftNib7(uint8_t rightNib);
	void leftNib8(uint8_t rightNib);
	void leftNib9(uint8_t rightNib);
	void leftNibA(uint8_t rightNib);
	void leftNibB(uint8_t rightNib);
	void leftNibC(uint8_t rightNib);
	void leftNibD(uint8_t rightNib);
	void leftNibE(uint8_t rightNib);
	void leftNibF(uint8_t rightNib);

private:


	uint8_t a;   // byte wide accumulator
	uint8_t x;   // byte wide index
	uint8_t y;   // byte wide index
	uint16_t pc; // two byte wide program counter
	uint8_t sp;  // byte wide stack pointer
	uint8_t p;   // byte wide status register

	uint8_t firstByteOfInterest; // first byte to look at after opcode
	uint8_t secondByteOfInterest; // second byte to look at after opcode

							        //   NVss DIZC
	bool carryFlag;			        //   |||| |||+	
	bool zeroFlag;			        //   |||| ||+|
	bool interruptDisable;          //   |||| |+||
	bool decimalFlag;               //   |||| +|||
	bool bytePushed;                //   |||+ ||||
	bool bytePushedByInstruction;   //   ||+| ||||
	bool overflowFlag;              //   |+|| ||||
	bool negativeFlag;              //   +||| ||||

	/*
		The B FLAG:
			 BIT 5   BIT 4  Side Effect
		PHP  1		 1		None
		BRK	 1		 1		I set to 1
		/IRQ 1		 0      I set to 1
		/NMI 1		 0		I set to 1

		PLP and RTI pull a byte from the stack and set all flags
		They ignore bits 5 and 4
	*/

	uint8_t memory[0xFFFFF];

	/*
		Memory map:

		0x0000 -> 0x07FF: 2KB of internal RAM
		##########################################
		0x0800 -> 0x0FFF: Mirror of internal RAM
		0x1000 -> 0x17FF: Mirror of internal RAM
		0x1800 -> 0x1FFF: Mirror of internal RAM
		##########################################
		0x2000 -> 0x2007: PPU Registers
		##########################################
		0x2008 -> 0x3FFF: Mirrors of PPU registers
		##########################################
		0x4000 -> 0x4017: APU and I/O registers
		##########################################
		0x4018 -> 0x401F: APU and I/O functionality
						  that is normally disabled
		###########################################
		0x4020 -> 0xFFFF: Cartridge space: PRG ROM,
										   PRG RAM,
										   Mappers
	*/


};

#endif