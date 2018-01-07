#ifndef __INPUTMAPPER__
#define __INPUTMAPPER__
#include <map>

struct InputMapper
{
	std::map<sf::Keyboard::Key, char const*> k2S;
	std::map<std::string, sf::Keyboard::Key> s2K;

	InputMapper();
	char const *InputType2String(InputType::IT input);
	InputType::IT String2InputType(std::string &input);
private:
	void Map(char const *key, sf::Keyboard::Key value);
};

#endif
