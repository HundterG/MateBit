#ifndef _GRAPHICS_
#define _GRAPHICS_
#include <memory>

class Block
{
public:
	enum Size
	{
		x0=0, x8=64, x16=256, x24=576, x32=1024, x64=4096, x128=16384, x256=65536
	};

private:
	std::shared_ptr<unsigned char> buffer;
	Size s = x0;
	friend class Screen;

public:
	void Set(unsigned char *source, Size size);
	void SetDebug(void);
	void SetErrorX(void);
};

struct Pallet
{
	unsigned char color[256];
};

struct ScreenWarp
{
	ScreenWarp(){ std::memset(offset, 0, sizeof(offset)); }
	char offset[256];
};

struct DrawSettings
{
	enum Fade
	{
		x0,
		x33,
		x66,
		x100,
		x133,
		x166,
		x200
	};
	int transparency = -1;
	bool flipX = false;
	bool flipY = false;
	bool rotateCW = false;
	bool rotateCCW = false;
	Fade red = x100;
	Fade green = x100;
	Fade blue = x100;
};

class Screen
{
	unsigned char buffer[65536];
	ScreenWarp warp;
public:
	void Clear(unsigned char color = 0);
	void DrawSolid(int x, int y, int w, int h, unsigned char color);
	void DrawBlock(Block &block, int x, int y, int trans = -1);
	void DrawBlock(Block &block, int x, int y, Pallet &reColor, int trans = -1);
	void DrawBlock(Block &block, int x, int y, DrawSettings &settings);
	void DrawBlock(Block &block, int x, int y, DrawSettings &settings, Pallet &reColor);
	void OverLay(Screen &other, int trans = -1);
	void SetWarp(ScreenWarp const &newWarp);
	void SetWarp(int i, char warp);
	void Present(void);
};

#endif
