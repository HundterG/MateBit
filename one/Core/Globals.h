#ifndef _GLOBALS_
#define _GLOBALS_

#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstring>
#include <cmath>
#include <exception>
#include <algorithm>

namespace Globals
{
	struct Size
	{
		unsigned int x;
		unsigned int y;
	};

	struct Aspect
	{
		enum Ratio
		{
			x11,
			x34,
			xany
		};
	};

	extern bool quit;
	extern bool fullScreen;
	extern Size windowSize;
	extern Size fullScreenSize;
	extern Aspect::Ratio aspectRatio;
	extern float volume;
}

typedef void* TypeID;
template <typename T>
TypeID GetTypeID(void)
{
	static char ref;
	return &ref;
}

void DirScan(char const *dir, char const *ext, void (*callback)(std::string&));
#if !defined(_WIN32)
void localtime_s(struct tm * _Tm, const time_t * _Time);
#endif

#define softcheck(__c, __m, __e) if(__c){ std::ofstream __file("Log.txt", std::ios_base::app); std::time_t __t(std::time(nullptr)); char __s[100]={0}; std::tm ___m; localtime_s(&___m,&__t); std::strftime(__s,sizeof(__s),"%m/%d/%y-%H:%M:%S",&___m); __file << "[" << __s << "] " << __m << "\n"; __file.close(); std::cout << "[" << __s << "] " << __m << "\n"; __e }
#define hardcheck(__c, __m) if(__c){ std::ofstream __file("Log.txt", std::ios_base::app); std::time_t __t(std::time(nullptr)); char __s[100]={0}; std::tm ___m; localtime_s(&___m,&__t); std::strftime(__s,sizeof(__s),"%m/%d/%y-%H:%M:%S",&___m); __file << "[" << __s << "] " << __m << "\n"; __file.close(); std::cout << "[" << __s << "] Fatal Error: " << __m << "\n"; throw std::runtime_error(__m); }
#define message(__m) softcheck(1, __m,)
#endif
