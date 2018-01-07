#include "Core/Globals.h"
#include "Core/Window.h"
#include "Core/Audio.h"
#include "Core/GSM.h"
#include "Core/Input.h"
#include "Core/InputMapper.h"
#include "Loaders/Loaders.h"

namespace
{
	void LoadGlobals(void)
	{
		// if cant load, just use defaults
		std::ifstream file("Settings.ini");
		softcheck(!file.is_open(), "Settings could not be opened. Using defaults.", return;);
		InputMapper mapper;

		// get key binding
		// get gamepad binding
		try
		{
			while(!file.eof())
			{
				std::string cmd;
				file >> cmd;
				
				if(cmd == "FullScreen")
				{
					std::string value;
					file >> value;
					if(value == "true" || value == "1")
						Globals::fullScreen = true;
				}
				else if(cmd == "AspectRatio")
				{
					std::string value;
					file >> value;
					if(value == "1x1") Globals::aspectRatio = Globals::Aspect::x11;
					else if(value == "3x4") Globals::aspectRatio = Globals::Aspect::x34;
					else if(value == "Any") Globals::aspectRatio = Globals::Aspect::xany;
				}
				else if(cmd == "KeyBind")
				{
					std::string inputtype, key;
					file >> inputtype;
					file >> key;
					InputType::IT fit = mapper.String2InputType(inputtype);
					auto found = mapper.s2K.find(key);
					if(fit == InputType::Invalid || found == mapper.s2K.end()) throw 5;
					InputSetBind(fit, found->second);
				}
				else if(cmd == "Volume")
				{
					file >> Globals::volume;
				}
				else break;
			}
		}
		catch(...)
		{
			message("Error reading settings file.");
		}
		file.close();
	}

	void SaveGlobals(void)
	{
		std::ofstream file("Settings.ini");
		softcheck(!file.is_open(), "Settings could not be saved.", return;);
		InputMapper mapper;

		file << "FullScreen " << ((Globals::fullScreen)? "true" : "false") << "\n";
		file << "AspectRatio ";
		switch(Globals::aspectRatio)
		{
		case Globals::Aspect::x11: file << "1x1"; break;
		case Globals::Aspect::x34: file << "3x4"; break;
		case Globals::Aspect::xany: file << "Any"; break;
		}
		file << "\n";
		file << "Volume " << Globals::volume << "\n";

		for(int i=0 ; i<8 ; ++i)
			file << "KeyBind " << mapper.InputType2String(InputType::IT(i)) << " " << mapper.k2S.find(Globals::keyBinding[i])->second << "\n";

		file.close();
	}
}

int main(void)
{
	try
	{
		LoadGlobals();
		GSMInit();
		AudioInit();
		LoadGraphics();
		LoadAudio();
		WindowInit();

		while(!Globals::quit)
		{
			InputResetPress();
			WindowUpdate();
			GSMUpdate();
			UpdateAudioFeeder();
			AudioUpdate();
			WindowDraw();
		}

		WindowExit();
		AudioExit();
		GSMExit();
		SaveGlobals();
		return 0;
	}
	catch(std::exception e)
	{
		WindowErrorMsg(e.what());
		return 1;
	}
}
