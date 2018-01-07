#include "../Core/Globals.h"
#include "../Core/Audio.h"
#include "../Core/BinaryFile.h"
#include "Loaders.h"
#include <map>

namespace
{
	struct Channel{ enum Type{Square1, Square2, Triangle, Noise, Invalid, };};
	struct Sample
	{
		int32_t position = 0;
		Channel::Type channel = Channel::Invalid;
		Note note;
	};

	struct PlayingAudioData
	{
		std::vector<Sample> *data = nullptr;
		std::vector<Sample>::iterator nextNote;
		int32_t time = 0;
		Priority::Type priority = Priority::Low;
	};

	std::map< size_t, std::vector<Sample> > loadedAudio;
	bool tickCounter = false;
	PlayingAudioData nowPlayingSlots[4];

	void LoadFile(std::string &fileName)
	{
		std::string errorMsg("file "); errorMsg += fileName + " could not be opened\n";
		std::string justTheName(fileName, fileName.rfind('/')+1, fileName.size() - 3 - (fileName.rfind('/')+1));
		softcheck(loadedAudio.find(std::hash<std::string>()(justTheName)) != loadedAudio.end(), errorMsg+" this name already exists\n", return; );

		BinaryFile file;
		softcheck(!file.Load(fileName), errorMsg, return; );

		int32_t size = -1;
		file.Read(size);
		softcheck(size < 0, errorMsg, return; );
		std::vector<Sample> newAudio;
		newAudio.reserve(size);

		int32_t lastPos = -1;
		for(int32_t i=0 ; i<size ; ++i)
		{
			Sample note;
			file.Read(note.position);
			softcheck(lastPos > note.position, errorMsg, return; );

			int32_t len = 0;
			file.Read(len);
			note.note.length = int(len);

			int8_t channel = -1;
			file.Read(channel);
			switch(channel)
			{
			case 1: note.channel = Channel::Square1; break;
			case 2: note.channel = Channel::Square2; break;
			case 3: note.channel = Channel::Triangle; break;
			case 4: note.channel = Channel::Noise; break;
			default: softcheck(true, errorMsg, return; );
			}

			int32_t freq = 0;
			file.Read(freq);
			note.note.freq = int(freq);

			int8_t mode = -1;
			file.Read(mode);
			note.note.mode = mode;

			int8_t volume = -1;
			file.Read(volume);
			switch(volume)
			{
			case 0: note.note.volume = Volume::vx0; break;
			case 1: note.note.volume = Volume::vx1; break;
			case 2: note.note.volume = Volume::vx2; break;
			default: case 3: note.note.volume = Volume::vx3; break;
			}

			newAudio.push_back(note);
		}

		loadedAudio.emplace(std::hash<std::string>()(justTheName), std::move(newAudio));
	}
}

void LoadAudio(void)
{
	DirScan("Sfx", "1a", LoadFile);
}

void UpdateAudioFeeder(void)
{
	if(tickCounter)
	{
		for(auto & slot : nowPlayingSlots)
		{
			if(slot.data != nullptr)
			{
				++slot.time;
				while(slot.nextNote != slot.data->end() && slot.nextNote->position <= slot.time)
				{
					Note temp = slot.nextNote->note;
					temp.priority = slot.priority;
					switch(slot.nextNote->channel)
					{
					case Channel::Square1: AudioSetSquare1Note(temp); break;
					case Channel::Square2: AudioSetSquare1Note(temp); break;
					case Channel::Triangle: AudioSetTriangleNote(temp); break;
					case Channel::Noise: AudioSetNoiseNote(temp); break;
					}
					++slot.nextNote;
				}

				if(slot.nextNote == slot.data->end())
				{
					slot.data = nullptr;
					slot.priority = Priority::Low;
				}
			}
		}
	}

	tickCounter = !tickCounter;
}

void PlayBGM(std::string const &name, bool fromLoop)
{
	auto f = loadedAudio.find(std::hash<std::string>()(name));
	if(f != loadedAudio.end() && nowPlayingSlots[0].data != &f->second)
	{
		nowPlayingSlots[0].data = &f->second;
		nowPlayingSlots[0].nextNote = f->second.begin();
		nowPlayingSlots[0].priority = Priority::Low;
		nowPlayingSlots[0].time = 0;
	}
}

void PlaySFX(std::string const &name, bool highPriority)
{
	auto f = loadedAudio.find(std::hash<std::string>()(name));
	if(f != loadedAudio.end())
	{
		Priority::Type p = (highPriority) ? Priority::High : Priority::Medium;
		for(int i=1 ; i<4 ; ++i)
		{
			if(nowPlayingSlots[i].priority < p)
			{
				nowPlayingSlots[i].data = &f->second;
				nowPlayingSlots[i].nextNote = f->second.begin();
				nowPlayingSlots[i].priority = p;
				nowPlayingSlots[i].time = 0;
			}
		}
	}
}
