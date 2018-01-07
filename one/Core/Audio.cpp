#include "Globals.h"
#include <SFML/Audio.hpp>
#include "Audio.h"
#include <algorithm>
#include <memory>
#include <atomic>

namespace
{
	class SquareWave : public sf::SoundStream
	{
	private:
		struct NoteParams
		{
			int width = 100;
			Duty::Type midWidth = Duty::x50;
			Volume::Type hight = Volume::vx0;
			Priority::Type priority = Priority::Low;
			int length = -1;
		};

		const int sampleRate = 44100;
		std::unique_ptr<int16_t[]> buffer;
		int time = 0;
		int thisHighWidth = 0;
		std::atomic<NoteParams> noteParams;

		// this function does not follow naming convention because it has to overload a library function.
		bool onGetData(Chunk& data)
		{
			NoteParams note = noteParams;
			data.sampleCount = (sampleRate/60)-1;
			data.samples = buffer.get();
			for(int i=0 ; i<sampleRate/60 ; ++i)
			{
				if(time <= 0)
				{
					time = note.width;
					thisHighWidth = note.width/int(note.midWidth);
				}

				if(thisHighWidth > 0)
					buffer.get()[i] = int16_t(note.hight);
				else
					buffer.get()[i] = -int16_t(note.hight);
				--time;
				--thisHighWidth;
			}

			NoteParams minusLength = noteParams;
			--minusLength.length;
			if(minusLength.length <= 0)
			{
				minusLength.hight = Volume::vx0;
				minusLength.priority = Priority::Low;
			}
			noteParams = minusLength;
			return true;
		}

		// this function does not follow naming convention because it has to overload a library function.
		void onSeek(sf::Time){}
	public:
		SquareWave()
		{
			buffer.reset(new int16_t[sampleRate/60]);
			std::memset(buffer.get(), 0, sampleRate/60*sizeof(int16_t));
			initialize(1, sampleRate);
		}

		void SetNote(Note const &in)
		{
			NoteParams note;
			if(in.freq > 0)
				note.width = sampleRate/in.freq;
			else
				note.width = 0;
			switch(in.mode)
			{
			case Duty::x13: note.midWidth = Duty::x13; break;
			case Duty::x25: note.midWidth = Duty::x25; break;
			default: note.midWidth = Duty::x50; break;
			}
			note.hight = in.volume;
			note.priority = in.priority;
			note.length = in.length;
			if(in.priority >= NoteParams(noteParams).priority)
				noteParams = note;
		}
	};

	class TriangleWave : public sf::SoundStream
	{
	private:
		struct NoteParams
		{
			int width = 100;
			bool mute = true;
			Priority::Type priority = Priority::Low;
			int length = -1;
		};

		int16_t Lerp(int16_t a, int16_t b, float t)
		{
			return int16_t((int32_t(b)-a)*t + a);
		}

		const int sampleRate = 44100;
		std::unique_ptr<int16_t[]> buffer;
		unsigned int time = 100;
		unsigned int thisWidth = 100;
		const int16_t hight = 25000;
		std::atomic<NoteParams> noteParams;

		// this function does not follow naming convention because it has to overload a library function.
		bool onGetData(Chunk& data)
		{
			NoteParams note = noteParams;
			data.sampleCount = (sampleRate/60)-1;
			data.samples = buffer.get();
			for(int i=0 ; i<sampleRate/60 ; ++i)
			{
				if(0 <= time && time < thisWidth/4)
					buffer.get()[i] = Lerp(0, hight, float(time)/float(thisWidth/4));
				else if(thisWidth/4 <= time && time < (thisWidth*3)/4)
					buffer.get()[i] = Lerp(hight, -hight, float(time)/float(thisWidth/2) - 0.50f);
				else if((thisWidth*3)/4 <= time && time < thisWidth)
					buffer.get()[i] = Lerp(-hight, 0, float(time-((thisWidth*3)/4))/float(thisWidth/4));
				else if(thisWidth <= time)
				{
					buffer.get()[i] = 0;
					if(!note.mute)
					{
						thisWidth = unsigned int(note.width);
						time = 0;
					}
				}
				++time;
			}

			NoteParams minusLength = noteParams;
			--minusLength.length;
			if(minusLength.length <= 0)
			{
				minusLength.mute = true;
				minusLength.priority = Priority::Low;
			}
			noteParams = minusLength;
			return true;
		}

		// this function does not follow naming convention because it has to overload a library function.
		void onSeek(sf::Time){}
	public:
		TriangleWave()
		{
			buffer.reset(new int16_t[sampleRate/60]);
			std::memset(buffer.get(), 0, sampleRate/60*sizeof(int16_t));
			initialize(1, sampleRate);
		}

		void SetNote(Note const &in)
		{
			NoteParams note;
			if(in.freq > 0)
			{
				note.width = sampleRate/in.freq;
				note.mute = false;
			}
			note.priority = in.priority;
			note.length = in.length;
			if(in.priority >= NoteParams(noteParams).priority)
				noteParams = note;
		}
	};

	class NoiseWave : public sf::SoundStream
	{
	private:
		struct NoteParams
		{
			int width = 100;
			bool shortMode = false;
			bool mute = true;
			bool resetRegister = true;
			Priority::Type priority = Priority::Low;
			int length = -1;
		};

		const int sampleRate = 44100;
		std::unique_ptr<int16_t[]> buffer;

		uint32_t entsLeft = 0;
		int16_t registe = 1;
		std::atomic<NoteParams> noteParams;

		// this function does not follow naming convention because it has to overload a library function.
		bool onGetData(Chunk& data)
		{
			NoteParams note = noteParams;
			data.sampleCount = (sampleRate/60)-1;
			data.samples = buffer.get();
			bool noisePos = false;
			if(note.resetRegister)
				registe = 1;
			for(int i=0 ; i<sampleRate/60 ; ++i)
			{
				if(entsLeft == 0)
				{
					if(note.shortMode)
						noisePos = bool(registe&0x0001)^bool(registe&0x0040);
					else
						noisePos = bool(registe&0x0001)^bool(registe&0x0002);
					registe = registe >> 1;
					registe |= int16_t(noisePos) << 14;
					entsLeft = note.width;
				}
				buffer.get()[i] = int16_t(noisePos * 10000 * int16_t(!note.mute));
				--entsLeft;
			}

			NoteParams minusLength = noteParams;
			--minusLength.length;
			minusLength.resetRegister = false;
			if(minusLength.length <= 0)
			{
				minusLength.mute = true;
				minusLength.priority = Priority::Low;
			}
			noteParams = minusLength;
			return true;
		}

		// this function does not follow naming convention because it has to overload a library function.
		void onSeek(sf::Time){}
	public:
		NoiseWave()
		{
			buffer.reset(new int16_t[sampleRate/60]);
			std::memset(buffer.get(), 0, sampleRate/60*sizeof(int16_t));
			initialize(1, sampleRate);
		}

		void SetNote(Note const &in)
		{
			NoteParams note;
			if(in.freq > 0) 
			{
				note.width = sampleRate/in.freq;
				note.mute = false;
			}
			if(in.mode > 0)
				note.shortMode = true;
			note.priority = in.priority;
			note.length = in.length;
			if(in.priority >= NoteParams(noteParams).priority)
				noteParams = note;
		}
	};

	struct LazyGlobals
	{
		SquareWave square1;
		SquareWave square2;
		TriangleWave triangle;
		NoiseWave noise;
	};

	// vars
	LazyGlobals *lGP = nullptr;
}

void AudioInit(void)
{
	// allocate lazy globals
	lGP = new LazyGlobals();
	hardcheck(!lGP, "Globals could not be allocated.");
	lGP->square1.play();
	lGP->square2.play();
	lGP->triangle.play();
	lGP->noise.play();
}

void AudioSetSquare1Note(Note const &newnote)
{
	lGP->square1.SetNote(newnote);
}

void AudioSetSquare2Note(Note const &newnote)
{
	lGP->square2.SetNote(newnote);
}

void AudioSetTriangleNote(Note const &newnote)
{
	lGP->triangle.SetNote(newnote);
}

void AudioSetNoiseNote(Note const &newnote)
{
	lGP->noise.SetNote(newnote);
}

void AudioUpdate(void)
{
	// set volume from master
	lGP->square1.setVolume(Globals::volume);
	lGP->square2.setVolume(Globals::volume);
	lGP->triangle.setVolume(Globals::volume);
	lGP->noise.setVolume(Globals::volume);
}

void AudioExit(void)
{
	// important to stop sounds before delete, otherwise heap corruption will ocure
	lGP->noise.stop();
	lGP->triangle.stop();
	lGP->square2.stop();
	lGP->square1.stop();

	delete lGP;
	lGP = nullptr;
}
