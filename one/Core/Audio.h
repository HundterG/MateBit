#ifndef _AUDIO_H_
#define _AUDIO_H_

struct Volume{ enum Type { vx0 = 0, vx1 = 1666, vx2 = 3333, vx3 = 5000, };};
struct Duty { enum Type { x50 = 2, x25 = 4, x13 = 8, };};
struct Priority { enum Type { Low = 0, Medium = 10, High = 20, };};

struct Note
{
	Note() = default;
	Note(int f, Volume::Type v, int m, int l) : freq(f), volume(v), mode(m), length(l){}
	int freq = 0;
	Volume::Type volume = Volume::vx0;
	int mode = 0;
	int length = -1;
	Priority::Type priority = Priority::Low;
};

void AudioInit(void);
void AudioSetSquare1Note(Note const &newnote);
void AudioSetSquare2Note(Note const &newnote);
void AudioSetTriangleNote(Note const &newnote);
void AudioSetNoiseNote(Note const &newnote);
void AudioUpdate(void);
void AudioExit(void);

#endif