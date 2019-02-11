#include "main.h"

CPU::CPU()
{
	std::cout << "CPU initializing..." << std::endl;

	a = 0x0;
	x = 0x0;
	y = 0x0;
	sp = 0xFD;
	pc = (uint16_t)0xC000;
	carryFlag = false;
	zeroFlag = false;
	interruptDisable = true;
	decimalFlag = false;
	bytePushed = false;
	bytePushedByInstruction = false;
	overflowFlag = false;
	negativeFlag = false;

	for (int i = 0x0; i <= 0xFFFFF; i++)
	{
		memory[i] = 0x0;
	}

	std::cout << "CPU initialized!" << std::endl;
}

CPU::~CPU()
{

}

void CPU::printStatus()
{
	printf("CPU STATUS:\n");
	printf("PC: %#x Op: %#x  %#x %#x A: %#x X: %#x Y: %#x SP: %#x\n", pc, memory[pc], memory[pc+1], memory[pc+2]
																  , a, x, y, sp);
	printf(" N  O PI BP  D  I  Z  C\n");
	printf(" %d  %d  %d  %d  %d  %d  %d  %d\n", negativeFlag, overflowFlag, bytePushedByInstruction, bytePushed, decimalFlag
									, interruptDisable, zeroFlag, carryFlag);
}

void CPU::loadCartridgeToMemory(Cartridge *cart)
{
	std::cout << "Loading Cartridge into CPU memory...\n";
	char tmp[1];
	int end = cart->getNumberOfBytes();
	for (int i = 0; i < CARTRIDGE_MEMORY_START+end; i++)
	{
		cart->file.read(tmp, 1);
		memory[0xBFF0+i] = *tmp;
		//memory[0xC000 + i] = *tmp;
	}
	std::cout << "Cartridge loaded!\n";
}

void CPU::printMemory()
{
	for (int i = 0; i <= MEMORY_END; i++)
	{
		std::cout << "0x"
				  << std::hex
				  << std::uppercase
				  << std::setw(4)
				  << std::setfill('0')
				  << memory[i]
				  << std::endl;
	}
}
void CPU::printMemory(int start, int end)
{
	if (end > MEMORY_END)
		end = MEMORY_END;
	if (start < 0x0)
		start = 0x0;
	for (int i = start; i <= end; i++)
	{
		printf("%#04x: %#04x\n", i, memory[i]);
	}
}

uint16_t CPU::getPC()
{
	return pc;
}

void CPU::determineOpCode()
{
	uint8_t temp = memory[pc];
	firstByteOfInterest = memory[pc + 1];
	secondByteOfInterest = memory[pc + 2];

	uint8_t leftNib = temp >> 4;
	uint8_t rightNib = temp & 0x0F;

	switch (leftNib)
	{
		case 0x0: leftNib0(rightNib); break;
		case 0x1: leftNib1(rightNib); break;
		case 0x2: leftNib2(rightNib); break;
		case 0x3: leftNib3(rightNib); break;
		case 0x4: leftNib4(rightNib); break;
		case 0x5: leftNib5(rightNib); break;
		case 0x6: leftNib6(rightNib); break;
		case 0x7: leftNib7(rightNib); break;
		case 0x8: leftNib8(rightNib); break;
		case 0x9: leftNib9(rightNib); break;
		case 0xA: leftNibA(rightNib); break;
		case 0xB: leftNibB(rightNib); break;
		case 0xC: leftNibC(rightNib); break;
		case 0xD: leftNibD(rightNib); break;
		case 0xE: leftNibE(rightNib); break;
		case 0xF: leftNibF(rightNib); break;
		default: std::cout << "I don't know what this nib is.\n"; break;
	}
}

void CPU::leftNib0(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRK
		{
			bytePushed = 1;
			bytePushedByInstruction = 1;
			pc += 0x1;
			return;
		}
		case 0x1: // ORA_INDIRECT_X
		{
			uint8_t tmp = x + firstByteOfInterest;
			uint16_t targetAddress = memory[tmp + 1];
			targetAddress <<= 8;
			targetAddress |= memory[tmp];
			a |= memory[targetAddress];
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // ORA_ZERO_PAGE
		{
			a |= memory[firstByteOfInterest];
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // ASL_ZERO_PAGE
		{
			uint8_t tmp = 0x0;
			tmp = memory[firstByteOfInterest];
			tmp >>= 7;
			memory[firstByteOfInterest] <<= 1;
			if (tmp == 1)
				carryFlag = 1;
			else
				carryFlag = 0;
			if ((memory[firstByteOfInterest] & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (memory[firstByteOfInterest] == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 2;
			return;
		}
		case 0x7: return;
		case 0x8: // PHP
		{
			uint8_t tmp = 0x0;
			if (carryFlag) tmp |= 0b00000001;
			if (zeroFlag) tmp |= 0b00000010;
			if (interruptDisable) tmp |= 0b00000100;
			if (decimalFlag) tmp |= 0b00001000;
			tmp |= 0b000100000;
			tmp |= 0b00100000;
			if (overflowFlag) tmp |= 0b01000000;
			if (negativeFlag) tmp |= 0b10000000;
			sp -= 0x1;
			memory[STACK_POINTER_OFFSET + sp] = tmp;
			pc += 0x1;
			return;
		}
		case 0x9: // ORA_IMMEDIATE
		{
			a |= firstByteOfInterest;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xA: // ASL_A
		{
			uint8_t tmp = 0x0;
			tmp = a;
			tmp >>= 7;
			a <<= 1;
			if (tmp == 1)
				carryFlag = 1;
			else
				carryFlag = 0;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 1;
			return;
		}
		case 0xB: return;
		case 0xC: return;
		case 0xD: // ORA_ABSOLUTE
		{
			uint16_t tmpAddress = 0x0;
			tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			a |= memory[tmpAddress];
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 3;
			return;
		}
		case 0xE: // ASL_ABSOLUTE
		{
			uint8_t tmp = 0x0;
			uint16_t tmpAddress = 0x0;
			tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmp = memory[tmpAddress];
			tmp >>= 7;
			memory[tmpAddress] <<= 1;
			pc += 3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib1(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_PLUS
		{
			if (!negativeFlag)
				pc += firstByteOfInterest + 2;
			else
				pc += 0x2;
			return;
		}
		case 0x1: // ORA_INDIRECT_Y
		{
			uint16_t targetAddress = memory[firstByteOfInterest + 1];
			targetAddress <<= 8;
			targetAddress |= memory[firstByteOfInterest];
			targetAddress += y;
			a |= memory[targetAddress];
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: // ASL_ZERO_PAGE_X
		{
			uint8_t tmp = 0x0;
			uint8_t tmpAddress = (x + firstByteOfInterest);

			tmp = memory[tmpAddress];
			tmp >>= 7;
			memory[tmpAddress] <<= 1;
			if (tmp == 1)
				carryFlag = 1;
			else
				carryFlag = 0;
			if ((memory[tmpAddress] & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (memory[tmpAddress] == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 2;
			return;
		}
		case 0x7: return;
		case 0x8: // CLEAR_CARRY
		{
			carryFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: // ASL_ABSOLUTE_X
		{
			uint8_t tmp = 0x0;
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			tmp = memory[tmpAddress];
			tmp >>= 7;
			memory[tmpAddress] <<= 1;
			if (tmp == 1)
				carryFlag = 1;
			else
				carryFlag = 0;
			if ((memory[tmpAddress] & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (memory[tmpAddress] == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib2(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // JSR
		{
			uint16_t targetAddress;
			targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			memory[sp + STACK_POINTER_OFFSET] = ((pc + 2) >> 8);
			sp -= 0x1;
			memory[sp + STACK_POINTER_OFFSET] = ((pc + 2) & 0x00FF);
			sp -= 0x1;
			pc = targetAddress;
			return;
		}
		case 0x1: // AND_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			x += firstByteOfInterest;
			firstIndex = x;
			secondIndex = x + 1;
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			a &= memory[targetAddress];

			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // BIT_ZERO_PAGE
		{
			if ((a & memory[firstByteOfInterest]) == 0) zeroFlag = 1;
			else zeroFlag = 1;
			if ((memory[firstByteOfInterest] & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if ((memory[firstByteOfInterest] & 0b01000000) == 0b01000000) overflowFlag = 1;
			else overflowFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x5: // AND_ZERO_PAGE
		{
			a &= memory[firstByteOfInterest];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 0x2;
			return;
		}
		case 0x6: return;
		case 0x7: return;
		case 0x8: // PLP
		{
			uint8_t tmp = 0x0;
			tmp = memory[STACK_POINTER_OFFSET + sp];
			if ((tmp & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			if ((tmp & 0b00000010) == 0b00000010) zeroFlag = 1;
			else zeroFlag = 0;
			if ((tmp & 0b00000100) == 0b00000100) interruptDisable = 1;
			else interruptDisable = 0;
			if ((tmp & 0b00001000) == 0b00001000) decimalFlag = 1;
			else decimalFlag = 0;
			if ((tmp & 0b00010000) == 0b00010000) bytePushed = 1;
			else bytePushed = 0;
			if ((tmp & 0b00100000) == 0b00100000) bytePushedByInstruction = 1;
			else bytePushedByInstruction = 0;
			if ((tmp & 0b01000000) == 0b01000000) overflowFlag = 1;
			else overflowFlag = 0;
			if ((tmp & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			sp += 0x1;
			pc += 0x1;
			return;
		}
		case 0x9: // AND_IMMEDIATE
		{
			a &= firstByteOfInterest;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 0x2;
			return;
		}
		case 0xA: // ROL_A
		{
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((a & 0b10000000) == 0b10000000) carryFlag = 1;
			else carryFlag = 0;
			a <<= 0x1;
			if (carryFlag == 1) a |= 0b00000001;
			else a &= 0b11111110;
			return;
		}
		case 0xB: return;
		case 0xC: // BIT_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			if ((a & memory[targetAddress]) == 0) zeroFlag = 1;
			else zeroFlag = 1;
			if ((memory[targetAddress] & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if ((memory[targetAddress] & 0b01000000) == 0b01000000) overflowFlag = 1;
			else overflowFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xD: // AND_ABSOLUTE
		{
			uint16_t tmp = 0x0;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			a &= memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 3;
			return;
		}
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib3(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_MINUS
		{
			if (negativeFlag)
				pc += firstByteOfInterest + 2;
			else
				pc += 0x2;
			return;
		}
		case 0x1: // AND_INDIRECT_Y
		{
			uint16_t tmpAddress;
			uint8_t firstIndex = memory[firstByteOfInterest];
			uint8_t secondIndex = memory[firstByteOfInterest + 1];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a &= memory[tmpAddress];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x2; 
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // AND_ZERO_PAGE_X
		{
			uint8_t tmp = (x + firstByteOfInterest);

			a &= memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 2;
			return;
		}
		case 0x6: return;
		case 0x7: return;
		case 0x8: // SET_CARRY
		{
			carryFlag = 1;
			pc += 0x1;
			return;
		}
		case 0x9: // AND_ABSOLUTE_Y
		{
			uint16_t tmp = 0x00;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			tmp += y;
			a &= memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 3;
			return;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // AND_ABSOLUTE_X
		{
			uint16_t tmp = 0x00;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			tmp += x;
			a &= memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 3;
			return;
		}
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib4(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: // EOR_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress;
			x += firstByteOfInterest;
			firstIndex = memory[x];
			secondIndex = memory[x + 1];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			a ^= memory[targetAddress];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // EOR_ZERO_PAGE
		{
			a ^= memory[firstByteOfInterest];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // LSR_ZERO_PAGE
		{
			if ((memory[firstByteOfInterest] | 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest] >>= 1;
			negativeFlag = 0;
			if (memory[firstByteOfInterest] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // PHA
		{
			sp -= 0x1;
			memory[sp] = a;
			pc += 0x1;
			return;
		}
		case 0x9: // EOR_IMMEDIATE
		{
			a ^= firstByteOfInterest;
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xA: // LSR_A
		{
			if ((a | 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			a >>= 1;
			negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			return;
		}
		case 0xB: return;
		case 0xC: // JMP_ABSOLUTE
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			pc = tmpAddress;
			return;
		}
		case 0xD: // EOR_ABSOLUTE
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			a ^= memory[tmpAddress];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // LSR_ABSOLUTE
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			if ((memory[tmpAddress] | 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[tmpAddress] >>= 1;
			negativeFlag = 0;
			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib5(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_OVERFLOW_CLEAR
		{
			if (!overflowFlag)
				pc += firstByteOfInterest + 2;
			else pc += 0x2;
			return;
		}
		case 0x1: // EOR_INDIRECT_Y
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress;
			firstIndex = memory[x];
			secondIndex = memory[x + 1];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			y += targetAddress;
			a ^= memory[targetAddress];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // EOR_ZERO_PAGE_X
		{
			a ^= memory[firstByteOfInterest + x];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // LSR_ZERO_PAGE_X
		{
			if ((memory[firstByteOfInterest+x] | 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest+x] >>= 1;
			negativeFlag = 0;
			if (memory[firstByteOfInterest+x] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // CLEAR_INTERRUPT
		{
			interruptDisable = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // EOR_ABSOLUTE_Y
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			a ^= memory[tmpAddress + y];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // EOR_ABSOLUTE_X
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			a ^= memory[tmpAddress + x];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // LSR_ABSOLUTE_X
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			if ((memory[tmpAddress] | 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[tmpAddress] >>= 1;
			negativeFlag = 0;
			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib6(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // RTS
		{
			sp += 0x1;
			uint8_t firstIndex = memory[sp + STACK_POINTER_OFFSET];
			sp += 0x1;
			uint16_t targetAddress = memory[sp + STACK_POINTER_OFFSET];
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += 0x1;
			pc = targetAddress;
			return;
		}
		case 0x1: // ADC_INDIRECT_X
		{
			uint8_t tmp = a;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			x += firstByteOfInterest;
			firstIndex = x;
			secondIndex = x + 1;
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			a += memory[targetAddress];

			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmp^a) & (memory[x]^a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // ADC_ZERO_PAGE
		{
			uint8_t tmp = a;
			a += memory[firstByteOfInterest];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmp^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // ROR_ZERO_PAGE
		{
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[firstByteOfInterest] & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest] >>= 0x1;
			if (tmp == 1) memory[firstByteOfInterest] |= 0b10000000;
			else memory[firstByteOfInterest] &= 0b01111111;
			if (memory[firstByteOfInterest] == 0x0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[firstByteOfInterest] >> 7) == 0x1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // PLA
		{
			a = memory[sp+STACK_POINTER_OFFSET];
			sp += 0x1;
			pc += 0x1;
			return;
		}
		case 0x9: // ADC_IMMEDIATE
		{
			uint8_t tmp = a;
			a += firstByteOfInterest;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmp^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xA: // ROR_A
		{
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((a & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			a >>= 0x1;
			if (tmp == 1) a |= 0b10000000;
			else a &= 0b01111111;
			if (a == 0x0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 0x1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: // JMP_INDIRECT
		{
			uint16_t tmpAddress = 0x00;
			tmpAddress |= secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			pc = memory[tmpAddress];
			return;
		}
		case 0xD: // ADC_ABSOLUTE
		{
			uint8_t tmpA = a;
			uint16_t tmp = 0x0;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			a += memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmpA^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 3;
			return;
		}
		case 0xE: // ROR_ABSOLUTE
		{
			uint16_t tmpAddress = 0x0;
			tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[tmpAddress] & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[tmpAddress] >>= 0x1;
			if (tmp == 1) memory[tmpAddress] |= 0b10000000;
			else memory[tmpAddress] &= 0b01111111;
			if (memory[tmpAddress] == 0x0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[tmpAddress] >> 7) == 0x1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib7(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_OVERFLOW_SET
		{
			if (overflowFlag)
				pc += firstByteOfInterest + 2;
			else
				pc += 0x2;
			return;
		}
		case 0x1: // ADC_INDIRECT_Y
		{
			uint8_t tmp = a;
			uint16_t tmpAddress;
			uint8_t firstIndex = memory[firstByteOfInterest];
			uint8_t secondIndex = memory[firstByteOfInterest + 1];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a += memory[tmpAddress];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 0;
			else
				zeroFlag = 1;
			if (((tmp^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // ADC_ZERO_PAGE_X
		{
			uint8_t tmpA = a;
			uint8_t tmp = (x + firstByteOfInterest);

			a += memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmpA^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 2;
			return;
		}
		case 0x6: // ROR_ZERO_PAGE_X
		{
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[firstByteOfInterest + x] & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest + x] >>= 0x1;
			if (tmp == 1) memory[firstByteOfInterest + x] |= 0b10000000;
			else memory[firstByteOfInterest + x] &= 0b01111111;
			if (memory[firstByteOfInterest + x] == 0x0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[firstByteOfInterest + x] >> 7) == 0x1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // SET_INTERRUPT
		{
			interruptDisable = 1;
			pc += 0x1;
			return;
		}
		case 0x9: // ADC_ABSOLUTE_Y
		{
			uint8_t tmpA = a;
			uint16_t tmp = 0x00;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			tmp += y;
			a += memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmpA^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 3;
			return;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // ADC_ABSOLUTE_X
		{
			uint8_t tmpA = a;
			uint16_t tmp = 0x00;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			tmp += x;
			a += memory[tmp];
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if (((tmpA^a) & (memory[x] ^ a) & 0x80) != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 3;
			return;
		}
		case 0xE: // ROR_ABSOLUTE_X
		{
			uint16_t tmpAddress = 0x0;
			tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[tmpAddress] & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			memory[tmpAddress] >>= 0x1;
			if (tmp == 1) memory[tmpAddress] |= 0b10000000;
			else memory[tmpAddress] &= 0b01111111;
			if (memory[tmpAddress] == 0x0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[tmpAddress] >> 7) == 0x1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib8(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: // STA_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			x += firstByteOfInterest;
			firstIndex = x;
			secondIndex = x + 1;
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			memory[targetAddress] = a;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // STY_ZERO_PAGE
		{
			memory[firstByteOfInterest] = y;
			pc += 0x2;
			return;
		}
		case 0x5: // STA_ZERO_PAGE
		{
			memory[firstByteOfInterest] = a;
			pc += 0x2;
			return;
		}
		case 0x6: // STX_ZERO_PAGE
		{
			memory[firstByteOfInterest] = x;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // DEY
		{
			y -= 0x1;
			pc += 0x1;
			return;
		}
		case 0x9: return;
		case 0xA: // TXA
		{
			a = x;
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: // STY_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			memory[tmpAddress] = y;
			pc += 0x3;
			return;
		}
		case 0xD: // STA_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			memory[tmpAddress] = a;
			pc += 0x3;
			return;
		}
		case 0xE: // STX_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			memory[tmpAddress] = x;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib9(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_CARRY_CLEAR
		{
			if (!carryFlag)
				pc += firstByteOfInterest + 2;
			else
				pc += 0x2;
			return;
		}
		case 0x1: // STA_INDIRECT_Y
		{
			uint16_t tmpAddress;
			uint8_t firstIndex = memory[firstByteOfInterest];
			uint8_t secondIndex = memory[firstByteOfInterest + 1];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			memory[tmpAddress] = a;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // STY_ZERO_PAGE_X
		{
			memory[firstByteOfInterest + x] = y;
			pc += 0x2;
			return;
		}
		case 0x5: // STA_ZERO_PAGE_X
		{
			memory[firstByteOfInterest + x] = a;
			pc += 0x2;
			return;
		}
		case 0x6: // STX_ZERO_PAGE_Y
		{
			memory[firstByteOfInterest + y] = x;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // TYA
		{
			a = y;
			pc += 0x1;
			return;
		}
		case 0x9: // STA_ABSOLUTE_Y
		{
			uint16_t tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += y;
			memory[tmpAddress] = a;
			pc += 0x3;
			return;
		}
		case 0xA: // TXS
		{
			sp -= 0x1;
			memory[sp] = x;
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: return;
		case 0xD: // STA_ABSOLUTE_X
		{
			uint16_t tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			memory[tmpAddress] = a;
			pc += 0x3;
			return;
		}
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibA(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // LDY_IMMEDIATE
		{
			y = firstByteOfInterest;
			uint8_t tmp = y;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x1: // LDA_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			x += firstByteOfInterest;
			firstIndex = x;
			secondIndex = x + 1;
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			a = memory[targetAddress];

			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: // LDX_IMMEDIATE
		{
			x = firstByteOfInterest;
			uint8_t tmp = x;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x3: return;
		case 0x4: // LDY_ZERO_PAGE
		{
			y = memory[firstByteOfInterest];
			uint8_t tmp = y;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x5: // LDA_ZERO_PAGE
		{
			a = memory[firstByteOfInterest];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // LDX_ZERO_PAGE
		{
			x = memory[firstByteOfInterest];
			uint8_t tmp = x;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // TAY
		{
			y = a;
			pc += 0x1;
			return;
		}
		case 0x9: // LDA_IMMEDIATE
		{
			a = firstByteOfInterest;
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xA: // TAX
		{
			x = a;
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: // LDY_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			y = memory[tmpAddress];
			tmp = y;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xD: // LDA_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			a = memory[tmpAddress];
			tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // LDX_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			x = memory[tmpAddress];
			tmp = x;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibB(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_CARRY_SET
		{
			if (carryFlag)
				pc += firstByteOfInterest + 2;
			else
				pc += 0x2;
			return;
		}
		case 0x1: // LDA_INDIRECT_Y
		{
			/*
				firstByteOfInterest is a memory location in page 0
				add the contents of that location to y
				the result is the low order 8 bits of the effective address
				the carry of the addition is added to the next zero page memory location
				the result is the high order 8 bits of the effective address
			*/
			uint16_t tmpAddress;
			uint8_t firstIndex = memory[firstByteOfInterest];
			uint8_t secondIndex = memory[firstByteOfInterest + 1];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a = memory[tmpAddress];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // LDY_ZERO_PAGE_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			y = memory[tmp];
			tmp = y;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x5: // LDA_ZERO_PAGE_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			a = memory[tmp];
			tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // LDX_ZERO_PAGE_Y
		{
			uint8_t tmp = firstByteOfInterest + y;
			x = memory[tmp];
			tmp = x;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // CLEAR_OVERFLOW
		{
			overflowFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // LDA_ABSOLUTE_Y
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += y;
			a = memory[tmpAddress];
			tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: // TSX
		{
			sp += 0x1;
			x = memory[sp];
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: // LDY_ABSOLUTE_X
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			y = memory[tmpAddress];
			tmp = y;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xD: // LDA_ABSOLUTE_X
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			a = memory[tmpAddress];
			tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // LDX_ABSOLUTE_Y
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += y;
			x = memory[tmpAddress];
			tmp = x;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibC(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // CPY_IMMEDIATE
		{
			if (y == firstByteOfInterest) zeroFlag = 1;
			else zeroFlag = 0;
			if (y >= firstByteOfInterest) carryFlag = 1;
			else carryFlag = 0;
			if (y < firstByteOfInterest) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x1: // CMP_INDIRECT_X
		{
			uint8_t tmp = x + firstByteOfInterest;
			uint16_t targetAddress = memory[tmp + 1];
			targetAddress <<= 8;
			targetAddress |= memory[tmp];
			if (a == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // CPY_ZERO_PAGE
		{
			if (y == memory[firstByteOfInterest]) zeroFlag = 1;
			else zeroFlag = 0;
			if (y >= memory[firstByteOfInterest]) carryFlag = 1;
			else carryFlag = 0;
			if (y < memory[firstByteOfInterest]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x5: // CMP_ZERO_PAGE
		{
			if (a == memory[firstByteOfInterest]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[firstByteOfInterest]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[firstByteOfInterest]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // DEC_ZERO_PAGE
		{
			uint8_t tmp;
			memory[firstByteOfInterest] -= (uint8_t)0x1;
			tmp = memory[firstByteOfInterest];
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // INY
		{
			y += 0x1;
			pc += 0x1;
			return;
		}
		case 0x9: // CMP_IMMEDIATE
		{
			if (a == firstByteOfInterest) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= firstByteOfInterest) carryFlag = 1;
			else carryFlag = 0;
			if (a < firstByteOfInterest) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xA: // DEX
		{
			x -= 0x1;
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: // CPY_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			if (y == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (y >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (y < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xD: // CMP_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			if (a == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // DEC_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			memory[tmpAddress] -= 0x1;
			tmp = memory[tmpAddress];
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibD(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_NOT_EQUAL
		{
			if (!zeroFlag)
				pc += firstByteOfInterest + 2;
			else pc += 0x2;
			return;
		}
		case 0x1: // CMP_INDIRECT_Y
		{
			uint8_t tmp = firstByteOfInterest;
			uint16_t targetAddress = memory[tmp + 1];
			targetAddress <<= 8;
			targetAddress |= memory[tmp];
			targetAddress += y;
			if (a == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // CMP_ZERO_PAGE_X
		{
			if (a == memory[firstByteOfInterest + x]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[firstByteOfInterest + x]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[firstByteOfInterest + x]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // DEC_ZERO_PAGE_X
		{
			uint8_t tmp;
			memory[firstByteOfInterest+x] -= (uint8_t)0x1;
			tmp = memory[firstByteOfInterest+x];
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // CLEAR_DECIMAL
		{
			decimalFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // CMP_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			if (a == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // CMP_ABSOLUTE_X
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			if (a == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // DEC_ABSOLUTE_X
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			memory[tmpAddress] -= 0x1;
			tmp = memory[tmpAddress];
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibE(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // CPX_IMMEDIATE
		{
			if (x == firstByteOfInterest) zeroFlag = 1;
			else zeroFlag = 0;
			if (x >= firstByteOfInterest) carryFlag = 1;
			else carryFlag = 0;
			if (x < firstByteOfInterest) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x1: // SBC_INDIRECT_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			uint16_t targetAddress = memory[x + 1];
			targetAddress <<= 8;
			targetAddress |= memory[x];
			a -= memory[targetAddress];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // CPX_ZERO_PAGE
		{
			if (x == memory[firstByteOfInterest]) zeroFlag = 1;
			else zeroFlag = 0;
			if (x >= memory[firstByteOfInterest]) carryFlag = 1;
			else carryFlag = 0;
			if (x < memory[firstByteOfInterest]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x5: // SBC_ZERO_PAGE
		{
			a -= memory[firstByteOfInterest];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // INC_ZERO_PAGE
		{
			uint8_t tmp;
			memory[firstByteOfInterest] += (uint8_t)0x1;
			tmp = memory[firstByteOfInterest];
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // INX
		{
			x += 0x1;
			pc += 0x1;
			return;
		}
		case 0x9: // SBC_IMMEDIATE
		{
			//carryFlag = 1;
			a -= firstByteOfInterest;
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: return;
		case 0xC: // CPX_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			if (x == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (x >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (x < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xD: // SBC_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			a -= memory[targetAddress];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // INC_ABSOLUTE
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			memory[tmpAddress] += 0x1;
			tmp = memory[tmpAddress];
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibF(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // BRANCH_ON_EQUAL
		{
			if (zeroFlag)
				pc += firstByteOfInterest + 2;
			else pc += 0x2;
			return;
		}
		case 0x1: // SBC_INDIRECT_Y
		{
			uint16_t targetAddress = memory[firstByteOfInterest + 1];
			targetAddress <<= 8;
			targetAddress |= memory[firstByteOfInterest];
			targetAddress += y;
			a -= memory[targetAddress];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // SBC_ZERO_PAGE_X
		{
			a -= memory[firstByteOfInterest + x];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // INC_ZERO_PAGE_X
		{
			uint8_t tmp;
			memory[firstByteOfInterest + x] += (uint8_t)0x1;
			tmp = memory[firstByteOfInterest + x];
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: return;
		case 0x8: // SET_DECIMAL
		{
			decimalFlag = 1;
			pc += 0x1;
			return;
		}
		case 0x9: // SBC_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			a -= memory[targetAddress + y];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // SBC_ABSOLUTE_X
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			a -= memory[targetAddress + x];
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((a << 1) >> 7) == 0b00000001) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xE: // INC_ABSOLUTE_X
		{
			uint16_t tmpAddress = secondByteOfInterest;
			uint8_t tmp;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			tmpAddress += x;
			memory[tmpAddress] += 0x1;
			tmp = memory[tmpAddress];
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}
