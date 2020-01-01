#ifndef _WINMIXER_
#define _WINMIXER_

#include "FrameWindow.h"
#include "FileRequest.h"

class WinMixer : public FrameWindow
{
	public:
		WinMixer(FrameApp *fApp);
		~WinMixer();

		void menuPicked(int menuID);
		int closeRequested();
		void gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp);

	protected:
		FileRequest *mReqExportFile;

};

#endif

