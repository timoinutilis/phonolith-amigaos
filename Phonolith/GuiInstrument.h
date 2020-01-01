#ifndef _GUIINSTRUMENT_
#define _GUIINSTRUMENT_

#include "FrameGui.h"
#include "CoreInstrument.h"
#include "FrameWindow.h"

#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <classes/window.h>
#include <exec/lists.h>

class GuiInstrument : public FrameGui
{
	public:
		GuiInstrument(FrameApp *fApp, CoreInstrument *instrument, FrameWindow *fWin);
		~GuiInstrument();

		enum gadgetid {
			GAD_NAME = 1,
			GAD_NUMVOICES,
			GAD_ATTACK,
			GAD_DECAY,
			GAD_SUSTAIN,
			GAD_RELEASE,
			GAD_ZONELIST,
			GAD_ADD,
			GAD_REMOVE,
			GAD_UP,
			GAD_DOWN,
			GAD_TOOLS,
			GAD_ZONEGROUP,
			GAD_FILE,
			GAD_PLAYSAMPLE,
			GAD_POSSTART,
			GAD_POSEND,
			GAD_BASEKEY,
			GAD_PLAYTONE,
			GAD_KEYLOW,
			GAD_KEYHIGH,
			GAD_VELOLOW,
			GAD_VELOHIGH,
			GAD_MAINTUNE,
			GAD_FINETUNE,
			GAD_PANNING,
			GAD_START,
			GAD_STOP,

			GAD_NUM
		};

		enum menuid {
			MENU_INSTRUMENT_NEW = 1000,
			MENU_INSTRUMENT_LOAD,
			MENU_INSTRUMENT_IMPORT,
			MENU_INSTRUMENT_SAVE,
			MENU_INSTRUMENT_SAVEAS
		};

		void changeInstrument(CoreInstrument *instrument);

		void disableGadget(int gadgetID, bool disable);
		int getZoneSelection();
		void setZoneSelection(int index);
		void refreshZoneList();
		void refreshZoneListEntry(int groupIndex);
		void refreshInstrumentGadgets();
		void refreshZoneGadgets();
		void refreshZoneRangeGadgets();
		const char *requestFile(int gadgetID);
		const char *getGadgetString(int gadgetID);
		void setGadgetString(int gadgetID, const char *text);
		long getIntegerNumber(int gadgetID);

		void makeKeyString(char key, char *buffer);

	protected:
		Gadget *mGadgets[GAD_NUM];
	    Hook mLevelHook;
		List mZoneList;
		static int mWindowCount;

		CoreInstrument *mInstrument;
		int mSelectedZone;

	    static uint32 LevelFunc(Hook *hook, Object *sliderObject, TagItem *tagList);

};

#endif

