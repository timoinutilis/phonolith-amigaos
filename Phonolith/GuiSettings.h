#ifndef _GUISETTINGS_
#define _GUISETTINGS_

#include "FrameGui.h"
#include "CorePrefs.h"

#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <classes/window.h>
#include <exec/lists.h>

class GuiSettings : public FrameGui
{
	public:
		GuiSettings(FrameApp *fApp, CorePrefs *newPrefs);
		~GuiSettings();

		enum gadgetid {
			GAD_REFRESHPORTS,
			GAD_PORTLIST,
			GAD_MIDIPORT,

			GAD_SELECTAUDIO,
			GAD_AUDIOMODE,
			GAD_AUDIOFREQ,
			GAD_PLAYERBUFFER,

			GAD_MAXVOICES,

			GAD_DIRPROJECTS,
			GAD_DIRINSTRUMENTS,
			GAD_DIRSAMPLES,
			GAD_DIRIMPORTS,
			GAD_ACTIVATEPLAYER,
//			  GAD_SHOWDOCKICON,

			GAD_SAVE,
			GAD_USE,
			GAD_CANCEL,

			GAD_NUM
		};

		void disableGadget(int gadgetID, bool disable);
		int getZoneSelection();
		void setZoneSelection(int index);
//		  void setSamplerUnit(CoreSamplerUnit *samplerUnit);
		char *getSelectedPortName();
		void refreshPortList();
		void refreshZoneList();
		void refreshZoneListEntry(int groupIndex);
		void refreshInstrumentGadgets();
		void refreshZoneGadgets();
		char *requestFile(int gadgetID);
		char *getGadgetString(int gadgetID);
		void setGadgetString(int gadgetID, char *text);
		void requestDir(int gadgetID);
		char *getDir(int gadgetID);
		void updateAudioGadgets();

	protected:
		Gadget *mGadgets[GAD_NUM];
		List mPortList;
		CorePrefs *mNewPrefs;

};

#endif

