#ifndef _FRAMEWINDOW_
#define _FRAMEWINDOW_

#include "FrameGui.h"
#include "FrameApp.h"

#include <devices/inputevent.h>

class FrameApp;
class FrameGui;

class FrameWindow
{
	public:
		FrameWindow(FrameApp *fApp);
		virtual ~FrameWindow();

		const static short QUALIFIER_SHIFT = IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
		const static short QUALIFIER_CTRL = IEQUALIFIER_CONTROL;
		const static short QUALIFIER_ALT = IEQUALIFIER_LALT | IEQUALIFIER_RALT;

		virtual void menuPicked(int menuID) = 0;
		virtual void gadgetPressed(int gadgetID, int code, short qualifier, bool gadgetUp) = 0;

		const static int CLOSE_IGNORE = 0;
		const static int CLOSE_WINDOW = 1;
		const static int CLOSE_APP = 2;

		virtual int closeRequested();

		FrameGui *mFGui;
		bool mLeaveWindow;

	protected:
		FrameApp *mFApp;

		void leaveWindow();

};

#endif

