#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>
#include "../Core/Audio.h"
#include "../Core/BinaryFile.h"

#if SFML_VERSION_MAJOR < 2 && SFML_VERSION_MINOR < 4
#define setFillColor setColor
#endif

namespace
{
	// struct
	struct Clickable
	{
		virtual bool Click(int x, int y, bool drag) = 0;
		virtual void Unclick(void){}
	};

	struct Slider : public Clickable
	{
		sf::Vector2f center;
		int value = 0;
		float cx = 0;
		int min = 0;
		int max = 2000;
		bool active = false;

		void Set(sf::Vector2f pos, int start) { center = pos; value = start; }
		void Draw(void);
		bool Click(int x, int y, bool drag);
		void Unclick(void);
	};

	struct CVal { int v; char const *s; };
	struct Cycle : public Clickable
	{
		sf::Vector2f center;
		size_t curval = 0;
		std::vector<CVal> values;
		bool clicked = false;
		bool active = false;

		void Set(sf::Vector2f pos, std::initializer_list<CVal> const &vs) { center = pos; values = vs; curval = 0; }
		void SetCurrent(int v) { for(size_t i=0 ; i<values.size() ; ++i) if(values[i].v == v) { curval = i; break; } }
		int GetCurrent(void) { return values[curval].v; }
		void Draw(void);
		bool Click(int x, int y, bool drag);
		void Unclick(void);
	};

	struct Button : public Clickable
	{
		sf::Vector2f center;
		sf::Vector2f size;
		std::string text;
		bool clicked = false;
		bool active = true;
		void (*func)(void) = nullptr;

		void Set(sf::Vector2f pos, sf::Vector2f wh, std::string const &str, void (*f)(void)){ center = pos; size = wh; text = str; func = f; }
		void Draw(void);
		bool Click(int x, int y, bool drag);
		void Unclick(void);
	};

	// vars
#define WHITE sf::Color(255, 255, 255, 255)
#define PURPLE sf::Color(255, 0, 255, 255)
#define CYAN sf::Color(0, 255, 255, 255)
#define YELLOW sf::Color(255, 255, 0, 255)
#define GREY sf::Color(128, 128, 128, 255)
#define BLACK sf::Color(0, 0, 0, 255)
#define TRANSPARENT(_c) (_c-sf::Color(0, 0, 0, 128))

	struct LazyGlobals
	{
		sf::RenderWindow win;
		sf::Font font;
	};
	LazyGlobals *lGP = nullptr;

	std::vector<Clickable*> clickables;

	Button saveButton;
	Button loadButton;
	Button seekLeftButton;
	Button seekRightButton;
	Button seekPosButton;
	Button playStopButton;
	Button copyButton;
	Button pasteButton;

	Slider square1Freq;
	Cycle square1Duty;
	Cycle square1Volume;
	Slider square2Freq;
	Cycle square2Duty;
	Cycle square2Volume;
	Slider triangleFreq;
	Slider noiseFreq;
	Cycle noiseMode;

	std::map<int32_t, Note> square1Notes;
	std::vector<std::pair<int32_t, Note>> square1Select;
	std::map<int32_t, Note> square2Notes;
	std::vector<std::pair<int32_t, Note>> square2Select;
	std::map<int32_t, Note> triangleNotes;
	std::vector<std::pair<int32_t, Note>> triangleSelect;
	std::map<int32_t, Note> noiseNotes;
	std::vector<std::pair<int32_t, Note>> noiseSelect;
	bool square1Refresh = false;
	bool square2Refresh = false;
	bool triangleRefresh = false;
	bool noiseRefresh = false;

	int32_t currentPos = 0;
	int32_t screenPos = 0;
	int32_t screenWidth = 0;
	bool playingStatus = false;
	bool tickCounter = false;
	int32_t playingStartPos = 0;
	int32_t selectionInitClickX = 0;
	bool selectionOnEdge = false;
	std::clock_t lastClickTime = 0;
	std::vector<std::pair<int32_t, std::pair<int32_t, Note>>> clipBoard;

	// declares
	void Draw_Line(sf::Vector2f const &p1, sf::Vector2f const &p2, sf::Color const &color);
	void Draw_Circle(sf::Vector2f const &center, float radius, sf::Color const &color);
	void Draw_Rect(sf::Rect<float> const &rect, sf::Color const &color);
	void Draw_Number(sf::Vector2f const &center, int number, sf::Color const &color);
	void Draw_Text(sf::Vector2f const &center, char const *text, sf::Color const &color);
	bool Circle2Point(sf::Vector2f const &center, float radius, sf::Vector2f const &point);
	std::string GetString(std::string init);
	void Save(void);
	void Load(void);
	void SeekLeft(void);
	void SeekRight(void);
	void SetSeekPos(void);
	void PlayStop(void);
	void Copy(void);
	void Paste(void);

	// funcs
	void OnCreate(void)
	{
		lGP->win.create(sf::VideoMode(800,600), "The One Sound Editor", sf::Style::Default);
		lGP->win.setFramerateLimit(60);
		lGP->win.setView( sf::View(sf::Vector2f(400, 300), sf::Vector2f(float(800), float(600))) );
		lGP->font.loadFromFile("arial.ttf");
		screenWidth = int32_t(lGP->win.getSize().x - 160)/10;

		clickables.push_back(nullptr);
		square1Freq.Set(sf::Vector2f(80, 180), 440);
		clickables.push_back(&square1Freq);
		square1Duty.Set(sf::Vector2f(80, 130), { {int(Duty::x50),"50%"}, {int(Duty::x13),"13%"}, {int(Duty::x25),"25%"} });
		clickables.push_back(&square1Duty);
		square1Volume.Set(sf::Vector2f(80, 80), { {int(Volume::vx3),"Full"}, {int(Volume::vx0),"None"}, {int(Volume::vx1),"Some"}, {int(Volume::vx2),"More"} });
		clickables.push_back(&square1Volume);

		square2Freq.Set(sf::Vector2f(80, 360), 440);
		clickables.push_back(&square2Freq);
		square2Duty.Set(sf::Vector2f(80, 310), { {int(Duty::x50),"50%"}, {int(Duty::x13),"13%"}, {int(Duty::x25),"25%"} });
		clickables.push_back(&square2Duty);
		square2Volume.Set(sf::Vector2f(80, 260), { {int(Volume::vx3),"Full"}, {int(Volume::vx0),"None"}, {int(Volume::vx1),"Some"}, {int(Volume::vx2),"More"} });
		clickables.push_back(&square2Volume);

		triangleFreq.Set(sf::Vector2f(80, 430), 440);
		clickables.push_back(&triangleFreq);

		noiseFreq.Set(sf::Vector2f(80, 500), 1024);
		noiseFreq.min = 0;
		noiseFreq.max = 8000;
		clickables.push_back(&noiseFreq);
		noiseMode.Set(sf::Vector2f(80, 565), { {0, "Mode 1"}, {1, "Mode 2"} });
		clickables.push_back(&noiseMode);

		saveButton.Set(sf::Vector2f(160, 25), sf::Vector2f(40, 20), "Save", Save);
		clickables.push_back(&saveButton);
		loadButton.Set(sf::Vector2f(80, 25), sf::Vector2f(40, 20), "Load", Load);
		clickables.push_back(&loadButton);
		seekLeftButton.Set(sf::Vector2f(240, 25), sf::Vector2f(10, 25), "<", SeekLeft);
		clickables.push_back(&seekLeftButton);
		seekRightButton.Set(sf::Vector2f(265, 25), sf::Vector2f(10, 25), ">", SeekRight);
		clickables.push_back(&seekRightButton);
		seekPosButton.Set(sf::Vector2f(330, 25), sf::Vector2f(40, 25), "", SetSeekPos);
		clickables.push_back(&seekPosButton);
		playStopButton.Set(sf::Vector2f(410, 25), sf::Vector2f(30, 25), "Play", PlayStop);
		clickables.push_back(&playStopButton);
		copyButton.Set(sf::Vector2f(500, 25), sf::Vector2f(30, 25), "Copy", Copy);
		clickables.push_back(&copyButton);
		pasteButton.Set(sf::Vector2f(590, 25), sf::Vector2f(30, 25), "Paste", Paste);
		clickables.push_back(&pasteButton);
	}

	void OnUpdate(void)
	{
		if(playingStatus && tickCounter)
		{
			auto findfunc = [](std::pair<int32_t, Note> const &r){ return r.first==currentPos; };

			auto square1atpos = std::find_if(square1Select.begin(), square1Select.end(), findfunc);
			if(square1atpos != square1Select.end())
				AudioSetSquare1Note(square1atpos->second);
			else
			{
				auto square1atpos = square1Notes.find(currentPos);
				if(square1atpos != square1Notes.end())
					AudioSetSquare1Note(square1atpos->second);
			}
			
			auto square2atpos = std::find_if(square2Select.begin(), square2Select.end(), findfunc);
			if(square2atpos != square2Select.end())
				AudioSetSquare2Note(square2atpos->second);
			else
			{
				auto square2atpos = square2Notes.find(currentPos);
				if(square2atpos != square2Notes.end())
					AudioSetSquare2Note(square2atpos->second);
			}

			auto triangleatpos = std::find_if(triangleSelect.begin(), triangleSelect.end(), findfunc);
			if(triangleatpos != triangleSelect.end())
				AudioSetTriangleNote(triangleatpos->second);
			else
			{
				auto triangleatpos = triangleNotes.find(currentPos);
				if(triangleatpos != triangleNotes.end())
					AudioSetTriangleNote(triangleatpos->second);
			}

			auto noiseatpos = std::find_if(noiseSelect.begin(), noiseSelect.end(), findfunc);
			if(noiseatpos != noiseSelect.end())
				AudioSetNoiseNote(noiseatpos->second);
			else
			{
				auto noiseatpos = noiseNotes.find(currentPos);
				if(noiseatpos != noiseNotes.end())
					AudioSetNoiseNote(noiseatpos->second);
			}

			int32_t end = 0;
			if(!square1Notes.empty()) end = std::max(end, square1Notes.rbegin()->first + (square1Notes.rbegin()->second.length/2));
			for(auto & note : square1Select) end = std::max(end, note.first + (note.second.length/2));
			if(!square2Notes.empty()) end = std::max(end, square2Notes.rbegin()->first + (square2Notes.rbegin()->second.length/2));
			for(auto & note : square2Select) end = std::max(end, note.first + (note.second.length/2));
			if(!triangleNotes.empty()) end = std::max(end, triangleNotes.rbegin()->first + (triangleNotes.rbegin()->second.length/2));
			for(auto & note : triangleSelect) end = std::max(end, note.first + (note.second.length/2));
			if(!noiseNotes.empty()) end = std::max(end, noiseNotes.rbegin()->first + (noiseNotes.rbegin()->second.length/2));
			for(auto & note : noiseSelect) end = std::max(end, note.first + (note.second.length/2));
			if(end < currentPos)
				PlayStop();
			else
				++currentPos;
		}
		tickCounter = !tickCounter;

		if(currentPos < screenPos)
			screenPos = std::max(0, currentPos-(screenWidth/4));
		if(screenPos + screenWidth < currentPos)
			screenPos = currentPos - screenWidth + (screenWidth/4);

		if(square1Select.size() == 1)
		{
			if(square1Refresh)
			{
				square1Duty.active = square1Freq.active = square1Volume.active = true;
				square1Freq.value = square1Select[0].second.freq;
				square1Duty.SetCurrent(square1Select[0].second.mode);
				square1Volume.SetCurrent(int(square1Select[0].second.volume));
				square1Refresh = false;
			}
			else
			{
				square1Select[0].second.freq = square1Freq.value;
				square1Select[0].second.mode = square1Duty.GetCurrent();
				square1Select[0].second.volume = Volume::Type(square1Volume.GetCurrent());
			}
		}
		else
			square1Duty.active = square1Freq.active = square1Volume.active = false;

		if(square2Select.size() == 1)
		{
			if(square2Refresh)
			{
				square2Duty.active = square2Freq.active = square2Volume.active = true;
				square2Freq.value = square2Select[0].second.freq;
				square2Duty.SetCurrent(square2Select[0].second.mode);
				square2Volume.SetCurrent(int(square2Select[0].second.volume));
				square2Refresh = false;
			}
			else
			{
				square2Select[0].second.freq = square2Freq.value;
				square2Select[0].second.mode = square2Duty.GetCurrent();
				square2Select[0].second.volume = Volume::Type(square2Volume.GetCurrent());
			}
		}
		else
			square2Duty.active = square2Freq.active = square2Volume.active = false;

		if(triangleSelect.size() == 1)
		{
			if(triangleRefresh)
			{
				triangleFreq.active = true;
				triangleFreq.value = triangleSelect[0].second.freq;
				triangleRefresh = false;
			}
			else
				triangleSelect[0].second.freq = triangleFreq.value;
		}
		else
			triangleFreq.active = false;

		if(noiseSelect.size() == 1)
		{
			if(noiseRefresh)
			{
				noiseFreq.active = noiseMode.active = true;
				noiseFreq.value = noiseSelect[0].second.freq;
				noiseMode.SetCurrent(noiseSelect[0].second.mode);
				noiseRefresh = false;
			}
			else
			{
				noiseSelect[0].second.freq = noiseFreq.value;
				noiseSelect[0].second.mode = noiseMode.GetCurrent();
			}
		}
		else
			noiseFreq.active = noiseMode.active = false;
	}

#define DRAWNOTES(_cont, _sel, _y, _h) { auto start = _cont.lower_bound(screenPos); \
	if(start != _cont.end()){ \
		while(start != _cont.begin() && screenPos < start->first + (start->second.length/2)) \
			--start; \
		while(start != _cont.end() && start->first < screenPos + screenWidth){ \
			Draw_Rect(sf::Rect<float>(160.0f + (start->first - screenPos) * 10.0f, _y, start->second.length * 5.0f, _h), GREY); \
			++start; \
		}} \
	for(auto &select : _sel) { \
		Draw_Rect(sf::Rect<float>(160.0f + (select.first - screenPos) * 10.0f, _y, select.second.length * 5.0f, _h), YELLOW); \
	}}

	void OnDraw(void)
	{
		lGP->win.clear();
		
		DRAWNOTES(square1Notes, square1Select, 50, 170);
		DRAWNOTES(square2Notes, square2Select, 220, 180);
		DRAWNOTES(triangleNotes, triangleSelect, 400, 70);
		DRAWNOTES(noiseNotes, noiseSelect, 470, 130);
		for(unsigned int i=160 ; i<lGP->win.getSize().x ; i+=10)
			Draw_Line(sf::Vector2f(float(i), 50.0f), sf::Vector2f(float(i), 600.0f), GREY);
		Draw_Rect(sf::Rect<float>(float(160+(10*(currentPos-screenPos))), 50.0f, 10.0f, 550.0f), TRANSPARENT(YELLOW));

		Draw_Rect(sf::Rect<float>(0.0f, 0.0f, 160.0f, 600), BLACK);

		saveButton.Draw();
		loadButton.Draw();
		seekLeftButton.Draw();
		seekRightButton.Draw();
		seekPosButton.text = std::to_string(currentPos);
		seekPosButton.Draw();
		playStopButton.text = (playingStatus) ? "Stop" : "Play";
		playStopButton.Draw();
		copyButton.Draw();
		pasteButton.Draw();
		Draw_Line(sf::Vector2f(0, 50), sf::Vector2f(10000, 50), CYAN);

		square1Freq.Draw();
		square1Duty.Draw();
		square1Volume.Draw();
		Draw_Line(sf::Vector2f(0, 220), sf::Vector2f(10000, 220), CYAN);

		square2Freq.Draw();
		square2Duty.Draw();
		square2Volume.Draw();
		Draw_Line(sf::Vector2f(0, 400), sf::Vector2f(10000, 400), CYAN);

		triangleFreq.Draw();
		Draw_Line(sf::Vector2f(0, 470), sf::Vector2f(10000, 470), CYAN);

		noiseFreq.Draw();
		noiseMode.Draw();

		lGP->win.display();
	}
#undef DRAWNOTES

#define CLICKNOTES(_cont, _sel, _y, _h, _bol) { std::pair<int32_t, Note> save; save.first = -1; if(_y < y && y < _y + _h) { \
	for(auto select = _sel.begin() ; select != _sel.end() ; ++select) { \
		if((160 + (select->first - screenPos) * 10) <= x && x < ((160 + (select->first - screenPos) * 10) + select->second.length * 5)) { \
			if(((160 + (select->first - screenPos) * 10) + (select->second.length * 5) - 3) < x) selectionOnEdge = true; \
			save = *select; \
			_sel.erase(select); \
			break; } \
	} \
	if(save.first == -1) { auto start = _cont.lower_bound(screenPos); if(start != _cont.end()) { \
		while(start != _cont.begin() && screenPos < start->first + (start->second.length/2)) \
			--start; \
		while(start != _cont.end() && start->first < screenPos + screenWidth) { \
			if((160 + (start->first - screenPos) * 10) <= x && x < ((160 + (start->first - screenPos) * 10) + start->second.length * 5)) { \
				if(((160 + (start->first - screenPos) * 10) + (start->second.length * 5) - 3) < x) selectionOnEdge = true; \
				save = *start; \
				_bol = true; \
				auto l = start++; \
				_cont.erase(l); } \
			else { \
				++start; } \
	}}} \
	if(!alt) { for(auto select = _sel.begin() ; select != _sel.end() ; ++select) { \
		auto ret = _cont.emplace(select->first, select->second); \
		if(ret.second == false) ret.first->second = select->second; } \
	_sel.clear(); } \
	if(save.first != -1) _sel.push_back(save); \
	}}

#define CREATEDELETE(_sel, _y, _h, _bol, _defnote) if(_y < y && y < _y + _h) { bool delhappend = false; \
	if(x > 160) { \
		for(auto select = _sel.begin() ; select != _sel.end() ; ++select) { \
			if((160 + (select->first - screenPos) * 10) <= x && x < ((160 + (select->first - screenPos) * 10) + select->second.length * 5)) { \
				_sel.erase(select); \
				delhappend = true; \
				break; \
		}} \
		if(delhappend == false) { \
			_sel.push_back(std::pair<int32_t, Note>(((x-160)/10)+screenPos, _defnote)); \
			_bol = true; \
	}}}

	void OnClick(bool drag, int x, int y, bool alt)
	{
		if(!drag)
		{
			auto f = std::find_if(clickables.begin(), clickables.end(), [x,y](Clickable *lhs){ if(lhs != nullptr) return lhs->Click(x, y, false); return false; });
			if(f != clickables.end())
				std::swap(*f, *clickables.begin());
			else
			{
				if(std::clock() - lastClickTime < 300 && !alt)
				{
					CREATEDELETE(square1Select, 50, 170, square1Refresh, Note(440, Volume::vx3, Duty::x50, 3));
					CREATEDELETE(square2Select, 220, 180, square2Refresh, Note(440, Volume::vx3, Duty::x50, 3));
					CREATEDELETE(triangleSelect, 400, 70, triangleRefresh, Note(440, Volume::vx3, Duty::x50, 3));
					CREATEDELETE(noiseSelect, 470, 130, noiseRefresh, Note(1024, Volume::vx3, Duty::x50, 3));
				}
				else
				{
					CLICKNOTES(square1Notes, square1Select, 50, 170, square1Refresh);
					CLICKNOTES(square2Notes, square2Select, 220, 180, square2Refresh);
					CLICKNOTES(triangleNotes, triangleSelect, 400, 70, triangleRefresh);
					CLICKNOTES(noiseNotes, noiseSelect, 470, 130, noiseRefresh);
				}
				std::swap(*std::find(clickables.begin(), clickables.end(), nullptr), *clickables.begin());
			}
			selectionInitClickX = x;
		}
		else
		{
			if(clickables.front() != nullptr)
				clickables.front()->Click(x, y, true);
			else
			{
				if(selectionOnEdge)
				{
					int32_t moveover = (x-selectionInitClickX)/5;
					selectionInitClickX += moveover * 5;
					if(moveover != 0)
					{
						if(50 < y && y < 220)
							for(auto &select : square1Select) select.second.length = std::max(1, select.second.length + moveover);
						if(220 < y && y < 400)
							for(auto &select : square2Select) select.second.length = std::max(1, select.second.length + moveover);
						if(400 < y && y < 470)
							for(auto &select : triangleSelect) select.second.length = std::max(1, select.second.length + moveover);
						if(470 < y && y < 600)
							for(auto &select : noiseSelect) select.second.length = std::max(1, select.second.length + moveover);
					}
				}
				else
				{
					int32_t moveover = (x-selectionInitClickX)/10;
					selectionInitClickX += moveover * 10;
					if(moveover != 0)
					{
						if(50 < y && y < 220)
							for(auto &select : square1Select) select.first = std::max(0, select.first + moveover);
						if(220 < y && y < 400)
							for(auto &select : square2Select) select.first = std::max(0, select.first + moveover);
						if(400 < y && y < 470)
							for(auto &select : triangleSelect) select.first = std::max(0, select.first + moveover);
						if(470 < y && y < 600)
							for(auto &select : noiseSelect) select.first = std::max(0, select.first + moveover);
					}
				}
			}
		}
		lastClickTime = std::clock();
	}
#undef CREATEDELETE
#undef CLICKNOTES

	void OnUnClick(void)
	{
		for(auto click : clickables)
			if(click != nullptr)
				click->Unclick();
		selectionOnEdge = false;
	}

	void Draw_Line(sf::Vector2f const &p1, sf::Vector2f const &p2, sf::Color const &color)
	{
		sf::Vertex line[2] = {sf::Vertex(p1, color), sf::Vertex(p2, color)};
		lGP->win.draw(line, 2, sf::Lines);
	}

	void Draw_Circle(sf::Vector2f const &center, float radius, sf::Color const &color)
	{
		sf::CircleShape c(radius);
		c.setFillColor(color);
		c.setPosition(center-sf::Vector2f(radius,radius));
		lGP->win.draw(c);
	}

	void Draw_Rect(sf::Rect<float> const &rect, sf::Color const &color)
	{
		sf::RectangleShape r(sf::Vector2f(rect.width,rect.height));
		r.setFillColor(color);
		r.setPosition(sf::Vector2f(rect.left,rect.top));
		lGP->win.draw(r);
	}

	void Draw_Number(sf::Vector2f const &center, int number, sf::Color const &color)
	{
		std::string str(std::to_string(number));
		Draw_Text(center, str.c_str(), color);
	}

	void Draw_Text(sf::Vector2f const &center, char const *text, sf::Color const &color)
	{
		sf::Text t;
		t.setFont(lGP->font);
		t.setString(text);
		auto r = t.getGlobalBounds();
		t.setPosition(center-sf::Vector2f(r.width/2, r.height/2));
		t.setFillColor(color);
		lGP->win.draw(t);
	}

	bool Circle2Point(sf::Vector2f const &center, float radius, sf::Vector2f const &point)
	{
		return (center.x-point.x)*(center.x-point.x) + (center.y-point.y)*(center.y-point.y) > radius*radius;
	}

	void Slider::Draw(void)
	{
		Draw_Number(center+sf::Vector2f(0, -10), value, (active) ? WHITE : GREY);
		Draw_Line(center+sf::Vector2f(-50, 25), center+sf::Vector2f(50, 25), (active) ? WHITE : GREY);
		Draw_Line(center+sf::Vector2f(-50, 20), center+sf::Vector2f(-50, 30), (active) ? WHITE : GREY);
		Draw_Line(center+sf::Vector2f(50, 20), center+sf::Vector2f(50, 30), (active) ? WHITE : GREY);
		Draw_Circle(center+sf::Vector2f(cx, 25), 10, (active) ? ((cx==0) ? WHITE : PURPLE) : GREY);
	}

	bool Slider::Click(int x, int y, bool drag)
	{
		if(active)
		{
			if((center.y+20 < y && y < center.y+30 && center.x-50 < x && x < center.x+50) || drag)
			{
				cx = std::min(50.0f, std::max(-50.0f, x-center.x));
				value = std::min(max, std::max(min, value+int(cx/5)));
				return true;
			}
			else if(center.y-10 < y && y < center.y+10 && center.x-50 < x && x < center.x+50)
			{
				std::string newval = GetString(std::to_string(value));
				int newint = 0;
				try
				{
					newint = stoi(newval);
				}
				catch(...){}
				if(min <= newint && newint <= max)
					value = newint;
			}
		}
		return false;
	}

	void Slider::Unclick(void)
	{
		cx = 0;
	}

	void Cycle::Draw(void)
	{
		Draw_Line(center+sf::Vector2f(-75, -25), center+sf::Vector2f(75, -25), (active) ? ((clicked) ? PURPLE : WHITE) : GREY);
		Draw_Line(center+sf::Vector2f(-75, -25), center+sf::Vector2f(-75, 25), (active) ? ((clicked) ? PURPLE : WHITE) : GREY);
		Draw_Line(center+sf::Vector2f(-75, 25), center+sf::Vector2f(75, 25), (active) ? ((clicked) ? PURPLE : WHITE) : GREY);
		Draw_Line(center+sf::Vector2f(75, 25), center+sf::Vector2f(75, -25), (active) ? ((clicked) ? PURPLE : WHITE) : GREY);
		if(!values.empty()) Draw_Text(center, values[curval].s, (active) ? ((clicked) ? PURPLE : WHITE) : GREY);
	}

	bool Cycle::Click(int x, int y, bool drag)
	{
		if(active)
		{
			if(center.y-25 < y && y < center.y+25 && center.x-75 < x && x < center.x+75 && !clicked)
			{
				clicked = true;
				++curval;
				if(curval >= values.size()) curval = 0;
				return true;
			}
		}
		return false;
	}

	void Cycle::Unclick(void)
	{
		clicked = false;
	}

	void Button::Draw(void)
	{
		Draw_Text(center, text.c_str(), (active) ? WHITE : GREY);
	}

	bool Button::Click(int x, int y, bool drag)
	{
		if(active)
		{
			if(center.y-size.y < y && y < center.y+size.y && center.x-size.x < x && x < center.x+size.x)
			{
				clicked = true;
				return true;
			}
		}
		return false;
	}

	void Button::Unclick(void)
	{
		if(clicked)
		{
			if(func)
				func();
		}
		clicked = false;
	}

	std::string GetString(std::string retstr)
	{
		sf::RenderWindow textwin;
		textwin.create(sf::VideoMode(350,50), "EnterText", sf::Style::Default);
		textwin.setFramerateLimit(60);
		sf::Font font;
		sf::Text text;
		font.loadFromFile("arial.ttf");
		text.setFont(font);
		text.setString(retstr.c_str());
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

#define DoError() { std::cout << "An error has occured saving the file.\n"; return; }
#define TestNote(_it, _cont, _num) while(_it!=_cont.end() && _it->first<=i) { \
	file.Write(_it->first); \
	file.Write(int32_t(_it->second.length)); \
	file.Write(int8_t(_num)); \
	file.Write(int32_t(_it->second.freq)); \
	file.Write(int8_t(_it->second.mode)); \
	switch(_it->second.volume){ \
		case Volume::vx0:file.Write(int8_t(0));break; \
		case Volume::vx1:file.Write(int8_t(1));break; \
		case Volume::vx2:file.Write(int8_t(2));break; \
		default:file.Write(int8_t(3));break;} \
	++_it;}
	#define FLATENHELPER(_cont, _sel) for(auto select = _sel.begin() ; select != _sel.end() ; ++select) { \
	auto ret = _cont.emplace(select->first, select->second); \
	if(ret.second == false) ret.first->second = select->second; } \
	_sel.clear();

	void Save(void)
	{
		FLATENHELPER(square1Notes, square1Select);
		FLATENHELPER(square2Notes, square2Select);
		FLATENHELPER(triangleNotes, triangleSelect);
		FLATENHELPER(noiseNotes, noiseSelect);

		std::string filename = GetString("");
		BinaryFile file;

		int32_t size = int32_t(square1Notes.size() + square2Notes.size() + triangleNotes.size() + noiseNotes.size());
		if(size < 0)
			DoError();
		file.Write(size);

		auto s1i = square1Notes.begin();
		auto s2i = square2Notes.begin();
		auto ti = triangleNotes.begin();
		auto ni = noiseNotes.begin();
		for(int32_t i=0 ; s1i != square1Notes.end() || s2i != square2Notes.end() || ti != triangleNotes.end() || ni != noiseNotes.end() ; ++i)
		{
			TestNote(s1i, square1Notes, 1);
			TestNote(s2i, square2Notes, 2);
			TestNote(ti, triangleNotes, 3);
			TestNote(ni, noiseNotes, 4);
		}

		if(!file.Save(filename))
			DoError();
	}
#undef FLATENHELPER
#undef TestNote
#undef DoError

#define DoError() { std::cout << "An error has occured opening the file.\n"; return; }
	void Load(void)
	{
		std::string filename = GetString("");
		BinaryFile file;
		if(!file.Load(filename))
			DoError();

		int32_t size = -1;
		file.Read(size);
		if(size < 0)
			DoError();
		std::map<int32_t, Note> newSquare1Notes;
		std::map<int32_t, Note> newSquare2Notes;
		std::map<int32_t, Note> newTriangleNotes;
		std::map<int32_t, Note> newNoiseNotes;

		int32_t lastPos = -1;
		for(int32_t i=0 ; i<size ; ++i)
		{
			Note note;
			int32_t pos = -1;
			file.Read(pos);
			if(lastPos > pos)
				DoError();

			int32_t len = 0;
			file.Read(len);
			note.length = int(len);

			int8_t channel = -1;
			file.Read(channel);

			int32_t freq = 0;
			file.Read(freq);
			note.freq = int(freq);

			int8_t mode = -1;
			file.Read(mode);
			note.mode = mode;

			int8_t volume = -1;
			file.Read(volume);
			switch(volume)
			{
			case 0: note.volume = Volume::vx0; break;
			case 1: note.volume = Volume::vx1; break;
			case 2: note.volume = Volume::vx2; break;
			default: case 3: note.volume = Volume::vx3; break;
			}

			note.priority = Priority::Low;
			switch(channel)
			{
			case 1: newSquare1Notes.emplace(pos, note); break;
			case 2: newSquare2Notes.emplace(pos, note); break;
			case 3: newTriangleNotes.emplace(pos, note); break;
			case 4: newNoiseNotes.emplace(pos, note); break;
			default: DoError();
			}
		}

		currentPos = 0;
		screenPos = 0;
		playingStatus = false;
		square1Select.clear();
		square2Select.clear();
		triangleSelect.clear();
		noiseSelect.clear();
		square1Notes = std::move(newSquare1Notes);
		square2Notes = std::move(newSquare2Notes);
		triangleNotes = std::move(newTriangleNotes);
		noiseNotes = std::move(newNoiseNotes);
	}
#undef DoError

	void SeekLeft(void)
	{
		if(playingStatus)
			return;
		--currentPos;
		if(currentPos < 0)
			currentPos = 0;
	}

	void SeekRight(void)
	{
		if(playingStatus)
			return;
		++currentPos;
	}

	void SetSeekPos(void)
	{
		if(playingStatus)
			return;
		std::string newval = GetString(std::to_string(currentPos));
		int32_t newint = 0;
		try
		{
			newint = int32_t(stoi(newval));
		}
		catch(...){}
		if(0 <= newint)
			currentPos = newint;
	}

	void PlayStop(void)
	{
		if(playingStatus)
		{
			currentPos = playingStartPos;
			playingStatus = false;

			Note note;
			note.priority = Priority::High;
			AudioSetSquare1Note(note);
			AudioSetSquare2Note(note);
			AudioSetTriangleNote(note);
			AudioSetNoiseNote(note);
		}
		else
		{
			playingStartPos = currentPos;
			playingStatus = true;
		}
	}

#define COPYHELPER(_sel, _ind) if(!_sel.empty()) { \
	int32_t s = std::min_element(_sel.begin(), _sel.end(), [](std::pair<int32_t, Note> const &l, std::pair<int32_t, Note> const &r){ return l.first < r.first; })->first; \
	for(auto & e : _sel) { \
		tempclip.push_back(std::pair<int32_t, std::pair<int32_t, Note>>(_ind, std::pair<int32_t, Note>(e.first - s, e.second))); } \
	}

	void Copy(void)
	{
		std::vector<std::pair<int32_t, std::pair<int32_t, Note>>> tempclip;
		COPYHELPER(square1Select, 0);
		COPYHELPER(square2Select, 1);
		COPYHELPER(triangleSelect, 2);
		COPYHELPER(noiseSelect, 3);
		if(!tempclip.empty())
			clipBoard = std::move(tempclip);
	}
#undef COPYHELPER

#define PASTEHELPER(_cont, _sel) for(auto select = _sel.begin() ; select != _sel.end() ; ++select) { \
	auto ret = _cont.emplace(select->first, select->second); \
	if(ret.second == false) ret.first->second = select->second; } \
	_sel.clear();

	void Paste(void)
	{
		PASTEHELPER(square1Notes, square1Select);
		PASTEHELPER(square2Notes, square2Select);
		PASTEHELPER(triangleNotes, triangleSelect);
		PASTEHELPER(noiseNotes, noiseSelect);
		
		for(auto & ele : clipBoard)
		{
			switch(ele.first)
			{
			case 0: square1Select.push_back(std::pair<int32_t, Note>(ele.second.first + currentPos, ele.second.second)); break;
			case 1: square2Select.push_back(std::pair<int32_t, Note>(ele.second.first + currentPos, ele.second.second)); break;
			case 2: triangleSelect.push_back(std::pair<int32_t, Note>(ele.second.first + currentPos, ele.second.second)); break;
			case 3: noiseSelect.push_back(std::pair<int32_t, Note>(ele.second.first + currentPos, ele.second.second)); break;
			}
		}
	}
#undef PASTEHELPER
}

int main(void)
{
	lGP = new LazyGlobals;
	AudioInit();
	OnCreate();
	int click = -1;
	bool altpressed = false;

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
				screenWidth = int32_t(ent.size.width - 160)/10;
				break;
			case sf::Event::MouseButtonPressed:
				OnClick(false, ent.mouseButton.x, ent.mouseButton.y, altpressed); click = 0;
				break;
			case sf::Event::LostFocus:
				altpressed = false;
			case sf::Event::MouseButtonReleased:
				click = -1;
				OnUnClick();
				break;
			case sf::Event::MouseMoved:
				if(click!=-1) OnClick(true, ent.mouseMove.x, ent.mouseMove.y, altpressed);
				break;
			case sf::Event::KeyPressed:
				if(ent.key.code == sf::Keyboard::LAlt || ent.key.code == sf::Keyboard::RAlt)
					altpressed = true;
				break;
			case sf::Event::KeyReleased:
				if(ent.key.code == sf::Keyboard::LAlt || ent.key.code == sf::Keyboard::RAlt)
					altpressed = false;
				break;
			}
		}

		OnUpdate();
		AudioUpdate();
		OnDraw();
	}
}
