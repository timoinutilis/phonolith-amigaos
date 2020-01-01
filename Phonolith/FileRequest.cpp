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
#include <cstring>

using namespace std;

#include "FileRequest.h"

#include <workbench/startup.h>

#include <proto/asl.h>
#include <proto/dos.h>

//==================================================================================
// Constructor/Destructor
//==================================================================================

FileRequest::FileRequest(const char *pattern, const char *drawer, const char *file)
{
	char *path = (char *) drawer;
	char pathBuffer[1024];
	if (!drawer || strlen(drawer) == 0)
	{
		IDOS->NameFromLock(IDOS->GetCurrentDir(), pathBuffer, 1023);
		path = pathBuffer;
	}
	mRequester = (FileRequester *) IAsl->AllocAslRequestTags(ASL_FileRequest,
			ASLFR_DoPatterns, TRUE,
			ASLFR_RejectIcons, TRUE,
			ASLFR_InitialPattern, pattern,
			ASLFR_InitialDrawer, path,
			ASLFR_InitialFile, file,
			TAG_DONE);

	mFileString = "";
	mFileList = NULL;
}

//----------------------------------------------------------------------------------

FileRequest::~FileRequest()
{
	IAsl->FreeAslRequest((APTR) mRequester);
	freeFileVector();
}

//==================================================================================
// Requests
//==================================================================================

char *FileRequest::requestSaveFile(const char *title, const char *postfix)
{
	if (IAsl->AslRequestTags(mRequester,
			ASLFR_DoSaveMode, TRUE,
			ASLFR_TitleText, title,
			ASLFR_DrawersOnly, FALSE,
			TAG_DONE))
	{
		int size = strlen(mRequester->fr_Drawer) + strlen(mRequester->fr_File) + 2;
		char buffer[size];
		strcpy(buffer, mRequester->fr_Drawer);
		IDOS->AddPart(buffer, mRequester->fr_File, size);
		mFileString = buffer;
		if (postfix)
		{
			if (mFileString.rfind((const char *) postfix) != mFileString.length() - strlen(postfix))
			{
				mFileString += postfix;
			}
		}
		return (char *) mFileString.c_str();
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------------

char *FileRequest::requestLoadFile(const char *title)
{
	if (IAsl->AslRequestTags(mRequester,
			ASLFR_DoSaveMode, FALSE,
			ASLFR_DoMultiSelect, FALSE,
			ASLFR_TitleText, title,
			ASLFR_DrawersOnly, FALSE,
			TAG_DONE))
	{
		int size = strlen(mRequester->fr_Drawer) + strlen(mRequester->fr_File) + 2;
		char buffer[size];
		strcpy(buffer, mRequester->fr_Drawer);
		IDOS->AddPart(buffer, mRequester->fr_File, size);
		mFileString = buffer;
		return (char *) mFileString.c_str();
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------------

Vector *FileRequest::requestLoadMultiFiles(const char *title)
{
	if (IAsl->AslRequestTags(mRequester,
			ASLFR_DoSaveMode, FALSE,
			ASLFR_DoMultiSelect, TRUE,
			ASLFR_TitleText, title,
			ASLFR_DrawersOnly, FALSE,
			TAG_DONE))
	{
		freeFileVector();
		mFileList = new Vector();

		int num = mRequester->fr_NumArgs;
		WBArg *files = mRequester->fr_ArgList;
		for (int i = 0; i < num; i++)
		{
			char *fileName = files[i].wa_Name;

			int size = strlen(mRequester->fr_Drawer) + strlen(fileName) + 2;
			char *buffer = new char[size];
			strcpy(buffer, mRequester->fr_Drawer);
			IDOS->AddPart(buffer, fileName, size);
			mFileList->addElement((void *) buffer);
		}

		return mFileList;
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------------

void FileRequest::freeFileVector()
{
	if (mFileList)
	{
		for (int i = mFileList->getSize() - 1; i >= 0; i--)
		{
			delete[] (char *)mFileList->getElement(i);
		}
		delete mFileList;
		mFileList = NULL;
	}
}

//----------------------------------------------------------------------------------

char *FileRequest::requestDirectory(const char *title)
{
	if (IAsl->AslRequestTags(mRequester,
			ASLFR_DoSaveMode, FALSE,
			ASLFR_DoMultiSelect, FALSE,
			ASLFR_TitleText, title,
			ASLFR_DrawersOnly, TRUE,
			TAG_DONE))
	{
		mFileString = mRequester->fr_Drawer;
		return (char *) mFileString.c_str();
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------------

char *FileRequest::getFileString()
{
	if (mFileString.length() > 0)
	{
		return (char *) mFileString.c_str();
	}
	else
	{
		return NULL;
	}
}

