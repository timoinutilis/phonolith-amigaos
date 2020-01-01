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

using namespace std;

#include "WinSettings.h"
#include "Phonolith.h"
#include "CorePrefs.h"
#include "SysAudio.h"
#include "main.h"

//==================================================================================
// Constructor/Destructor
//==================================================================================

WinSettings::WinSettings(FrameApp *fApp) : FrameWindow(fApp)
{
	CorePrefs *prefs = ((Phonolith *) mFApp)->mPrefs;
	mNewPrefs.getFrom(prefs);

	mFGui = new GuiSettings(fApp, &mNewPrefs);
}

//----------------------------------------------------------------------------------

WinSettings::~WinSettings()
{
	((Phonolith *) mFApp)->settingsWindowClosed();
}

//==================================================================================
// Implementations
//==================================================================================

int WinSettings::closeRequested()
{
	return CLOSE_WINDOW;
}

//----------------------------------------------------------------------------------

void WinSettings::menuPicked(int menuID)
{
	#ifdef DEBUGWIN
	cout << "WinSettings: menuPicked(" << menuID << ")\n";
	#endif
/*
*/
}

//----------------------------------------------------------------------------------

void WinSettings::gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp)
{
	#ifdef DEBUGWIN
	cout << "WinSettings::gadgetPressed(" << gadgetID << ")\n";
	#endif

	GuiSettings *gui = (GuiSettings *) mFGui;

	switch (gadgetID)
	{
		//===MIDI Settings===
		case GuiSettings::GAD_REFRESHPORTS: {
			gui->refreshPortList();
		} break;

		case GuiSettings::GAD_PORTLIST: {
			char *port = gui->getSelectedPortName();
			gui->setGadgetString(GuiSettings::GAD_MIDIPORT, port);
			mNewPrefs.mMidiClusterName = port;
		} break;

		//===AUDIO Settings===
		case GuiSettings::GAD_SELECTAUDIO: {
			if (SysAudio::requestMode(&mNewPrefs.mAudioModeID, &mNewPrefs.mAudioFrequency, &mNewPrefs.mAudioModeName))
			{
				gui->updateAudioGadgets();
			}
		} break;

		case GuiSettings::GAD_PLAYERBUFFER: {
			mNewPrefs.mPlayerBufferMs = (short) code;
		} break;

		//===SAMPLER Settings===
		case GuiSettings::GAD_MAXVOICES: {
			mNewPrefs.mSamplerMaxVoices = (short) code;
		} break;

		//===APPLICATION Settings===
		case GuiSettings::GAD_DIRPROJECTS: {
			gui->requestDir(GuiSettings::GAD_DIRPROJECTS);
		} break;

		case GuiSettings::GAD_DIRINSTRUMENTS: {
			gui->requestDir(GuiSettings::GAD_DIRINSTRUMENTS);
		} break;

		case GuiSettings::GAD_DIRSAMPLES: {
			gui->requestDir(GuiSettings::GAD_DIRSAMPLES);
		} break;

		case GuiSettings::GAD_DIRIMPORTS: {
			gui->requestDir(GuiSettings::GAD_DIRIMPORTS);
		} break;

		case GuiSettings::GAD_ACTIVATEPLAYER: {
			mNewPrefs.mActivatePlayer = (bool) code;
		} break;

//		  case GuiSettings::GAD_SHOWDOCKICON: {
//			  mNewPrefs.mShowDockIcon = (bool) code;
//		  } break;

		//===General===
		case GuiSettings::GAD_SAVE: {
			CorePrefs *prefs = ((Phonolith *) mFApp)->mPrefs;
			readDirs(gui);
			prefs->getFrom(&mNewPrefs);
            mFApp->setBusyMouse(true);
			prefs->save(mFApp);
			mFApp->setBusyMouse(false);
			leaveWindow();
		} break;

		case GuiSettings::GAD_USE: {
			CorePrefs *prefs = ((Phonolith *) mFApp)->mPrefs;
			readDirs(gui);
			prefs->getFrom(&mNewPrefs);
			leaveWindow();
		} break;

		case GuiSettings::GAD_CANCEL: {
			leaveWindow();
		} break;

	}

}

void WinSettings::readDirs(GuiSettings *gui)
{
	mNewPrefs.mDirProjects = gui->getDir(GuiSettings::GAD_DIRPROJECTS);
	mNewPrefs.mDirInstruments = gui->getDir(GuiSettings::GAD_DIRINSTRUMENTS);
	mNewPrefs.mDirSamples = gui->getDir(GuiSettings::GAD_DIRSAMPLES);
	mNewPrefs.mDirImports = gui->getDir(GuiSettings::GAD_DIRIMPORTS);
}

