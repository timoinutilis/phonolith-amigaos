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

#include "IOImporterEXS24.h"
#include "IOEXS24FileStructs.h"
#include "IOUtils.h"

#include <proto/dos.h>

bool IOImporterEXS24::mIntelMode = false;

//==================================================================================
// Importer
//==================================================================================

struct ExsZone
{
	long sampleNumber;
	unsigned long startPosition;
	unsigned long endPosition;
};

void IOImporterEXS24::import(char *fileName, CoreInstrument *instrument)
{
	ifstream ifile;
	string lastChangedPath;

	#ifdef DEBUGIO
	cout << "Import: " << fileName << endl;
	#endif

	ifile.open(fileName, ios_base::in | ios_base::binary);
	if (ifile.good())
	{
		long id = getNextChunkID(ifile);
		if (id != ID_INSTRUMENT)
		{
			string error = "No EXS24 format in file:\n";
			error += fileName;
			throw runtime_error(error);
		}

		instrument->reinit();

		EXS24Instrument *instr = (EXS24Instrument *) readChunk(ifile);

		instrument->mName = instr->name;

		long numZones = getLong(instr->numZones);

		delete instr;

		ExsZone exsZones[numZones];
		long counterZone = 0;
		long counterSample = 0;
		do
		{
			id = getNextChunkID(ifile);

			switch (id)
			{
				case ID_ZONE: {
					if (counterZone >= numZones)
					{
						string error = "Unexpected structure in file:\n";
						error += fileName;
						throw runtime_error(error);
					}
					EXS24Zone *zone = (EXS24Zone *) readChunk(ifile);

					//create zone
					exsZones[counterZone].sampleNumber = getLong(zone->sampleNumber);
					exsZones[counterZone].startPosition = getLong(zone->startPosition);
					exsZones[counterZone].endPosition = getLong(zone->endPosition);

					instrument->addZone(zone->name,
							NULL, //file name
							zone->keyLow, zone->keyHigh, zone->keyBase,
							0, 127, //velocity
							zone->roughTune,
							zone->fineTune,
							0); //panning

					counterZone++;

					delete zone;
				} break;

				/*case ID_GROUP: {
					chunk = readChunk(ifile);
					char *name = (char *)(&chunk[3]);

					delete chunk;
				} break;*/

				case ID_SAMPLE: {
					EXS24Sample *sample = (EXS24Sample *) readChunk(ifile);
					char *fileName = sample->name;
					long fileFormat = getLong(sample->fileFormat);

					if (fileFormat == AIFF || fileFormat == WAVE)
					{
						//create original filename string
						string originalPath = convertPath(sample->path);
						int maxLen = originalPath.length() + strlen(fileName) + 2;
						char originalFileName[maxLen];
						strcpy(originalFileName, originalPath.c_str());
						IDOS->AddPart(originalFileName, fileName, maxLen);
						string soundFileName = originalFileName;

						//check for existance
						if (!IOUtils::fileExists(soundFileName.c_str()))
						{
							soundFileName = IOUtils::changePathOfFile(soundFileName.c_str(), lastChangedPath.c_str());

							if (!IOUtils::fileExists(soundFileName.c_str()))
							{
								soundFileName = IOUtils::requestSamplePath(originalFileName, instrument->mName.c_str());
								lastChangedPath = soundFileName;
							}
						}

						//load sample
						for (int i = 0; i < instrument->getNumOfZones(); i++)
						{
							if (counterSample == exsZones[i].sampleNumber)
							{
								instrument->changeZoneSound(i, NULL, soundFileName.c_str());
								instrument->setZonePositions(i, exsZones[i].startPosition, exsZones[i].endPosition);
							}
						}
					}
					else
					{
						string error = "Unsupported sample format in file:\n";
						error += fileName;
						throw runtime_error(error);
					}
					counterSample++;

					delete sample;
				} break;

				/*case ID_PARAM: {
					chunk = readChunk(ifile);
					char *name = (char *)&chunk[3];

					delete chunk;
				} break;*/

				default:
					skipChunk(ifile);
			}
		} while (id != ID_EOF);

		ifile.close();
	}
	else
	{
		string error = "Could not open file:\n";
		error += fileName;
		throw runtime_error(error);
	}

}

//==================================================================================
// Chunk handling
//==================================================================================

long IOImporterEXS24::getNextChunkID(ifstream &ifile)
{
	long id;
	ifile.read((char *) &id, 4);
	if (ifile.fail() || ifile.eof())
	{
		return ID_EOF;
	}
	else
	{
		mIntelMode = ((id & 0xFF000000) == 0x01000000);
		return id & 0x00FFFFFF;
	}
}

//----------------------------------------------------------------------------------

void IOImporterEXS24::skipChunk(ifstream &ifile)
{
	unsigned long size;

	ifile.read((char *) &size, 4);
	ifile.seekg(getLong(size) + 76, ios::cur);
}

//----------------------------------------------------------------------------------

unsigned long *IOImporterEXS24::readChunk(ifstream &ifile)
{
	unsigned long size;
	unsigned long *data;

	ifile.read((char *) &size, 4);
	size = getLong(size) + 76;
	data = new unsigned long[size / 4];
	ifile.read((char *)data, size);
	return data;
}

//==================================================================================
// Utilities
//==================================================================================

string IOImporterEXS24::convertPath(char *path)
{
	string amigaPath = path;

	if (strstr(path, ":\\"))
	{
		//Windows path
		amigaPath = "";
		int len = strlen(path);
		for (int i = 0; i < len; i++)
		{
			char c = path[i];
			if (c == ':')
			{
				amigaPath += c;
				i++; //skip next '\'
			}
			else if (c == '\\')
			{
				amigaPath += '/';
			}
			else
			{
				amigaPath += c;
			}
		}

	}
	else
	{
		//Macintosh path
		bool firstColon = true;
		for (int i = 0; i < amigaPath.length(); i++)
		{
			if (amigaPath[i] == ':')
			{
				if (firstColon)
				{
					firstColon = false;
				}
				else
				{
					amigaPath[i] = '/';
				}
			}
		}
	}
	return amigaPath;
}

//----------------------------------------------------------------------------------

short IOImporterEXS24::getShort(short value)
{
	if (mIntelMode)
	{
		return ((value << 8) & 0xFF00) | ((value >> 8) & 0x00FF);
	}
	return value;
}

//----------------------------------------------------------------------------------

long IOImporterEXS24::getLong(long value)
{
	if (mIntelMode)
	{
		return ((value << 24) & 0xFF000000) | ((value << 8) & 0x00FF0000) | ((value >> 8) & 0x0000FF00) | ((value >> 24) & 0x000000FF);
	}
	return value;
}

