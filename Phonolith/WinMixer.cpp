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
#include <new>
#include <cmath>

using namespace std;

#include "GuiMixer.h"
#include "Phonolith.h"
#include "WinMixer.h"
#include "CorePrefs.h"
#include "main.h"
#include "Texts.h"

//==================================================================================
// Constructor/Destructor
//==================================================================================

WinMixer::WinMixer(FrameApp *fApp) : FrameWindow(fApp)
{
	mReqExportFile = NULL;

	mFGui = new GuiMixer(fApp, this);
	((GuiMixer *) mFGui)->disableGadget(GuiMixer::GAD_STOP, true);

	mReqExportFile = new FileRequest("#?");
}

//----------------------------------------------------------------------------------

WinMixer::~WinMixer()
{
	if (mReqExportFile) delete mReqExportFile;
}

//==================================================================================
// Implementations
//==================================================================================

void WinMixer::menuPicked(int menuID)
{
//	  cout << "WinMixer: menuPicked(" << menuID << ")\n";
}

//----------------------------------------------------------------------------------

int WinMixer::closeRequested()
{
	return CLOSE_APP;
}

//----------------------------------------------------------------------------------

void WinMixer::gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp)
{
//	  cout << "WinMixer::gadgetPressed(" << gadgetID << ") code: " << code << endl;

	GuiMixer *gui = (GuiMixer *) mFGui;
	Phonolith *app = (Phonolith *) mFApp;

	if (gadgetID >= GuiMixer::CHANNELGAD_ID_OFFSET)
	{
		int unitIndex = (gadgetID - GuiMixer::CHANNELGAD_ID_OFFSET) / GuiMixer::CHANNELGAD_NUM;

		switch ((gadgetID - GuiMixer::CHANNELGAD_ID_OFFSET) % GuiMixer::CHANNELGAD_NUM)
		{
			case GuiMixer::CHANNELGAD_VOLUME: {
				app->mProject->setChannelVolume(unitIndex, code);
			} break;

			case GuiMixer::CHANNELGAD_PANORAMA: {
				app->mProject->setChannelPanorama(unitIndex, (signed short) code);
			} break;

			case GuiMixer::CHANNELGAD_EDIT: {
				app->showInstrumentWindow(unitIndex, (qualifier & QUALIFIER_SHIFT));
			} break;
		}
	}
	else
	{
		switch (gadgetID)
		{
			//===MASTER===
			case GuiMixer::GAD_MASTERVOLUME: {
				app->mProject->setMasterVolume(code);
			} break;

			//===SAMPLER PROCESSES===
			case GuiMixer::GAD_START: {
				gui->disableGadget(GuiMixer::GAD_START, true);
				gui->disableGadget(GuiMixer::GAD_STOP, false);
				mFApp->setBusyMouse(true);
				try
				{
					app->activatePlayer();
				}
				catch (exception &e)
				{
					gui->disableGadget(GuiMixer::GAD_START, false);
					gui->disableGadget(GuiMixer::GAD_STOP, true);
					showError(e);
				}
				mFApp->setBusyMouse(false);
			} break;

			case GuiMixer::GAD_STOP: {
				mFApp->setBusyMouse(true);

				gui->disableGadget(GuiMixer::GAD_STOP, true);
				gui->disableGadget(GuiMixer::GAD_START, false);
				gui->selectGadget(GuiMixer::GAD_MIXDOWN, FALSE);

				app->deactivatePlayer();

				mFApp->setBusyMouse(false);
			} break;

			case GuiMixer::GAD_MIXDOWN: {
				if (code && app->mRegistered)
				{
					char *name = mReqExportFile->requestSaveFile(Texts::ASL_MIXDOWN, "");
					if (name)
					{
						if (app->mSampler)
						{
							app->mSampler->resetMidiReceived();
						}
						app->mProject->mRecordFileName = name;
						app->mProject->mRecordingRequested = true;
					}
					else
					{
						app->mProject->mRecordingRequested = false;
						gui->selectGadget(GuiMixer::GAD_MIXDOWN, FALSE);
					}
				}
				else
				{
					app->mProject->mRecordingRequested = false;
				}
				app->updateStatus();
			} break;
		}
	}

}


