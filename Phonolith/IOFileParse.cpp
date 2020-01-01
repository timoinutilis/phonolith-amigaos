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
#include <stdexcept>
#include <new>

using namespace std;

#include "IOFileParse.h"
#include "Version.h"


//==================================================================================
// Constructor/Destructor
//==================================================================================

IOFileParse::IOFileParse(const char *filename, int mode, unsigned long formatID, unsigned short version)
{
	mMode = mode;
	mOutStream = NULL;
	mInStream = NULL;
	mBuffer = NULL;
	mBufferSize = BUFFER_SIZE_STEP;
	mPosition = 0;
	mVersion = version;
	mFileVersion = 0;

	if (mMode == SAVE)
	{
		#ifdef DEBUGIO
		cout << "open in save mode: " << filename << endl;
		#endif
		mBuffer = new char[mBufferSize];

		mOutStream = new ofstream(filename, ios_base::out | ios_base::binary);
		if (!mOutStream || mOutStream->fail())
		{
			throw runtime_error("Could not open file.");
		}

		mOutStream->write((char *) &formatID, 4);
		mOutStream->write((char *) &mVersion, 2);

	}
	else if (mMode == LOAD)
	{
		#ifdef DEBUGIO
		cout << "open in load mode: " << filename << endl;
		#endif
		mInStream = new ifstream(filename, ios_base::in | ios_base::binary);
		if (!mInStream || mInStream->fail())
		{
			string error = "Could not open file:\n";
			error += filename;
			throw runtime_error(error);
		}

		unsigned long fileFormatID = 0;
		mInStream->read((char *) &fileFormatID, 4);

		if (fileFormatID != formatID)
		{
			delete mInStream;
			string error = "Wrong file format:\n";
			error += filename;
			throw runtime_error(error);
		}

		mInStream->read((char *) &mFileVersion, 2);

	}
	else
	{
		throw invalid_argument("Wrong FileParse mode.");
	}

}

//----------------------------------------------------------------------------------

IOFileParse::~IOFileParse()
{
	if (mOutStream)
	{
		mOutStream->close();
		delete mOutStream;
	}
	if (mInStream)
	{
		mInStream->close();
		delete mInStream;
	}
	if (mBuffer) delete[] mBuffer;
}

//==================================================================================
// Save Mode
//==================================================================================

void IOFileParse::addTagShort(unsigned short tagID, short value)
{
	if ((tagID & 0x000F) != 0)
	{
		throw invalid_argument("invalid Short tagID");
	}
	short *shortPtr = (short *) &mBuffer[mPosition];
	*shortPtr++ = tagID;
	*shortPtr = value;
	mPosition += 4;
}

//----------------------------------------------------------------------------------

void IOFileParse::addTagLong(unsigned short tagID, long value)
{
	if ((tagID & 0x000F) != 1)
	{
		throw invalid_argument("invalid Long tagID");
	}
	short *shortPtr = (short *) &mBuffer[mPosition];
	*shortPtr++ = tagID;
	long *longPtr = (long *) shortPtr;
	*longPtr = value;

	mPosition += 6;
}

//----------------------------------------------------------------------------------

void IOFileParse::addTagString(unsigned short tagID, const char *value)
{
	if ((tagID & 0x000F) != 2)
	{
		throw invalid_argument("invalid String tagID");
	}
	short *shortPtr = (short *) &mBuffer[mPosition];
	*shortPtr++ = tagID;
	int length = strlen(value) + 1;
	*shortPtr = length;

	mPosition += 4;

	for (int i = 0; i < length; i++)
	{
		mBuffer[mPosition++] = value[i];
	}
}

//----------------------------------------------------------------------------------

void IOFileParse::addTagData(unsigned short tagID, unsigned long length, char *data)
{
	if ((tagID & 0x000F) != 3)
	{
		throw invalid_argument("invalid Data tagID");
	}
	short *shortPtr = (short *) &mBuffer[mPosition];
	*shortPtr++ = tagID;
	long *longPtr = (long *) shortPtr;
	*longPtr = length;

	mPosition += 6;

	for (int i = 0; i < length; i++)
	{
		mBuffer[mPosition++] = data[i];
	}
}

//----------------------------------------------------------------------------------

void IOFileParse::writeArea(unsigned long areaID)
{
	mOutStream->write((char *) &areaID, 4);
	mOutStream->write((char *) &mPosition, 4);
	mOutStream->write(mBuffer, mPosition);
	mPosition = 0;
}

//==================================================================================
// Load Mode
//==================================================================================

bool IOFileParse::isNewerVersion()
{
	return (mFileVersion > mVersion);
}

//----------------------------------------------------------------------------------

unsigned long IOFileParse::readNextArea()
{
	if (mBuffer)
	{
		delete[] mBuffer;
		mBuffer = NULL;
	}

	if (mInStream->eof())
	{
		return END_OF_FILE;
	}

	unsigned long areaID;
	mInStream->read((char *) &areaID, 4);
	mInStream->read((char *) &mBufferSize, 4);
	mBuffer = new char[mBufferSize];
	mInStream->read(mBuffer, mBufferSize);
	mPosition = 0;

	return areaID;
}

//----------------------------------------------------------------------------------

unsigned short IOFileParse::readNextTag(long *value, unsigned long *length)
{
	if (mPosition >= mBufferSize)
	{
		return END_OF_AREA;
	}

	short *shortPtr = (short *) &mBuffer[mPosition];
	unsigned short tagID = *shortPtr++;

	switch (tagID & 0x000F)
	{
		case 0: { //short
			*value = *shortPtr;
			mPosition += 4;
		} break;

		case 1: { //long
			*value = *(long *) shortPtr;
			mPosition += 6;
		} break;

		case 2: { //string
			*length = *shortPtr++;
			*value = (long) shortPtr;
			mPosition += 4 + *length;
		} break;

		case 3: { //data
			long *longPtr = (long *) shortPtr;
			*length = *longPtr++;
			*value = (long) longPtr;
			mPosition += 6 + *length;
		} break;

		default:
			throw runtime_error("File has invalid content.");
	}

	return tagID;
}

