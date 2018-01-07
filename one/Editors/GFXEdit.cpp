#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstring>

#if SFML_VERSION_MAJOR < 2 && SFML_VERSION_MINOR < 4
#define setFillColor setColor
#endif

namespace
{
	// struct
	struct Color
	{
		unsigned char r, g, b, a;
		bool operator==(Color const &rhs)
		{
			return rhs.r == r &&
				rhs.g == g &&
				rhs.b == b &&
				rhs.a == a;
		}
		bool operator!=(Color const &rhs)
		{
			return !(*this == rhs);
		}
	};

	struct LazyGlobals
	{
		sf::RenderWindow win;
		sf::Texture gpuBuffer;
		sf::Texture selector;
		sf::Texture templatebuff;
		sf::Texture leftct;
		sf::Texture rightct;
		sf::Sprite gpuSprite;
		sf::Sprite selectS;
		sf::Sprite tempS;
		sf::Sprite leftcs;
		sf::Sprite rightcs;
		sf::Font font;
		sf::Text loadsave;
		sf::Text zoomt;
		sf::Text sizet;
		sf::Text fillt;
		sf::Text brusht;
		sf::Text framet;
		sf::Text tempt;
		sf::Text cpt;
		sf::Text color1t;
		sf::Text color2t;
	};

	// vars
	LazyGlobals *lGP = nullptr;
	std::vector< std::unique_ptr<Color[]> > frames;
	std::unique_ptr<Color[]> copyBuffer(new Color[65536]);
	unsigned char leftColor = 0;
	unsigned char rightColor = 255;
	int zoomAmmount = 1;
	int sizeAmmount = 16;
	int brush = 0;
	unsigned char currFrame = 0;
	int maxFrame = 0;
	bool drawTemp = false;
	float tempSize = 1;

	// declares
	void Save(std::string const &filename);
	void Load(std::string const &filename);
	std::string GetString(void);
	void Copy(void);
	void Paste(void);
	void FlipX(void);
	void FlipY(void);
	
	// funcs
	Color GetColor(unsigned char c)
	{
		Color ret;
		// red
		switch((c & 0xe0) >> 5)
		{
		case 0: ret.r = 0; break;
		case 1: ret.r = 36; break;
		case 2: ret.r = 72; break;
		case 3: ret.r = 108; break;
		case 4: ret.r = 144; break;
		case 5: ret.r = 180; break;
		case 6: ret.r = 216; break;
		case 7: ret.r = 255; break;
		}
		// green
		switch((c & 0x1c) >> 2)
		{
		case 0: ret.g = 0; break;
		case 1: ret.g = 36; break;
		case 2: ret.g = 72; break;
		case 3: ret.g = 108; break;
		case 4: ret.g = 144; break;
		case 5: ret.g = 180; break;
		case 6: ret.g = 216; break;
		case 7: ret.g = 255; break;
		}
		// blue
		switch(c & 0x03)
		{
		case 0: ret.b = 0; break;
		case 1: ret.b = 85; break;
		case 2: ret.b = 170; break;
		case 3: ret.b = 255; break;
		}
		// alpha
		ret.a = 255;
		return ret;
	}

	unsigned char Get8Bit(Color c)
	{
		unsigned char r = unsigned char(std::round(((c.r)/255.0f)*7));
		unsigned char g = unsigned char(std::round(((c.g)/255.0f)*7));
		unsigned char b = unsigned char(std::round(((c.b)/255.0f)*3));
		return (r<<5) | (g<<2) | b;
	}

	void OnCreate(void)
	{
		std::unique_ptr<Color[]> newFrame(new Color[65536]);
		for(int i=0 ; i<65536 ; ++i)
			newFrame.get()[i] = Color{0,0,0,255};
		frames.push_back(std::move(newFrame));
		
		lGP->win.create(sf::VideoMode(800,600), "The One Graphics Editor", sf::Style::Default);
		lGP->win.setFramerateLimit(60);
		lGP->win.setView( sf::View(sf::Vector2f(400, 300), sf::Vector2f(float(800), float(600))) );

		if(!lGP->gpuBuffer.create(256, 256)) throw;
		if(!lGP->selector.create(256, 600)) throw;
		lGP->selectS.setTexture(lGP->selector);
		lGP->selectS.setOrigin(sf::Vector2f(255, 0));
		std::unique_ptr<Color[]> tempbuff(new Color[256*600]);
		for(int y=0 ; y<256 ; ++y)
			for(int x=0 ; x<256 ; ++x)
				tempbuff.get()[(y*256)+x] = GetColor((((y/16)*16)+(x/16)));
		for(int y=256 ; y<600 ; ++y)
			for(int x=0 ; x<256 ; ++x)
				tempbuff.get()[(y*256)+x] = Color{0,0,0,255};
		lGP->selector.update((unsigned char*)tempbuff.get());
		lGP->tempS.setColor(sf::Color(255, 255, 255, 128));

		if(!lGP->leftct.create(1,1)) throw;
		lGP->leftcs.setTexture(lGP->leftct);
		lGP->leftcs.setScale(sf::Vector2f(50,50));
		lGP->leftcs.setOrigin(sf::Vector2f(1,0));
		if(!lGP->rightct.create(1,1)) throw;
		lGP->rightcs.setTexture(lGP->rightct);
		lGP->rightcs.setScale(sf::Vector2f(50,50));
		lGP->rightcs.setOrigin(sf::Vector2f(1,0));

		lGP->font.loadFromFile("arial.ttf");
		lGP->loadsave.setFont(lGP->font);
		lGP->loadsave.setString("Load   Save");
		lGP->zoomt.setFont(lGP->font);
		lGP->sizet.setFont(lGP->font);
		lGP->fillt.setFont(lGP->font);
		lGP->fillt.setString("Fill");
		lGP->brusht.setFont(lGP->font);
		lGP->brusht.setString("Brush");
		lGP->framet.setFont(lGP->font);
		lGP->tempt.setFont(lGP->font);
		lGP->tempt.setString("< > /\\ \\/ O o X Load");
		lGP->cpt.setFont(lGP->font);
		lGP->cpt.setString("Copy Paste FX FY");
		lGP->color1t.setFont(lGP->font);
		lGP->color2t.setFont(lGP->font);

		lGP->gpuSprite.setTexture(lGP->gpuBuffer);
	}

	void OnClick(bool right, bool drag, int x, int y)
	{
		if(x>=lGP->win.getSize().x-256 && x<lGP->win.getSize().x)
		{
			// in toolbar
			if(y>=0 && y<256)
			{
				if(right)
					rightColor = (((y/16)*16)+((x-lGP->win.getSize().x+256)/16));
				else
					leftColor = (((y/16)*16)+((x-lGP->win.getSize().x+256)/16));
			}
			else if(y>=500 && y<550)
			{
				if(x<lGP->win.getSize().x-235)
				{
					lGP->tempS.move(-1,0);
				}
				else if(x<lGP->win.getSize().x-209)
				{
					lGP->tempS.move(1,0);
				}
				else if(x<lGP->win.getSize().x-183)
				{
					lGP->tempS.move(0,-1);
				}
				else if(x<lGP->win.getSize().x-159)
				{
					lGP->tempS.move(0,1);
				}
				else if(x<lGP->win.getSize().x-130)
				{
					tempSize *= 1.01f;
				}
				else if(x<lGP->win.getSize().x-105)
				{
					tempSize *= 0.99f;
				}
				else if(x<lGP->win.getSize().x-78)
				{
					if(!drag)
					{
						lGP->tempS.setScale(1,1);
						lGP->tempS.setPosition(0,0);
						drawTemp = false;
					}
				}
				else
				{
					if(!drag)
					{
						lGP->tempS.setScale(1,1);
						lGP->tempS.setPosition(0,0);
						std::string fileString = GetString();
						if(!fileString.empty())
						{
							if(lGP->templatebuff.loadFromFile(fileString))
							{
								drawTemp = true;
								lGP->tempS.setTexture(lGP->templatebuff);
							}
							else
								drawTemp = false;
						}
					}
				}
			}
			else if(!drag)
			{
				if(y>=256 && y<300)
				{
					// save / load
					std::string fileString = GetString();
					if(!fileString.empty())
					{
						if(x<lGP->win.getSize().x-256+80)
							Load(fileString);
						else
							Save(fileString);
					}
				}
				else if(y>=300 && y<350)
				{
					// zoom
					zoomAmmount++;
					if(zoomAmmount>12) zoomAmmount = 1;
				}
				else if(y>=350 && y<400)
				{
					// size
					sizeAmmount *= 2;
					if(sizeAmmount>256) sizeAmmount = 8;
				}
				else if(y>=400 && y<450)
				{
					// brush
					if(x<lGP->win.getSize().x-128)
						brush = 1;
					else
						brush = 0;
				}
				else if(y>=450 && y<500)
				{
					if(x<lGP->win.getSize().x-235)
					{
						if(currFrame == 0)
							currFrame = maxFrame;
						else
							--currFrame;
					}
					else if(x<lGP->win.getSize().x-209)
					{
						if(currFrame == maxFrame)
							currFrame = 0;
						else
							++currFrame;
					}
					else if(x<lGP->win.getSize().x-183)
					{
						if(maxFrame < 200-1)
						{
							++maxFrame;
							++currFrame;
							std::unique_ptr<Color[]> newFrame(new Color[65536]);
							for(int i=0 ; i<65536 ; ++i)
								newFrame.get()[i] = Color{0,0,0,255};
							frames.insert(frames.begin()+currFrame, std::move(newFrame));
						}
					}
					else if(x<lGP->win.getSize().x-164)
					{
						if(maxFrame > 0)
						{
							--maxFrame;
							frames.erase(frames.begin()+currFrame);
						}
						if(currFrame > maxFrame)
							currFrame = maxFrame;
					}
				}
				else if(y>=550 && y<600)
				{
					if(x<lGP->win.getSize().x-181)
						Copy();
					else if(x<lGP->win.getSize().x-97)
						Paste();
					else if(x<lGP->win.getSize().x-50)
						FlipX();
					else
						FlipY();
				}
			}
		}
		else if(x>=0 && y>=0 && x<sizeAmmount*zoomAmmount && y<sizeAmmount*zoomAmmount)
		{
			// in draw
			if(brush == 0)
				(frames.begin()+currFrame)->get()[(y/zoomAmmount)*256+(x/zoomAmmount)] = GetColor(right?rightColor:leftColor);
			else if(brush == 1 && !drag)
			{
				std::queue< std::pair<int,int> > fillq;
				auto thisFrame = (frames.begin()+currFrame);
				clock_t start = std::clock();
				Color thisColor = thisFrame->get()[(y/zoomAmmount)*256+(x/zoomAmmount)];
				if(thisFrame->get()[(y/zoomAmmount)*256+(x/zoomAmmount)] != GetColor(right?rightColor:leftColor))
					fillq.push(std::pair<int,int>(x/zoomAmmount, y/zoomAmmount));
				while(!fillq.empty())
				{
					std::pair<int,int> c = fillq.front();
					if(thisFrame->get()[(c.second)*256+(c.first)] == thisColor)
					{
						thisFrame->get()[(c.second)*256+(c.first)] = GetColor(right?rightColor:leftColor);
						if(c.first+1<sizeAmmount && thisFrame->get()[(c.second)*256+(c.first+1)] == thisColor)
							fillq.push(std::pair<int,int>(c.first+1, c.second));
						if(c.first-1>=0 && thisFrame->get()[(c.second)*256+(c.first-1)] == thisColor)
							fillq.push(std::pair<int,int>(c.first-1, c.second));
						if(c.second+1<sizeAmmount && thisFrame->get()[(c.second+1)*256+(c.first)] == thisColor)
							fillq.push(std::pair<int,int>(c.first, c.second+1));
						if(c.second-1>=0 && thisFrame->get()[(c.second-1)*256+(c.first)] == thisColor)
							fillq.push(std::pair<int,int>(c.first, c.second-1));
					}
					fillq.pop();
					if(start + 1*CLOCKS_PER_SEC < std::clock())
						break;
				}
			}
		}
	}

	void OnDraw(void)
	{
		lGP->win.clear(sf::Color(200,200,200,255));
		lGP->loadsave.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 256));
		lGP->zoomt.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 300));
		lGP->sizet.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 350));
		lGP->fillt.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 400));
		if(brush == 1)
			lGP->fillt.setFillColor(sf::Color(0,0,255,255));
		else
			lGP->fillt.setFillColor(sf::Color(255,255,255,255));
		lGP->brusht.setPosition(sf::Vector2f(lGP->win.getSize().x-128, 400));
		if(brush == 0)
			lGP->brusht.setFillColor(sf::Color(0,0,255,255));
		else
			lGP->brusht.setFillColor(sf::Color(255,255,255,255));
		Color c = GetColor(leftColor);
		lGP->leftct.update((unsigned char*)&c);
		lGP->leftcs.setPosition(sf::Vector2f(lGP->win.getSize().x-1, 300));
		lGP->color1t.setPosition(sf::Vector2f(lGP->win.getSize().x-100, 300));
		lGP->color1t.setString(std::to_string(leftColor));
		c = GetColor(rightColor);
		lGP->rightct.update((unsigned char*)&c);
		lGP->rightcs.setPosition(sf::Vector2f(lGP->win.getSize().x-1, 350));
		lGP->color2t.setPosition(sf::Vector2f(lGP->win.getSize().x-100, 350));
		lGP->color2t.setString(std::to_string(rightColor));
		lGP->zoomt.setString("Zoom x" + std::to_string(zoomAmmount));
		lGP->sizet.setString("Size x" + std::to_string(sizeAmmount));
		lGP->selectS.setPosition(sf::Vector2f(lGP->win.getSize().x-1, 0));
		lGP->framet.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 450));
		lGP->framet.setString("< > + - Frame " + std::to_string(currFrame));
		lGP->tempt.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 500));
		lGP->cpt.setPosition(sf::Vector2f(lGP->win.getSize().x-256, 550));
		lGP->gpuBuffer.update((unsigned char*)(frames.begin()+currFrame)->get());
		lGP->gpuSprite.setScale(sf::Vector2f(zoomAmmount, zoomAmmount));
		lGP->gpuSprite.setTextureRect(sf::IntRect(0,0,sizeAmmount,sizeAmmount));
		lGP->tempS.setScale(tempSize,tempSize);
		lGP->win.draw(lGP->gpuSprite);
		if(drawTemp)
			lGP->win.draw(lGP->tempS);
		lGP->win.draw(lGP->selectS);
		lGP->win.draw(lGP->loadsave);
		lGP->win.draw(lGP->zoomt);
		lGP->win.draw(lGP->sizet);
		lGP->win.draw(lGP->leftcs);
		lGP->win.draw(lGP->rightcs);
		lGP->win.draw(lGP->fillt);
		lGP->win.draw(lGP->brusht);
		lGP->win.draw(lGP->framet);
		lGP->win.draw(lGP->tempt);
		lGP->win.draw(lGP->cpt);
		lGP->win.draw(lGP->color1t);
		lGP->win.draw(lGP->color2t);
		lGP->win.display();
	}

	std::string GetString(void)
	{
		std::string retstr;
		sf::RenderWindow textwin;
		textwin.create(sf::VideoMode(350,50), "EnterText", sf::Style::Default);
		textwin.setFramerateLimit(60);
		sf::Font font;
		sf::Text text;
		font.loadFromFile("arial.ttf");
		text.setFont(font);
		while(textwin.isOpen())
		{
			sf::Event ent;
			while(textwin.pollEvent(ent))
			{
				switch(ent.type)
				{
				case sf::Event::Closed:
					textwin.close();
					break;
				case sf::Event::KeyPressed:
					if(sf::Keyboard::Key::A <= ent.key.code && ent.key.code <= sf::Keyboard::Key::Z)
					{
						if(ent.key.shift)
							retstr.push_back(char(ent.key.code - sf::Keyboard::Key::A + 'A'));
						else
							retstr.push_back(char(ent.key.code - sf::Keyboard::Key::A + 'a'));
					}
					else if(sf::Keyboard::Key::Num0 <= ent.key.code && ent.key.code <= sf::Keyboard::Key::Num9)
						retstr.push_back(char(ent.key.code - sf::Keyboard::Key::Num0 + '0'));
					else
					{
						switch(ent.key.code)
						{
						case sf::Keyboard::Key::Escape:
							retstr.clear();
						case sf::Keyboard::Key::Return:
							textwin.close();
							break;
						case sf::Keyboard::Key::BackSpace:
							if(!retstr.empty())
								retstr.pop_back();
							break;
						case sf::Keyboard::Key::BackSlash:
							retstr.push_back('\\');
							break;
						case sf::Keyboard::Key::Slash:
							retstr.push_back('/');
							break;
						case sf::Keyboard::Key::Period:
							retstr.push_back('.');
							break;
						}
					}
					text.setString(retstr.c_str());
					break;
				}
			}
			textwin.clear(sf::Color(0,0,0,255));
			textwin.draw(text);
			textwin.display();
		}
		return retstr;
	}
	
	void Save(std::string const &filename)
	{
		std::ofstream file(filename, std::ios_base::binary);
		if(!file.is_open()) return;
		file.put(char(maxFrame));
		file.put(char(sizeAmmount-1));
		for(auto & frame : frames)
		{
			for(int y=0 ; y<sizeAmmount ; ++y)
			{
				for(int x=0 ; x<sizeAmmount ; ++x)
				{
					Color c = frame.get()[(y)*256+(x)];
					file.put(char(Get8Bit(c)));
				}
			}
		}
	}

#define DoError() { std::cout << "An error has occured opening the file.\n"; maxFrame=0; currFrame=0; zoomAmmount=1; sizeAmmount=8; frames.clear(); return; }
	
	void Load(std::string const &filename)
	{
		std::ifstream file(filename, std::ios_base::binary);
		if(!file.is_open()) return;
		char f;
		char s;
		file.get(f);
		file.get(s);
		maxFrame = unsigned char(f);
		sizeAmmount = unsigned char(s)+1;
		if(maxFrame >= 200)
			DoError();
		if(!(sizeAmmount == 8 || sizeAmmount == 16 || sizeAmmount == 32 || sizeAmmount == 64 || sizeAmmount == 128 || sizeAmmount == 256))
			DoError();
		currFrame = 0;
		zoomAmmount = 1;
		frames.clear();
		for(int i=0 ; i<=maxFrame ; ++i)
		{
			std::unique_ptr<Color[]> newframe(new Color[65536]);
			for(int i=0 ; i<65536 ; ++i)
				newframe.get()[i] = Color{0,0,0,255};
			for(int y=0 ; y<sizeAmmount ; ++y)
			{
				for(int x=0 ; x<sizeAmmount ; ++x)
				{
					unsigned char c;
					if(!file.get((char&)c)) DoError();
					newframe.get()[(y)*256+(x)] = GetColor(c);
				}
			}
			frames.push_back(std::move(newframe));
		}
	}
	
	void Copy(void)
	{
		std::memcpy(copyBuffer.get(), (frames.begin()+currFrame)->get(), sizeof(Color)*65536);
	}
	
	void Paste(void)
	{
		std::memcpy((frames.begin()+currFrame)->get(), copyBuffer.get(), sizeof(Color)*65536);
	}

	void FlipX(void)
	{
		auto buffer = (frames.begin()+currFrame)->get();
		for(int i = 0 ; i < sizeAmmount ; ++i, (buffer += 256))
			std::reverse(buffer, buffer + sizeAmmount);
	}

	void FlipY(void)
	{
		std::cout << "Flip Y\n";
	}
}

int main(void)
{
	lGP = new LazyGlobals();
	OnCreate();
	int click = -1;

	while(lGP->win.isOpen())
	{
		sf::Event ent;
		while(lGP->win.pollEvent(ent))
		{
			switch(ent.type)
			{
			case sf::Event::Closed:
				lGP->win.close();
				break;
			case sf::Event::Resized:
				lGP->win.setView( sf::View(sf::Vector2f(ent.size.width/2, ent.size.height/2), sf::Vector2f(float(ent.size.width), float(ent.size.height))) );
				break;
			case sf::Event::MouseButtonPressed:
				if(ent.mouseButton.button == sf::Mouse::Left) { OnClick(false, false, ent.mouseButton.x, ent.mouseButton.y); click = 0; }
				if(ent.mouseButton.button == sf::Mouse::Right) { OnClick(true, false, ent.mouseButton.x, ent.mouseButton.y); click = 1; }
				break;
			case sf::Event::MouseButtonReleased:
			case sf::Event::LostFocus:
				click = -1;
				break;
			case sf::Event::MouseMoved:
				if(click!=-1) OnClick(click?true:false, true, ent.mouseMove.x, ent.mouseMove.y);
				break;
			}
		}

		OnDraw();
	}
}
