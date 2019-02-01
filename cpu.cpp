#include "main.h"

CPU::CPU()
{
	std::cout << "CPU initializing..." << std::endl;

	a = 0x0;
	x = 0x0;
	y = 0x0;
	sp = 0xFD;
	pc = 0xFFFC;
	carryFlag = false;
	zeroFlag = false;
	interruptDisable = true;
	decimalFlag = false;
	bytePushed = false;
	bytePushedByInstruction = false;
	overflowFlag = false;
	negativeFlag = false;

	for (int i = 0x0; i <= 0xFFFF; i++)
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
	printf("A REGISTER: %#x\n", a);
	printf("X REGISTER: %#x\n", x);
	printf("Y REGISTER: %#x\n", y);
}

void CPU::loadCartridgeToMemory(Cartridge *cart)
{
	std::cout << "Loading Cartridge into CPU memory...\n";
	char tmp[1];
	int end = cart->getNumberOfBytes();
	for (int i = CARTRIDGE_MEMORY_START; i < end; i++)
	{
		cart->file.read(tmp, 1);
		memory[i] = *tmp;
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

void CPU::determineOpCode()
{
	uint8_t temp = memory[pc];
	firstByteOfInterest = memory[pc + 1];
	secondByteOfInterest = memory[pc + 2];

	uint8_t leftNib = temp & 0xF0;
	uint8_t rightNib = temp & 0x0F;

	switch (leftNib)
	{
		case 0x0: leftNib0(rightNib);
		case 0x1: leftNib1(rightNib);
		case 0x2: leftNib2(rightNib);
		case 0x3: leftNib3(rightNib);
		case 0x4: leftNib4(rightNib);
		case 0x5: leftNib5(rightNib);
		case 0x6: leftNib6(rightNib);
		case 0x7: leftNib7(rightNib);
		case 0x8: leftNib8(rightNib);
		case 0x9: leftNib9(rightNib);
		case 0xA: leftNibA(rightNib);
		case 0xB: leftNibB(rightNib);
		case 0xC: leftNibC(rightNib);
		case 0xD: leftNibD(rightNib);
		case 0xE: leftNibE(rightNib);
		case 0xF: leftNibF(rightNib);
		default: std::cout << "I don't know what this nib is.\n";
	}
}

void CPU::leftNib0(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
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
			if (memory[firstByteOfInterest] & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (memory[firstByteOfInterest] == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 2;
		}
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
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
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 1;
		}
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
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
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib1(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
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
			if (memory[tmpAddress] & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (memory[tmpAddress] == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 2;
		}
		case 0x7: return;
		case 0x8: return;
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
			if (memory[tmpAddress] & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (memory[tmpAddress] == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 3;
		}
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib2(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: // AND_INDIRECT_X
		{
			x += firstByteOfInterest;
			if (x > 0xFF)
			{
				x -= 0x100;
			}
			a &= memory[x];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 0x2;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: // BIT_ZERO_PAGE
		{

		}
		case 0x5: // AND_ZERO_PAGE
		{
			a &= memory[firstByteOfInterest];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 0x2;
		}
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: // AND_IMMEDIATE
		{
			a &= firstByteOfInterest;
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 0x2;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // AND_ABSOLUTE
		{
			uint16_t tmp = 0x0;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			a &= memory[tmp];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = true;
			else
				negativeFlag = false;
			if (a == 0x0)
				zeroFlag = true;
			else
				zeroFlag = false;
			pc += 3;
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
		case 0x0: return;
		case 0x1: // AND_INDIRECT_Y
		{
			y += memory[firstByteOfInterest];
			a &= y;
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 0x2; 
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // AND_ZERO_PAGE_X
		{
			uint8_t tmp = (x + firstByteOfInterest);

			a &= memory[tmp];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 2;
		}
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: // AND_ABSOLUTE_Y
		{
			uint16_t tmp = 0x00;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			tmp += y;
			a &= memory[tmp];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 3;
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
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			pc += 3;
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
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib5(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib6(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: // ADC_INDIRECT_X
		{
			uint8_t tmp = a;
			x += firstByteOfInterest;

			a += memory[x];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmp^a) & (memory[x]^a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // ADC_ZERO_PAGE
		{
			uint8_t tmp = a;
			a += memory[firstByteOfInterest];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmp^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
		}
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: // ADC_IMMEDIATE
		{
			uint8_t tmp = a;
			a += firstByteOfInterest;
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmp^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
		}
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: // ADC_ABSOLUTE
		{
			uint8_t tmpA = a;
			uint16_t tmp = 0x0;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			a += memory[tmp];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmpA^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 3;
		}
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib7(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: // ADC_INDIRECT_Y
		{
			uint8_t tmp = a;
			y += memory[firstByteOfInterest];
			a += y;
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 0;
			else
				zeroFlag = 1;
			if ((tmp^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmp + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 0x2;
		}
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: // ADC_ZERO_PAGE_X
		{
			uint8_t tmpA = a;
			uint8_t tmp = (x + firstByteOfInterest);

			a += memory[tmp];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmpA^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 2;
		}
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: // ADC_ABSOLUTE_Y
		{
			uint8_t tmpA = a;
			uint16_t tmp = 0x00;
			tmp |= secondByteOfInterest;
			tmp <<= 8;
			tmp |= firstByteOfInterest;
			tmp += y;
			a += memory[tmp];
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmpA^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 3;
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
			if (a & 0b10000000 == 0b10000000)
				negativeFlag = 1;
			else
				negativeFlag = 0;
			if (a == 0x0)
				zeroFlag = 1;
			else
				zeroFlag = 0;
			if ((tmpA^a) & (memory[x] ^ a) & 0x80 != 0)
				overflowFlag = 1;
			else
				overflowFlag = 0;
			if (tmpA + memory[x] > 0xFF)
				carryFlag = 1;
			else
				carryFlag = 0;
			pc += 3;
		}
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib8(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNib9(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibA(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibB(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibC(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibD(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibE(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}

void CPU::leftNibF(uint8_t rightNib)
{
	switch (rightNib)
	{
		case 0x0: return;
		case 0x1: return;
		case 0x2: return;
		case 0x3: return;
		case 0x4: return;
		case 0x5: return;
		case 0x6: return;
		case 0x7: return;
		case 0x8: return;
		case 0x9: return;
		case 0xA: return;
		case 0xB: return;
		case 0xC: return;
		case 0xD: return;
		case 0xE: return;
		case 0xF: return;
		default: std::cout << "I don't know what this is.\n";
	}
}
