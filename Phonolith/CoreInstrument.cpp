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
#include <cmath>
#include <stdexcept>
#include <new>

using namespace std;

#include "CoreInstrument.h"
#include "IOFileParse.h"
#include "IOUtils.h"

#include "IOFileTagsInstrument.h"

//==================================================================================
// Constructor/Destructor
//==================================================================================

CoreInstrument::CoreInstrument()
{
	mZones = new Vector();
	mAmpEnvelope.setADSR(0, 0, 127, 0);
	mName = "";
	mNumVoices = 16;
}

//----------------------------------------------------------------------------------

CoreInstrument::~CoreInstrument()
{
	deleteAllZones();
	delete mZones;
}

//==================================================================================
// Settings
//==================================================================================

void CoreInstrument::reinit()
{
	deleteAllZones();
	mAmpEnvelope.setADSR(0, 0, 127, 0);
	mName = "";
	mNumVoices = 16;
}

//==================================================================================
// Disk
//==================================================================================

void CoreInstrument::saveInstrument(char *fileName)
{
	IOFileParse file(fileName, IOFileParse::SAVE, INSTRUMENTID, VERSION);

	//INFO
	file.addTagString(TAG_INFO_NAME, mName.c_str());
	file.writeArea(AREA_INFO);

	//SETTINGS
	file.addTagShort(TAG_SETTINGS_NUMVOICES, mNumVoices);
	file.writeArea(AREA_SETTINGS);

	//AMPENV
	file.addTagShort(TAG_AMPENV_ATTACK, mAmpEnvelope.mAttackTime);
	file.addTagShort(TAG_AMPENV_DECAY, mAmpEnvelope.mDecayTime);
	file.addTagShort(TAG_AMPENV_SUSTAIN, mAmpEnvelope.mSustainLevel);
	file.addTagShort(TAG_AMPENV_RELEASE, mAmpEnvelope.mReleaseTime);
	file.writeArea(AREA_AMPENV);

	//ZONEs
	for (int i = 0; i < getNumOfZones(); i++)
	{
		Zone *kg = getZone(i);
		file.addTagString(TAG_ZONE_NAME, (char *) kg->name.c_str());
		if (kg->sound)
		{
			file.addTagString(TAG_ZONE_FILENAME, kg->sound->getFileName());
		}
		file.addTagShort(TAG_ZONE_LOWKEY, kg->lowKey);
		file.addTagShort(TAG_ZONE_HIGHKEY, kg->highKey);
		file.addTagShort(TAG_ZONE_BASEKEY, kg->baseKey);
		file.addTagShort(TAG_ZONE_LOWVELO, kg->lowVelocity);
		file.addTagShort(TAG_ZONE_HIGHVELO, kg->highVelocity);
		file.addTagShort(TAG_ZONE_MAINTUNE, kg->mainTune);
		file.addTagShort(TAG_ZONE_FINETUNE, kg->fineTune);
		file.addTagShort(TAG_ZONE_PANNING, kg->panning);
		file.writeArea(AREA_ZONE);
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::loadInstrument(char *fileName)
{
	IOFileParse file(fileName, IOFileParse::LOAD, INSTRUMENTID, VERSION);

	reinit();

	unsigned long areaID;
	unsigned long tagID;
	long value;
	unsigned long length;

	string lastChangedPath;

	while ((areaID = file.readNextArea()) != IOFileParse::END_OF_FILE)
	{
		switch (areaID)
		{
			case AREA_INFO: {
				while ((tagID = file.readNextTag(&value, &length)) != IOFileParse::END_OF_AREA)
				{
					switch (tagID)
					{
						case TAG_INFO_NAME: mName = (const char *) value; break;
					}
				}
			} break;

			case AREA_SETTINGS: {
				while ((tagID = file.readNextTag(&value, &length)) != IOFileParse::END_OF_AREA)
				{
					switch (tagID)
					{
						case TAG_SETTINGS_NUMVOICES: mNumVoices = (short) value; break;
					}
				}
			} break;

			case AREA_AMPENV: {
				while ((tagID = file.readNextTag(&value, &length)) != IOFileParse::END_OF_AREA)
				{
					switch (tagID)
					{
						case TAG_AMPENV_ATTACK: mAmpEnvelope.mAttackTime = (char) value; break;
						case TAG_AMPENV_DECAY: mAmpEnvelope.mDecayTime = (char) value; break;
						case TAG_AMPENV_SUSTAIN: mAmpEnvelope.mSustainLevel = (char) value; break;
						case TAG_AMPENV_RELEASE: mAmpEnvelope.mReleaseTime = (char) value; break;
					}
				}
			} break;

			case AREA_ZONE: {
				char *name = "???";
				char *originalFileName = NULL;
				Zone kg = {"", NULL, 0, 0, 0, 0, 0, 0, 0};
				while ((tagID = file.readNextTag(&value, &length)) != IOFileParse::END_OF_AREA)
				{
					switch (tagID)
					{
						case TAG_ZONE_NAME: name = (char *) value; break;
						case TAG_ZONE_FILENAME: originalFileName = (char *) value; break;
						case TAG_ZONE_LOWKEY: kg.lowKey = (char) value; break;
						case TAG_ZONE_HIGHKEY: kg.highKey = (char) value; break;
						case TAG_ZONE_BASEKEY: kg.baseKey = (char) value; break;
						case TAG_ZONE_LOWVELO: kg.lowVelocity = (char) value; break;
						case TAG_ZONE_HIGHVELO: kg.highVelocity = (char) value; break;
						case TAG_ZONE_MAINTUNE: kg.mainTune = (signed char) value; break;
						case TAG_ZONE_FINETUNE: kg.fineTune = (signed char) value; break;
						case TAG_ZONE_PANNING: kg.panning = (signed char) value; break;
					}
				}

				string fileName = originalFileName;
				if (!IOUtils::fileExists(fileName.c_str()))
				{
					fileName = IOUtils::changePathOfFile(fileName.c_str(), lastChangedPath.c_str());

					if (!IOUtils::fileExists(fileName.c_str()))
					{
						fileName = IOUtils::requestSamplePath(originalFileName, mName.c_str());
						lastChangedPath = fileName;
					}
				}

				addZone(name, fileName.c_str(), kg.lowKey, kg.highKey, kg.baseKey, kg.lowVelocity, kg.highVelocity, kg.mainTune, kg.fineTune, kg.panning);
			} break;

		}
	}
}

//==================================================================================
// Zones
//==================================================================================

void CoreInstrument::addZone(const char *name, const char *soundFileName, char lowKey, char highKey, char baseKey, char lowVelocity, char highVelocity, signed char mainTune, signed char fineTune, signed char panning)
{
	#ifdef DEBUGCORE
	cout << "addZone\n";
	#endif
	Zone *newKg = new Zone;
	try
	{
		newKg->name = name;
		newKg->startPosition = 0;
		if (soundFileName)
		{
			newKg->sound = new CoreSound(soundFileName);
			newKg->endPosition = newKg->sound->mNumSampleFrames;
		}
		else
		{
			newKg->sound = NULL;
			newKg->endPosition = 0;
		}
		newKg->lowKey = lowKey;
		newKg->highKey = highKey;
		newKg->baseKey = baseKey;
		newKg->lowVelocity = lowVelocity;
		newKg->highVelocity = highVelocity;
		newKg->mainTune = mainTune;
		newKg->fineTune = fineTune;
		newKg->panning = panning;
		mZones->addElement(newKg);
	}
	catch (exception &e)
	{
		if (newKg)
		{
			delete newKg;
		}
		throw;
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::setZonePositions(int index, unsigned long start, unsigned long end)
{
	Zone *kg = getZone(index);

	if (!kg->sound || start < kg->sound->mNumSampleFrames)
	{
		kg->startPosition = start;
	}
	else
	{
		kg->startPosition = 0;
	}

	if (!kg->sound || end <= kg->sound->mNumSampleFrames)
	{
		kg->endPosition = end;
	}
	else
	{
		kg->endPosition = kg->sound->mNumSampleFrames;
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::deleteZone(int index)
{
	#ifdef DEBUGCORE
	cout << "deleteZone " << index << endl;
	#endif
	Zone *kg = getZone(index);
	if (kg->sound)
	{
		delete kg->sound;
	}
	delete kg;
	mZones->deleteElement(index);
}

//----------------------------------------------------------------------------------

void CoreInstrument::deleteAllZones()
{
	#ifdef DEBUGCORE
	cout << "deleteAllZones\n";
	#endif
	for (int i = getNumOfZones() - 1; i >= 0; i--)
	{
		Zone *kg = getZone(i);
		if (kg->sound)
		{
			delete kg->sound;
		}
		delete kg;
	}
	mZones->deleteAllElements();
}

//----------------------------------------------------------------------------------

Zone *CoreInstrument::getZone(int index)
{
	return (Zone *) mZones->getElement(index);
}

//----------------------------------------------------------------------------------

int CoreInstrument::getNumOfZones()
{
	return mZones->getSize();
}

//----------------------------------------------------------------------------------

bool CoreInstrument::moveZoneUp(int zoneIndex)
{
	if (zoneIndex > 0)
	{
		mZones->moveElement(zoneIndex, zoneIndex - 1);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

bool CoreInstrument::moveZoneDown(int zoneIndex)
{
	if (zoneIndex < getNumOfZones() - 1)
	{
		mZones->moveElement(zoneIndex, zoneIndex + 1);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

void CoreInstrument::changeZoneSound(int zoneIndex, const char *name, const char *soundFileName)
{
	Zone *kg = getZone(zoneIndex);
	if (kg->sound)
	{
		delete kg->sound;
		kg->sound = NULL;
	}
	if (soundFileName)
	{
		CoreSound *sound = new CoreSound(soundFileName);
		kg->sound = sound;

		kg->startPosition = 0;
		kg->endPosition = sound->mNumSampleFrames;
	}
	if (name)
	{
		kg->name = name;
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::zoneSortByBaseKey()
{
	//bubble sort
	int i = 0;
	while (i < getNumOfZones() - 1)
	{
		Zone *kg1 = getZone(i);
		Zone *kg2 = getZone(i + 1);
		if (kg1->baseKey > kg2->baseKey)
		{
			mZones->moveElement(i, i + 1);
			if (i > 0)
			{
				i -= 2; //with following i++ this is one step back
			}
		}
		i++;
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::zoneReadBaseKeysFromNames()
{
	for (int i = 0; i < getNumOfZones(); i++)
	{
		Zone *kg = getZone(i);
		int key = readKeyFromName(kg->name.c_str());
		if (key >= 0)
		{
			kg->baseKey = key;
		}
	}
}

//----------------------------------------------------------------------------------

int CoreInstrument::readKeyFromName(const char *name)
{
	const static char *gaps = " .-_";
	const static char *notes = "CDEFGAB";
	const static char *numbers = "0123456789";
	const static int PREGAP = 0;
	const static int GAP = 1;
	const static int NOTE = 2;
	const static int SIGN = 3;
	const static int OCTAVE = 4;
	int i = 0;
	int step = PREGAP;
	char *note = NULL;
	char *octave = NULL;

//	  cout << "check '" << name << "'\n";
	while (char ch = name[i])
	{
//		  cout << "  " << i << ":  char " << ch << "  step " << step << endl;
		if (step == PREGAP)
		{
			if (strchr(gaps, ch) != NULL)
			{
				step = GAP;
			}
		}
		else if (step == GAP)
		{
			if (strchr(notes, ch) != NULL)
			{
				note = (char *) &name[i];
				step = NOTE;
			}
			else if (strchr(gaps, ch) != NULL)
			{
				step = PREGAP;
			}
		}
		else if (step == NOTE)
		{
			if (ch == '-')
			{
				octave = (char *) &name[i];
				step = SIGN;
			}
			else if (strchr(numbers, ch) != NULL)
			{
				octave = (char *) &name[i];
				step = OCTAVE;
			}
			else if (ch != '#')
			{
				step = PREGAP;
			}
		}
		else if (step == SIGN)
		{
			if (strchr(numbers, ch) != NULL)
			{
				step = OCTAVE;
			}
			else
			{
				step = PREGAP;
			}
		}
		else if (step == OCTAVE)
		{
			if (strchr(numbers, ch) == NULL)
			{
				break;
			}
		}
		i++;
	}

//	  if (note) cout << "  note: " << note << endl;
//	  if (octave) cout << "  octave: " << octave << endl;

	if (step == OCTAVE)
	{
		int key = 12 * (atol(octave) + 2);
		const static char ladder[] = { 'C', '#', 'D', '#', 'E', 'F', '#', 'G', '#', 'A', '#', 'B' };
		for (int i = 0; i < 12; i++)
		{
			if (note[0] == ladder[i])
			{
				key += i;
				break;
			}
		}
		if (note[1] == '#')
		{
			key++;
		}

//		  cout << "    key " << key << endl;
		return key;
	}
	else
	{
		return -1;
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::zoneSpreadBaseKeys(bool onlyOnWhiteKeys)
{
	const static bool black[] = { false, true, false, true, false, false, true, false, true, false, true, false };
	int key = 48;
	for (int i = 0; i < getNumOfZones(); i++)
	{
		Zone *kg = getZone(i);
		kg->baseKey = key;
		key++;
		if (onlyOnWhiteKeys && black[key % 12]) //jump over black keys, if requested
		{
			key++;
		}
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::zoneSetKeyRangesFromBaseKeys()
{
	int i = 0;
	while (i < getNumOfZones())
	{
		Zone *kg = getZone(i);

		//how many sequent zones have the same base key?
		int sameBaseKeys = 1;
		for (int j = i + 1; j < getNumOfZones(); j++)
		{
			if (getZone(j)->baseKey == kg->baseKey)
			{
				sameBaseKeys++;
			}
			else break;
		}

		//low key is last high key +1, or 0 for first zone
		int lowKey = 0;
		if (i > 0)
		{
			Zone *prevKg = getZone(i - 1);
			lowKey = prevKg->highKey + 1;
		}

		//high key is in the middle between current zone's base key and the base key of the next zone with a different base key
		int highKey = 127;
		if (i < getNumOfZones() - sameBaseKeys)
		{
			Zone *nextKg = getZone(i + sameBaseKeys);
			highKey = (kg->baseKey + nextKg->baseKey) / 2;
		}

		//set values for all sequent zones with the same base key
		for (int j = 0; j < sameBaseKeys; j++)
		{
			kg = getZone(i + j);
			kg->lowKey = lowKey;
			kg->highKey = highKey;
		}

		//go to next zone with a different base key
		i += sameBaseKeys;
	}
}

//----------------------------------------------------------------------------------

void CoreInstrument::zoneShiftKeys(int keys)
{
	for (int i = 0; i < getNumOfZones(); i++)
	{
		Zone *kg = getZone(i);
		kg->baseKey = max(0, min(127, kg->baseKey + keys));
		kg->lowKey = max(0, min(127, kg->lowKey + keys));
		kg->highKey = max(0, min(127, kg->highKey + keys));
	}
}

//----------------------------------------------------------------------------------

bool CoreInstrument::zoneCheckBaseKeySorting()
{
	for (int i = 0; i < getNumOfZones() - 1; i++)
	{
		if (getZone(i)->baseKey > getZone(i + 1)->baseKey)
		{
			return false;
		}
	}
	return true;
}

