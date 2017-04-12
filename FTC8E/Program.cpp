#define VERBOSE
#define DEBUG

#include <Program.h>
#include <SFML/Main.hpp>
#include <SFML/Graphics/Image.hpp>
#include "CPUTest.h"
#include <stdio.h>
#include <string>

sf::RenderWindow* Program::window;
CPU Program::cpu;
bool Program::restart = true;

int main(int argc, char* args[])
{
	CPUTest test;
	test.runTests();
	while (Program::restart)
	{
		Program::mainLoop();
	}
	return 0;
}
float lerp(float x1, float x2, float t)
{
	return x1 + (x2 - x1)*t;
}
void Program::updateInput(sf::Event evt)
{
	if (evt.key.code == sf::Keyboard::Escape)
	{
		window->close();
	}
	char value = evt.type == sf::Event::EventType::KeyPressed ? 1 : 0;
	switch (evt.key.code)
	{
		case sf::Keyboard::Num1:
		{
			cpu.keypad[0x1] = value;
			break;
		}
		case sf::Keyboard::Num2:
		{
			cpu.keypad[0x2] = value;
			break;
		}
		case sf::Keyboard::Num3:
		{
			cpu.keypad[0x3] = value;
			break;
		}
		case sf::Keyboard::Num4:
		{
			cpu.keypad[0xC] = value;
			break;
		}
		case sf::Keyboard::Q:
		{
			cpu.keypad[0x4] = value;
			break;
		}
		case sf::Keyboard::W:
		{
			cpu.keypad[0x5] = value;
			break;
		}
		case sf::Keyboard::E:
		{
			cpu.keypad[0x6] = value;
			break;
		}
		case sf::Keyboard::R:
		{
			cpu.keypad[0xD] = value;
			break;
		}
		case sf::Keyboard::A:
		{
			cpu.keypad[0x7] = value;
			break;
		}
		case sf::Keyboard::S:
		{
			cpu.keypad[0x8] = value;
			break;
		}
		case sf::Keyboard::D:
		{
			cpu.keypad[0x9] = value;
			break;
		}
		case sf::Keyboard::F:
		{
			cpu.keypad[0xE] = value;
			break;
		}
		case sf::Keyboard::Z:
		{
			cpu.keypad[0xA] = value;
			break;
		}
		case sf::Keyboard::X:
		{
			cpu.keypad[0x0] = value;
			break;
		}
		case sf::Keyboard::C:
		{
			cpu.keypad[0xB] = value;
			break;
		}
		case sf::Keyboard::V:
		{
			cpu.keypad[0xF] = value;
			break;
		}
		default:
		{
			break;
		}
	}
}

int Program::mainLoop()
{
	window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "FTC8E");
	window->setFramerateLimit(60);
	window->setVerticalSyncEnabled(false);
	cpu = CPU();
	cpu.loadROM("D:\\GameDev\\CPPProjects\\FTC8E\\Debug\\ROMs\\PONG2");

	sf::Texture texture;
	texture.create(CPU::CANVAS_WIDTH, CPU::CANVAS_HEIGHT);
	sf::Sprite sprite(texture);
	sprite.setScale(sf::Vector2f(SCALING, SCALING));
	sf::Clock timer;
	sf::Uint8 *colors = new sf::Uint8[cpu.gfx.size() * 4];
	while (window->isOpen())
	{
		// Input
		sf::Event evt;
		while (window->pollEvent(evt))
		{
			if (evt.type == sf::Event::Closed)
			{
				restart = false;
				window->close();
			}
			if (evt.type == sf::Event::KeyPressed || evt.type == sf::Event::KeyReleased)
			{
				updateInput(evt);
			}
		}
		// CPU Tick
		if (timer.getElapsedTime().asSeconds() >= CPU::CLOCK)
		{
			timer.restart();
			cpu.tick();
		}
		// Draw
		if (cpu.draw)
		{
			for (unsigned int i = 0; i < cpu.gfx.size(); i++)
			{
				if (cpu.gfx[i] == 1)
				{
					reg8 c = lerp(255, 0, (float)i / cpu.gfx.size());
					colors[i * 4] = c;
					colors[(i * 4) + 1] = 0xFF;
					colors[(i * 4) + 2] = 0xFF;
					colors[(i * 4) + 3] = 0xFF;
				}
				else
				{
					colors[(i * 4)] = 0;
					colors[(i * 4) + 1] = 0;
					colors[(i * 4) + 2] = 0;
					colors[(i * 4) + 3] = 0;
				}
			}
			texture.update(colors);
			cpu.draw = false;
			window->clear();
			window->draw(sprite);
			window->display();
		}
	}
	delete[] colors;
	delete window;
	return 0;
}