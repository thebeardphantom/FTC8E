#include "CPUTest.h"

CPUTest::CPUTest()
{
	cpu = CPU();
}
CPUTest::~CPUTest() {}

void CPUTest::runTests()
{
	cpu.draw = true;
	cpu.pc = 0;
	cpu.v[0xA] = 0x50;
	cpu.memory[0] = 0xA0;
	cpu.memory[1] = 0x05;
	cpu.tick();
	printf("Testing complete");
}