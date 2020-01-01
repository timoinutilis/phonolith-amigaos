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

#include "GuiInstrument.h"
#include "Phonolith.h"
#include "CoreInstrument.h"
#include "Vector.h"
#include "SysMidi.h"
#include "Texts.h"

#include "GuiNoteStrings.h"

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
#include <proto/clicktab.h>
#include <proto/chooser.h>
#include <proto/bitmap.h>
#include <proto/requester.h>

#include <intuition/icclass.h>
#include <exec/exec.h>
#include <images/label.h>
#include <gadgets/layout.h>
#include <gadgets/integer.h>
#include <gadgets/button.h>
#include <gadgets/listbrowser.h>
#include <gadgets/getfile.h>
#include <gadgets/slider.h>
#include <gadgets/string.h>
#include <gadgets/clicktab.h>
#include <gadgets/chooser.h>
#include <images/bitmap.h>
#include <classes/requester.h>
#include <libraries/gadtools.h>

#include <reaction/reaction_macros.h>

int GuiInstrument::mWindowCount = 0;

const static TagItem mapSlider[] = {SLIDER_Level, ICSPECIAL_CODE, TAG_DONE};

//==================================================================================
// Constructor/Destructor
//==================================================================================

GuiInstrument::GuiInstrument(FrameApp *fApp, CoreInstrument *instrument, FrameWindow *fWin) : FrameGui(fApp)
{
	mInstrument = NULL;
	mSelectedZone = -1;

	static ColumnInfo columnInfo[] = {
		{45, (char *) Texts::SAMPLE_NAME_GAD, 0},
		{15, (char *) Texts::BASE_KEY_GAD, 0},
		{20, (char *) Texts::KEY_RANGE_GAD, 0},
		{20, (char *) Texts::VELO_RANGE_GAD, 0},
		{-1, NULL, 0}
	};
	IExec->NewList(&mZoneList);

	STRPTR zoneTools[] = {
		(char *) Texts::SORT_BY_BASE_KEY_GAD,
		(char *) Texts::READ_BASE_KEYS_FROM_NAMES_GAD,
		(char *) Texts::SPREAD_BASE_KEYS_GAD,
		(char *) Texts::SET_KEY_RANGES_AROUND_BASE_KEYS_GAD,
		(char *) Texts::SHIFT_KEYS_GAD,
		NULL
	};

	mLevelHook.h_MinNode.mln_Succ = NULL;
	mLevelHook.h_MinNode.mln_Pred = NULL;
	mLevelHook.h_SubEntry = NULL;
	mLevelHook.h_Data = NULL;
	mLevelHook.h_Entry = (HOOKFUNC)LevelFunc;

	Object *pageInstrument = (Object *) VLayoutObject,
		LAYOUT_DeferLayout, TRUE,

		LAYOUT_AddChild, mGadgets[GAD_NAME] = (Gadget *) StringObject,
			GA_ID, GAD_NAME,
			GA_RelVerify, TRUE,
		End,
		Label(Texts::INSTRUMENT_NAME_GAD),

		LAYOUT_AddChild, HLayoutObject,
			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::SETTINGS_GAD,

				LAYOUT_AddChild, mGadgets[GAD_NUMVOICES] = (Gadget *) IntegerObject,
					GA_ID, GAD_NUMVOICES,
					GA_RelVerify, TRUE,
					INTEGER_Minimum, 1,
					INTEGER_MinVisible, 3,
				End,
				Label(Texts::VOICES_GAD),

				LAYOUT_AddImage, Phonolith::mImageAdsr,
				CHILD_NoDispose, TRUE,
			End,
			CHILD_WeightedWidth, 1,

			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::AMPLIFIER_ENVELOPE_GAD,

				LAYOUT_AddChild, mGadgets[GAD_ATTACK] = (Gadget *) SliderObject,
					GA_ID, GAD_ATTACK,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelFormat, "%ld",
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,
				Label(Texts::ATTACK_TIME_GAD),

				LAYOUT_AddChild, mGadgets[GAD_DECAY] = (Gadget *) SliderObject,
					GA_ID, GAD_DECAY,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelFormat, "%ld",
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,
				Label(Texts::DECAY_TIME_GAD),

				LAYOUT_AddChild, mGadgets[GAD_SUSTAIN] = (Gadget *) SliderObject,
					GA_ID, GAD_SUSTAIN,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelFormat, "%ld",
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,
				Label(Texts::SUSTAIN_LEVEL_GAD),

				LAYOUT_AddChild, mGadgets[GAD_RELEASE] = (Gadget *) SliderObject,
					GA_ID, GAD_RELEASE,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelFormat, "%ld",
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,
				Label(Texts::RELEASE_TIME_GAD),
			End,
			CHILD_WeightedWidth, 3,
		End,
		CHILD_WeightedHeight, 0,

		LAYOUT_AddChild, VLayoutObject,
			LAYOUT_BevelStyle, BVS_SBAR_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_Label, Texts::ZONES_GAD,

			LAYOUT_AddChild, mGadgets[GAD_ZONELIST] = (Gadget *) ListBrowserObject,
				GA_ID, GAD_ZONELIST,
	            GA_RelVerify, TRUE,
				LISTBROWSER_ColumnInfo, &columnInfo,
				LISTBROWSER_ColumnTitles, TRUE,
				LISTBROWSER_Labels, &mZoneList,
				LISTBROWSER_ShowSelected, TRUE,
				LISTBROWSER_MinVisible, 5,
			End,

			LAYOUT_AddChild, HLayoutObject,
				LAYOUT_AddChild, ButtonObject,
					GA_ID, GAD_ADD,
					GA_RelVerify, TRUE,
					//GA_Text, "Add",
					BUTTON_RenderImage, Phonolith::mImageAdd,
				End,

				LAYOUT_AddChild, ButtonObject,
					GA_ID, GAD_REMOVE,
					GA_RelVerify, TRUE,
					//GA_Text, "Remove",
					BUTTON_RenderImage, Phonolith::mImageRemove,
				End,

				LAYOUT_AddChild, ButtonObject,
					GA_ID, GAD_UP,
					GA_RelVerify, TRUE,
					BUTTON_RenderImage, Phonolith::mImageUp,
				End,
				LAYOUT_AddChild, ButtonObject,
					GA_ID, GAD_DOWN,
					GA_RelVerify, TRUE,
					BUTTON_RenderImage, Phonolith::mImageDown,
				End,
				LAYOUT_AddChild, ChooserObject,
					GA_ID, GAD_TOOLS,
					GA_RelVerify, TRUE,
					CHOOSER_Title, Texts::TOOLS_GAD,
					CHOOSER_DropDown, TRUE,
					CHOOSER_LabelArray, zoneTools,
				End,
			End,
			CHILD_WeightedWidth, 0,
			CHILD_WeightedHeight, 0,
		End,

		LAYOUT_AddChild, mGadgets[GAD_ZONEGROUP] = (Gadget *) VLayoutObject,
			LAYOUT_BevelStyle, BVS_SBAR_VERT,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_Label, Texts::ZONE_SETTINGS_GAD,

			LAYOUT_AddChild, HLayoutObject,

				LAYOUT_AddChild, mGadgets[GAD_FILE] = (Gadget *) GetFileObject,
					GA_ID, GAD_FILE,
		            GA_RelVerify, TRUE,
					GETFILE_ReadOnly, TRUE,
					GETFILE_DoPatterns, TRUE,
					GETFILE_Pattern, "#?.(aif|aiff|wav)",
					GETFILE_TitleText, Texts::ASL_CHOOSE_SAMPLE_FILE,
				End,

				LAYOUT_AddChild, mGadgets[GAD_PLAYSAMPLE] = (Gadget *) ButtonObject,
					GA_ID, GAD_PLAYSAMPLE,
					GA_RelVerify, TRUE,
					//GA_Text, "Play",
					BUTTON_RenderImage, Phonolith::mImagePlay,
					BUTTON_PushButton, TRUE,
				End,
				CHILD_WeightedWidth, 0,
			End,
			Label(Texts::SAMPLE_FILE_GAD),

			LAYOUT_AddChild, HLayoutObject,

				LAYOUT_AddChild, mGadgets[GAD_POSSTART] = (Gadget *) IntegerObject,
					GA_ID, GAD_POSSTART,
					GA_RelVerify, TRUE,
					INTEGER_Minimum, 0,
					INTEGER_Maximum, 0,
				End,

				LAYOUT_AddChild, mGadgets[GAD_POSEND] = (Gadget *) IntegerObject,
					GA_ID, GAD_POSEND,
                    GA_RelVerify, TRUE,
					INTEGER_Minimum, 0,
					INTEGER_Maximum, 0,
				End,
			End,
			Label(Texts::START_END_GAD),

			LAYOUT_AddChild, HLayoutObject,

				LAYOUT_AddChild, mGadgets[GAD_BASEKEY] = (Gadget *) SliderObject,
					GA_ID, GAD_BASEKEY,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelMaxLen, 4,
					SLIDER_LevelDomain, "D#-2 ",
					SLIDER_LevelFormat, "%s",
					SLIDER_LevelHook, &mLevelHook,
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,

				LAYOUT_AddChild, mGadgets[GAD_PLAYTONE] = (Gadget *) ButtonObject,
					GA_ID, GAD_PLAYTONE,
					GA_RelVerify, TRUE,
					//GA_Text, "Play",
					BUTTON_RenderImage, Phonolith::mImagePlay,
					BUTTON_PushButton, TRUE,
				End,
				CHILD_WeightedWidth, 0,
			End,
			Label(Texts::BASE_KEY_GAD),

			LAYOUT_AddChild, HLayoutObject,

				LAYOUT_AddChild, mGadgets[GAD_KEYLOW] = (Gadget *) SliderObject,
					GA_ID, GAD_KEYLOW,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelMaxLen, 4,
					SLIDER_LevelDomain, "D#-2 ",
					SLIDER_LevelFormat, "%s",
					SLIDER_LevelHook, &mLevelHook,
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,

				LAYOUT_AddChild, mGadgets[GAD_KEYHIGH] = (Gadget *) SliderObject,
					GA_ID, GAD_KEYHIGH,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelMaxLen, 4,
					SLIDER_LevelDomain, "D#-2 ",
					SLIDER_LevelFormat, "%s",
					SLIDER_LevelHook, &mLevelHook,
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,
			End,
			Label(Texts::KEY_RANGE_GAD),

			LAYOUT_AddChild, HLayoutObject,
				LAYOUT_AddChild, mGadgets[GAD_VELOLOW] = (Gadget *) SliderObject,
					GA_ID, GAD_VELOLOW,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelFormat, "%ld",
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,

				LAYOUT_AddChild, mGadgets[GAD_VELOHIGH] = (Gadget *) SliderObject,
					GA_ID, GAD_VELOHIGH,
					GA_RelVerify, TRUE,
					SLIDER_Orientation, SLIDER_HORIZONTAL,
					SLIDER_LevelFormat, "%ld",
					SLIDER_LevelPlace, PLACETEXT_IN,
					SLIDER_Ticks, 5,
					SLIDER_Min, 0,
					SLIDER_Max, 127,
                    ICA_TARGET, ICTARGET_IDCMP,
					ICA_MAP, mapSlider,
				End,
			End,
			Label(Texts::VELO_RANGE_GAD),

			LAYOUT_AddChild, mGadgets[GAD_MAINTUNE] = (Gadget *) SliderObject,
				GA_ID, GAD_MAINTUNE,
				GA_RelVerify, TRUE,
				SLIDER_Orientation, SLIDER_HORIZONTAL,
				SLIDER_LevelFormat, "%ld",
				SLIDER_LevelPlace, PLACETEXT_IN,
				SLIDER_LevelMaxLen, 4,
				SLIDER_Ticks, 5,
				SLIDER_Min, -63,
				SLIDER_Max, 63,
				ICA_TARGET, ICTARGET_IDCMP,
				ICA_MAP, mapSlider,
			End,
			Label(Texts::TUNE_GAD),

			LAYOUT_AddChild, mGadgets[GAD_FINETUNE] = (Gadget *) SliderObject,
				GA_ID, GAD_FINETUNE,
				GA_RelVerify, TRUE,
				SLIDER_Orientation, SLIDER_HORIZONTAL,
				SLIDER_LevelFormat, "%ld",
				SLIDER_LevelPlace, PLACETEXT_IN,
				SLIDER_LevelMaxLen, 4,
				SLIDER_Ticks, 5,
				SLIDER_Min, -100,
				SLIDER_Max, 100,
				ICA_TARGET, ICTARGET_IDCMP,
				ICA_MAP, mapSlider,
			End,
			Label(Texts::FINE_TUNE_GAD),

			LAYOUT_AddChild, mGadgets[GAD_PANNING] = (Gadget *) SliderObject,
				GA_ID, GAD_PANNING,
				GA_RelVerify, TRUE,
				SLIDER_Orientation, SLIDER_HORIZONTAL,
				SLIDER_LevelFormat, "%ld",
				SLIDER_LevelPlace, PLACETEXT_IN,
				SLIDER_LevelMaxLen, 4,
				SLIDER_Ticks, 5,
				SLIDER_Min, -63,
				SLIDER_Max, 63,
				ICA_TARGET, ICTARGET_IDCMP,
				ICA_MAP, mapSlider,
			End,
			Label(Texts::PANORAMA_GAD),
		End,
		CHILD_WeightedHeight, 0,
	End;

	static HintInfo hints[] = {
		{GAD_ADD, -1, (char *) Texts::ADD_HINT, 0},
		{GAD_REMOVE, -1, (char *) Texts::REMOVE_HINT, 0},
		{GAD_UP, -1, (char *) Texts::MOVE_UP_HINT, 0},
		{GAD_DOWN, -1, (char *) Texts::MOVE_DOWN_HINT, 0},
		{GAD_PLAYSAMPLE, -1, (char *) Texts::PLAY_SAMPLE_HINT, 0},
		{GAD_PLAYTONE, -1, (char *) Texts::PLAY_TONE_HINT, 0},
		{-1, -1, NULL, 0}
	};
	setGadgetHints(hints);

	Phonolith::addDefaultMenu(this);

	struct NewMenu newMenu[] = {
		{NM_TITLE,	(char *) Texts::INSTRUMENT_MENU, 0, 0, 0, 0},
		{NM_ITEM,	(char *) Texts::INSTRUMENT_NEW_MENU, 0, 0, 0, (void *)MENU_INSTRUMENT_NEW},
		{NM_ITEM,	(char *) Texts::INSTRUMENT_LOAD_MENU, "L", 0, 0, (void *)MENU_INSTRUMENT_LOAD},
		{NM_ITEM,	(char *) Texts::INSTRUMENT_IMPORT_EXS24_MENU, 0, 0, 0, (void *)MENU_INSTRUMENT_IMPORT},
		{NM_ITEM,	NM_BARLABEL, 0, 0, 0, 0},
		{NM_ITEM,	(char *) Texts::INSTRUMENT_SAVE_MENU, "W", 0, 0, (void *)MENU_INSTRUMENT_SAVE},
		{NM_ITEM,	(char *) Texts::INSTRUMENT_SAVE_AS_MENU, 0, 0, 0, (void *)MENU_INSTRUMENT_SAVEAS},

		{NM_END, 0, 0, 0, 0, 0}
	};
	addMenu(newMenu);

	char numName[20];
	sprintf(numName, "instrument%d", mWindowCount++);
	createWindow(numName, Texts::INSTRUMENT, pageInstrument, 0, fWin);

	mInstrument = instrument;

	refreshInstrumentGadgets();

}

//----------------------------------------------------------------------------------

GuiInstrument::~GuiInstrument()
{
	deleteWindow();
	IListBrowser->FreeListBrowserList(&mZoneList);
	mWindowCount--;
}

//==================================================================================
// Stuff
//==================================================================================

void GuiInstrument::changeInstrument(CoreInstrument *instrument)
{
	mInstrument = instrument;
	refreshInstrumentGadgets();
}

//----------------------------------------------------------------------------------

uint32 GuiInstrument::LevelFunc(Hook *hook, Object *sliderObject, TagItem *tagList)
{
	TagItem *levelTag = IUtility->FindTagItem(SLIDER_Level, tagList);
	return (uint32) NOTE_STRINGS[levelTag->ti_Data];
}


//==================================================================================
// Gadgets
//==================================================================================

void GuiInstrument::disableGadget(int gadgetID, bool disable)
{
	IIntuition->SetGadgetAttrs(mGadgets[gadgetID], mWindow, NULL,
			GA_Disabled, disable,
			TAG_DONE);
}

//----------------------------------------------------------------------------------

int GuiInstrument::getZoneSelection()
{
	unsigned long index;
	IIntuition->GetAttr(LISTBROWSER_Selected, mGadgets[GAD_ZONELIST], &index);
	if (mInstrument && index < mInstrument->getNumOfZones())
	{
		return (int)index;
	}
	else
	{
		return -1;
	}
}

//----------------------------------------------------------------------------------

void GuiInstrument::setZoneSelection(int index)
{
	IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONELIST], mWindow, NULL,
			LISTBROWSER_Selected, index,
			LISTBROWSER_MakeVisible, index,
			TAG_DONE);
}

//----------------------------------------------------------------------------------

void GuiInstrument::refreshZoneList()
{
	#ifdef DEBUGGUI
	cout << "refreshZoneList\n";
	#endif
	IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONELIST], mWindow, NULL, LISTBROWSER_Labels, NULL, TAG_DONE);
	IListBrowser->FreeListBrowserList(&mZoneList);
	if (mInstrument && mInstrument->getNumOfZones() > 0)
	{
		int num = mInstrument->getNumOfZones();
		for (int i = 0; i < num; i++)
		{
			Zone *kg = mInstrument->getZone(i);

			char baseKeyName[5];
			makeKeyString(kg->baseKey, baseKeyName);

			char buffer1[5];
			char buffer2[5];
			char keyRangeName[12];
			makeKeyString(kg->lowKey, buffer1);
			makeKeyString(kg->highKey, buffer2);
			sprintf(keyRangeName, "%s - %s", buffer1, buffer2);

			char veloRangeName[10];
			sprintf(veloRangeName, "%d - %d", kg->lowVelocity, kg->highVelocity);

			Node *node = IListBrowser->AllocListBrowserNode(4,
					LBNA_Column, 0, LBNCA_CopyText, TRUE, LBNCA_Text, kg->name.c_str(),
					LBNA_Column, 1, LBNCA_Justification, LCJ_CENTRE, LBNCA_CopyText, TRUE, LBNCA_Text, baseKeyName,
					LBNA_Column, 2, LBNCA_Justification, LCJ_CENTRE, LBNCA_CopyText, TRUE, LBNCA_Text, keyRangeName,
					LBNA_Column, 3, LBNCA_Justification, LCJ_CENTRE, LBNCA_CopyText, TRUE, LBNCA_Text, veloRangeName,
					TAG_DONE);
			IExec->AddTail(&mZoneList, node);
		}

		IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONELIST], mWindow, NULL,
				GA_Disabled, FALSE,
				LISTBROWSER_Labels, &mZoneList,
				TAG_DONE);

		if (getZoneSelection() == -1)
		{
			setZoneSelection(0);
		}
	}
	else
	{
		Node *node = IListBrowser->AllocListBrowserNode(4,
				LBNA_Column, 0, LBNCA_Text, Texts::NO_SAMPLE_GAD,
				LBNA_Column, 1, LBNCA_Justification, LCJ_CENTRE, LBNCA_Text, "-",
				LBNA_Column, 2, LBNCA_Justification, LCJ_CENTRE, LBNCA_Text, "-",
				LBNA_Column, 3, LBNCA_Justification, LCJ_CENTRE, LBNCA_Text, "-",
				TAG_DONE);
		IExec->AddTail(&mZoneList, node);

		IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONELIST], mWindow, NULL,
				GA_Disabled, TRUE,
				LISTBROWSER_Labels, &mZoneList,
				LISTBROWSER_Selected, -1,
				TAG_DONE);
	}

}

//----------------------------------------------------------------------------------

void GuiInstrument::refreshZoneListEntry(int groupIndex)
{
	#ifdef DEBUGGUI
	cout << "refreshZoneListEntry\n";
	#endif
	IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONELIST], mWindow, NULL, LISTBROWSER_Labels, NULL, TAG_DONE);

	Zone *kg = mInstrument->getZone(groupIndex);

	char baseKeyName[5];
	makeKeyString(kg->baseKey, baseKeyName);

	char buffer1[5];
	char buffer2[5];
	char keyRangeName[12];
	makeKeyString(kg->lowKey, buffer1);
	makeKeyString(kg->highKey, buffer2);
	sprintf(keyRangeName, "%s - %s", buffer1, buffer2);

	char veloRangeName[10];
	sprintf(veloRangeName, "%d - %d", kg->lowVelocity, kg->highVelocity);

	Node *node = IExec->GetHead(&mZoneList);
	for (int i = 0; i < groupIndex; i++)
	{
		node = IExec->GetSucc(node);
	}

	IListBrowser->SetListBrowserNodeAttrs(node,
			LBNA_Column, 0, LBNCA_CopyText, TRUE, LBNCA_Text, kg->name.c_str(),
			LBNA_Column, 1, LBNCA_CopyText, TRUE, LBNCA_Text, baseKeyName,
			LBNA_Column, 2, LBNCA_CopyText, TRUE, LBNCA_Text, keyRangeName,
			LBNA_Column, 3, LBNCA_CopyText, TRUE, LBNCA_Text, veloRangeName,
			TAG_DONE);

	IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONELIST], mWindow, NULL,
			LISTBROWSER_Labels, &mZoneList,
			TAG_DONE);

}

//----------------------------------------------------------------------------------

void GuiInstrument::refreshInstrumentGadgets()
{
	if (mInstrument)
	{
		Phonolith *app = (Phonolith *) mFApp;

		IIntuition->SetGadgetAttrs(mGadgets[GAD_NAME], mWindow, NULL, STRINGA_TextVal, mInstrument->mName.c_str(), TAG_DONE);

		IIntuition->SetGadgetAttrs(mGadgets[GAD_NUMVOICES], mWindow, NULL,
				INTEGER_Number, mInstrument->mNumVoices,
				INTEGER_Maximum, app->mPrefs->mSamplerMaxVoices,
				TAG_DONE);

		IIntuition->SetGadgetAttrs(mGadgets[GAD_ATTACK], mWindow, NULL, SLIDER_Level, mInstrument->mAmpEnvelope.mAttackTime, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_DECAY], mWindow, NULL, SLIDER_Level, mInstrument->mAmpEnvelope.mDecayTime, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_SUSTAIN], mWindow, NULL, SLIDER_Level, mInstrument->mAmpEnvelope.mSustainLevel, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_RELEASE], mWindow, NULL, SLIDER_Level, mInstrument->mAmpEnvelope.mReleaseTime, TAG_DONE);

		refreshZoneList();
		refreshZoneGadgets();
	}
}

//----------------------------------------------------------------------------------

void GuiInstrument::refreshZoneGadgets()
{
	IIntuition->SetGadgetAttrs(mGadgets[GAD_PLAYSAMPLE], mWindow, NULL, GA_Selected, FALSE, TAG_DONE);
	IIntuition->SetGadgetAttrs(mGadgets[GAD_PLAYTONE], mWindow, NULL, GA_Selected, FALSE, TAG_DONE);

	int kgIndex = getZoneSelection();
	if (kgIndex != -1)
	{
		IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONEGROUP], mWindow, NULL, GA_Disabled, FALSE, TAG_DONE);

		Zone *kg = mInstrument->getZone(kgIndex);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_FILE], mWindow, NULL,
				GETFILE_FullFile, (kg->sound ? kg->sound->getFileName() : ""),
				TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_POSSTART], mWindow, NULL,
				INTEGER_Number, kg->startPosition,
				INTEGER_Maximum, (kg->sound ? kg->sound->mNumSampleFrames - 1 : 0),
				TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_POSEND], mWindow, NULL,
				INTEGER_Number, kg->endPosition,
				INTEGER_Maximum, (kg->sound ? kg->sound->mNumSampleFrames : 0),
				TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_BASEKEY], mWindow, NULL, SLIDER_Level, kg->baseKey, TAG_DONE);
		refreshZoneRangeGadgets();
		IIntuition->SetGadgetAttrs(mGadgets[GAD_MAINTUNE], mWindow, NULL, SLIDER_Level, kg->mainTune, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_FINETUNE], mWindow, NULL, SLIDER_Level, kg->fineTune, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_PANNING], mWindow, NULL, SLIDER_Level, kg->panning, TAG_DONE);

		IIntuition->RefreshGList(mGadgets[GAD_ZONEGROUP], mWindow, NULL, 1);
	}
	else
	{
		IIntuition->SetGadgetAttrs(mGadgets[GAD_ZONEGROUP], mWindow, NULL, GA_Disabled, TRUE, TAG_DONE);
		IIntuition->RefreshGList(mGadgets[GAD_ZONEGROUP], mWindow, NULL, 1);
	}
}

//----------------------------------------------------------------------------------

void GuiInstrument::refreshZoneRangeGadgets()
{
	int kgIndex = getZoneSelection();
	if (kgIndex != -1)
	{
		Zone *kg = mInstrument->getZone(kgIndex);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_KEYLOW], mWindow, NULL, SLIDER_Level, kg->lowKey, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_KEYHIGH], mWindow, NULL, SLIDER_Level, kg->highKey, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_VELOLOW], mWindow, NULL, SLIDER_Level, kg->lowVelocity, TAG_DONE);
		IIntuition->SetGadgetAttrs(mGadgets[GAD_VELOHIGH], mWindow, NULL, SLIDER_Level, kg->highVelocity, TAG_DONE);
	}
}

//----------------------------------------------------------------------------------

const char *GuiInstrument::requestFile(int gadgetID)
{
	if (gfRequestFile((Object *) mGadgets[gadgetID], mWindow))
	{
		char *fileName = NULL;
		IIntuition->GetAttr(GETFILE_FullFile, (Object *) mGadgets[gadgetID], (unsigned long *) &fileName);
		return fileName;
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------------

const char *GuiInstrument::getGadgetString(int gadgetID)
{
	char *text = NULL;
	IIntuition->GetAttr(STRINGA_TextVal, mGadgets[gadgetID], (unsigned long *) &text);
	return text;

}

//----------------------------------------------------------------------------------

void GuiInstrument::setGadgetString(int gadgetID, const char *text)
{
	IIntuition->SetGadgetAttrs(mGadgets[gadgetID], mWindow, NULL, STRINGA_TextVal, text, TAG_DONE);
}

//----------------------------------------------------------------------------------

long GuiInstrument::getIntegerNumber(int gadgetID)
{
	long value = 0;
	IIntuition->GetAttr(INTEGER_Number, mGadgets[gadgetID], (unsigned long *) &value);
	return value;

}

//----------------------------------------------------------------------------------

void GuiInstrument::makeKeyString(char key, char *buffer)
{
	static char *names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	sprintf(buffer, "%s%d", names[key % 12], key / 12 - 2);
}

