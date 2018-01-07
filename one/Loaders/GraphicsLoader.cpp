#include "../Core/Globals.h"
#include "../Core/Graphics.h"
#include "Loaders.h"
#include <map>

namespace
{
	std::map< size_t, std::vector<Block> > loadedGraphics;
	Block missing;

	void LoadFile(std::string &fileName)
	{
		std::string errorMsg("file "); errorMsg += fileName + " could not be opened\n";
		std::string justTheName(fileName, fileName.rfind('/')+1, fileName.size() - 3 - (fileName.rfind('/')+1));
		softcheck(loadedGraphics.find(std::hash<std::string>()(justTheName)) != loadedGraphics.end(), errorMsg+" this name already exists\n", return; );

		std::ifstream file(fileName, std::ios_base::binary);
		softcheck(!file.is_open(), errorMsg, return; );
		char f;
		char s;
		file.get(f);
		file.get(s);
		int maxFrame = (unsigned char)(f);
		int s2 = ((unsigned char)(s)+1);
		Block::Size sizeAmmount = Block::Size(s2 * s2);
		softcheck(maxFrame >= 200, errorMsg, return; );
		softcheck(!(sizeAmmount == Block::x8 || sizeAmmount == Block::x16 || sizeAmmount == Block::x32 || sizeAmmount == Block::x64 || sizeAmmount == Block::x128 || sizeAmmount == Block::x256), errorMsg, return; );

		std::vector<Block> frames;
		std::unique_ptr<unsigned char[]> newFrame(new unsigned char[sizeAmmount]);
		for(int i=0 ; i<=maxFrame ; ++i)
		{
			for(int d=0 ; d<sizeAmmount ; ++d)
			{
				softcheck(!file.get((char&) newFrame.get()[d]), errorMsg, return; );
			}
			frames.push_back(Block());
			frames.back().Set(newFrame.get(), sizeAmmount);
		}

		loadedGraphics.emplace(std::hash<std::string>()(justTheName), std::move(frames));
	}
};

void LoadGraphics(void)
{
	DirScan("Gfx", "1g", LoadFile);
	missing.SetErrorX();
}

Block &GetBlock(std::string const &name, int frame)
{
	auto f = loadedGraphics.find(std::hash<std::string>()(name));
	if(f != loadedGraphics.end())
	{
		if(0 <= frame && frame < f->second.size())
			return f->second[frame];
	}
	return missing;
}
