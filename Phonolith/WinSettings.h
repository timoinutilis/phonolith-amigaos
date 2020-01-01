#ifndef _WINSETTINGS_
#define _WINSETTINGS_

#include "FrameApp.h"
#include "FrameWindow.h"
#include "CorePrefs.h"
#include "GuiSettings.h"

class WinSettings : public FrameWindow
{
	public:
		WinSettings(FrameApp *fApp);
		~WinSettings();

		int closeRequested();
		void menuPicked(int menuID);
		void gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp);

	protected:
		CorePrefs mNewPrefs;

		void readDirs(GuiSettings *gui);


};

#endif

