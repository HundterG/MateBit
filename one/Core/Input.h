#ifndef _INPUT_
#define _INPUT_
#include <SFML/Window/Keyboard.hpp>

namespace Globals
{
	extern sf::Keyboard::Key keyBinding[8];
}

struct InputType
{
	enum IT
	{
		Invalid = -1,
		A,
		B,
		Start,
		Select,
		Up,
		Down,
		Left,
		Right
	};
};

void InputPress(InputType::IT button);
void InputPress(sf::Keyboard::Key key);
void InputUnPress(InputType::IT button);
void InputUnPress(sf::Keyboard::Key key);
void InputResetAll(void);
void InputResetPress(void);
void InputReBind(InputType::IT button);
void InputSetBind(InputType::IT button, sf::Keyboard::Key key);

bool InputIsPressed(InputType::IT button);
bool InputIsHeld(InputType::IT button);

void InputClickLeft(bool press);
void InputClickRight(bool press);
void InputSetMousePos(int x, int y);

bool InputIsLeftClick(void);
bool InputIsLeftHeld(void);
bool InputIsRightClick(void);
bool InputIsRightHeld(void);
int InputGetMouseX(void);
int InputGetMouseY(void);

#endif
