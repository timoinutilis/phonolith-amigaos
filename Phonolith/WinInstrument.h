#ifndef _WININSTRUMENT_
#define _WININSTRUMENT_

#include "FrameWindow.h"
#include "FileRequest.h"
#include "CoreInstrument.h"

class WinInstrument : public FrameWindow
{
	public:
		WinInstrument(FrameApp *fApp, CoreInstrument *instrument, int unitIndex);
		~WinInstrument();

		void changeInstrument(CoreInstrument *instrument, int unitIndex);
		void updateTitle();
		void lockUnit();
		void unlockUnit();
		void playKey(char key);
		void stopKey();
		void changeKey(char key);
		void stopTestSounds();

		void menuPicked(int menuID);
		void gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp);
		int closeRequested();

	protected:
		FileRequest *mReqSample;
		FileRequest *mReqImport;
		FileRequest *mReqInstrument;
		CoreInstrument *mInstrument;
		int mUnitIndex;
		int mPlayKey;

};

#endif

