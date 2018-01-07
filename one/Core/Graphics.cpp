#include "Globals.h"
#include "Graphics.h"
#include "Window.h"

namespace
{
	int D2D3(int x, int y, int sx)
	{
		return (sx*y) + x;
	}

	int D2D3Trans(int x, int y, int sx, bool flipX, bool flipY, bool cW, bool cCW)
	{
		int ox = x;
		int oy = y;
		if(flipX) ox = sx-1-x;
		if(flipY) oy = sx-1-y;
		if(cW && !cCW) { int temp = sx-1-ox; ox = oy; oy = temp; }
		if(cCW && !cW) { int temp = sx-1-oy; oy = ox; ox = temp; }
		return (sx*oy) + ox;
	}

	unsigned char CalcFade(unsigned char input, DrawSettings::Fade red, DrawSettings::Fade green, DrawSettings::Fade blue)
	{
		unsigned char res = 0;

		switch(red)
		{
		case DrawSettings::x33: res |= (input >> 2) & 0xe0; break;
		case DrawSettings::x66: res |= (input >> 1) & 0xe0; break;
		case DrawSettings::x100: res |= input & 0xe0; break;
		case DrawSettings::x133: res |= (input << 1) | 0x3f & 0xe0; break;
		case DrawSettings::x166: res |= (input << 2) | 0x7f & 0xe0; break;
		case DrawSettings::x200: res |= 0xe0; break;
		}

		switch(green)
		{
		case DrawSettings::x33: res |= ((input & 0x1c) >> 2) & 0x1c; break;
		case DrawSettings::x66: res |= ((input & 0x1c) >> 1) & 0x1c; break;
		case DrawSettings::x100: res |= input & 0x1c; break;
		case DrawSettings::x133: res |= ((input & 0x1c) << 1) | 0x07 & 0x1c; break;
		case DrawSettings::x166: res |= ((input & 0x1c) << 2) | 0x0f & 0x1c; break;
		case DrawSettings::x200: res |= 0x1c; break;
		}

		switch(blue)
		{
		case DrawSettings::x33:
		case DrawSettings::x66: res |= (input & 0x03) >> 1; break;
		case DrawSettings::x100: res |= input & 0x03; break;
		case DrawSettings::x133:
		case DrawSettings::x166: res |= (input << 1) | 0x01 & 0x03; break;
		case DrawSettings::x200: res |= 0x03; break;
		}

		return res;
	}

	struct DefaultPallet
	{
		Pallet reColor;
		DefaultPallet()
		{
			for(int i=0 ; i<256 ; ++i)
				reColor.color[i] = char(i);
		}
	} defaultPallet;
}

void Block::Set(unsigned char *source, Size size)
{
	buffer.reset(new unsigned char[size], std::default_delete<unsigned char[]>());
	softcheck(!buffer, "Block could not be allocated. Out of memory.", s = x0; return;);
	if(source)
		std::memcpy(buffer.get(), source, size);
	else
		std::memset(buffer.get(), 0, size);
	s = size;
}

void Block::SetDebug(void)
{
	buffer.reset(new unsigned char[x16], std::default_delete<unsigned char[]>());
	softcheck(!buffer, "Block could not be allocated. Out of memory.", s = x0; return;);
	for(int i=0 ; i<x16 ; ++i)
		buffer.get()[i] = char(i);
	s = x16;
}

void Block::SetErrorX(void)
{
	buffer.reset(new unsigned char[x8], std::default_delete<unsigned char[]>());
	softcheck(!buffer, "Block could not be allocated. Out of memory.", s = x0; return;);
	for(int i=0 ; i<x8 ; ++i)
		buffer.get()[i] = 0;
	auto d = buffer.get();
	d[9] = d[14] = d[18] = d[21] = d[27] = d[28] = d[35] = d[36] = d[42] = d[45] = d[49] = d[54] = 255;
	s = x8;
}

void Screen::Clear(unsigned char color)
{
	std::memset(buffer, color, sizeof(buffer));
}

void Screen::DrawSolid(int x, int y, int w, int h, unsigned char color)
{
	for(int cy=std::max(y, 0) ; cy < y+h && cy < 256 ; ++cy)
		for(int cx=std::max(x, 0) ; cx < x+w && cx < 256 ; ++cx)
			buffer[D2D3(cx, cy, 256)] = color;
}

void Screen::DrawBlock(Block &block, int x, int y, int trans)
{
	DrawBlock(block, x, y, defaultPallet.reColor, trans);
}

void Screen::DrawBlock(Block &block, int x, int y, Pallet &reColor, int trans)
{
	int sx = 0, sy = 0;
	// if block is too far right or far down, dont draw.
	// if block has no data, dont draw.
	if(x>255 || y>255 || block.s==0)
		return;
	if(x<0)
	{
		sx = -x;
		x = 0;
	}
	if(y<0)
	{
		sy = -y;
		y = 0;
	}

	int s = int(std::sqrt(int(block.s)));
	if(sx >= s || sy >= s)
		return;

	for(int dy=sy ; dy<s && y<256 ; ++dy)
	{
		int mx = x + warp.offset[y];
		for(int dx=sx ; dx<s && mx<256 ; ++dx)
		{
			if(mx >= 0 && int( reColor.color[ block.buffer.get()[D2D3(dx, dy, s)] ] ) != trans)
				buffer[D2D3(mx, y, 256)] = reColor.color[ block.buffer.get()[D2D3(dx, dy, s)] ];
			++mx;
		}
		++y;
	}
}

void Screen::DrawBlock(Block &block, int x, int y, DrawSettings &settings)
{
	DrawBlock(block, x, y, settings, defaultPallet.reColor);
}

void Screen::DrawBlock(Block &block, int x, int y, DrawSettings &settings, Pallet &reColor)
{
	int sx = 0, sy = 0;
	// if block is too far right or far down, dont draw.
	// if block has no data, dont draw.
	if(x>255 || y>255 || block.s==0)
		return;
	if(x<0)
	{
		sx = -x;
		x = 0;
	}
	if(y<0)
	{
		sy = -y;
		y = 0;
	}

	int s = int(std::sqrt(int(block.s)));
	if(sx >= s || sy >= s)
		return;

	for(int dy=sy ; dy<s && y<256 ; ++dy)
	{
		int mx = x + warp.offset[y];
		for(int dx=sx ; dx<s && mx<256 ; ++dx)
		{
			if(mx >= 0 && int( reColor.color[ block.buffer.get()[D2D3Trans(dx, dy, s, settings.flipX, settings.flipY, settings.rotateCW, settings.rotateCCW)] ] ) != settings.transparency)
				buffer[D2D3(mx, y, 256)] = CalcFade( reColor.color[ block.buffer.get()[D2D3Trans(dx, dy, s, settings.flipX, settings.flipY, settings.rotateCW, settings.rotateCCW)] ], settings.red, settings.green, settings.blue );
			++mx;
		}
		++y;
	}
}

void Screen::SetWarp(ScreenWarp const &newWarp)
{
	warp = newWarp;
}

void Screen::SetWarp(int i, char ammount)
{
	if(0 <= i && i < 256)
		warp.offset[i] = ammount;
}

void Screen::OverLay(Screen &other, int trans)
{
	for(int i=0 ; i<sizeof(buffer) ; ++i)
	{
		if(int(other.buffer[i]) != trans)
			buffer[i] = other.buffer[i];
	}
}

void Screen::Present(void)
{
	WindowPresentRequest(buffer);
}
