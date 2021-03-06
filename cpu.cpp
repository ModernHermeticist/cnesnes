#include "main.h"

CPU::CPU()
{
	std::cout << "CPU initializing..." << std::endl;

	a = 0x0;
	x = 0x0;
	y = 0x0;
	sp = 0xFD;
	pc = 0x0;
	p = 0;
	memoryOffset = 0;
	carryFlag = false;
	zeroFlag = false;
	interruptDisable = true;
	decimalFlag = false;
	bytePushed = false;
	bytePushedByInstruction = true;
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
	printf(" N  O PI BP  D  I  Z  C ------  P: %#x \n", p);
	printf(" %d  %d  %d  %d  %d  %d  %d  %d\n", negativeFlag, overflowFlag, bytePushedByInstruction, bytePushed, decimalFlag
									, interruptDisable, zeroFlag, carryFlag);
	printf("___________________________________________________________________\n");
}

void CPU::loadCartridgeToMemory(Cartridge *cart)
{
	std::cout << "Loading Cartridge into CPU memory...\n";
	char tmp[1];
	int end = cart->getNumberOfBytes();
	for (int i = 0; i < CARTRIDGE_MEMORY_START+end; i++)
	{
		cart->file.read(tmp, 1);
		memory[memoryOffset+i] = *tmp;
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

void CPU::setPC(uint16_t val)
{
	pc = val;
}

uint8_t CPU::getP()
{
	return p;
}

uint8_t CPU::getA()
{
	return a;
}

uint8_t CPU::getY()
{
	return y;
}

uint8_t CPU::getX()
{
	return x;
}

uint8_t CPU::getSP()
{
	return sp;
}

uint16_t CPU::getMemoryOffset()
{
	return memoryOffset;
}

void CPU::setMemoryOffset(uint16_t val)
{
	memoryOffset = val;
}

void CPU::mergePRegister()
{
	p = 0;
	if (negativeFlag == 1) p |= 0b00000001;
	p <<= 1;
	if (overflowFlag == 1) p |= 0b00000001;
	p <<= 1;
	if (bytePushedByInstruction == 1) p |= 0b00000001;
	p <<= 1;
	if (bytePushed == 1) p |= 0b00000001;
	p <<= 1;
	if (decimalFlag == 1) p |= 0b00000001;
	p <<= 1;
	if (interruptDisable == 1) p |= 0b00000001;
	p <<= 1;
	if (zeroFlag == 1) p |= 0b00000001;
	p <<= 1;
	if (carryFlag == 1) p |= 0b00000001;
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
			uint8_t tmp = 0x0;
			interruptDisable = 1;
			if (carryFlag) tmp |= 0b00000001;
			if (zeroFlag) tmp |= 0b00000010;
			tmp |= 0b00000100;
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
		case 0x2: return; // 0x02
		case 0x3: // ASO_INDIRECT_X
		{
			uint8_t tmp;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			tmp = memory[targetAddress];
			tmp >>= 7;
			memory[targetAddress] <<= 1;
			a |= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x2;
			return;


		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
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
		case 0x7: // ASO_ZERO_PAGE
		{
			uint8_t tmp = 0x0;
			tmp = memory[firstByteOfInterest];
			tmp >>= 7;
			memory[firstByteOfInterest] <<= 1;

			a |= memory[firstByteOfInterest];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x8: // PHP
		{
			uint8_t tmp = 0x0;
			if (carryFlag) tmp |= 0b00000001;
			if (zeroFlag) tmp |= 0b00000010;
			if (interruptDisable) tmp |= 0b00000100;
			if (decimalFlag) tmp |= 0b00001000;
			tmp |= 0b00010000;
			tmp |= 0b00100000;
			if (overflowFlag) tmp |= 0b01000000;
			if (negativeFlag) tmp |= 0b10000000;
			memory[STACK_POINTER_OFFSET + sp] = tmp;
			sp -= 0x1;
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
		case 0xB: return; // 0x0B
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
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
			uint8_t old7 = 0x0;
			uint16_t tmpAddress = 0x0;
			tmpAddress = secondByteOfInterest;
			tmpAddress <<= 8;
			tmpAddress |= firstByteOfInterest;
			old7 = memory[tmpAddress];
			old7 >>= 7;
			memory[tmpAddress] <<= 1;

			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[tmpAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 3;
			return;
		}
		case 0xF: // ASO_ABSOLUTE
		{
			uint8_t tmp;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			tmp = memory[targetAddress];
			tmp >>= 7;
			memory[targetAddress] <<= 1;
			a |= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x3;
			return;
		}
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
			uint16_t tmpAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a |= memory[tmpAddress];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return; // 0x12
		case 0x3: // ASO_INDIRECT_Y
		{
			uint8_t tmp;
			uint16_t targetAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;
			tmp = memory[targetAddress];
			tmp >>= 7;
			memory[targetAddress] <<= 1;
			a |= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x5: // ORA_ZERO_PAGE_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			a |= memory[tmp];
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
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
		case 0x7: // ASO_ZERO_PAGE_X
		{
			uint8_t tmp = 0x0;
			uint8_t targetAddress = firstByteOfInterest + x;
			tmp = memory[targetAddress];
			tmp >>= 7;
			memory[targetAddress] <<= 1;

			a |= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x8: // CLEAR_CARRY
		{
			carryFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // ORA_ABSOLUTE_Y
		{
			uint16_t targetAddress = 0x00;
			targetAddress |= secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			a |= memory[targetAddress];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: // ASO_ABSOLUTE_Y
		{
			uint8_t tmp;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			tmp = memory[targetAddress];
			tmp >>= 7;
			memory[targetAddress] <<= 1;
			a |= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
		case 0xD: // ORA_ABSOLUTE_X
		{
			uint16_t targetAddress = 0x00;
			targetAddress |= secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			a |= memory[targetAddress];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
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
		case 0xF: // ASO_ABSOLUTE_X
		{
			uint8_t tmp;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			tmp = memory[targetAddress];
			tmp >>= 7;
			memory[targetAddress] <<= 1;
			a |= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			if (tmp == 1) carryFlag = 1;
			else carryFlag = 0;
			pc += 0x3;
			return;
		}
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
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
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
		case 0x2: return; // 0x22
		case 0x3: // RLA_INDIRECT_X
		{
			uint8_t old7 = 0x0;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;

			a &= memory[targetAddress];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x4: // BIT_ZERO_PAGE
		{
			if ((a & memory[firstByteOfInterest]) == 0) zeroFlag = 1;
			else zeroFlag = 0;
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
		case 0x6: // ROL_ZERO_PAGE
		{
			uint8_t old7 = 0x0;
			old7 = memory[firstByteOfInterest] & 0b10000000;
			old7 >>= 7;
			memory[firstByteOfInterest] <<= 0x1;
			if (carryFlag == 1) memory[firstByteOfInterest] |= 0b00000001;
			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[firstByteOfInterest] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[firstByteOfInterest] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: // RLA_ZERO_PAGE
		{
			uint8_t old7 = 0x0;
			old7 = memory[firstByteOfInterest] & 0b10000000;
			old7 >>= 7;
			memory[firstByteOfInterest] <<= 0x1;
			if (carryFlag == 1) memory[firstByteOfInterest] |= 0b00000001;

			a &= memory[firstByteOfInterest];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[firstByteOfInterest] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[firstByteOfInterest] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x8: // PLP
		{
			uint8_t tmp = 0x0;
			sp += 0x1;
			tmp = memory[STACK_POINTER_OFFSET + sp];
			if ((tmp & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			if ((tmp & 0b00000010) == 0b00000010) zeroFlag = 1;
			else zeroFlag = 0;
			if ((tmp & 0b00000100) == 0b00000100) interruptDisable = 1;
			else interruptDisable = 0;
			if ((tmp & 0b00001000) == 0b00001000) decimalFlag = 1;
			else decimalFlag = 0;
			tmp &= 0b11101111;
			bytePushed = 0;
			tmp |= 0b00100000;
			bytePushedByInstruction = 1;
			if ((tmp & 0b01000000) == 0b01000000) overflowFlag = 1;
			else overflowFlag = 0;
			if ((tmp & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
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
			uint8_t old7 = 0x0;
			old7 = a & 0b10000000;
			old7 >>= 7;
			a <<= 0x1;
			if (carryFlag == 1) a |= 0b00000001;
			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0xB: return; // 0x2B
		case 0xC: // BIT_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			if ((a & memory[targetAddress]) == 0) zeroFlag = 1;
			else zeroFlag = 0;
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
		case 0xE: // ROL_ABSOLUTE
		{
			uint16_t targetAddress = 0x0;
			targetAddress |= secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			uint8_t old7 = 0x0;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;
			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // RLA_ABSOLUTE
		{
			uint8_t old7 = 0x0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;

			a &= memory[targetAddress];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
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
		case 0x2: return; // 0x32
		case 0x3: // RLA_INDIRECT_Y
		{
			uint8_t old7;
			uint16_t targetAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;

			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;

			a &= memory[targetAddress];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
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
		case 0x6: // ROL_ZERO_PAGE_X
		{
			uint8_t old7 = 0x0;
			uint8_t targetAddress = firstByteOfInterest + x;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;
			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: // RLA_ZERO_PAGE_X
		{
			uint8_t old7 = 0x0;
			uint8_t targetAddress = firstByteOfInterest + x;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;

			a &= memory[targetAddress];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
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
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: // RLA_ABSOLUTE_Y
		{
			uint8_t old7 = 0x0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;

			a &= memory[targetAddress];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
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
		case 0xE: // ROL_ABSOLUTE_X
		{
			uint16_t targetAddress = 0x0;
			targetAddress |= secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			uint8_t old7 = 0x0;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;
			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // RLA_ABSOLUTE_X
		{
			uint8_t old7 = 0x0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			old7 = memory[targetAddress] & 0b10000000;
			old7 >>= 7;
			memory[targetAddress] <<= 0x1;
			if (carryFlag == 1) memory[targetAddress] |= 0b00000001;

			a &= memory[targetAddress];

			if (old7 == 1) carryFlag = 1;
			else carryFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((memory[targetAddress] >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib4(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // RTI
		{
			uint8_t tmp = 0x0;
			sp += 0x1;
			tmp = memory[STACK_POINTER_OFFSET + sp];
			if ((tmp & 0b00000001) == 0b00000001) carryFlag = 1;
			else carryFlag = 0;
			if ((tmp & 0b00000010) == 0b00000010) zeroFlag = 1;
			else zeroFlag = 0;
			if ((tmp & 0b00000100) == 0b00000100) interruptDisable = 1;
			else interruptDisable = 0;
			if ((tmp & 0b00001000) == 0b00001000) decimalFlag = 1;
			else decimalFlag = 0;
			tmp &= 0b11101111;
			bytePushed = 0;
			tmp |= 0b00100000;
			bytePushedByInstruction = 1;
			if ((tmp & 0b01000000) == 0b01000000) overflowFlag = 1;
			else overflowFlag = 0;
			if ((tmp & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;

			sp += 0x1;
			uint8_t firstIndex = memory[sp + STACK_POINTER_OFFSET];
			sp += 0x1;
			uint16_t targetAddress = memory[sp + STACK_POINTER_OFFSET];
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			pc = targetAddress;
			return;
		}
		case 0x1: // EOR_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
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
		case 0x2: return; // 0x42
		case 0x3: // SRE_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;

			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;

			a ^= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			pc += 0x2;
			return;
		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
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
			if ((memory[firstByteOfInterest] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest] >>= 1;
			negativeFlag = 0;
			if (memory[firstByteOfInterest] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: // SRE_ZERO_PAGE
		{
			if ((memory[firstByteOfInterest] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest] >>= 1;

			a ^= memory[firstByteOfInterest];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			
			pc += 0x2;
			return;
		}
		case 0x8: // PHA
		{
			memory[sp + STACK_POINTER_OFFSET] = a;
			sp -= 0x1;
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
			if ((a & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			a >>= 1;
			negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x1;
			return;
		}
		case 0xB: return; // 0x4B
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
			if ((memory[tmpAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[tmpAddress] >>= 1;
			negativeFlag = 0;
			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // SRE_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;

			a ^= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			pc += 0x3;
			return;
		}
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
			uint16_t tmpAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a ^= memory[tmpAddress];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return; // 0x52
		case 0x3: // SRE_INDIRECT_Y
		{
			uint16_t targetAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;


			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;

			a ^= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			pc += 0x2;
			return;
		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x5: // EOR_ZERO_PAGE_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			a ^= memory[tmp];
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x6: // LSR_ZERO_PAGE_X
		{
			uint8_t targetAddress = firstByteOfInterest + x;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;
			negativeFlag = 0;
			if (memory[targetAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x7: // SRE_ZERO_PAGE_X
		{
			uint8_t targetAddress = firstByteOfInterest + x;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;

			a ^= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			pc += 0x2;
			return;
		}
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
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: // SRE_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;

			a ^= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			pc += 0x3;
			return;
		}
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
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
			if ((memory[tmpAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[tmpAddress] >>= 1;
			negativeFlag = 0;
			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // SRE_ABSOLUTE_X
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 1;

			a ^= memory[targetAddress];

			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;

			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;

			pc += 0x3;
			return;
		}
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
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			a += memory[targetAddress];
			if ((tmp ^ a) & (memory[targetAddress] ^ a) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;
			//if (carryFlag == 1) a += 0x1;
			if ((a & 0b10000000) == 0b10000000) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0x0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return; // 0x62
		case 0x3: // RRA_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;


			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 0x1;
			if (tmp == 1) memory[targetAddress] |= 0x80;
			else memory[targetAddress] &= 0x7F;

			tmp = a;

			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x5: // ADC_ZERO_PAGE
		{
			uint8_t tmp = a;
			a += memory[firstByteOfInterest];
			if (carryFlag == 1)
			{
				if (tmp + memory[firstByteOfInterest] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[firstByteOfInterest] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[firstByteOfInterest] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0x6: // ROR_ZERO_PAGE
		{
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[firstByteOfInterest] & 0x1) == 0x1) carryFlag = 1;
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
		case 0x7: // RRA_ZERO_PAGE
		{
			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[firstByteOfInterest] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[firstByteOfInterest] >>= 0x1;
			if (tmp == 1) memory[firstByteOfInterest] |= 0x80;
			else memory[firstByteOfInterest] &= 0x7F;

			tmp = a;

			a += memory[firstByteOfInterest];

			if (carryFlag == 1)
			{
				if (tmp + memory[firstByteOfInterest] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[firstByteOfInterest] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[firstByteOfInterest] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0x8: // PLA
		{
			sp += 0x1;
			a = memory[sp+STACK_POINTER_OFFSET];
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // ADC_IMMEDIATE
		{
			uint16_t tmp = a;

			a += firstByteOfInterest;

			if (carryFlag == 1)
			{
				if (tmp + firstByteOfInterest + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + firstByteOfInterest > 0xFF) carryFlag = 1;
			else carryFlag = 0;
			
			if (((tmp ^ a) & (firstByteOfInterest ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0xB: return; // 0x6B
		case 0xC: // JMP_INDIRECT
		{
			uint16_t targetAddress = 0x00;
			uint16_t indexLocation = secondByteOfInterest;
			indexLocation <<= 8;
			indexLocation |= firstByteOfInterest;
			if ((indexLocation & 0x00FF) == 0x00FF)
				indexLocation -= 0x00FF;
			else indexLocation += 1;
			targetAddress = memory[indexLocation];
			if ((indexLocation & 0x00FF) == 0x0000)
				indexLocation += 0x00FF;
			else indexLocation -= 1;
			targetAddress <<= 8;
			targetAddress |= memory[indexLocation];
			pc = targetAddress;
			return;
		}
		case 0xD: // ADC_ABSOLUTE
		{
			uint8_t tmp = a;
			uint16_t targetAddress = 0x0;
			targetAddress |= secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			a += memory[targetAddress];
			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0xF: // RRA_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;


			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 0x1;
			if (tmp == 1) memory[targetAddress] |= 0x80;
			else memory[targetAddress] &= 0x7F;

			tmp = a;

			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x3;
			return;
		}
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
			uint16_t tmpAddress;
			uint8_t tmp = a;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a += memory[tmpAddress];
			if (carryFlag == 1)
			{
				if (tmp + memory[tmpAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[tmpAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;
			
			if (((tmp ^ a) & (memory[tmpAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x2;
		}
		case 0x2: return; // 0x72
		case 0x3: // RRA_INDIRECT_Y
		{
			uint16_t targetAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;


			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 0x1;
			if (tmp == 1) memory[targetAddress] |= 0x80;
			else memory[targetAddress] &= 0x7F;

			tmp = a;

			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x5: // ADC_ZERO_PAGE_X
		{
			uint16_t tmp = a;
			uint8_t tmp2 = firstByteOfInterest + x;
			a += memory[tmp2];

			if (carryFlag == 1)
			{
				if (tmp + memory[tmp2] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[tmp2] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[tmp2] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0x7: // RRA_ZERO_PAGE_X
		{
			uint8_t tmp = 0x0;
			uint8_t targetAddress = firstByteOfInterest + x;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 0x1;
			if (tmp == 1) memory[targetAddress] |= 0x80;
			else memory[targetAddress] &= 0x7F;

			tmp = a;

			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0x8: // SET_INTERRUPT
		{
			interruptDisable = 1;
			pc += 0x1;
			return;
		}
		case 0x9: // ADC_ABSOLUTE_Y
		{
			uint8_t tmp = a;
			uint16_t targetAddress = 0x00;
			targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: // RRA_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;

			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 0x1;
			if (tmp == 1) memory[targetAddress] |= 0x80;
			else memory[targetAddress] &= 0x7F;

			tmp = a;

			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
		case 0xD: // ADC_ABSOLUTE_X
		{
			uint8_t tmp = a;
			uint16_t targetAddress = 0x0;
			targetAddress |= secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			a += memory[targetAddress];
			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
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
		case 0xF: // RRA_ABSOLUTE_X
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;

			uint8_t tmp = 0x0;
			if (carryFlag == 1) tmp = 0x1;
			if ((memory[targetAddress] & 0x1) == 0x1) carryFlag = 1;
			else carryFlag = 0;
			memory[targetAddress] >>= 0x1;
			if (tmp == 1) memory[targetAddress] |= 0x80;
			else memory[targetAddress] &= 0x7F;

			tmp = a;

			a += memory[targetAddress];

			if (carryFlag == 1)
			{
				if (tmp + memory[targetAddress] + 1 > 0xFF) carryFlag = 1;
				else carryFlag = 0;

				a += 0x1;
			}
			else if (tmp + memory[targetAddress] > 0xFF) carryFlag = 1;
			else carryFlag = 0;

			if (((tmp ^ a) & (memory[targetAddress] ^ a)) & 0x80) overflowFlag = 1;
			else overflowFlag = 0;
			if ((a & 0b10000000) == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x3;
			return;
		}
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib8(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x1: // STA_INDIRECT_X
		{
			uint8_t indexLocation = firstByteOfInterest + x + 1;
			uint16_t targetAddress = 0;
			targetAddress = memory[indexLocation];
			indexLocation -= 0x1;
			targetAddress <<= 8;
			targetAddress |= memory[indexLocation];
			memory[targetAddress] = a;
			pc += 0x2;
			return;
		}
		case 0x2: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x3: // SAX_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			uint8_t tmp = a & x;
			memory[targetAddress] = tmp;
			pc += 0x2;
			return;
		}
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
		case 0x7: // SAX_ZERO_PAGE
		{
			memory[firstByteOfInterest] = a & x;
			pc += 0x2;
			return;
		}
		case 0x8: // DEY
		{
			y -= 0x1;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((y >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: return; // 0x89
		case 0xA: // TXA
		{
			a = x;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((a >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0xB: return; // 0x8B
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
		case 0xF: // SAX_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			memory[targetAddress] = a & x;
			pc += 3;
			return;
		}
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
		case 0x2: return; // 0x92
		case 0x3: return; // 0x93
		case 0x4: // STY_ZERO_PAGE_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			memory[tmp] = y;
			pc += 0x2;
			return;
		}
		case 0x5: // STA_ZERO_PAGE_X
		{
			uint8_t tmp = firstByteOfInterest + x;
			memory[tmp] = a;
			pc += 0x2;
			return;
		}
		case 0x6: // STX_ZERO_PAGE_Y
		{
			uint8_t tmp = firstByteOfInterest + y;
			memory[tmp] = x;
			pc += 0x2;
			return;
		}
		case 0x7: // SAX_ZERO_PAGE_Y
		{
			uint8_t targetAddress = firstByteOfInterest + y;
			memory[targetAddress] = a & x;
			pc += 0x2;
			return;
		}
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
			sp = x;
			//memory[sp+STACK_POINTER_OFFSET] = x;
			pc += 0x1;
			return;
		}
		case 0xB: return; // 0x9B
		case 0xC: return; // 0x9C
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
		case 0xE: return; // 0x9E
		case 0xF: return; // 0x9F
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
			tmp >>= 7;
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
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
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
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x3: // LAX_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			a = memory[targetAddress];
			x = memory[targetAddress];

			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
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
		case 0x7: // LAX_ZERO_PAGE
		{
			a = memory[firstByteOfInterest];
			x = memory[firstByteOfInterest];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
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
		case 0xB: return; // 0xAB
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
		case 0xF: // LAX_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			uint8_t tmp;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			a = memory[targetAddress];
			x = memory[targetAddress];
			tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
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
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
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
		case 0x2: return; // 0xB2
		case 0x3: // LAX_INDIRECT_Y
		{
			uint16_t tmpAddress;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			tmpAddress = secondIndex;
			tmpAddress <<= 8;
			tmpAddress |= firstIndex;
			tmpAddress += y;
			a = memory[tmpAddress];
			x = memory[tmpAddress];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
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
		case 0x7: // LAX_ZERO_PAGE_Y
		{
			uint8_t targetAddress = firstByteOfInterest + y;
			a = memory[targetAddress];
			x = memory[targetAddress];
			uint8_t tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
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
			//x = memory[sp+STACK_POINTER_OFFSET];
			x = sp;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((x >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0xB: return; // 0xBB
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
		case 0xF: // LAX_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			uint8_t tmp;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			a = memory[targetAddress];
			x = memory[targetAddress];
			tmp = a;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibC(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // CPY_IMMEDIATE
		{
			negativeFlag = 0;
			carryFlag = 0;
			zeroFlag = 0;
			uint8_t diff = y - firstByteOfInterest;
			diff >>= 7;

			if (y < firstByteOfInterest && diff == 1) negativeFlag = 1;

			if (y == firstByteOfInterest)
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (y > firstByteOfInterest)
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x1: // CMP_INDIRECT_X
		{
			zeroFlag = 0;
			carryFlag = 0;
			negativeFlag = 0;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint8_t diff;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}

			pc += 0x2;
			return;
		}
		case 0x2: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x3: // DCP_INDIRECT_X
		{
			uint8_t tmp = a;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;

			uint8_t val = memory[targetAddress];
			val -= 0x1;
			memory[targetAddress] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x4: // CPY_ZERO_PAGE
		{
			negativeFlag = 0;
			carryFlag = 0;
			zeroFlag = 0;
			uint8_t diff = y - memory[firstByteOfInterest];
			diff >>= 7;

			if (y < memory[firstByteOfInterest] && diff == 1) negativeFlag = 1;

			if (y == memory[firstByteOfInterest])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (y > memory[firstByteOfInterest])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x5: // CMP_ZERO_PAGE
		{
			negativeFlag = 0;
			carryFlag = 0;
			zeroFlag = 0;
			uint8_t diff = a - memory[firstByteOfInterest];
			diff >>= 7;

			if (a < memory[firstByteOfInterest] && diff == 1) negativeFlag = 1;

			if (a == memory[firstByteOfInterest])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[firstByteOfInterest])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
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
		case 0x7: // DCP_ZERO_PAGE
		{
			uint8_t val = memory[firstByteOfInterest];
			val -= 0x1;
			memory[firstByteOfInterest] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t targetAddress = firstByteOfInterest;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x8: // INY
		{
			y += 0x1;
			if (y == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((y >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // CMP_IMMEDIATE
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - firstByteOfInterest;
			diff >>= 7;

			if (a < firstByteOfInterest && diff == 1) negativeFlag = 1;

			if (a == firstByteOfInterest)
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > firstByteOfInterest)
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0xA: // DEX
		{
			x -= 0x1;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((x >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0xB: return; // 0xCB
		case 0xC: // CPY_ABSOLUTE
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			uint8_t diff = y - memory[targetAddress];
			diff >>= 7;

			if (y < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (y == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (y > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x3;
			return;
		}
		case 0xD: // CMP_ABSOLUTE
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
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
			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // DCP_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			uint8_t val = memory[targetAddress];
			val -= 0x1;
			memory[targetAddress] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x3;
			return;
		}
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
			{
				uint8_t pcRightByte = (uint8_t)pc;
				uint8_t targetAddress = firstByteOfInterest + 2;
				targetAddress += pcRightByte;
				pc = (pc & 0xFF00) + targetAddress;
			}
			else pc += 0x2;
			return;
		}
		case 0x1: // CMP_INDIRECT_Y
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t tmp = firstByteOfInterest;
			tmp += 1;
			uint16_t targetAddress = memory[tmp];
			tmp -= 1;
			targetAddress <<= 8;
			targetAddress |= memory[tmp];
			targetAddress += y;
			uint8_t diff = a - memory[targetAddress];

			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;



			if (a == memory[targetAddress]) zeroFlag = 1;
			else zeroFlag = 0;
			if (a >= memory[targetAddress]) carryFlag = 1;
			else carryFlag = 0;
			if (a < memory[targetAddress]) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return; // 0xD2
		case 0x3: // DCP_INDIRECT_Y
		{
			uint16_t targetAddress;
			uint8_t tmp = a;
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;

			uint8_t val = memory[targetAddress];
			val -= 0x1;
			memory[targetAddress] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x5: // CMP_ZERO_PAGE_X
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t targetAddress = firstByteOfInterest + x;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
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
		case 0x7: // DCP_ZERO_PAGE_X
		{
			uint8_t targetAddress = firstByteOfInterest + x;
			uint8_t val = memory[targetAddress];
			val -= 0x1;
			memory[targetAddress] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x8: // CLEAR_DECIMAL
		{
			decimalFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // CMP_ABSOLUTE_Y
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x3;
			return;
		}
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: // DCP_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			uint8_t val = memory[targetAddress];
			val -= 0x1;
			memory[targetAddress] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x3;
			return;
		}
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
		case 0xD: // CMP_ABSOLUTE_X
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
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
			if (tmp == 0) zeroFlag = 1;
			else zeroFlag = 0;
			tmp >>= 7;
			if (tmp == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // DCP_ABSOLUTE_X
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			uint8_t val = memory[targetAddress];
			val -= 0x1;
			memory[targetAddress] = val;

			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint8_t diff = a - memory[targetAddress];
			diff >>= 7;

			if (a < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (a == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (a > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x3;
			return;
		}
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibE(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: // CPX_IMMEDIATE
		{
			negativeFlag = 0;
			carryFlag = 0;
			zeroFlag = 0;
			uint8_t diff = x - firstByteOfInterest;
			diff >>= 7;

			if (x < firstByteOfInterest && diff == 1) negativeFlag = 1;

			if (x == firstByteOfInterest)
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (x > firstByteOfInterest)
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x1: // SBC_INDIRECT_X
		{
			carryFlag = 1;
			uint8_t tmp = a;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			if (memory[targetAddress] == 0)
			{
				a -= 1;
			}
			else if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[targetAddress];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x3: // INS_INDIRECT_X
		{
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest + x;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			uint8_t val = memory[targetAddress];
			val += 0x1;
			memory[targetAddress] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a -= (memory[targetAddress] + 1 );
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x4: // CPX_ZERO_PAGE
		{
			negativeFlag = 0;
			carryFlag = 0;
			zeroFlag = 0;
			uint8_t diff = x - memory[firstByteOfInterest];
			diff >>= 7;

			if (x < memory[firstByteOfInterest] && diff == 1) negativeFlag = 1;

			if (x == memory[firstByteOfInterest])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (x > memory[firstByteOfInterest])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x2;
			return;
		}
		case 0x5: // SBC_ZERO_PAGE
		{
			uint8_t tmp = a;
			carryFlag = 1;
			if (memory[firstByteOfInterest] == 0)
			{
				a -= 1;
			}

			else if (a >= memory[firstByteOfInterest])
			{
				a -= memory[firstByteOfInterest];
			}
			else if (memory[firstByteOfInterest] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[firstByteOfInterest];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[firstByteOfInterest]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
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
		case 0x7: // INS_ZERO_PAGE
		{
			uint8_t val = memory[firstByteOfInterest];
			val += 0x1;
			memory[firstByteOfInterest] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[firstByteOfInterest])
			{
				a -= memory[firstByteOfInterest];
			}
			else if (memory[firstByteOfInterest] > a)
			{
				carryFlag = 0;
				a -= (memory[firstByteOfInterest] + 1);
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[firstByteOfInterest]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x8: // INX
		{
			x += 0x1;
			if (x == 0) zeroFlag = 1;
			else zeroFlag = 0;
			if ((x >> 7) == 1) negativeFlag = 1;
			else negativeFlag = 0;
			pc += 0x1;
			return;
		}
		case 0x9: // SBC_IMMEDIATE
		{
			uint8_t tmp = a;
			carryFlag = 1;
			if (firstByteOfInterest == 0)
			{
				a -= 1;
			}

			else if (a >= firstByteOfInterest)
			{
				a -= firstByteOfInterest;
			}
			else if (firstByteOfInterest > a)
			{
				carryFlag = 0;
				a = (a + 256) - firstByteOfInterest;
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ firstByteOfInterest) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
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
		case 0xB: // SBC_IMMEDIATE_DUP
		{
			uint8_t tmp = a;
			carryFlag = 1;
			if (firstByteOfInterest == 0)
			{
				a -= 1;
			}

			else if (a >= firstByteOfInterest)
			{
				a -= firstByteOfInterest;
			}
			else if (firstByteOfInterest > a)
			{
				carryFlag = 0;
				a = (a + 256) - firstByteOfInterest;
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ firstByteOfInterest) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0xC: // CPX_ABSOLUTE
		{
			negativeFlag = 0;
			zeroFlag = 0;
			carryFlag = 0;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			uint8_t diff = x - memory[targetAddress];
			diff >>= 7;

			if (x < memory[targetAddress] && diff == 1) negativeFlag = 1;

			if (x == memory[targetAddress])
			{
				zeroFlag = 1;
				carryFlag = 1;
			}

			if (x > memory[targetAddress])
			{
				carryFlag = 1;
				if (diff == 1) negativeFlag = 1;
			}
			pc += 0x3;
			return;
		}
		case 0xD: // SBC_ABSOLUTE
		{
			uint8_t tmp = a;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			carryFlag = 1;
			if (memory[targetAddress] == 0)
			{
				a -= 1;
			}

			else if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[targetAddress];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ firstByteOfInterest) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
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
			if (memory[tmpAddress] == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xF: // INS_ABSOLUTE
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			uint8_t val = memory[targetAddress];
			val += 0x1;
			memory[targetAddress] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a -= (memory[targetAddress] + 1);
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
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
			carryFlag = 1;
			uint8_t tmp = a;
			uint8_t firstIndex;
			uint8_t secondIndex;
			uint16_t targetAddress = 0;
			uint8_t indexLocation = firstByteOfInterest;
			firstIndex = memory[indexLocation];
			indexLocation += 0x1;
			secondIndex = memory[indexLocation];
			targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;
			if (memory[targetAddress] == 0)
			{
				a -= 1;
			}
			else if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[targetAddress];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x2: return; // 0xF2
		case 0x3: // INS_INDIRECT_Y
		{
			uint8_t byte = firstByteOfInterest;
			uint8_t firstIndex = memory[byte];
			byte += 1;
			uint8_t secondIndex = memory[byte];
			uint16_t targetAddress = secondIndex;
			targetAddress <<= 8;
			targetAddress |= firstIndex;
			targetAddress += y;
			uint8_t val = memory[targetAddress];
			val += 0x1;
			memory[targetAddress] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a -= (memory[targetAddress] + 1);
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x4: // SKB
		{
			pc += 0x2;
			return;
		}
		case 0x5: // SBC_ZERO_PAGE_X
		{
			uint8_t tmp = a;
			uint8_t targetAddress = firstByteOfInterest + x;
			carryFlag = 1;
			if (memory[targetAddress] == 0)
			{
				a -= 1;
			}

			else if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[targetAddress];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
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
		case 0x7: // INS_ZERO_PAGE_X
		{
			uint8_t targetAddress = firstByteOfInterest + x;
			uint8_t val = memory[targetAddress];
			val += 0x1;
			memory[targetAddress] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a -= (memory[targetAddress] + 1);
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x2;
			return;
		}
		case 0x8: // SET_DECIMAL
		{
			decimalFlag = 1;
			pc += 0x1;
			return;
		}
		case 0x9: // SBC_ABSOLUTE_Y
		{
			uint8_t tmp = a;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			carryFlag = 1;
			if (memory[targetAddress] == 0)
			{
				a -= 1;
			}

			else if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[targetAddress];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xA: // NOP
		{
			pc += 0x1;
			return;
		}
		case 0xB: // INS_ABSOLUTE_Y
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += y;
			uint8_t val = memory[targetAddress];
			val += 0x1;
			memory[targetAddress] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a -= (memory[targetAddress] + 1);
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		case 0xC: // SKW
		{
			pc += 0x3;
			return;
		}
		case 0xD: // SBC_ABSOLUTE_X
		{
			uint8_t tmp = a;
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			carryFlag = 1;
			if (memory[targetAddress] == 0)
			{
				a -= 1;
			}

			else if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a = (a + 256) - memory[targetAddress];
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ firstByteOfInterest) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
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
		case 0xF: // INS_ABSOLUTE_X
		{
			uint16_t targetAddress = secondByteOfInterest;
			targetAddress <<= 8;
			targetAddress |= firstByteOfInterest;
			targetAddress += x;
			uint8_t val = memory[targetAddress];
			val += 0x1;
			memory[targetAddress] = val;

			uint8_t tmp = a;
			carryFlag = 1;

			if (a >= memory[targetAddress])
			{
				a -= memory[targetAddress];
			}
			else if (memory[targetAddress] > a)
			{
				carryFlag = 0;
				a -= (memory[targetAddress] + 1);
			}
			if ((a >> 7) == 0b00000001) negativeFlag = 1;
			else negativeFlag = 0;
			if (((tmp ^ memory[targetAddress]) & 0x80) != 0 && ((tmp ^ a) & 0x80) != 0) overflowFlag = 1;
			else overflowFlag = 0;
			if (a == 0) zeroFlag = 1;
			else zeroFlag = 0;
			pc += 0x3;
			return;
		}
		default: std::cout << "I don't know what this is.\n";
	}
}
