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
#include <stdexcept>
#include <string>
#include <new>

using namespace std;

#include "CoreSound.h"
#include "CoreSoundFileStructs.h"

//==================================================================================
// Constructor/Destructor
//==================================================================================

CoreSound::CoreSound(const char *filename)
{
	ifstream ifile;

	mNumChannels = 0;
	mNumBytesPerSample = 0;
	mNumSampleFrames = 0;
	mSampleRate = 0;
	mData = 0;
	mLoopActive = false;
	mLoopBeginFrame = 0;
	mLoopEndFrame = 0;

	mFileName = filename;

	#ifdef DEBUGCORE
	cout << "opening sound " << filename << endl;
	#endif
	ifile.open(filename, ios_base::in | ios_base::binary);
	if (ifile.good())
	{
		if (checkIFF(ifile, AIFF))
		{
			loadAIFF(ifile);
		}
		else if (checkRIFF(ifile, WAVE))
		{
			loadWAVE(ifile);
		}
		else
		{
			string error = "Unsupported format in file:\n";
			error += filename;
			throw runtime_error(error);
		}
	}
	else
	{
		string error = "Could not open file:\n";
		error += filename;
		throw runtime_error(error);
	}
}

//----------------------------------------------------------------------------------

CoreSound::~CoreSound()
{
	#ifdef DEBUGCORE
	cout << "delete sound\n";
	#endif
	delete[] mData;
}

//==================================================================================
// Loader
//==================================================================================

void CoreSound::loadAIFF(ifstream &ifile)
{
	const static int MAX_MARKER_ID = 4;
	unsigned long markers[MAX_MARKER_ID] = {0, 0, 0, 0};
	unsigned short loopBeginMarkerID;
	unsigned short loopEndMarkerID;

	long int id;
	do
	{
		id = getNextChunkID(ifile);
		switch (id)
		{
			case AIFF_COMM: {
				AIFFCommon *common = (AIFFCommon *) readChunk(ifile);
				mNumChannels = common->numChannels;
				mNumSampleFrames = common->numSampleFrames;
				mNumBytesPerSample = common->sampleSize / 8;
				mSampleRate = (long) extendedToFloat(common->sampleRate);
				delete[] common;
				if (mNumBytesPerSample != 2)
				{
					string error = "File is not in 16 bit format:\n";
					error += mFileName;
					throw runtime_error(error);
				}
			} break;

			case AIFF_MARK: {
				char *chunk = readChunk(ifile);
				int	numMarkers = *((unsigned short *)chunk);
				char *chPos = (char *)((unsigned long)chunk + 2);
				for (int i = 0; i < numMarkers; i++)
				{
					unsigned short id = *((short *) chPos); chPos += 2; //short
					unsigned long position = *((unsigned long *) chPos); chPos += 4; //long
					int count = *((unsigned char *)chPos); chPos += count + 1 + ((count + 1) % 2); //pstring

					if (id <= MAX_MARKER_ID)
					{
						markers[id - 1] = position;
					}
				}
				delete[] chunk;
			} break;

			case AIFF_INST: {
				AIFFInstrument *instr = (AIFFInstrument *) readChunk(ifile);
				short loopMode = instr->sustainLoop.PlayMode;
				if (loopMode > 0)
				{
					mLoopActive = true;
				}
				loopBeginMarkerID = instr->sustainLoop.beginLoop;
				loopEndMarkerID = instr->sustainLoop.endLoop;

				delete[] instr;
			} break;

			case AIFF_SSND: {
				unsigned long size = 0;
				unsigned long bytes = mNumSampleFrames * mNumChannels * mNumBytesPerSample;
				ifile.read((char *) &size, 4);
				size -= 8;
				if (size == bytes)
				{
					ifile.seekg(8, ios::cur);
					mData = new char[bytes];
					ifile.read(mData, bytes);
				}
				else
				{
					string error = "Corrupt file:\n";
					error += mFileName;
					throw runtime_error(error);
				}
			} break;

			case END_OF_FILE: break;

			default:
//						  cout << "unknown\n";
				skipChunk(ifile);
		}
	} while (id != END_OF_FILE);

	mLoopEndFrame = mNumSampleFrames;
	if (mLoopActive)
	{
		if (loopBeginMarkerID <= MAX_MARKER_ID && loopEndMarkerID <= MAX_MARKER_ID)
		{
			if (markers[loopEndMarkerID - 1] > 0)
			{
				mLoopBeginFrame = markers[loopBeginMarkerID - 1];
				mLoopEndFrame = markers[loopEndMarkerID - 1];
			}
			else
			{
				//marker was not defined
				mLoopActive = false;
			}
		}
	}
}

//----------------------------------------------------------------------------------

void CoreSound::loadWAVE(ifstream &ifile)
{
	long int id;
	do
	{
		id = getNextRChunkID(ifile);
		switch (id)
		{
			case WAVE_FMT: {
				WAVEFormat *format = (WAVEFormat *) readRChunk(ifile);
				mNumChannels = intelShort(format->numberOfChannels);
				mNumBytesPerSample = intelShort(format->significantBitsPerSample) / 8;
				mSampleRate = intelLong(format->sampleRate);
				short compression = intelShort(format->compressionCode);
				delete[] format;
				if (compression != 1) // not PCM
				{
					string error = "File has unsupported compression:\n";
					error += mFileName;
					throw runtime_error(error);
				}
				if (mNumBytesPerSample != 2) // not 16 Bit
				{
					string error = "File is not in 16 bit format:\n";
					error += mFileName;
					throw runtime_error(error);
				}
			} break;

			case WAVE_DATA: {
				unsigned long size = 0;
				ifile.read((char *) &size, 4);
				size = intelLong(size);
				mNumSampleFrames = size / mNumBytesPerSample / mNumChannels;

				mData = new char[size];
				ifile.read(mData, size);

				short *shortData = (short *) mData;
				int length = size / sizeof(short);
				for (int i = 0; i < length; i++)
				{
					shortData[i] = intelShort(shortData[i]);
				}
			} break;

			case END_OF_FILE: break;

			default:
//						  cout << "unknown\n";
				skipRChunk(ifile);
		}
	} while (id != END_OF_FILE);

}


//==================================================================================
// Attributes
//==================================================================================

char *CoreSound::getFileName()
{
	return (char *) mFileName.c_str();
}

//==================================================================================
// IFF handling
//==================================================================================

bool CoreSound::checkIFF(ifstream &ifile, long formatID)
{
	long id = 0;
	unsigned long size = 0;
	long fid = 0;

	ifile.read((char *) &id, 4);
	ifile.read((char *) &size, 4);
	ifile.read((char *) &fid, 4);

	if (id == FORM && fid == formatID)
	{
		return true;
	}
	else
	{
		ifile.seekg(0, ios::beg);
		return false;
	}
}

//----------------------------------------------------------------------------------

long CoreSound::getNextChunkID(ifstream &ifile)
{
	long id;
	ifile.read((char *) &id, 4);
	if (ifile.fail() || ifile.eof())
	{
		return END_OF_FILE;
	}
	else
	{
		return id;
	}
}

//----------------------------------------------------------------------------------

void CoreSound::skipChunk(ifstream &ifile)
{
	unsigned long size;

	ifile.read((char *) &size, 4);
	ifile.seekg(size, ios::cur);
}

//----------------------------------------------------------------------------------

char *CoreSound::readChunk(ifstream &ifile)
{
	unsigned long size;
	char *data;

	ifile.read((char *) &size, 4);
	data = new char[size];
	ifile.read(data, size);
	return data;
}

//----------------------------------------------------------------------------------

float CoreSound::extendedToFloat(char *extended)
{
	short se = *((short *)extended);
	unsigned long m1 = *((unsigned long	*)((unsigned long)extended + 2));
	unsigned long m2 = *((unsigned long	*)((unsigned long)extended + 6));

	short s = (se & 0x8000);
	short e = (se & 0x7FFF) - 16383;
	m1 &= 0x7FFFFFFF;

	long val = ((long)s << 16) | ((e + 127) << 23) | (m1 >> 8);

	return *( (float *) &val );
}

//==================================================================================
// RIFF handling
//==================================================================================

bool CoreSound::checkRIFF(ifstream &ifile, long formatID)
{
	long id = 0;
	unsigned long size = 0;
	long fid = 0;

	ifile.read((char *) &id, 4);
	ifile.read((char *) &size, 4);
	ifile.read((char *) &fid, 4);

	if (id == RIFF && fid == formatID)
	{
		return true;
	}
	else
	{
		ifile.seekg(0, ios::beg);
		return false;
	}
}

//----------------------------------------------------------------------------------

long CoreSound::getNextRChunkID(ifstream &ifile)
{
	long id;
	ifile.read((char *) &id, 4);
	if (ifile.fail() || ifile.eof())
	{
		return END_OF_FILE;
	}
	else
	{
		return id;
	}
}

//----------------------------------------------------------------------------------

void CoreSound::skipRChunk(ifstream &ifile)
{
	unsigned long size;

	ifile.read((char *) &size, 4);
	size = intelLong(size);
	if (size % 2) //word align
	{
		size++;
	}
	ifile.seekg(size, ios::cur);
}

//----------------------------------------------------------------------------------

char *CoreSound::readRChunk(ifstream &ifile)
{
	unsigned long size;
	char *data;

	ifile.read((char *) &size, 4);
	size = intelLong(size);
	if (size % 2) //word align
	{
		size++;
	}
	data = new char[size];
	ifile.read(data, size);
	return data;
}

//----------------------------------------------------------------------------------

inline short CoreSound::intelShort(short value)
{
	return ((value << 8) & 0xFF00) | ((value >> 8) & 0x00FF);
}

//----------------------------------------------------------------------------------

inline long CoreSound::intelLong(long value)
{
	return ((value << 24) & 0xFF000000) | ((value << 8) & 0x00FF0000) | ((value >> 8) & 0x0000FF00) | ((value >> 24) & 0x000000FF);
}


