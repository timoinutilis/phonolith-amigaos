#ifndef _GUIMIXER_
#define _GUIMIXER_

#include <string>

#include "FrameGui.h"

#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <classes/window.h>
#include <exec/lists.h>

class GuiMixer : public FrameGui
{
	public:
		GuiMixer(FrameApp *fApp, FrameWindow *fWin);
		~GuiMixer();

		enum gadgetid {
			GAD_MASTERVOLUME = 1,
			GAD_MASTERMETER,
			GAD_START,
			GAD_STOP,
			GAD_MIXDOWN,
			GAD_STATUS,

			GAD_NUM
		};

		enum channelgadid {
			CHANNELGAD_METER,
			CHANNELGAD_VOLUME,
			CHANNELGAD_PANORAMA,
			CHANNELGAD_EDIT,
			CHANNELGAD_LABEL,

			CHANNELGAD_NUM
		};

		const static int CHANNELGAD_ID_OFFSET = 1000;

		void disableGadget(int gadgetID, bool disable);
		void selectGadget(int gadgetID, bool select);
		void updateMixerGadgets();
		void updatePeakMeters();
		void updateFaders();
		void updateLabels();
		void setStatusText(const char *text);

	protected:
		Object *createChannelStrip(int unitIndex, Screen *screen);

		Gadget *mGadgets[GAD_NUM];
		Gadget *mChannelGadgets[CHANNELGAD_NUM * 16];

		string mStatusText;
};

#endif

