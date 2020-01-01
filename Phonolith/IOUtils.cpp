/*
Phonolith Software Sampler
Copyright (C) 2006-2008 Timo Kloss

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <new>

using namespace std;

#include "IOUtils.h"
#include "FileRequest.h"
#include "FrameApp.h"
#include "Texts.h"

#include <proto/dos.h>

//==================================================================================
// Sample Path Requester
//==================================================================================

string IOUtils::requestSamplePath(const char *oldFileName, const char *instrumentName)
{
	string result = oldFileName;

	char *filePart = IDOS->FilePart(oldFileName);

	int len = strlen(oldFileName);
	char pathPart[len + 1];
	strcpy(pathPart, oldFileName);
	* (IDOS->PathPart(pathPart)) = 0;

	string message = Texts::REQ_SELECT_MANUALLY_1;
	message += instrumentName;
	message += Texts::REQ_SELECT_MANUALLY_2;
	message += oldFileName;
	message += Texts::REQ_SELECT_MANUALLY_3;
	FrameApp::systemMessage(FrameApp::REQ_WARNING, message.c_str());

	FileRequest *req = new FileRequest("#?", pathPart, filePart);
	char *newFileName = req->requestLoadFile(Texts::ASL_IO_SELECT_SAMPLE_FILE);
	if (newFileName)
	{
		result = newFileName;
	}
	delete req;

	return result;
}

//==================================================================================
// DOS
//==================================================================================

string IOUtils::changePathOfFile(const char *oldFileName, const char *newPath)
{
	//get filename from	oldFileName
	char *filePart = IDOS->FilePart(oldFileName);

	//get path from newPath
	int len = strlen(newPath);
	char pathPart[len + 1];
	strcpy(pathPart, newPath);
	* (IDOS->PathPart(pathPart)) = 0;

	len = strlen(pathPart) + strlen(filePart) + 4;
	char resultName[len];
	strcpy(resultName, pathPart);
	IDOS->AddPart(resultName, filePart, len);
	string result = resultName;

	return result;
}

//----------------------------------------------------------------------------------

bool IOUtils::fileExists(const char *fileName)
{
	ifstream ifile;

	/* stop any "Insert volume..." type requesters */
	APTR oldwin = IDOS->SetProcWindow((APTR)-1);

	ifile.open(fileName, ios_base::in | ios_base::binary);
	bool success = ifile.good();
	ifile.close();

	/* turn requesters back on */
	IDOS->SetProcWindow( oldwin );

	return success;
}

