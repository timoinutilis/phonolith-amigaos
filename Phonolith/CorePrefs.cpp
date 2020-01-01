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

using namespace std;

#include "CorePrefs.h"
#include "main.h"

#include <proto/application.h>
#include <libraries/application.h>

extern ApplicationIFace *IApplication;
extern PrefsObjectsIFace *IPrefsObjects;


//==================================================================================
// Constructor/Destructor
//==================================================================================

const char *CorePrefs::DEFAULT_CLUSTER = "phonolith.thru";

CorePrefs::CorePrefs()
{
	mMidiClusterName = DEFAULT_CLUSTER;
	mAudioModeID = 0;
	mAudioModeName = "default";
	mAudioFrequency = 44100;
	mPlayerBufferMs = 40;
	mSamplerMaxVoices = 16;
	mDirProjects = "";
	mDirInstruments = "";
	mDirSamples = "";
	mDirImports = "";
	mActivatePlayer = false;
//	  mShowDockIcon = true;
}

//----------------------------------------------------------------------------------

CorePrefs::~CorePrefs()
{
}

//==================================================================================
// Data Handling
//==================================================================================

void CorePrefs::getFrom(CorePrefs *other)
{
	mMidiClusterName = other->mMidiClusterName;
	mAudioModeID = other->mAudioModeID;
	mAudioModeName = other->mAudioModeName;
	mAudioFrequency = other->mAudioFrequency;
	mPlayerBufferMs = other->mPlayerBufferMs;
	mSamplerMaxVoices = other->mSamplerMaxVoices;
	mDirProjects = other->mDirProjects;
	mDirInstruments = other->mDirInstruments;
	mDirSamples = other->mDirSamples;
	mDirImports = other->mDirImports;
	mActivatePlayer = other->mActivatePlayer;
//	  mShowDockIcon = other->mShowDockIcon;
}

//----------------------------------------------------------------------------------

void CorePrefs::load(FrameApp *app)
{
	PrefsObject *dict = NULL;
	IApplication->GetApplicationAttrs(main_appID, APPATTR_MainPrefsDict, &dict, TAG_DONE);

	if (dict)
	{
		mMidiClusterName = IPrefsObjects->DictGetStringForKey(dict, "MidiClusterName", mMidiClusterName.c_str());

		mAudioModeID = IPrefsObjects->DictGetIntegerForKey(dict, "AudioModeID", mAudioModeID);
		mAudioModeName = IPrefsObjects->DictGetStringForKey(dict, "AudioModeName", mAudioModeName.c_str());
		mAudioFrequency = IPrefsObjects->DictGetIntegerForKey(dict, "AudioFrequency", mAudioFrequency);
		mPlayerBufferMs = IPrefsObjects->DictGetIntegerForKey(dict, "PlayerBufferMs", mPlayerBufferMs);

		mSamplerMaxVoices = IPrefsObjects->DictGetIntegerForKey(dict, "SamplerMaxVoices", mSamplerMaxVoices);

		mDirProjects = IPrefsObjects->DictGetStringForKey(dict, "DirProjects", mDirProjects.c_str());
		mDirInstruments = IPrefsObjects->DictGetStringForKey(dict, "DirInstruments", mDirInstruments.c_str());
		mDirSamples = IPrefsObjects->DictGetStringForKey(dict, "DirSamples", mDirSamples.c_str());
		mDirImports = IPrefsObjects->DictGetStringForKey(dict, "DirImports", mDirImports.c_str());
		mActivatePlayer = IPrefsObjects->DictGetBoolForKey(dict, "ActivatePlayer", mActivatePlayer);
//		  mShowDockIcon = IPrefsObjects->DictGetBoolForKey(dict, "ShowDockIcon", mShowDockIcon);

		app->getPrefsWindowBorders(dict);
	}
}

//----------------------------------------------------------------------------------

void CorePrefs::save(FrameApp *app)
{
	PrefsObject *dict = NULL;
	IApplication->GetApplicationAttrs(main_appID, APPATTR_MainPrefsDict, &dict, TAG_DONE);

	if (dict)
	{
		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsString(NULL, NULL,
				ALPOSTR_AllocSetString, mMidiClusterName.c_str(), TAG_DONE),
				"MidiClusterName");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsNumber(NULL, NULL,
				ALPONUM_AllocSetLong, mAudioModeID, TAG_DONE),
				"AudioModeID");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsString(NULL, NULL,
				ALPOSTR_AllocSetString, mAudioModeName.c_str(), TAG_DONE),
				"AudioModeName");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsNumber(NULL, NULL,
				ALPONUM_AllocSetLong, mAudioFrequency, TAG_DONE),
				"AudioFrequency");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsNumber(NULL, NULL,
				ALPONUM_AllocSetLong, mPlayerBufferMs, TAG_DONE),
				"PlayerBufferMs");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsNumber(NULL, NULL,
				ALPONUM_AllocSetLong, mSamplerMaxVoices, TAG_DONE),
				"SamplerMaxVoices");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsString(NULL, NULL,
				ALPOSTR_AllocSetString, mDirProjects.c_str(), TAG_DONE),
				"DirProjects");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsString(NULL, NULL,
				ALPOSTR_AllocSetString, mDirInstruments.c_str(), TAG_DONE),
				"DirInstruments");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsString(NULL, NULL,
				ALPOSTR_AllocSetString, mDirSamples.c_str(), TAG_DONE),
				"DirSamples");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsString(NULL, NULL,
				ALPOSTR_AllocSetString, mDirImports.c_str(), TAG_DONE),
				"DirImports");

		IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsNumber(NULL, NULL,
				ALPONUM_AllocSetBool, mActivatePlayer, TAG_DONE),
				"ActivatePlayer");

        
//		  IPrefsObjects->DictSetObjectForKey(dict, IPrefsObjects->PrefsNumber(NULL, NULL,
//				  ALPONUM_AllocSetBool, mShowDockIcon, TAG_DONE),
//				  "ShowDockIcon");


		app->addPrefsWindowBorders(dict);

		IPrefsObjects->WritePrefs(dict,
				WRITEPREFS_AppID, main_appID,
				WRITEPREFS_WriteENV, TRUE,
				WRITEPREFS_WriteENVARC, TRUE,
				TAG_DONE);
	}

}

