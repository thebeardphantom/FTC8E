#pragma once
#include <SFML/Graphics.hpp>
#include <CPU.h>

class Program
{
public:
	static const int SCALING = 15;
	static const int WINDOW_WIDTH = CPU::CANVAS_WIDTH * SCALING;
	static const int WINDOW_HEIGHT = CPU::CANVAS_HEIGHT * SCALING;

	static bool restart;
	CPU static cpu;

	int static mainLoop();
private:
	static sf::RenderWindow* window;
	static void updateInput(sf::Event evt);
};