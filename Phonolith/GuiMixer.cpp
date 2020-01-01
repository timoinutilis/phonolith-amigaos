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

#include "Version.h"

#include "GuiMixer.h"
#include "Phonolith.h"
#include "CoreSampler.h"
#include "CoreSamplerUnit.h"
#include "BoopsiPeakMeter.h"
#include "BoopsiTextLabel.h"
#include "CoreProject.h"
#include "Texts.h"

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/window.h>
#include <proto/layout.h>
#include <proto/button.h>
#include <proto/slider.h>
#include <proto/space.h>
//#include <proto/virtual.h>
#include <proto/string.h>
#include <proto/bitmap.h>
#include <proto/requester.h>

#include <exec/exec.h>
#include <libraries/gadtools.h>
#include <intuition/icclass.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <gadgets/slider.h>
#include <gadgets/string.h>
#include <gadgets/space.h>
//#include <gadgets/virtual.h>
#include <images/bitmap.h>
#include <classes/requester.h>

#include <reaction/reaction_macros.h>

const static TagItem mapSlider[] = {SLIDER_Level, ICSPECIAL_CODE, TAG_DONE};

//==================================================================================
// Constructor/Destructor
//==================================================================================

GuiMixer::GuiMixer(FrameApp *fApp, FrameWindow *fWin) : FrameGui(fApp)
{
	Screen *screen = IIntuition->LockPubScreen(NULL);

	Phonolith *app = (Phonolith *) mFApp;

	Object *channelStrips = (Object *) HLayoutObject,
		LAYOUT_BevelStyle, BVS_GROUP,
		LAYOUT_SpaceOuter, TRUE,

		LAYOUT_AddChild, createChannelStrip(0, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(1, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(2, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(3, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(4, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(5, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(6, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(7, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(8, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(9, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(10, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(11, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(12, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(13, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(14, screen), CHILD_WeightedWidth, 0,
		LAYOUT_AddChild, createChannelStrip(15, screen), CHILD_WeightedWidth, 0,
	End;

	Object *pageMixer = (Object *) VLayoutObject,
		LAYOUT_DeferLayout, TRUE,

		LAYOUT_AddChild, HLayoutObject,
//			  LAYOUT_AddChild, IIntuition->NewObject(IVirtual->VIRTUAL_GetClass(), NULL,
//				  VIRTUALA_Contents, channelStrips,
//			  TAG_DONE),
			LAYOUT_AddChild, channelStrips,

			LAYOUT_AddChild, VLayoutObject,
				LAYOUT_BevelStyle, BVS_GROUP,
				LAYOUT_SpaceOuter, TRUE,

				LAYOUT_AddChild, HLayoutObject,
					LAYOUT_AddChild, mGadgets[GAD_MASTERMETER] = (Gadget *) IIntuition->NewObject(PEAKMETER_GetClass(), NULL,
							GA_ID, GAD_MASTERMETER,
							GA_ReadOnly, FALSE,
							PEAKMETER_Screen, screen,
							TAG_DONE),

					LAYOUT_AddChild, mGadgets[GAD_MASTERVOLUME] = (Gadget *) SliderObject,
						GA_ID, GAD_MASTERVOLUME,
						GA_RelVerify, TRUE,
						SLIDER_Orientation, SLIDER_VERTICAL,
						SLIDER_Min, 0,
						SLIDER_Max, 127,
						SLIDER_Level, 100,
						SLIDER_Invert, TRUE,
						SLIDER_LevelDomain, "000",
						SLIDER_LevelFormat, "%ld",
						SLIDER_LevelPlace, PLACETEXT_IN,
						ICA_TARGET, ICTARGET_IDCMP,
						ICA_MAP, mapSlider,
					End,
				End,
				//CHILD_WeightedHeight, 0,

				LAYOUT_AddChild, SpaceObject, End,

				LAYOUT_AddChild, ButtonObject,
					GA_ReadOnly, TRUE,
					GA_Text, "ST",
				End,
				CHILD_WeightedHeight, 0,

				LAYOUT_AddChild, IIntuition->NewObject(TEXTLABEL_GetClass(), NULL,
						TEXTLABEL_Text, Texts::MASTER_GAD,
						TAG_DONE),
				CHILD_WeightedHeight, 0,

			End,
			CHILD_WeightedWidth, 0,
		End,
		CHILD_WeightedHeight, 0,


		LAYOUT_AddChild, HLayoutObject,
			LAYOUT_AddImage, Phonolith::mImageBanner,
			CHILD_NoDispose, TRUE,
			CHILD_WeightedWidth, 0,

			LAYOUT_AddChild, HLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::STATUS_GAD,

				LAYOUT_AddChild, mGadgets[GAD_STATUS] = (Gadget *) StringObject,
					GA_ReadOnly, TRUE,
				End,
			End,

			LAYOUT_AddChild, HLayoutObject,
				LAYOUT_BevelStyle, BVS_SBAR_VERT,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_Label, Texts::PLAYER_GAD,

				LAYOUT_AddChild, mGadgets[GAD_START] = (Gadget *) ButtonObject,
					GA_ID, GAD_START,
					GA_RelVerify, TRUE,
					BUTTON_RenderImage, Phonolith::mImagePlayerOn,
				End,

				LAYOUT_AddChild, mGadgets[GAD_STOP] = (Gadget *) ButtonObject,
					GA_ID, GAD_STOP,
					GA_RelVerify, TRUE,
					BUTTON_RenderImage, Phonolith::mImagePlayerOff,
				End,

				LAYOUT_AddChild, mGadgets[GAD_MIXDOWN] = (Gadget *) ButtonObject,
					GA_ID, GAD_MIXDOWN,
					GA_RelVerify, TRUE,
					GA_Disabled, !app->mRegistered,
					BUTTON_PushButton, TRUE,
					BUTTON_RenderImage, Phonolith::mImagePlayerMixdown,
				End,
			End,
			CHILD_WeightedWidth, 0,
		End,
		CHILD_WeightedHeight, 0,

	End;

	static HintInfo hints[] = {
		{GAD_MASTERVOLUME, -1, (char *) Texts::MASTER_VOLUME_HINT, 0},
		{GAD_MASTERMETER, -1, (char *) Texts::MASTER_METER_HINT, 0},
		{GAD_START, -1, (char *) Texts::ACTIVATE_HINT, 0},
		{GAD_STOP, -1, (char *) Texts::DEACTIVATE_HINT, 0},
		{GAD_MIXDOWN, -1, (char *) Texts::MIXDOWN_HINT, 0},
		// channel 1
		{CHANNELGAD_ID_OFFSET + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 2
		{CHANNELGAD_ID_OFFSET + 1 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 1 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 1 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 1 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 3
		{CHANNELGAD_ID_OFFSET + 2 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 2 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 2 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 2 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 4
		{CHANNELGAD_ID_OFFSET + 3 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 3 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 3 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 3 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 5
		{CHANNELGAD_ID_OFFSET + 4 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 4 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 4 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 4 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 6
		{CHANNELGAD_ID_OFFSET + 5 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 5 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 5 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 5 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 7
		{CHANNELGAD_ID_OFFSET + 6 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 6 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 6 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 6 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 8
		{CHANNELGAD_ID_OFFSET + 7 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 7 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 7 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 7 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 9
		{CHANNELGAD_ID_OFFSET + 8 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 8 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 8 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 8 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 10
		{CHANNELGAD_ID_OFFSET + 9 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 9 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 9 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 9 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 11
		{CHANNELGAD_ID_OFFSET + 10 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 10 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 10 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 10 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 12
		{CHANNELGAD_ID_OFFSET + 11 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 11 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 11 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 11 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 13
		{CHANNELGAD_ID_OFFSET + 12 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 12 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 12 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 12 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 14
		{CHANNELGAD_ID_OFFSET + 13 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 13 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 13 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 13 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 15
		{CHANNELGAD_ID_OFFSET + 14 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 14 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 14 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 14 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},
		// channel 16
		{CHANNELGAD_ID_OFFSET + 15 * CHANNELGAD_NUM + CHANNELGAD_METER, -1, (char *) Texts::CHANNEL_METER_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 15 * CHANNELGAD_NUM + CHANNELGAD_VOLUME, -1, (char *) Texts::CHANNEL_VOLUME_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 15 * CHANNELGAD_NUM + CHANNELGAD_PANORAMA, -1, (char *) Texts::CHANNEL_PANORAMA_HINT, 0},
		{CHANNELGAD_ID_OFFSET + 15 * CHANNELGAD_NUM + CHANNELGAD_EDIT, -1, (char *) Texts::CHANNEL_EDIT_HINT, 0},

		{-1, -1, NULL, 0}
	};
	setGadgetHints(hints);

	Phonolith::addDefaultMenu(this);

	string title = "Phonolith "APPVERSION;
	if (app->mRegistered)
	{
//		  title += Texts::REGISTERED_FOR;
//		  title += app->mUserName;
	}
	else
	{
		title += " Lite";
	}
	createWindow("mixer", (char *) title.c_str(), pageMixer, FLAG_LOCKHEIGHT, fWin);

	IIntuition->UnlockPubScreen(NULL, screen);

}

//----------------------------------------------------------------------------------

GuiMixer::~GuiMixer()
{
	deleteWindow();
}

//----------------------------------------------------------------------------------

Object *GuiMixer::createChannelStrip(int unitIndex, Screen *screen)
{
	static char *numbers[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16"};
	int offsetID = CHANNELGAD_ID_OFFSET + unitIndex * CHANNELGAD_NUM;

	return (Object *) VLayoutObject,
		LAYOUT_AddChild, HLayoutObject,
			LAYOUT_AddChild, mChannelGadgets[CHANNELGAD_NUM * unitIndex + CHANNELGAD_METER] =
					(Gadget *) IIntuition->NewObject(PEAKMETER_GetClass(), NULL,
					GA_ID, offsetID + CHANNELGAD_METER,
					GA_ReadOnly, FALSE,
					PEAKMETER_Screen, screen,
					TAG_DONE),

			LAYOUT_AddChild, mChannelGadgets[CHANNELGAD_NUM * unitIndex + CHANNELGAD_VOLUME] = (Gadget *) SliderObject,
				GA_ID, offsetID + CHANNELGAD_VOLUME,
				GA_RelVerify, TRUE,
				SLIDER_Orientation, SLIDER_VERTICAL,
				SLIDER_Min, 0,
				SLIDER_Max, 127,
				SLIDER_Level, 100,
				SLIDER_Invert, TRUE,
				SLIDER_LevelDomain, "000",
				SLIDER_LevelFormat, "%ld",
				SLIDER_LevelPlace, PLACETEXT_IN,
				ICA_TARGET, ICTARGET_IDCMP,
				ICA_MAP, mapSlider,
			End,
		End,

		LAYOUT_AddChild, mChannelGadgets[CHANNELGAD_NUM * unitIndex + CHANNELGAD_PANORAMA] = (Gadget *) SliderObject,
			GA_ID, offsetID + CHANNELGAD_PANORAMA,
            GA_RelVerify, TRUE,
			SLIDER_Orientation, SLIDER_HORIZONTAL,
			SLIDER_Min, -63,
			SLIDER_Max, 63,
			SLIDER_Level, 0,
			SLIDER_KnobDelta, 10,
			ICA_TARGET, ICTARGET_IDCMP,
			ICA_MAP, mapSlider,
		End,
		CHILD_WeightedHeight, 0,

		LAYOUT_AddChild, ButtonObject,
			GA_ID, offsetID + CHANNELGAD_EDIT,
			GA_RelVerify, TRUE,
			GA_Text, numbers[unitIndex],
		End,
		CHILD_WeightedHeight, 0,

		LAYOUT_AddChild, mChannelGadgets[CHANNELGAD_NUM * unitIndex + CHANNELGAD_LABEL] =
				(Gadget *) IIntuition->NewObject(TEXTLABEL_GetClass(), NULL,
				TAG_DONE),
		CHILD_WeightedHeight, 0,
	End;
}

//==================================================================================
// Gadget Commands
//==================================================================================

void GuiMixer::disableGadget(int gadgetID, bool disable)
{
	IIntuition->SetGadgetAttrs(mGadgets[gadgetID], mWindow, NULL,
			GA_Disabled, disable,
			TAG_DONE);
}

//----------------------------------------------------------------------------------

void GuiMixer::selectGadget(int gadgetID, bool select)
{
	IIntuition->SetGadgetAttrs(mGadgets[gadgetID], mWindow, NULL,
			GA_Selected, select,
			TAG_DONE);
}

//----------------------------------------------------------------------------------

void GuiMixer::updateMixerGadgets()
{
	updateFaders();
	updateLabels();
}

//----------------------------------------------------------------------------------

void GuiMixer::updatePeakMeters()
{
	//channels
	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		int peakL = CoreSampler::mPeakL[i];
		CoreSampler::mPeakL[i] = 0;
		int peakR = CoreSampler::mPeakR[i];
		CoreSampler::mPeakR[i] = 0;

		IIntuition->SetGadgetAttrs(mChannelGadgets[CHANNELGAD_NUM * i + CHANNELGAD_METER], mWindow, NULL,
				PEAKMETER_Process, 512,
				PEAKMETER_PeakLeft, peakL,
				PEAKMETER_PeakRight, peakR,
				TAG_DONE);

	}

	//master
	int peakL = CoreSampler::mMasterPeakL;
    CoreSampler::mMasterPeakL = 0;
	int peakR = CoreSampler::mMasterPeakR;
    CoreSampler::mMasterPeakR = 0;

	IIntuition->SetGadgetAttrs(mGadgets[GAD_MASTERMETER], mWindow, NULL,
			PEAKMETER_Process, 512,
			PEAKMETER_PeakLeft, peakL,
			PEAKMETER_PeakRight, peakR,
			TAG_DONE);
}

//----------------------------------------------------------------------------------

void GuiMixer::updateFaders()
{
	CoreProject *project = ((Phonolith *) mFApp)->mProject;
	IIntuition->SetGadgetAttrs(mGadgets[GAD_MASTERVOLUME], mWindow, NULL, SLIDER_Level, project->mMasterVolumeInt, TAG_DONE);
	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		IIntuition->SetGadgetAttrs(mChannelGadgets[CHANNELGAD_NUM * i + CHANNELGAD_VOLUME], mWindow, NULL, SLIDER_Level, project->mChannelVolumeInt[i], TAG_DONE);
		IIntuition->SetGadgetAttrs(mChannelGadgets[CHANNELGAD_NUM * i + CHANNELGAD_PANORAMA], mWindow, NULL, SLIDER_Level, project->mChannelPanoramaInt[i], TAG_DONE);
	}
}

//----------------------------------------------------------------------------------

void GuiMixer::updateLabels()
{
	CoreProject *project = ((Phonolith *) mFApp)->mProject;
	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		IIntuition->SetGadgetAttrs(mChannelGadgets[CHANNELGAD_NUM * i + CHANNELGAD_LABEL], mWindow, NULL,
				TEXTLABEL_Text, project->mInstruments[i]->mName.c_str(),
				TAG_DONE);
	}
}

//----------------------------------------------------------------------------------

void GuiMixer::setStatusText(const char *text)
{
	IIntuition->SetGadgetAttrs(mGadgets[GAD_STATUS], mWindow, NULL,
			STRINGA_TextVal, "",
			TAG_DONE);

	mStatusText = text;

	IIntuition->SetGadgetAttrs(mGadgets[GAD_STATUS], mWindow, NULL,
			STRINGA_TextVal, mStatusText.c_str(),
			TAG_DONE);

}

