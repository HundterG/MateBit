#include "Globals.h"
#include <cstdarg>
#include <array>
#include <memory>
#include <mutex>

namespace Globals
{
	bool quit = false;
	bool fullScreen = false;
	Size windowSize = {768, 768};
	Size fullScreenSize = {0, 0};
	Aspect::Ratio aspectRatio = Aspect::x11;
	float volume = 100.0f;
}

void DirScan(char const *dir, char const *ext, void (*callback)(std::string&))
{
	std::array<char, 128> buffer;
	std::string fileList;
#if defined(_WIN32)
	std::shared_ptr<FILE> pipe(_popen((std::string("dir /B /S ") + dir + "\\*." + ext).c_str(), "r"), _pclose);
#elif defined(__GNUG__)
	std::shared_ptr<FILE> pipe(popen((std::string("find ./") + dir + "/*." + ext + " -print").c_str(), "r"), pclose);
#else
#error "Must specify a folder scan protocol for this arcetecture."
#endif
	hardcheck(!pipe, "Dir could not be read.");
	while (!feof(pipe.get()))
	{
		if (fgets(buffer.data(), 128, pipe.get()) != NULL)
			fileList += buffer.data();
	}
	size_t scan = 0;
	while(scan < fileList.size())
	{
		size_t end = fileList.find("\n", scan);
		std::string file(fileList, scan, end - scan);
		scan = end + strlen("\n");
		std::replace(file.begin(), file.end(), '\\', '/');
		callback(file);
	}
}

#if !defined(_WIN32)
void localtime_s(struct tm * _Tm, const time_t * _Time)
{
	static std::mutex localTimeLock;
	std::lock_guard<std::mutex> guard(localTimeLock);
	*_Tm = *localtime(_Time);
}
#endif
