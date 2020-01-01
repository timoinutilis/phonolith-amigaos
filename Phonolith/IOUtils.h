#ifndef _IOUTILS_
#define _IOUTILS_

#include <string>

class IOUtils
{
	public:
		static string requestSamplePath(const char *oldFileName, const char *instrumentName);
		static string changePathOfFile(const char *oldFileName, const char *newPath);
		static bool fileExists(const char *fileName);

};

#endif
