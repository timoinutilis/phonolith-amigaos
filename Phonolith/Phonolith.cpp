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
#include <cstring>
#include <new>

using namespace std;

#include "Phonolith.h"
#include "Version.h"

// #include "UtilKeyfile.h"
#include "FileRequest.h"
#include "CoreSampler.h"
#include "CoreInstrument.h"
#include "IOImporterEXS24.h"
#include "SysMidi.h"
#include "IOFileParse.h"
#include "WinMixer.h"
#include "WinInstrument.h"
#include "WinSettings.h"
#include "GuiInstrument.h"
#include "main.h"
#include "Texts.h"

#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/bitmap.h>

#include <images/bitmap.h>

//==================================================================================
// Constructor/Destructor
//==================================================================================

Phonolith::Phonolith() : FrameApp("Phonolith")
{
    mSampler = NULL;
	mReqProject = NULL;
	mSettingsGui = NULL;
	mMixerGui = NULL;
	mProject = NULL;
	mUserName = "";

	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		mInstrumentWins[i] = NULL;
	}

	mRegistered = true; //UtilKeyfile::check("PROGDIR:phonolith.key", 0x819AFE12, 0x83726EF5, &mUserName);

	mPrefs = new CorePrefs();
	mPrefs->load(this);
	mProject = new CoreProject();

	mReqProject = new FileRequest("#?.pproj", mPrefs->mDirProjects.c_str());

	FrameWindow *newWin = new WinMixer(this);
	addWindow(newWin);
	mMixerGui = (GuiMixer *) newWin->mFGui;

	if (!mRegistered)
	{
		message(REQ_INFORMATION, Texts::REQ_UNREGISTERED);
	}

	if (mPrefs->mActivatePlayer)
	{
		setBusyMouse(true);
		mMixerGui->disableGadget(GuiMixer::GAD_START, true);
		mMixerGui->disableGadget(GuiMixer::GAD_STOP, false);

		try
		{
			activatePlayer();

		}
		catch (exception &e)
		{
			showError(e);
			mMixerGui->disableGadget(GuiMixer::GAD_START, false);
			mMixerGui->disableGadget(GuiMixer::GAD_STOP, true);
		}

		setBusyMouse(false);
	}

	updateStatus();
}

//----------------------------------------------------------------------------------

Phonolith::~Phonolith()
{
	deactivatePlayer();
	if (mProject) delete mProject;
	if (mReqProject) delete mReqProject;
	if (mPrefs) delete mPrefs;
}

//==================================================================================
// Standard Menu
//==================================================================================

void Phonolith::addDefaultMenu(FrameGui *gui)
{
	NewMenu newMenu[] = {
		{NM_TITLE,	(char *) Texts::PHONOLITH_MENU, 0, 0, 0, 0},
		{NM_ITEM,	(char *) Texts::PHONOLITH_SETTINGS_MENU, "P", 0, 0, (void *)MENU_SETTINGS},
		{NM_ITEM,	(char *) Texts::PHONOLITH_ABOUT_MENU, "A", 0, 0, (void *)MENU_ABOUT},
		{NM_ITEM,	NM_BARLABEL, 0, 0, 0, 0},
		{NM_ITEM,	(char *) Texts::PHONOLITH_QUIT_MENU, "Q", 0, 0, (void *)MENU_QUIT},

		{NM_TITLE,	(char *) Texts::PROJECT_MENU, 0, 0, 0, 0},
		{NM_ITEM,	(char *) Texts::PROJECT_NEW_MENU, "N", 0, 0, (void *)MENU_PROJECT_NEW},
		{NM_ITEM,	(char *) Texts::PROJECT_LOAD_MENU, "O", 0, 0, (void *)MENU_PROJECT_LOAD},
		{NM_ITEM,	NM_BARLABEL, 0, 0, 0, 0},
		{NM_ITEM,	(char *) Texts::PROJECT_SAVE_MENU, "S", 0, 0, (void *)MENU_PROJECT_SAVE},
		{NM_ITEM,	(char *) Texts::PROJECT_SAVE_AS_MENU, 0, 0, 0, (void *)MENU_PROJECT_SAVEAS},

		{NM_END, 0, 0, 0, 0, 0}
	};

	gui->addMenu(newMenu);
}

//==================================================================================
// Implementations
//==================================================================================

void Phonolith::menuPicked(int menuID)
{
	#ifdef DEBUGAPP
	cout << "Phonolith menuPicked(" << menuID << ")\n";
	#endif

	switch (menuID)
	{
		//===PROGRAM===
		case MENU_ABOUT: {
			string info = Texts::REQ_ABOUT_1;
			info += APPVERSION;
			info += Texts::REQ_ABOUT_2;
			info += __DATE__;
			info += Texts::REQ_ABOUT_3;
			message(REQ_INFORMATION, info.c_str());
		} break;

		case MENU_QUIT: {
			quitRequested();
		} break;

		//===PROJECT===
		case MENU_PROJECT_NEW: {
			if ((mProject->mChanged && question(FrameApp::REQ_WARNING, Texts::REQ_NEW_PROJECT_NOT_SAVED, Texts::YES_NO))
					|| (!mProject->mChanged && question(FrameApp::REQ_QUESTION, Texts::REQ_NEW_PROJECT, Texts::YES_NO)))
			{
				mProject->reinit();
				for (int i = 0; i < CoreProject::CHANNELS; i++)
				{
					if (mInstrumentWins[i])
					{
						((GuiInstrument *) mInstrumentWins[i]->mFGui)->refreshInstrumentGadgets();
					}
				}
				mMixerGui->updateMixerGadgets();
			}
		} break;

		case MENU_SETTINGS: {
			showSettingsWindow();
		} break;

		case MENU_PROJECT_LOAD: {
			char *name = mReqProject->requestLoadFile(Texts::ASL_LOAD_PROJECT);
			if (name)
			{
				openProject(name);
			}
		} break;

		case MENU_PROJECT_SAVE: {
		    char *name = mReqProject->getFileString();
			if (!name)
			{
				name = mReqProject->requestSaveFile(Texts::ASL_SAVE_PROJECT, ".pproj");
			}
			if (name)
			{
				setBusyMouse(true);
				try
				{
					mProject->saveProject(name);
				}
				catch (exception &e)
				{
					showError(e);
				}
				setBusyMouse(false);
			}

		} break;

		case MENU_PROJECT_SAVEAS: {
			char *name = mReqProject->requestSaveFile(Texts::ASL_SAVE_PROJECT, ".pproj");
			if (name)
			{
				setBusyMouse(true);
				try
				{
					mProject->saveProject(name);
				}
				catch (exception &e)
				{
					showError(e);
				}
				setBusyMouse(false);
			}

		} break;
	}

}

//----------------------------------------------------------------------------------

extern bool main_quit;

void Phonolith::quitRequested()
{
	#ifdef DEBUGAPP
	cout << "Phonolith quitRequested()\n";
	#endif
	if ((mProject->mChanged && question(FrameApp::REQ_WARNING, Texts::REQ_QUIT_NOT_SAVED, Texts::YES_NO))
			|| (!mProject->mChanged && question(FrameApp::REQ_QUESTION, Texts::REQ_QUIT, Texts::YES_NO)))
	{
		main_quit = true;
	}

}

//----------------------------------------------------------------------------------

void Phonolith::openProject(const char *fileName)
{
	setBusyMouse(true);
	try
	{
		mProject->loadProject(fileName);
	}
	catch (exception &e)
	{
		showError(e);
	}

	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		if (mInstrumentWins[i])
		{
			((GuiInstrument *) mInstrumentWins[i]->mFGui)->refreshInstrumentGadgets();
		}
	}
	mMixerGui->updateMixerGadgets();

	setBusyMouse(false);
}

//==================================================================================
// Windows
//==================================================================================

void Phonolith::instrumentWindowClosed(int unitIndex)
{
	mInstrumentWins[unitIndex] = NULL;
}

//----------------------------------------------------------------------------------

void Phonolith::showInstrumentWindow(int unitIndex, bool forceNewWindow)
{
	if (mInstrumentWins[unitIndex])
	{
		//instrument window exists already
		mInstrumentWins[unitIndex]->mFGui->activate();
	}
	else
	{
		//find open windows
		int oldUnitIndex = -1;
		for (int i = 0; i < CoreProject::CHANNELS; i++)
		{
			if (mInstrumentWins[i])
			{
				oldUnitIndex = i;
				break;
			}
		}

		if (forceNewWindow || oldUnitIndex == -1)
		{
			//open new window
			FrameWindow *newWin = new WinInstrument(this, mProject->mInstruments[unitIndex], unitIndex);
			addWindow(newWin);
			mInstrumentWins[unitIndex] = (WinInstrument *) newWin;
		}
		else
		{
			//change instrument of open window
			mInstrumentWins[oldUnitIndex]->changeInstrument(mProject->mInstruments[unitIndex], unitIndex);
			mInstrumentWins[unitIndex] = mInstrumentWins[oldUnitIndex];
			mInstrumentWins[oldUnitIndex] = NULL;

            mInstrumentWins[unitIndex]->mFGui->activate();
		}
	}
}

//----------------------------------------------------------------------------------

void Phonolith::settingsWindowClosed()
{
	mSettingsGui = NULL;
}

//----------------------------------------------------------------------------------

void Phonolith::showSettingsWindow()
{
	if (mSettingsGui)
	{
		mSettingsGui->activate();
	}
	else
	{
		FrameWindow *newWin = new WinSettings(this);
		addWindow(newWin);
		mSettingsGui = (GuiSettings *) newWin->mFGui;
	}
}

//==================================================================================
// Mixer
//==================================================================================

void Phonolith::updateMixer()
{
	mMixerGui->updatePeakMeters();
}

//----------------------------------------------------------------------------------

void Phonolith::updateMixerLabels()
{
	mMixerGui->updateLabels();
}

//----------------------------------------------------------------------------------

void Phonolith::updateStatus()
{
	if (CoreSampler::mLastProblem.length() > 0)
	{
		// CoreSampler had a problem
		runtime_error e(CoreSampler::mLastProblem);
		CoreSampler::mLastProblem = "";
		showError(e);

		mMixerGui->disableGadget(GuiMixer::GAD_STOP, true);
		mMixerGui->disableGadget(GuiMixer::GAD_START, false);
		mMixerGui->selectGadget(GuiMixer::GAD_MIXDOWN, FALSE);

		deactivatePlayer();

	}
	else if (mSampler)
	{
		if (mProject->mRecordingRequested)
		{
			if (mProject->mRecordingActive)
			{
				string text = Texts::STATUS_RECORDING;
				text += mProject->mRecordFileName + "'";
				mMixerGui->setStatusText(text.c_str());
			}
			else
			{
				mMixerGui->setStatusText(Texts::STATUS_ACTIVE_WAITING);
			}
		}
		else
		{
			mMixerGui->setStatusText(Texts::STATUS_ACTIVE);
		}
	}
	else
	{
		if (mProject->mRecordingRequested)
		{
			mMixerGui->setStatusText(Texts::STATUS_NOT_ACTIVE_RECORDING);
		}
		else
		{
			mMixerGui->setStatusText(Texts::STATUS_NOT_ACTIVE);
		}
	}
}

//==================================================================================
// Player
//==================================================================================

bool Phonolith::activatePlayer()
{
	if (!mSampler)
	{
		mSampler = new CoreSampler(
				mPrefs->mAudioModeID,
				mPrefs->mAudioFrequency,
				2,
				mPrefs->mAudioFrequency * mPrefs->mPlayerBufferMs / 2000, // ms / 1000 / 2 <- two buffers are used, so half time
				mPrefs->mSamplerMaxVoices,
				mPrefs->mMidiClusterName.c_str(),
				mProject);

		updateStatus();
	}
}

//----------------------------------------------------------------------------------

void Phonolith::deactivatePlayer()
{
	if (mSampler)
	{
		mMixerGui->setStatusText(Texts::STATUS_SHUTTING_DOWN);
		delete mSampler;
		mSampler = NULL;
		mProject->mRecordingRequested = false;

		updateStatus();
	}
}

//==================================================================================
// Images
//==================================================================================

Object *Phonolith::mImagePlayerOn = NULL;
Object *Phonolith::mImagePlayerOff = NULL;
Object *Phonolith::mImagePlayerMixdown = NULL;
Object *Phonolith::mImageAdd = NULL;
Object *Phonolith::mImageRemove = NULL;
Object *Phonolith::mImageUp = NULL;
Object *Phonolith::mImageDown = NULL;
Object *Phonolith::mImagePlay = NULL;
Object *Phonolith::mImageBanner = NULL;
Object *Phonolith::mImageAdsr = NULL;

//----------------------------------------------------------------------------------

void Phonolith::loadImages()
{
	Screen *screen = IIntuition->LockPubScreen(NULL);

	mImagePlayerOn = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/player_on", BITMAP_Masking, TRUE, TAG_DONE);

	mImagePlayerOff = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/player_off", BITMAP_Masking, TRUE, TAG_DONE);

	mImagePlayerMixdown = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/player_mixdown", BITMAP_Masking, TRUE, TAG_DONE);

	mImageAdd = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/add", BITMAP_Masking, TRUE, TAG_DONE);

	mImageRemove = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/remove", BITMAP_Masking, TRUE, TAG_DONE);

	mImageUp = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/up", BITMAP_Masking, TRUE, TAG_DONE);

	mImageDown = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/down", BITMAP_Masking, TRUE, TAG_DONE);

	mImagePlay = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/play", BITMAP_Masking, TRUE, TAG_DONE);

	mImageBanner = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/banner", BITMAP_Masking, TRUE, TAG_DONE);

	mImageAdsr = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
			BITMAP_Screen, screen, BITMAP_SourceFile, "PROGDIR:Images/ADSR", BITMAP_Masking, TRUE, TAG_DONE);

	IIntuition->UnlockPubScreen(NULL, screen);

	if (!mImagePlayerOn || !mImagePlayerOff || !mImagePlayerMixdown || !mImageAdd || !mImageRemove || !mImageUp || !mImageDown ||
			!mImagePlay || !mImageBanner || !mImageAdsr)
	{
		throw runtime_error("Error while opening GUI images.");
	}

}   

//----------------------------------------------------------------------------------

void Phonolith::freeImages()
{
	IIntuition->DisposeObject(mImagePlayerOn);
	mImagePlayerOn = NULL;

	IIntuition->DisposeObject(mImagePlayerOff);
	mImagePlayerOff = NULL;

	IIntuition->DisposeObject(mImagePlayerMixdown);
	mImagePlayerMixdown = NULL;

	IIntuition->DisposeObject(mImageAdd);
	mImageAdd = NULL;

	IIntuition->DisposeObject(mImageRemove);
	mImageRemove = NULL;

	IIntuition->DisposeObject(mImageUp);
	mImageUp = NULL;

	IIntuition->DisposeObject(mImageDown);
	mImageDown = NULL;

	IIntuition->DisposeObject(mImagePlay);
	mImagePlay = NULL;

	IIntuition->DisposeObject(mImageBanner);
	mImageBanner = NULL;

	IIntuition->DisposeObject(mImageAdsr);
	mImageAdsr = NULL;
}

