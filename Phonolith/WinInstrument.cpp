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

#include "WinInstrument.h"
#include "GuiInstrument.h"
#include "Phonolith.h"
#include "IOImporterEXS24.h"
#include "main.h"
#include "Texts.h"

#include <proto/dos.h>


//==================================================================================
// Constructor/Destructor
//==================================================================================

WinInstrument::WinInstrument(FrameApp *fApp, CoreInstrument *instrument, int unitIndex) : FrameWindow(fApp)
{
	Phonolith *app = (Phonolith *) mFApp;

	mFGui = new GuiInstrument(fApp, instrument, this);

	mReqSample = new FileRequest("#?.(aif|aiff|wav)", app->mPrefs->mDirSamples.c_str());
	mReqImport = new FileRequest("#?", app->mPrefs->mDirImports.c_str());
	mReqInstrument = new FileRequest("#?.pinst", app->mPrefs->mDirInstruments.c_str());

	mInstrument = instrument;
	mUnitIndex = unitIndex;

	mPlayKey = -1;

	updateTitle();
}

//----------------------------------------------------------------------------------

WinInstrument::~WinInstrument()
{
	delete mReqSample;
	delete mReqImport;
	delete mReqInstrument;
}

//==================================================================================
// Stuff
//==================================================================================

void WinInstrument::changeInstrument(CoreInstrument *instrument, int unitIndex)
{
	stopTestSounds();
	mInstrument = instrument;
	mUnitIndex = unitIndex;
	((GuiInstrument *) mFGui)->changeInstrument(instrument);
	updateTitle();
}

//----------------------------------------------------------------------------------

void WinInstrument::updateTitle()
{
	string title = Texts::INSTRUMENT;
	title += " ";

	char number[10];
	sprintf(number, "%d", mUnitIndex + 1);
	title += number;

	title += ": ";
	title += mInstrument->mName;
	mFGui->setTitle((char *) title.c_str());
}

//----------------------------------------------------------------------------------

void WinInstrument::lockUnit()
{
	CoreSampler *sampler = ((Phonolith *) mFApp)->mSampler;
	if (sampler)
	{
		sampler->lockUnit(mUnitIndex);
	}
}

//----------------------------------------------------------------------------------

void WinInstrument::unlockUnit()
{
	CoreSampler *sampler = ((Phonolith *) mFApp)->mSampler;
	if (sampler)
	{
		sampler->unlockUnit(mUnitIndex);
	}
}

//----------------------------------------------------------------------------------

void WinInstrument::playKey(char key)
{
	stopKey();
	CoreSamplerUnit *unit = ((Phonolith *) mFApp)->mSampler->getSamplerUnit(mUnitIndex);
	if (unit != NULL)
	{
		unit->pressedKey(key, 127, 0);
		mPlayKey = key;
	}
}

//----------------------------------------------------------------------------------

void WinInstrument::stopKey()
{
	if (mPlayKey != -1)
	{
		CoreSamplerUnit *unit = ((Phonolith *) mFApp)->mSampler->getSamplerUnit(mUnitIndex);
		if (unit != NULL)
		{
			unit->releasedKey((char) mPlayKey, 0);
			mPlayKey = -1;
		}
	}
}

//----------------------------------------------------------------------------------

void WinInstrument::changeKey(char key)
{
	if (mPlayKey != -1)
	{
		playKey(key);
	}
}

//----------------------------------------------------------------------------------

void WinInstrument::stopTestSounds()
{
	stopKey();
	CoreSamplerUnit *unit = ((Phonolith *) mFApp)->mSampler->getSamplerUnit(mUnitIndex);
	if (unit != NULL)
	{
		unit->stopTone();
	}
}

//==================================================================================
// Implementations
//==================================================================================

int WinInstrument::closeRequested()
{
	stopTestSounds();
	((Phonolith *) mFApp)->instrumentWindowClosed(mUnitIndex);
	return CLOSE_WINDOW;
}

//----------------------------------------------------------------------------------

void WinInstrument::menuPicked(int menuID)
{
//	  cout << "WinInstrument: menuPicked(" << menuID << ")\n";

	GuiInstrument *gui = (GuiInstrument *) mFGui;
	Phonolith *app = (Phonolith *) mFApp;

	switch (menuID)
	{
		//===INSTRUMENT===
		case GuiInstrument::MENU_INSTRUMENT_NEW: {
			lockUnit();
			mInstrument->reinit();
			gui->refreshInstrumentGadgets();
			updateTitle();
			app->updateMixerLabels();
			app->mProject->mChanged = true;
			unlockUnit();
		} break;

		case GuiInstrument::MENU_INSTRUMENT_LOAD: {
			char *name = mReqInstrument->requestLoadFile(Texts::ASL_LOAD_SAMPLER_INSTRUMENT);
			if (name)
			{
				lockUnit();
				app->setBusyMouse(true);
				try
				{
					mInstrument->loadInstrument(name);
				}
				catch (exception &e)
				{
					showError(e);
				}
				gui->refreshInstrumentGadgets();
				updateTitle();
				app->updateMixerLabels();
				app->setBusyMouse(false);
				app->mProject->mChanged = true;
				unlockUnit();
			}
		} break;

		case GuiInstrument::MENU_INSTRUMENT_SAVE: {
			char *name = mReqInstrument->getFileString();
			if (!name)
			{
				name = mReqInstrument->requestSaveFile(Texts::ASL_SAVE_SAMPLER_INSTRUMENT, ".pinst");
			}
			if (name)
			{
				app->setBusyMouse(true);
				try
				{
					mInstrument->saveInstrument(name);
				}
				catch (exception &e)
				{
					showError(e);
				}
				app->setBusyMouse(false);
			}
		} break;

		case GuiInstrument::MENU_INSTRUMENT_SAVEAS: {
			char *name = mReqInstrument->requestSaveFile(Texts::ASL_SAVE_SAMPLER_INSTRUMENT, ".pinst");
			if (name)
			{
				app->setBusyMouse(true);
				try
				{
					mInstrument->saveInstrument(name);
				}
				catch (exception &e)
				{
					showError(e);
				}
				app->setBusyMouse(false);
			}
		} break;

		case GuiInstrument::MENU_INSTRUMENT_IMPORT: {
			char *name = mReqImport->requestLoadFile(Texts::ASL_CHOOSE_EXS24_INSTRUMENT);
			if (name)
			{
				lockUnit();

				app->setBusyMouse(true);
				try
				{
					IOImporterEXS24::import(name, mInstrument);
				}
				catch (exception &e)
				{
					showError(e);
				}
				gui->refreshInstrumentGadgets();
				updateTitle();
				app->updateMixerLabels();
				app->setBusyMouse(false);
				app->mProject->mChanged = true;

				unlockUnit();
			}
		} break;
	}
}

//----------------------------------------------------------------------------------

void WinInstrument::gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp)
{
//	  cout << "WinInstrument::gadgetPressed(" << gadgetID << ") code: " << code << " gadgetUp: " << gadgetUp << endl;

	GuiInstrument *gui = (GuiInstrument *) mFGui;
	Phonolith *app = (Phonolith *) mFApp;

	switch (gadgetID)
	{
		//===INSTRUMENT===
		case GuiInstrument::GAD_NAME: {
			const char *name = gui->getGadgetString(GuiInstrument::GAD_NAME);
			mInstrument->mName = name;
			updateTitle();
			app->updateMixerLabels();
			app->mProject->mChanged = true;
		} break;

		case GuiInstrument::GAD_NUMVOICES: {
			mInstrument->mNumVoices = code;
			if (app->mSampler)
			{
				app->mSampler->setUnitNumVoices(mUnitIndex, code);
			}
			app->mProject->mChanged = true;
		} break;

		case GuiInstrument::GAD_ATTACK: {
			mInstrument->mAmpEnvelope.mAttackTime = (char)code;
			app->mProject->mChanged = true;
		} break;

		case GuiInstrument::GAD_DECAY: {
			mInstrument->mAmpEnvelope.mDecayTime = (char)code;
			app->mProject->mChanged = true;
		} break;

		case GuiInstrument::GAD_SUSTAIN: {
			mInstrument->mAmpEnvelope.mSustainLevel = (char)code;
			app->mProject->mChanged = true;
		} break;

		case GuiInstrument::GAD_RELEASE: {
			mInstrument->mAmpEnvelope.mReleaseTime = (char)code;
			app->mProject->mChanged = true;
		} break;


		//===ZONES===
		case GuiInstrument::GAD_ZONELIST: {
			stopTestSounds();
			gui->refreshZoneGadgets();
		} break;

		case GuiInstrument::GAD_ADD: {
			Vector *fileList = mReqSample->requestLoadMultiFiles(Texts::ASL_CHOOSE_SAMPLES);
			if (fileList)
			{
				lockUnit();
				mFApp->setBusyMouse(true);
				for (int i = 0; i < fileList->getSize(); i++)
				{
					char *fileName = (char *)fileList->getElement(i);
					try
					{
						mInstrument->addZone(IDOS->FilePart(fileName), fileName, 0, 127, 60, 0, 127, 0, 0, 0);
						if (mInstrument->mName == "")
						{
							mInstrument->mName = IDOS->FilePart(fileName);
							gui->setGadgetString(GuiInstrument::GAD_NAME, mInstrument->mName.c_str());
							updateTitle();
							app->updateMixerLabels();
						}
						app->mProject->mChanged = true;
					}
					catch (exception &e)
					{
						showError(e);
					}
				}
				gui->refreshZoneList();
				gui->refreshZoneGadgets();
				mFApp->setBusyMouse(false);
				unlockUnit();
			}
		} break;

		case GuiInstrument::GAD_REMOVE: {
            lockUnit();
			int groupIndex = gui->getZoneSelection();
			if (groupIndex != -1)
			{
				mInstrument->deleteZone(groupIndex);
				gui->refreshZoneList();
				app->mProject->mChanged = true;
				gui->refreshZoneGadgets();
			}
			unlockUnit();
		} break;

		case GuiInstrument::GAD_UP: {
            lockUnit();
			int groupIndex = gui->getZoneSelection();
			if (groupIndex != -1)
			{
				if (mInstrument->moveZoneUp(groupIndex))
				{
					gui->refreshZoneList();
					gui->setZoneSelection(groupIndex - 1);
					app->mProject->mChanged = true;
				}
			}
			unlockUnit();
		} break;

		case GuiInstrument::GAD_DOWN: {
            lockUnit();
			int groupIndex = gui->getZoneSelection();
			if (groupIndex != -1)
			{
				if (mInstrument->moveZoneDown(groupIndex))
				{
					gui->refreshZoneList();
					gui->setZoneSelection(groupIndex + 1);
					app->mProject->mChanged = true;
				}
			}
			unlockUnit();
		} break;

		case GuiInstrument::GAD_TOOLS: {
			lockUnit();
			switch (code)
			{
				case 0: {
					mInstrument->zoneSortByBaseKey();
				} break;
				case 1: {
					mInstrument->zoneReadBaseKeysFromNames();
				} break;
				case 2: {
					int result = mFApp->question(FrameApp::REQ_QUESTION, Texts::REQ_WHICH_KEYS_FOR_BASE_KEYS, Texts::REQ_WHITE_ONLY_WHITE_AND_BLACK);
					mInstrument->zoneSpreadBaseKeys(result == 1);
				} break;
				case 3: {
					bool sorted = mInstrument->zoneCheckBaseKeySorting();
					if (!sorted && mFApp->question(FrameApp::REQ_QUESTION, Texts::REQ_ZONES_MUST_BE_SORTED, Texts::REQ_SORT_BEFORE_CANCEL) == 1)
					{
                        mInstrument->zoneSortByBaseKey();
						sorted = true;
					}
					if (sorted)
					{
						mInstrument->zoneSetKeyRangesFromBaseKeys();
					}
				} break;
				case 4: {
					int result = mFApp->question(FrameApp::REQ_QUESTION, Texts::REQ_HOW_MANY_OCTAVES, "-3|-2|-1|1|2|3|0");
					if (result != 0)
					{
						int oct = (result <= 3 ? result - 4 : result - 3);
						mInstrument->zoneShiftKeys(oct * 12);
					}
				} break;
			}
			gui->refreshZoneList();
			gui->refreshZoneGadgets();
			app->mProject->mChanged = true;
			unlockUnit();
		} break;

	}

	//===ZONE SETTINGS==
	int groupIndex = gui->getZoneSelection();
	if (groupIndex != -1)
	{
		Zone *kg = mInstrument->getZone(groupIndex);

		switch (gadgetID)
		{

			case GuiInstrument::GAD_FILE: {
				const char *fileName = gui->requestFile(GuiInstrument::GAD_FILE);
				if (fileName)
				{
					lockUnit();
					mFApp->setBusyMouse(true);
					try
					{
						mInstrument->changeZoneSound(groupIndex, IDOS->FilePart(fileName), fileName);
                        app->mProject->mChanged = true;
					}
					catch (exception &e)
					{
						showError(e);
					}
					gui->refreshZoneListEntry(groupIndex);
					mFApp->setBusyMouse(false);
					unlockUnit();
				}
			} break;

			case GuiInstrument::GAD_PLAYSAMPLE: {
				if (code)
				{
					playKey(kg->baseKey);
				}
				else
				{
					stopKey();
				}
			} break;

			case GuiInstrument::GAD_POSSTART: {
				kg->startPosition = gui->getIntegerNumber(GuiInstrument::GAD_POSSTART);
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_POSEND: {
				kg->endPosition = gui->getIntegerNumber(GuiInstrument::GAD_POSEND);
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_BASEKEY: {
				kg->baseKey = code;
				CoreSamplerUnit *unit = app->mSampler->getSamplerUnit(mUnitIndex);
				if (unit != NULL)
				{
					unit->changeTone(kg->baseKey);
				}
				if (gadgetUp)
				{
					gui->refreshZoneListEntry(groupIndex);
				}
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_PLAYTONE: {
				CoreSamplerUnit *unit = app->mSampler->getSamplerUnit(mUnitIndex);
				if (unit != NULL)
				{
					if (code)
					{
						unit->startTone(kg->baseKey);
					}
					else
					{
						unit->stopTone();
					}
				}
			} break;

			case GuiInstrument::GAD_KEYLOW: {
				kg->lowKey = code;
				if (gadgetUp)
				{
					if (kg->lowKey > kg->highKey)
					{
						kg->highKey = kg->lowKey;
						gui->refreshZoneRangeGadgets();
					}
					gui->refreshZoneListEntry(groupIndex);
				}
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_KEYHIGH: {
				kg->highKey = code;
				if (gadgetUp)
				{
					if (kg->highKey < kg->lowKey)
					{
						kg->lowKey = kg->highKey;
						gui->refreshZoneRangeGadgets();
					}
					gui->refreshZoneListEntry(groupIndex);
				}
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_VELOLOW: {
				kg->lowVelocity = code;
				if (gadgetUp)
				{
					if (kg->lowVelocity > kg->highVelocity)
					{
						kg->highVelocity = kg->lowVelocity;
						gui->refreshZoneRangeGadgets();
					}
					gui->refreshZoneListEntry(groupIndex);
				}
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_VELOHIGH: {
				kg->highVelocity = code;
				if (gadgetUp)
				{
					if (kg->highVelocity < kg->lowVelocity)
					{
						kg->lowVelocity = kg->highVelocity;
						gui->refreshZoneRangeGadgets();
					}
					gui->refreshZoneListEntry(groupIndex);
				}
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_MAINTUNE: {
				kg->mainTune = code;
                changeKey(kg->baseKey);
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_FINETUNE: {
				kg->fineTune = code;
                changeKey(kg->baseKey);
                app->mProject->mChanged = true;
			} break;

			case GuiInstrument::GAD_PANNING: {
				kg->panning = code;
                changeKey(kg->baseKey);
                app->mProject->mChanged = true;
			} break;
		}
	}

}

