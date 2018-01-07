#include "Globals.h"
#include "Input.h"

namespace
{
	int buttonRef[8] = {0};
	bool buttonPress[8] = {false};
	InputType::IT reBind = InputType::IT::Invalid;

	int leftClick = 0;
	int rightClick = 0;
	int mx = 0, my = 0;
}

namespace Globals
{
	sf::Keyboard::Key keyBinding[8] = {sf::Keyboard::X, sf::Keyboard::Z, sf::Keyboard::Return, sf::Keyboard::Space, sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right};
}

void InputPress(InputType::IT button)
{
	buttonPress[button] = true;
	++buttonRef[button];
}

void InputPress(sf::Keyboard::Key key)
{
	if(reBind != InputType::IT::Invalid)
	{
		Globals::keyBinding[reBind] = key;
		reBind = InputType::IT::Invalid;
	}

	for(int i=0 ; i<sizeof(Globals::keyBinding) ; ++i)
	{
		if(key == Globals::keyBinding[i])
		{
			InputPress(InputType::IT(i));
			break;
		}
	}
}

void InputUnPress(InputType::IT button)
{
	if(buttonRef[button] > 0)
		if(--buttonRef[button] == 0)
			buttonPress[button] = false;
}

void InputUnPress(sf::Keyboard::Key key)
{
	for(int i=0 ; i<sizeof(Globals::keyBinding) ; ++i)
	{
		if(key == Globals::keyBinding[i])
		{
			InputUnPress(InputType::IT(i));
			break;
		}
	}
}

void InputResetAll(void)
{
	std::memset(buttonPress, 0, sizeof(buttonPress));
	std::memset(buttonRef, 0, sizeof(buttonRef));
	leftClick = rightClick = 0;
}

void InputResetPress(void)
{
	std::memset(buttonPress, 0, sizeof(buttonPress));
	if(leftClick == 1) leftClick = 2;
	if(rightClick == 1) rightClick = 2;
}

void InputReBind(InputType::IT button)
{
	reBind = button;
}

void InputSetBind(InputType::IT button, sf::Keyboard::Key key)
{
	Globals::keyBinding[button] = key;
}

bool InputIsPressed(InputType::IT button)
{
	return buttonPress[button];
}

bool InputIsHeld(InputType::IT button)
{
	return buttonRef[button] > 0;
}

void InputClickLeft(bool press)
{
	if(press)
		leftClick = 1;
	else
		leftClick = 0;
}

void InputClickRight(bool press)
{
	if(press)
		rightClick = 1;
	else
		rightClick = 0;
}

void InputSetMousePos(int x, int y)
{
	mx = x;
	my = y;
}

bool InputIsLeftClick(void)
{
	return leftClick == 1;
}

bool InputIsLeftHeld(void)
{
	return leftClick > 0;
}

bool InputIsRightClick(void)
{
	return rightClick == 1;
}

bool InputIsRightHeld(void)
{
	return rightClick > 0;
}

int InputGetMouseX(void)
{
	return mx;
}

int InputGetMouseY(void)
{
	return my;
}
