#ifndef _LOADER_
#define _LOADER_
class Block;

void LoadGraphics(void);
Block &GetBlock(std::string const &name, int frame);

void LoadAudio(void);
void UpdateAudioFeeder(void);
void PlayBGM(std::string const &name, bool fromLoop);
void PlaySFX(std::string const &name, bool highPriority);

#endif
