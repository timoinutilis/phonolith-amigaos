#ifndef _FILEREQUEST_
#define _FILEREQUEST_

#include <string>

#include "Vector.h"

#include <libraries/asl.h>


class FileRequest
{
	public:
		FileRequest(const char *pattern, const char *drawer = NULL, const char *file = "");
		~FileRequest();

		char *requestSaveFile(const char *title, const char *postfix = 0);
		char *requestLoadFile(const char *title);
		Vector *requestLoadMultiFiles(const char *title);
		void freeFileVector();
		char *requestDirectory(const char *title);
		char *getFileString();

	protected:
		FileRequester *mRequester;
		string mFileString;
		Vector *mFileList;
};

#endif

