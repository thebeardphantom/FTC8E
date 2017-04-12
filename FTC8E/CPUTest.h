#pragma once
#include "CPU.h"

class CPUTest
{
public:
	CPUTest();
	~CPUTest();

	void runTests();
private:
	CPU cpu;
};

