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

#include "CoreProject.h"
#include "CoreCalc.h"
#include "IOFileParse.h"
#include "main.h"

#include "IOFileTagsProject.h"

#include <proto/dos.h>

//==================================================================================
// Constructor/Destructor
//==================================================================================

CoreProject::CoreProject()
{
	mChanged = false;

	for (int i = 0; i < CHANNELS; i++)
	{
		mInstruments[i] = NULL;
	}

	for (int i = 0; i < CHANNELS; i++)
	{
		mInstruments[i] = new CoreInstrument();
	}

	setMasterVolume(100);
	for (int i = 0; i < CHANNELS; i++)
	{
		setChannelVolume(i, 100);
		setChannelPanorama(i, 0);
		mChannelVoices[i] = 16;
	}

	mRecordingRequested = false;
	mRecordingActive = false;
	mRecordFileName = "";
}

//----------------------------------------------------------------------------------

CoreProject::~CoreProject()
{
	for (int i = 0; i < CHANNELS; i++)
	{
		if (mInstruments[i]) delete mInstruments[i];
	}
}

//==================================================================================
// Project
//==================================================================================

void CoreProject::reinit()
{
	for (int i = 0; i < CHANNELS; i++)
	{
		mInstruments[i]->reinit();
	}

	setMasterVolume(100);
	for (int i = 0; i < CHANNELS; i++)
	{
		setChannelVolume(i, 100);
		setChannelPanorama(i, 0);
		mChannelVoices[i] = 16;
	}

	mChanged = false;
}

//----------------------------------------------------------------------------------

void CoreProject::saveProject(const char *name)
{
	IOFileParse projFile(name, IOFileParse::SAVE, PROJECTID, VERSION);

	//project file - master
	projFile.addTagShort(TAG_MASTER_VOLUME, mMasterVolumeInt);
	projFile.writeArea(AREA_MASTER);

	char projPath[strlen(name)];
	strcpy(projPath, name);
	char *endChar = IDOS->PathPart(projPath);
	*endChar = 0;

	char projName[strlen(name)];
	strcpy(projName, IDOS->FilePart(name));
	endChar = strrchr(projName, '.');
	*endChar = 0;

	char nameBuffer[10 + strlen(projName)];
	int length = strlen(name) + 11;

	//Channels / Instruments
	for (int i = 0; i < CHANNELS; i++)
	{
		if (mInstruments[i]->getNumOfZones() > 0)
		{
			//project file
			projFile.addTagShort(TAG_CHANNEL_INDEX, i);
			projFile.addTagShort(TAG_CHANNEL_VOLUME, mChannelVolumeInt[i]);
			projFile.addTagShort(TAG_CHANNEL_PANORAMA, mChannelPanoramaInt[i]);
			projFile.writeArea(AREA_CHANNEL);

			//instrument file
			char instName[length + 1];
			strcpy(instName, projPath);
			sprintf(nameBuffer, "%s%d.pinst", projName, i + 1);
			IDOS->AddPart(instName, nameBuffer, length);
			mInstruments[i]->saveInstrument(instName);
		}
	}

	mChanged = false;
}

//----------------------------------------------------------------------------------

void CoreProject::loadProject(const char *name)
{
	IOFileParse projFile(name, IOFileParse::LOAD, PROJECTID, VERSION);

	char projPath[strlen(name)];
	strcpy(projPath, name);
	char *endChar = IDOS->PathPart(projPath);
	*endChar = 0;

	char projName[strlen(name)];
	strcpy(projName, IDOS->FilePart(name));
	endChar = strrchr(projName, '.');
	*endChar = 0;

	char nameBuffer[10 + strlen(projName)];
	int nameLength = strlen(name) + 11;

	unsigned long areaID;
	unsigned long tagID;
	long value;
	unsigned long length;

	for (int i = 0; i < CHANNELS; i++)
	{
		mInstruments[i]->reinit();
	}

	while ((areaID = projFile.readNextArea()) != IOFileParse::END_OF_FILE)
	{
		switch (areaID)
		{
			case AREA_MASTER: {
				while ((tagID = projFile.readNextTag(&value, &length)) != IOFileParse::END_OF_AREA)
				{
					switch (tagID)
					{
						case TAG_MASTER_VOLUME: setMasterVolume((int)value); break;
					}
				}
			} break;

			case AREA_CHANNEL: {
				int index = -1;
				while ((tagID = projFile.readNextTag(&value, &length)) != IOFileParse::END_OF_AREA)
				{
					switch (tagID)
					{
						case TAG_CHANNEL_INDEX: index = (int)value; break;
						case TAG_CHANNEL_VOLUME: setChannelVolume(index, (int)value); break;
						case TAG_CHANNEL_PANORAMA: setChannelPanorama(index, (int)value); break;
					}
				}
				char instName[nameLength + 1];
				strcpy(instName, projPath);
				sprintf(nameBuffer, "%s%d.pinst", projName, index + 1);
				IDOS->AddPart(instName, nameBuffer, length);
				mInstruments[index]->loadInstrument(instName);
			} break;
		}
	}

	mChanged = false;
}

//==================================================================================
// Settings
//==================================================================================

void CoreProject::setMasterVolume(int volume)
{
	mMasterVolumeInt = volume;
	mMasterVolume = CoreCalc::volumeFactor(volume);
}

//----------------------------------------------------------------------------------

void CoreProject::setChannelVolume(int channelIndex, int volume)
{
	mChannelVolumeInt[channelIndex] = volume;
	mChannelVolume[channelIndex] = CoreCalc::volumeFactor(volume);
}

//----------------------------------------------------------------------------------

void CoreProject::setChannelPanorama(int channelIndex, int panning)
{
	mChannelPanoramaInt[channelIndex] = panning;
	mChannelPanoramaAmpL[channelIndex] = CoreCalc::panoramaAmpLeft(panning);
	mChannelPanoramaAmpR[channelIndex] = CoreCalc::panoramaAmpRight(panning);
}

