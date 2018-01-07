#include "Globals.h"
#include "Input.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <chrono>

namespace
{
	struct LazyGlobals
	{
		sf::RenderWindow win;
		sf::Texture gpuBuffer;
		sf::Sprite gpuSprite;
	};

	bool winFullScreen = false;
	std::unique_ptr<unsigned char[]> cpuBuffer(new unsigned char[65536*4]);
	LazyGlobals *lGP = nullptr;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	void CreateWindow(void)
	{
		// setup window
		sf::Uint32 style = sf::Style::Default;
		sf::VideoMode vm(Globals::windowSize.x,Globals::windowSize.y);
		if(Globals::fullScreen)
			vm = sf::VideoMode(Globals::fullScreenSize.x,Globals::fullScreenSize.y);

		if(vm.isValid() && Globals::fullScreen)
			style = sf::Style::Fullscreen;
		else if(!vm.isValid() && Globals::fullScreen)
		{
			Globals::fullScreen = false;
			vm = sf::VideoMode(Globals::windowSize.x,Globals::windowSize.y);
		}

		lGP->win.create(vm, "The One", style);
		lGP->win.setFramerateLimit(60);
		lGP->win.setMouseCursorVisible(false);
		lGP->win.setKeyRepeatEnabled(false);
		lGP->win.setView( sf::View(sf::Vector2f(0, 0), sf::Vector2f(float(256), float(256))) );
		winFullScreen = Globals::fullScreen;
	}
}

bool WindowInit(void)
{
	// allocate lazy globals
	lGP = new LazyGlobals();
	hardcheck(!lGP, "Globals could not be allocated.");

	// copy fullscreen sizes
	auto full = sf::VideoMode::getFullscreenModes().begin();
	softcheck(full == sf::VideoMode::getFullscreenModes().end(), "No fullscreen mode avalible.", Globals::fullScreen = false;)
	else
	{
		Globals::fullScreenSize.x = full->width;
		Globals::fullScreenSize.y = full->height;
	}
	if(Globals::aspectRatio == Globals::Aspect::x34)
		Globals::windowSize.x = 1024;

	// setup window
	CreateWindow();
	hardcheck(!lGP->gpuBuffer.create(256, 256), "BackBuffer could not be allocated.");
	hardcheck(!cpuBuffer, "BackBuffer could not be allocated.");
	lGP->gpuSprite.setTexture(lGP->gpuBuffer);
	lGP->gpuSprite.setOrigin(sf::Vector2f(128, 128));
	return false;
}

void WindowExit(void)
{
	lGP->win.close();
	delete lGP;
	lGP = nullptr;
}

void WindowUpdate(void)
{
	// joystick axis to button bools
	static bool axisButton[4] = {false, false, false, false};

	// change window based on settings
	if(Globals::fullScreen != winFullScreen)
	{
		CreateWindow();
		// input will be dropped when switch happens so reset input status
		InputResetAll();
	}

	sf::Event ent;
	while(lGP->win.pollEvent(ent))
	{
		switch(ent.type)
		{
		// system
		case sf::Event::Closed:
			lGP->win.close();
			Globals::quit = true;
			break;
		case sf::Event::Resized:
		{
			Globals::windowSize.x = ent.size.width;
			Globals::windowSize.y = ent.size.height;
			break;
		}
		case sf::Event::LostFocus:
			InputResetAll();
			break;

		// mouse
		case sf::Event::MouseButtonPressed:
			if(ent.mouseButton.button == sf::Mouse::Left) InputClickLeft(true);
			if(ent.mouseButton.button == sf::Mouse::Right) InputClickRight(true);
			break;
		case sf::Event::MouseButtonReleased:
			if(ent.mouseButton.button == sf::Mouse::Left) InputClickLeft(false);
			if(ent.mouseButton.button == sf::Mouse::Right) InputClickRight(false);
			break;
		case sf::Event::MouseMoved:
			if(Globals::aspectRatio == Globals::Aspect::xany)
				InputSetMousePos(int(ent.mouseMove.x/(Globals::windowSize.x/256.0f)), int(ent.mouseMove.y/(Globals::windowSize.y/256.0f)));
			else
			{
				float w = 1, h = 1, x = float(Globals::windowSize.x), y = float(Globals::windowSize.y);
				if(Globals::aspectRatio == Globals::Aspect::x34)
				{
					w = 4;
					h = 3;
				}
				if(Globals::fullScreen)
				{
					x = float(Globals::fullScreenSize.x);
					y = float(Globals::fullScreenSize.y);
				}
				if(float(x)/y > w/h)
					InputSetMousePos( int((ent.mouseMove.x-((x-(y*(w/h)))/2.0f))/(y/256.0f*(w/h))) , int(ent.mouseMove.y/(y/256.0f)));
				else
					InputSetMousePos(int(ent.mouseMove.x/(x/256.0f)), int((ent.mouseMove.y-((y-(x*(h/w)))/2.0f))/(x/256.0f*(h/w))));
			}
			break;

		// keyboard
		case sf::Event::KeyPressed:
			switch(ent.key.code)
			{
			case sf::Keyboard::Escape: Globals::quit = true; break;
			default: InputPress(ent.key.code);
			}
			break;
		case sf::Event::KeyReleased:
			InputUnPress(ent.key.code);
			break;

		// gamepad
		case sf::Event::JoystickButtonPressed:
			if(ent.joystickButton.joystickId == 0)
			{
				switch(ent.joystickButton.button)
				{
				case 0: /* A button */ InputPress(InputType::A); break;
				case 1: /* B button */ InputPress(InputType::B); break;
				case 2: case 6: /* Select button */ InputPress(InputType::Select); break;
				case 3: case 7: /* Start button */ InputPress(InputType::Start); break;
				}
			}
			break;

		case sf::Event::JoystickButtonReleased:
			if(ent.joystickButton.joystickId == 0)
			{
				switch(ent.joystickButton.button)
				{
				case 0: /* A button */ InputUnPress(InputType::A); break;
				case 1: /* B button */ InputUnPress(InputType::B); break;
				case 2: case 6: /* Select button */ InputUnPress(InputType::Select); break;
				case 3: case 7: /* Start button */ InputUnPress(InputType::Start); break;
				}
			}
			break;

		case sf::Event::JoystickMoved:
			if(ent.joystickMove.joystickId == 0)
			{
				float pos = ent.joystickMove.position;
				switch(ent.joystickMove.axis)
				{
				case 0: case 6: // left and right
					if(-20.0f < pos && pos < 20.0f)
					{
						if(axisButton[0] == true)
						{
							InputUnPress(InputType::Left);
							axisButton[0] = false;
						}
						if(axisButton[1] == true)
						{
							InputUnPress(InputType::Right);
							axisButton[1] = false;
						}
					}
					else if(pos < 0.0f)
					{
						if(axisButton[0] == false)
						{
							InputPress(InputType::Left);
							axisButton[0] = true;
						}
					}
					else
					{
						if(axisButton[1] == false)
						{
							InputPress(InputType::Right);
							axisButton[1] = true;
						}
					}
					break;

				case 1:
					pos *= -1.0f;
				case 7: // up and down
					if(-20.0f < pos && pos < 20.0f)
					{
						if(axisButton[2] == true)
						{
							InputUnPress(InputType::Down);
							axisButton[2] = false;
						}
						if(axisButton[3] == true)
						{
							InputUnPress(InputType::Up);
							axisButton[3] = false;
						}
					}
					else if(pos < 0.0f)
					{
						if(axisButton[2] == false)
						{
							InputPress(InputType::Down);
							axisButton[2] = true;
						}
					}
					else
					{
						if(axisButton[3] == false)
						{
							InputPress(InputType::Up);
							axisButton[3] = true;
						}
					}
					break;
				}
			}
			break;
		}
	}
}

void WindowDraw(void)
{
	lGP->win.clear();

	if(Globals::aspectRatio != Globals::Aspect::xany)
	{
		float w = 1, h = 1, x = float(Globals::windowSize.x), y = float(Globals::windowSize.y);
		if(Globals::aspectRatio == Globals::Aspect::x34)
		{
			w = 4;
			h = 3;
		}
		if(Globals::fullScreen)
		{
			x = float(Globals::fullScreenSize.x);
			y = float(Globals::fullScreenSize.y);
		}
		if(float(x)/y > w/h)
			lGP->gpuSprite.setScale(((y/(256*h)) * (256*w))/x, 1);
		else
			lGP->gpuSprite.setScale(1, ((x/(256*w)) * (256*h))/y);
	}
	else
		lGP->gpuSprite.setScale(1, 1);
	lGP->win.draw(lGP->gpuSprite);

	//std::cout << std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() << "\n";
	start = std::chrono::steady_clock::now();
	lGP->win.display();
}

void WindowPresentRequest(unsigned char buffer[65536])
{
	for(int i=0 ; i<65536 ; ++i)
	{
		// red
		switch((buffer[i] & 0xe0) >> 5)
		{
		case 0: cpuBuffer.get()[i*4] = 0; break;
		case 1: cpuBuffer.get()[i*4] = 36; break;
		case 2: cpuBuffer.get()[i*4] = 72; break;
		case 3: cpuBuffer.get()[i*4] = 108; break;
		case 4: cpuBuffer.get()[i*4] = 144; break;
		case 5: cpuBuffer.get()[i*4] = 180; break;
		case 6: cpuBuffer.get()[i*4] = 216; break;
		case 7: cpuBuffer.get()[i*4] = 255; break;
		}
		// green
		switch((buffer[i] & 0x1c) >> 2)
		{
		case 0: cpuBuffer.get()[i*4+1] = 0; break;
		case 1: cpuBuffer.get()[i*4+1] = 36; break;
		case 2: cpuBuffer.get()[i*4+1] = 72; break;
		case 3: cpuBuffer.get()[i*4+1] = 108; break;
		case 4: cpuBuffer.get()[i*4+1] = 144; break;
		case 5: cpuBuffer.get()[i*4+1] = 180; break;
		case 6: cpuBuffer.get()[i*4+1] = 216; break;
		case 7: cpuBuffer.get()[i*4+1] = 255; break;
		}
		// blue
		switch(buffer[i] & 0x03)
		{
		case 0: cpuBuffer.get()[i*4+2] = 0; break;
		case 1: cpuBuffer.get()[i*4+2] = 85; break;
		case 2: cpuBuffer.get()[i*4+2] = 170; break;
		case 3: cpuBuffer.get()[i*4+2] = 255; break;
		}
		// alpha
		cpuBuffer.get()[i*4+3] = 255;
	}
	lGP->gpuBuffer.update(cpuBuffer.get());
}

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

void WindowErrorMsg(char const *msg)
{
#if defined(_WIN32)
	std::string cmd("Sorry, the application has crashed.\nReason:\n\n");
	cmd += msg;
	MessageBoxA(NULL, cmd.c_str(), "Fatal Error", MB_ICONERROR);

#elif defined(__GNUG__)
	std::string cmd("zenity --title \"Fatal Error\" --error --text \"");
	cmd += "Sorry, the application has crashed.\nReason:\n\n";
	cmd += msg;
	cmd += "\" --width 300";
	std::system(cmd.c_str());
#else
#error "Must specify an error protocol for this arcetecture."
#endif
}
