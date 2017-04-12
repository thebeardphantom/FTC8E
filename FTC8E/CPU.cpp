#include "CPU.h"
#include <iostream>
#include <string>
#include <time.h>
#include <iomanip>
#include <sstream>

float CPU::CLOCK = 1.0f / 600.0f;

CPU::CPU()
{
	// Clear
	pc = 0x200;
	opcode = 0;
	index = 0;
	sp = 0;
	delayTimer = 0;
	soundTimer = 0;
	draw = true;
	srand(time(NULL));
	for (auto& i : memory)
	{
		i = 0;
	}
	for (auto& i : stack)
	{
		i = 0;
	}
	for (auto& i : gfx)
	{
		i = 0;
	}
	for (auto& i : v)
	{
		i = 0;
	}
	for (auto& i : keypad)
	{
		i = 0;
	}
	for (int i = 0; i < chip8_fontset.size(); i++)
	{
		memory[i] = chip8_fontset[i];
	}
}

void CPU::loadROM(char* path)
{
	FILE* rom = fopen(path, "rb");
	int c = 0;
	unsigned int i = 0;
	while ((c = fgetc(rom)) != EOF)
	{
		memory[512 + i] = c;
		i++;
	}
	fclose(rom);
}
reg8 CPU::rand()
{
	return std::rand() % 0xFF;
}
void CPU::tick()
{
	bool foundOpCode = true;
	bool incPC = true;

	opcode = memory[pc] << 8 | memory[pc + 1];
	switch (opcode & 0xF000)
	{
		case 0x0000:
		{
			// 00EX codes
			switch (opcode & 0x000F)
			{
				case 0x0000:
				{
					// 00E0
					for (auto& px : gfx)
					{
						px = 0;
					}
					draw = true;
					break;
				}
				case 0x000E:
				{
					// 00EE
					sp--;
					pc = stack[sp];
					break;
				}
				default:
				{
					foundOpCode = false;
					break;
				}
			}
			break;
		}
		case 0x1000:
		{
			// 1NNN
			pc = opcode & 0x0FFF;
			incPC = false;
			break;
		}
		case 0x2000:
		{
			// 2NNN
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
			incPC = false;
			break;
		}
		case 0x3000:
		{
			// 3XNN
			reg8 x = (opcode & 0x0F00) >> 8;
			reg8 val = (opcode & 0x00FF);
			if (v[x] == val)
			{
				pc += 2;
			}
			break;
		}
		case 0x4000:
		{
			// 4XNN
			reg8 x = (opcode & 0x0F00) >> 8;
			if (v[x] != opcode & 0x00FF)
			{
				pc += 2;
			}
			break;
		}
		case 0x5000:
		{
			// 5XY0
			reg8 x = (opcode & 0x0F00) >> 8;
			reg8 y = (opcode & 0x00F0) >> 4;
			if (v[x] == v[y])
			{
				pc += 2;
			}
			break;
		}
		case 0x6000:
		{
			// 6XNN
			reg8 x = (opcode & 0x0F00) >> 8;
			v[x] = opcode & 0x00FF;
			break;
		}
		case 0x7000:
		{
			// 7XNN
			reg8 x = (opcode & 0x0F00) >> 8;
			v[x] += opcode & 0x00FF;
			break;
		}
		case 0x8000:
		{
			// 8XXX opcodes
			switch (opcode & 0x000F)
			{
				case 0x0000:
				{
					// 8XY0
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					v[x] = v[y];
					break;
				}
				case 0x0001:
				{
					// 8XY1
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					v[x] |= v[y];
					break;
				}
				case 0x0002:
				{
					// 8XY2
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					v[x] &= v[y];
					break;
				}
				case 0x0003:
				{
					// 8XY3
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					v[x] ^= v[y];
					break;
				}
				case 0x0004:
				{
					// 8XY4
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					if (v[y] > (0xFF - v[x]))
					{
						v[0xF] = 1;
					}
					else
					{
						v[0xF] = 0;
					}
					v[x] += v[y];
					break;
				}
				case 0x0005:
				{
					// 8XY5
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					if (v[x] < v[y])
					{
						v[0xF] = 0;
					}
					else
					{
						v[0xF] = 1;
					}
					v[x] -= v[y];
					break;
				}
				case 0x0006:
				{
					// 8XY6
					reg8 x = (opcode & 0x0F00) >> 8;
					v[0xF] = v[x] & 0x1;
					v[x] >>= 1;
					break;
				}
				case 0x0007:
				{
					// 8XY7
					reg8 x = (opcode & 0x0F00) >> 8;
					reg8 y = (opcode & 0x00F0) >> 4;
					if (v[x] > v[y])
					{
						v[0xF] = 0;
					}
					else
					{
						v[0xF] = 1;
					}
					v[x] = v[y] - v[x];
					break;
				}
				case 0x000E:
				{
					// 8XYE
					reg8 x = (opcode & 0x0F00) >> 8;
					v[0xF] = v[x] >> 7;
					v[x] <<= 1;
					break;
				}
				default:
				{
					foundOpCode = false;
					break;
				}
			}
			break;
		}
		case 0x9000:
		{
			// 9XY0
			reg8 x = (opcode & 0x0F00) >> 8;
			reg8 y = (opcode & 0x00F0) >> 4;
			if (v[x] != v[y])
			{
				pc += 2;
			}
			break;
		}
		case 0xA000:
		{
			// ANNN
			index = opcode & 0x0FFF;
			break;
		}
		case 0xB000:
		{
			// BNNN
			pc = (opcode & 0x0FFF) + v[0];
			incPC = false;
			break;
		}
		case 0xC000:
		{
			// CXNN
			reg8 x = (opcode & 0x0F00) >> 8;
			v[x] = rand() & (opcode & 0x00FF);
			break;
		}
		case 0xD000:
		{
			// DXYN
			reg8 vx = v[(opcode & 0x0F00) >> 8];
			reg8 vy = v[(opcode & 0x00F0) >> 4];
			reg16 height = opcode & 0x000F;
			reg16 row;
			v[0xF] = 0;
			for (int h = 0; h < height; h++)
			{
				row = memory[index + h];
				for (int w = 0; w < 8; w++)
				{
					if ((row & (0x80 >> w)) != 0)
					{
						unsigned int coord = (vx + w + ((vy + h) * 64)) % CANVAS_SIZE;
						gfx[coord] ^= 1;
						if (gfx[coord] == 0)
						{
							v[0xF] |= 1;
						}
					}
				}
			}
			draw = v[0xF] == 0;
			break;
		}
		case 0xE000:
		{
			switch (opcode & 0x00FF)
			{
				case 0x009E:
				{
					// EX9E
					reg8 x = (opcode & 0x0F00) >> 8;
					if (keypad[v[x]] != 0x0)
					{
						pc += 2;
					}
					break;
				}
				case 0x00A1:
				{
					// EXA1
					reg8 x = (opcode & 0x0F00) >> 8;
					if (keypad[v[x]] == 0x0)
					{
						pc += 2;
					}
					break;
				}
				default:
				{
					foundOpCode = false;
					break;
				}
			}
			break;
		}
		case 0xF000:
		{
			switch (opcode & 0x00FF)
			{
				case 0x0007:
				{
					// FX07
					reg8 x = (opcode & 0x0F00) >> 8;
					v[x] = delayTimer;
					break;
				}
				case 0x000A:
				{
					// FX0A
					bool didPress = false;
					for (const auto k : keypad)
					{
						if (k != 0)
						{
							reg8 x = (opcode & 0x0F00) >> 8;
							v[x] = k;
							didPress = true;
							break;
						}
					}
					incPC = didPress;
					break;
				}
				case 0x0015:
				{
					// FX15
					reg8 x = (opcode & 0x0F00) >> 8;
					delayTimer = v[x];
					break;
				}
				case 0x0018:
				{
					// FX18
					reg8 x = (opcode & 0x0F00) >> 8;
					soundTimer = v[x];
					break;
				}
				case 0x001E:
				{
					// FX1E
					reg8 x = (opcode & 0x0F00) >> 8;
					if (index + v[x] > 0xFFF)
					{
						v[x] = 1;
					}
					else
					{
						v[x] = 0;
					}
					index += v[x];
					break;
				}
				case 0x0029:
				{
					// FX29
					reg8 x = (opcode & 0x0F00) >> 8;
					index = v[x] * 0x5;
					break;
				}
				case 0x0033:
				{
					// FX33
					reg8 x = (opcode & 0x0F00) >> 8;
					memory[index] = v[x] / 100;
					memory[index + 1] = (v[x] / 10) % 10;
					memory[index + 2] = (v[x] % 100) % 10;
					break;
				}
				case 0x0055:
				{
					// FX55
					reg8 x = (opcode & 0x0F00) >> 8;
					for (reg8 i = 0; i <= x; i++)
					{
						memory[index + i] = v[i];
					}
					index += v[x] + 1;
					break;
				}
				case 0x0065:
				{
					// FX65
					reg8 x = (opcode & 0x0F00) >> 8;
					for (reg8 i = 0; i <= x; i++)
					{
						v[i] = memory[index + i];
					}
					index += v[x] + 1;
					break;
				}
				default:
				{
					foundOpCode = false;
					break;
				}
			}
			break;
		}
		default:
		{
			foundOpCode = false;
			break;
		}
	}
	if (incPC)
	{
		pc += 2;
	}

	unsigned char printMode = 2;
	switch (printMode)
	{
		case 1:
		{
			system("cls");
			std::cout << "PC: " << std::uppercase << std::hex << pc;
			std::cout << '\n';
			std::cout << "Index: " << std::uppercase << std::hex << index;
			std::cout << '\n';
			for (int i = 0; i < v.size(); i++)
			{
				std::cout << "V[" << std::uppercase << std::hex << i << "] = " << std::hex << ((int)v[i]);
				std::cout << '\n';
			}
			break;
		}
		case 2:
		{
			//if (foundOpCode)
			//{
			//	printf("OPCODE: 0x%X\n", opcode);
			//}
			if (!foundOpCode)
			{
				printf("INVALID OPCODE: 0x%X\n", opcode);
			}
			//}
			break;
		}
		default:
			break;
	}

	// Update Timers
	if (delayTimer > 0)
	{
		delayTimer--;
	}
	if (soundTimer > 0)
	{
		if (soundTimer == 1)
		{
			printf("BEEP\n");
		}
		soundTimer--;
	}
}