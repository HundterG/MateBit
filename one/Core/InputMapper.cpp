#include "Input.h"
#include "InputMapper.h"

void InputMapper::Map(char const *key, sf::Keyboard::Key value)
{
	s2K.emplace(key, value);
	k2S.emplace(value, key);
}

InputMapper::InputMapper()
{
	Map("A", sf::Keyboard::A);
	Map("B", sf::Keyboard::B);
	Map("C", sf::Keyboard::C);
	Map("D", sf::Keyboard::D);
	Map("E", sf::Keyboard::E);
	Map("F", sf::Keyboard::F);
	Map("G", sf::Keyboard::G);
	Map("H", sf::Keyboard::H);
	Map("I", sf::Keyboard::I);
	Map("J", sf::Keyboard::J);
	Map("K", sf::Keyboard::K);
	Map("L", sf::Keyboard::L);
	Map("M", sf::Keyboard::M);
	Map("N", sf::Keyboard::N);
	Map("O", sf::Keyboard::O);
	Map("P", sf::Keyboard::P);
	Map("Q", sf::Keyboard::Q);
	Map("R", sf::Keyboard::R);
	Map("S", sf::Keyboard::S);
	Map("T", sf::Keyboard::T);
	Map("U", sf::Keyboard::U);
	Map("V", sf::Keyboard::V);
	Map("W", sf::Keyboard::W);
	Map("X", sf::Keyboard::X);
	Map("Y", sf::Keyboard::Y);
	Map("Z", sf::Keyboard::Z);

	Map("0", sf::Keyboard::Num0);
	Map("1", sf::Keyboard::Num1);
	Map("2", sf::Keyboard::Num2);
	Map("3", sf::Keyboard::Num3);
	Map("4", sf::Keyboard::Num4);
	Map("5", sf::Keyboard::Num5);
	Map("6", sf::Keyboard::Num6);
	Map("7", sf::Keyboard::Num7);
	Map("8", sf::Keyboard::Num8);
	Map("9", sf::Keyboard::Num9);
	Map("N0", sf::Keyboard::Numpad0);
	Map("N1", sf::Keyboard::Numpad1);
	Map("N2", sf::Keyboard::Numpad2);
	Map("N3", sf::Keyboard::Numpad3);
	Map("N4", sf::Keyboard::Numpad4);
	Map("N5", sf::Keyboard::Numpad5);
	Map("N6", sf::Keyboard::Numpad6);
	Map("N7", sf::Keyboard::Numpad7);
	Map("N8", sf::Keyboard::Numpad8);
	Map("N9", sf::Keyboard::Numpad9);

	Map("F1" , sf::Keyboard::F1	);
	Map("F2" , sf::Keyboard::F2	);
	Map("F3" , sf::Keyboard::F3	);
	Map("F4" , sf::Keyboard::F4	);
	Map("F5" , sf::Keyboard::F5	);
	Map("F6" , sf::Keyboard::F6	);
	Map("F7" , sf::Keyboard::F7	);
	Map("F8" , sf::Keyboard::F8	);
	Map("F9" , sf::Keyboard::F9	);
	Map("F10", sf::Keyboard::F10);
	Map("F11", sf::Keyboard::F11);
	Map("F12", sf::Keyboard::F12);
	Map("F13", sf::Keyboard::F13);
	Map("F14", sf::Keyboard::F14);
	Map("F15", sf::Keyboard::F15);

	Map("Escape"   , sf::Keyboard::Escape   );
	Map("LControl" , sf::Keyboard::LControl );
	Map("LShift"   , sf::Keyboard::LShift   );
	Map("LAlt"     , sf::Keyboard::LAlt     );
	Map("LSystem"  , sf::Keyboard::LSystem  );
	Map("RControl" , sf::Keyboard::RControl );
	Map("RShift"   , sf::Keyboard::RShift   );
	Map("RAlt"     , sf::Keyboard::RAlt     );
	Map("RSystem"  , sf::Keyboard::RSystem  );
	Map("Menu"     , sf::Keyboard::Menu     );
	Map("LBracket" , sf::Keyboard::LBracket );
	Map("RBracket" , sf::Keyboard::RBracket );
	Map("SemiColon", sf::Keyboard::SemiColon);
	Map("Comma"    , sf::Keyboard::Comma    );
	Map("Period"   , sf::Keyboard::Period   );
	Map("Quote"    , sf::Keyboard::Quote    );
	Map("Slash"    , sf::Keyboard::Slash    );
	Map("BackSlash", sf::Keyboard::BackSlash);
	Map("Tilde"    , sf::Keyboard::Tilde    );
	Map("Equal"    , sf::Keyboard::Equal    );
	Map("Dash"     , sf::Keyboard::Dash     );
	Map("Space"    , sf::Keyboard::Space    );
	Map("Return"   , sf::Keyboard::Return   );
	Map("BackSpace", sf::Keyboard::BackSpace);
	Map("Tab"      , sf::Keyboard::Tab      );
	Map("PageUp"   , sf::Keyboard::PageUp   );
	Map("PageDown" , sf::Keyboard::PageDown );
	Map("End"      , sf::Keyboard::End      );
	Map("Home"     , sf::Keyboard::Home     );
	Map("Insert"   , sf::Keyboard::Insert   );
	Map("Delete"   , sf::Keyboard::Delete   );
	Map("Add"      , sf::Keyboard::Add      );
	Map("Subtract" , sf::Keyboard::Subtract );
	Map("Multiply" , sf::Keyboard::Multiply );
	Map("Divide"   , sf::Keyboard::Divide   );
	Map("Left"     , sf::Keyboard::Left     );
	Map("Right"    , sf::Keyboard::Right    );
	Map("Up"       , sf::Keyboard::Up       );
	Map("Down"     , sf::Keyboard::Down     );
	Map("Pause"    , sf::Keyboard::Pause    );
}

char const *InputMapper::InputType2String(InputType::IT input)
{
	switch(input)
	{
	case InputType::A: return "A";
	case InputType::B: return "B";
	case InputType::Select: return "Select";
	case InputType::Start: return "Start";
	case InputType::Up: return "Up";
	case InputType::Down: return "Down";
	case InputType::Left: return "Left";
	case InputType::Right: return "Right";
	}
	return "";
}

InputType::IT InputMapper::String2InputType(std::string &input)
{
	if(input == "A") return InputType::A;
	if(input == "B") return InputType::B;
	if(input == "Select") return InputType::Select;
	if(input == "Start") return InputType::Start;
	if(input == "Up") return InputType::Up;
	if(input == "Down") return InputType::Down;
	if(input == "Left") return InputType::Left;
	if(input == "Right") return InputType::Right;
	return InputType::IT::Invalid;
}
