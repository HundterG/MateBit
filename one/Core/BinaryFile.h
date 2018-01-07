#ifndef BINFILE_H_
#define BINFILE_H_

#include "../SFML/Network/Packet.hpp"
#include <fstream>
#include <cstdint>

class BinaryFile
{
	sf::Packet stream;
public:
	void Write(int32_t d)
	{
		stream << d;
	}

	void Write(int8_t d)
	{
		stream << d;
	}

	void Write(std::string const &d)
	{
		stream << d;
	}

	void Read(int32_t &d)
	{
		stream >> d;
	}

	void Read(int8_t &d)
	{
		stream >> d;
	}

	void Read(std::string &d)
	{
		stream >> d;
	}

	bool Load(std::string const &filename)
	{
		std::ifstream file(filename, std::ios_base::binary);
		if(file.is_open())
		{
			while(!file.eof())
			{
				char buffer[1024];
				file.read(buffer, 1024);
				stream.append(buffer, size_t(file.gcount()));
			}
			return true;
		}
		return false;
	}

	bool Save(std::string const &filename)
	{
		std::ofstream file(filename, std::ios_base::binary);
		if(file.is_open())
		{
			file.write(reinterpret_cast<char const*>(stream.getData()), stream.getDataSize());
			return true;
		}
		return false;
	}
};

#endif
