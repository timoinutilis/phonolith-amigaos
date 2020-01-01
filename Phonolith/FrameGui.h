#ifndef _FRAMEGUI_
#define _FRAMEGUI_

#include "FrameWindow.h"
#include "FrameApp.h"

#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <exec/exec.h>
#include <classes/window.h>

class FrameApp;

class FrameGui
{
	public:
		FrameGui(FrameApp *fApp);
		virtual ~FrameGui();

		static void openLib();
		static void closeLib();

		void activate();
		void setTitle(char *title);
		void iconifyWindow();
		void closeWindow();
		void reopenWindow();

		void addMenu(NewMenu *newMenu);
		int getMenuItemID(unsigned long code);

		void rememberWindowBorders();

        MsgPort *mAppPort;
		Window *mWindow;
		Object *mWindowObject;
		unsigned long mSignalMask;

	protected:
		const static int FLAG_LOCKWIDTH = 0x01;
		const static int FLAG_LOCKHEIGHT = 0x02;

		void createWindow(const char *idName, const char *title, Object *layout, int flags, class FrameWindow *fWin = NULL);
		void deleteWindow();
		void setGadgetHints(HintInfo *hints);

		FrameApp *mFApp;

		NewMenu *mNewMenuItems;
		int mNumOfNewMenuItems;
		Menu *mMenu;
		APTR mVisualInfo;
		HintInfo *mHints;

		string mTitle;
		string mIdName;
};

#endif

