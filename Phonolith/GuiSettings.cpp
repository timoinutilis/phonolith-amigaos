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

#include "GuiSettings.h"
#include "CoreSamplerUnit.h"
#include "Vector.h"
#include "SysMidi.h"
#include "Phonolith.h"
#include "CorePrefs.h"
#include "Texts.h"

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/window.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/integer.h>
#include <proto/button.h>
#include <proto/listbrowser.h>
#include <proto/getfile.h>
#include <proto/slider.h>
#include <proto/string.h>
#include <proto/checkbox.h>
#include <proto/clicktab.h>
#include <proto/bitmap.h>
#include <proto/requester.h>

#include <exec/exec.h>
#include <images/label.h>
#include <gadgets/layout.h>
#include <gadgets/integer.h>
#include <gadgets/button.h>
#include <gadgets/listbrowser.h>
#include <gadgets/getfile.h>
#include <gadgets/slider.h>
#include <gadgets/string.h>
#include <gadgets/checkbox.h>
#include <gadgets/clicktab.h>
#include <images/bitmap.h>
#include <classes/requester.h>
#include <libraries/gadtools.h>

#include <reaction/reaction_macros.h>

//==================================================================================
// Constructor/Destructor
//==================================================================================

GuiSettings::GuiSettings(FrameApp *fApp, CorePrefs *newPrefs) : FrameGui(fApp)
{
	IExec->NewList(&mPortList);
	mNewPrefs = newPrefs;

	//**** MIDI PAGE

	Object *pageMidi = (Object *) VLayoutObject,
		LAYOUT_AddChild, VLayoutObject,
			LAYOUT_BevelStyle, BVS_SBAR_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_Label, Texts::MIDI_INPUT_GAD,

			LAYOUT_AddChild, mGadgets[GAD_REFRESHPORTS] = (Gadget *) Button(Texts::REFRESH_PORT_LIST_GAD, GAD_REFRESHPORTS),
			CHILD_WeightedHeight, 0,
			CHILD_WeightedWidth, 0,

			LAYOUT_AddChild, mGadgets[GAD_PORTLIST] = (Gadget *) ListBrowserObject,
				GA_ID, GAD_PORTLIST,
	            GA_RelVerify, TRUE,
				LISTBROWSER_Labels, &mPortList,
				LISTBROWSER_ShowSelected, FALSE,
				LISTBROWSER_MinVisible, 5,
			End,

			LAYOUT_AddChild, mGadgets[GAD_MIDIPORT] = (Gadget *) StringObject,
				GA_ID, GAD_MIDIPORT,
				STRINGA_TextVal, mNewPrefs->mMidiClusterName.c_str(),
			End,
			CHILD_WeightedHeight, 0,
		End,

		LAYOUT_AddImage, LabelObject,
			LABEL_Text, Texts::CHANGES_AFTER_PLAYER_RESTART_GAD,
		End,

	End;

	char freqString[20];
	sprintf(freqString, "%ld Hz", mNewPrefs->mAudioFrequency);

	//**** AUDIO PAGE

	Object *pageAudio = (Object *) VLayoutObject,
		LAYOUT_AddChild, VLayoutObject,
			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::AUDIO_OUTPUT_GAD,

				LAYOUT_AddChild, ButtonObject,
					GA_ID, GAD_SELECTAUDIO,
					GA_RelVerify, TRUE,
					GA_Text, Texts::SELECT_AUDIO_OUTPUT_GAD,
				End,
				CHILD_WeightedWidth, 0,

				LAYOUT_AddChild, mGadgets[GAD_AUDIOMODE] = (Gadget *) StringObject,
					GA_ReadOnly, TRUE,
					STRINGA_TextVal, mNewPrefs->mAudioModeName.c_str(),
				End,
				Label(Texts::AUDIO_MODE_GAD),

				LAYOUT_AddChild, mGadgets[GAD_AUDIOFREQ] = (Gadget *) StringObject,
					GA_ReadOnly, TRUE,
					STRINGA_TextVal, freqString,
				End,
				Label(Texts::MIX_FREQUENCY_GAD),
			End,

			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::PLAYER_TITLE_GAD,
				LAYOUT_HorizAlignment, LALIGN_RIGHT,

				LAYOUT_AddChild, mGadgets[GAD_PLAYERBUFFER] = (Gadget *) IntegerObject,
					GA_ID, GAD_PLAYERBUFFER,
					GA_RelVerify, TRUE,
					INTEGER_Minimum, 10,
					INTEGER_Number, (long) mNewPrefs->mPlayerBufferMs,
					INTEGER_MinVisible, 5,
				End,
				Label(Texts::BUFFER_SIZE_GAD),
				CHILD_WeightedWidth, 0,

			End,
		End,
		CHILD_WeightedHeight, 0,

		LAYOUT_AddImage, LabelObject,
			LABEL_Text, Texts::CHANGES_AFTER_PLAYER_RESTART_GAD,
		End,

	End;

	//**** SAMPLER PAGE

	Object *pageSampler = (Object *) VLayoutObject,
		LAYOUT_AddChild, VLayoutObject,
			LAYOUT_HorizAlignment, LALIGN_RIGHT,
			LAYOUT_SpaceOuter, TRUE,

			LAYOUT_AddChild, mGadgets[GAD_MAXVOICES] = (Gadget *) IntegerObject,
				GA_ID, GAD_MAXVOICES,
				GA_RelVerify, TRUE,
				INTEGER_Minimum, 1,
				INTEGER_Maximum, 127,
				INTEGER_Number, (long) mNewPrefs->mSamplerMaxVoices,
				INTEGER_MinVisible, 5,
			End,
			Label(Texts::MAX_VOICES_GAD),
			CHILD_WeightedWidth, 0,
		End,
		CHILD_WeightedHeight, 0,

		LAYOUT_AddImage, LabelObject,
			LABEL_Text, Texts::CHANGES_AFTER_PLAYER_RESTART_GAD,
		End,

	End;

	//**** APPLICATION PAGE

	Object *pagePaths = (Object *) VLayoutObject,
		LAYOUT_AddChild, VLayoutObject,
			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::PATHS_TITLE_GAD,

				LAYOUT_AddChild, mGadgets[GAD_DIRPROJECTS] = (Gadget *) GetFileObject,
					GA_ID, GAD_DIRPROJECTS,
					GA_RelVerify, TRUE,
					GETFILE_DrawersOnly, TRUE,
					GETFILE_FullFileExpand, FALSE,
					GETFILE_Drawer, mNewPrefs->mDirProjects.c_str(),
				End,
				Label(Texts::PATH_PROJECTS_GAD),

				LAYOUT_AddChild, mGadgets[GAD_DIRINSTRUMENTS] = (Gadget *) GetFileObject,
					GA_ID, GAD_DIRINSTRUMENTS,
					GA_RelVerify, TRUE,
					GETFILE_DrawersOnly, TRUE,
					GETFILE_FullFileExpand, FALSE,
					GETFILE_Drawer, mNewPrefs->mDirInstruments.c_str(),
				End,
				Label(Texts::PATH_INSTRUMENTS_GAD),

				LAYOUT_AddChild, mGadgets[GAD_DIRSAMPLES] = (Gadget *) GetFileObject,
					GA_ID, GAD_DIRSAMPLES,
					GA_RelVerify, TRUE,
					GETFILE_DrawersOnly, TRUE,
					GETFILE_FullFileExpand, FALSE,
					GETFILE_Drawer, mNewPrefs->mDirSamples.c_str(),
				End,
				Label(Texts::PATH_SAMPLES_GAD),

				LAYOUT_AddChild, mGadgets[GAD_DIRIMPORTS] = (Gadget *) GetFileObject,
					GA_ID, GAD_DIRIMPORTS,
					GA_RelVerify, TRUE,
					GETFILE_DrawersOnly, TRUE,
					GETFILE_FullFileExpand, FALSE,
					GETFILE_Drawer, mNewPrefs->mDirImports.c_str(),
				End,
				Label(Texts::PATH_IMPORTS_GAD),
			End,

			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::MISCELLANEOUS_GAD,

				LAYOUT_AddChild, mGadgets[GAD_ACTIVATEPLAYER] = (Gadget *) CheckBoxObject,
					GA_ID, GAD_ACTIVATEPLAYER,
					GA_RelVerify, TRUE,
					GA_Text, Texts::ACTIVATE_PLAYER_ON_START_GAD,
					GA_Selected, mNewPrefs->mActivatePlayer,
				End,
	/*
				LAYOUT_AddChild, mGadgets[GAD_SHOWDOCKICON] = (Gadget *) CheckBoxObject,
					GA_ID, GAD_SHOWDOCKICON,
					GA_RelVerify, TRUE,
					GA_Text, "Show AmiDock Icon",
					GA_Selected, mNewPrefs->mShowDockIcon,
				End,*/
			
			End,

		End,
		CHILD_WeightedHeight, 0,
	End;


	static char *tabs[] = {(char*)Texts::MIDI_GAD, (char*)Texts::AUDIO_GAD, (char*)Texts::SAMPLER_GAD, (char*)Texts::APPLICATION_GAD, NULL};

	Object *pageSettings = (Object *) VLayoutObject,
		LAYOUT_DeferLayout, TRUE,

		LAYOUT_AddChild, ClickTabObject,
			GA_Text, &tabs,
			CLICKTAB_PageGroup, PageObject,
				PAGE_Add, pageMidi,
				PAGE_Add, pageAudio,
				PAGE_Add, pageSampler,
				PAGE_Add, pagePaths,
				PAGE_Current, 0,
			End,
		End,

		LAYOUT_AddImage, LabelObject,
			LABEL_Text, Texts::SAVES_WINDOW_POSITIONS_GAD,
		End,

		LAYOUT_AddChild, HLayoutObject,
			LAYOUT_BevelStyle, BVS_SBAR_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_EvenSize, TRUE,

			LAYOUT_AddChild, ButtonObject,
				GA_ID, GAD_SAVE,
				GA_RelVerify, TRUE,
				GA_Text, Texts::SAVE_GAD,
			End,
			CHILD_WeightedWidth, 0,

			LAYOUT_AddChild, ButtonObject,
				GA_ID, GAD_USE,
				GA_RelVerify, TRUE,
				GA_Text, Texts::USE_GAD,
			End,
			CHILD_WeightedWidth, 0,

			LAYOUT_AddChild, ButtonObject,
				GA_ID, GAD_CANCEL,
				GA_RelVerify, TRUE,
				GA_Text, Texts::CANCEL_GAD,
			End,
			CHILD_WeightedWidth, 0,
		End,
		CHILD_WeightedHeight, 0,
	End;

	Phonolith::addDefaultMenu(this);

	createWindow("settings", Texts::SETTINGS, pageSettings, 0);
	refreshPortList();
}

//----------------------------------------------------------------------------------

GuiSettings::~GuiSettings()
{
	deleteWindow();
	IListBrowser->FreeListBrowserList(&mPortList);
}

//==================================================================================
// Gadgets
//==================================================================================

void GuiSettings::disableGadget(int gadgetID, bool disable)
{
	IIntuition->SetGadgetAttrs(mGadgets[gadgetID], mWindow, NULL,
			GA_Disabled, disable,
			TAG_DONE);
}

//----------------------------------------------------------------------------------

char *GuiSettings::getSelectedPortName()
{
	Node *node = NULL;
	IIntuition->GetAttr(LISTBROWSER_SelectedNode, mGadgets[GAD_PORTLIST], (unsigned long *) &node);
	char *portName = NULL;
	IListBrowser->GetListBrowserNodeAttrs(node, LBNCA_Text, &portName, TAG_DONE);
	return portName;
}

//----------------------------------------------------------------------------------

void GuiSettings::refreshPortList()
{
	#ifdef DEBUGGUI
	cout << "refreshPortList\n";
	#endif

	IIntuition->SetGadgetAttrs(mGadgets[GAD_PORTLIST], mWindow, NULL, LISTBROWSER_Labels, NULL, TAG_DONE);
	IListBrowser->FreeListBrowserList(&mPortList);

	//add DEFAULT_CLUSTER
	Node *node = IListBrowser->AllocListBrowserNode(1,
			LBNA_Column, 0,
			LBNCA_CopyText, TRUE,
			LBNCA_Text, CorePrefs::DEFAULT_CLUSTER,
			TAG_DONE);
	IExec->AddTail(&mPortList, node);

	//add all ports except of DEFAULT_CLUSTER
	Vector *ports = SysMidi::getPorts();
	for (int i = 0; i < ports->getSize(); i++)
	{
		Node *node = IListBrowser->AllocListBrowserNode(1,
				LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, (char *)ports->getElement(i),
				TAG_DONE);

		if (strcmp((char *)ports->getElement(i), CorePrefs::DEFAULT_CLUSTER) != 0)
		{
			IExec->AddTail(&mPortList, node);
		}
	}
	SysMidi::freePortsVector(ports);

	IIntuition->SetGadgetAttrs(mGadgets[GAD_PORTLIST], mWindow, NULL,
			LISTBROWSER_Labels, &mPortList,
			LISTBROWSER_Selected, 0,
			TAG_DONE);
}


//----------------------------------------------------------------------------------

char *GuiSettings::getGadgetString(int gadgetID)
{
	char *text = NULL;
	IIntuition->GetAttr(STRINGA_TextVal, mGadgets[gadgetID], (unsigned long *) &text);
	return text;

}

//----------------------------------------------------------------------------------

void GuiSettings::setGadgetString(int gadgetID, char *text)
{
	IIntuition->SetGadgetAttrs(mGadgets[gadgetID], mWindow, NULL, STRINGA_TextVal, text, TAG_DONE);
}

//----------------------------------------------------------------------------------

void GuiSettings::requestDir(int gadgetID)
{
	gfRequestDir((Object *) mGadgets[gadgetID], mWindow);
}

//----------------------------------------------------------------------------------

char *GuiSettings::getDir(int gadgetID)
{
	char *text = NULL;
	IIntuition->GetAttr(GETFILE_Drawer, mGadgets[gadgetID], (unsigned long *) &text);
	return text;
}

//----------------------------------------------------------------------------------

void GuiSettings::updateAudioGadgets()
{
	IIntuition->SetGadgetAttrs(mGadgets[GAD_AUDIOMODE], mWindow, NULL, STRINGA_TextVal, mNewPrefs->mAudioModeName.c_str(), TAG_DONE);
	char freqString[20];
	sprintf(freqString, "%ld Hz", mNewPrefs->mAudioFrequency);
	IIntuition->SetGadgetAttrs(mGadgets[GAD_AUDIOFREQ], mWindow, NULL, STRINGA_TextVal, freqString, TAG_DONE);
}

