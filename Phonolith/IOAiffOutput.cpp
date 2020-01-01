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
#include <string.h>
#include <stdexcept>

using namespace std;

#include "IOAiffOutput.h"

#include <exec/exec.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/asyncio.h>

#pragma pack(2)
struct AiffHeader
{
	long formID;
	unsigned long formSize;
	long formType;

	//COMM
	long commID;
	unsigned long commSize;
	short numChannels;
	unsigned long numSampleFrames;
	short sampleSize;
	char sampleRate[10]; //80 bit float

	//SSND
	long ssndID;
	unsigned long ssndSize;
	unsigned long offset;
	unsigned long blockSize;
};
#pragma pack()

//==================================================================================
// Constructor/Destructor
//==================================================================================

IOAiffOutput::IOAiffOutput()
{
	mFrames = 0;

	mChannels = 0;
	mBytesPerSample = 0;
	mFrequency = 0;

	mFile = NULL;
}

//----------------------------------------------------------------------------------

IOAiffOutput::~IOAiffOutput()
{
}

//==================================================================================
// File Creating
//==================================================================================

bool IOAiffOutput::openFile(const char *fileName, int channels, int bytesPerSample, unsigned long frequency)
{
	mFrames = 0;

	mChannels = channels;
	mBytesPerSample = bytesPerSample;
	mFrequency = frequency;

	mFile = IDOS->FOpen(fileName, MODE_NEWFILE, 0);

	if (mFile)
	{
		char header[sizeof(AiffHeader)];
		memset(header, 0, sizeof(AiffHeader));
		long written = IDOS->FWrite(mFile, header, 1, sizeof(AiffHeader));
		if (written != sizeof(AiffHeader))
		{
			IDOS->FClose(mFile);
			mFile = NULL;
			return false;
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

bool IOAiffOutput::closeFile()
{
	if (mFile)
	{
		int64 size = IDOS->GetFilePosition(mFile);
		IDOS->ChangeFilePosition(mFile, (int64) 0, OFFSET_BEGINNING);

		AiffHeader header;
		memset((char *)&header, 0, sizeof(AiffHeader));

		//FORM
		header.formID = I_FORM;
		header.formSize = size - 8;
		header.formType = I_AIFF;

		//COMM
		header.commID = I_COMM;
		header.commSize = 18;
		header.numChannels = mChannels;
		header.numSampleFrames = mFrames;
		header.sampleSize = mBytesPerSample * 8;
		floatToExtended((float)mFrequency, header.sampleRate);

		//SSND
		header.ssndID = I_SSND;
		header.ssndSize = 8 + mFrames * mChannels * mBytesPerSample;
		header.offset = 0;
		header.blockSize = 0;

		long written = IDOS->FWrite(mFile, &header, 1, sizeof(AiffHeader));

		IDOS->FClose(mFile);
		mFile = NULL;

		return (written == sizeof(AiffHeader));
	}
	return false;

}

//----------------------------------------------------------------------------------

bool IOAiffOutput::isOpen()
{
	return (mFile != NULL);
}

//==================================================================================
// Writer
//==================================================================================

bool IOAiffOutput::writeData(void *buffer, unsigned long frames)
{
	mFrames += frames;
	int bytes = frames * mChannels * mBytesPerSample;
	long written = IDOS->FWrite(mFile, buffer, 1, bytes);

	return (written == bytes);
}

//==================================================================================
// Util
//==================================================================================

void IOAiffOutput::floatToExtended(float value, char *extended)
{
	unsigned long valuebits = *((unsigned long *) &value);
	short s = (valuebits & 0x80000000) >> 16;
	short e = ((valuebits >> 23) & 0x000000FF) - 127;
	unsigned long m = (valuebits & 0x007FFFFF);

	short *se = (short *)extended;
	unsigned long *m1 = (unsigned long *)((unsigned long)extended + 2);
	unsigned long *m2 = (unsigned long *)((unsigned long)extended + 6);

	*se = s | (e + 16383);
	*m1 = 0x80000000 | (m << 8);
	*m2 = 0x00000000;
}

